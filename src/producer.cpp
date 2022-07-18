//
// Created by 武腾 on 2022/7/6.
//


/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "producer.h"
#include "workers/producer/send_message.h"
#include "workers/producer/start_or_shutdown.h"

#include <MQClientException.h>
#include <string>
#include <iostream>


using namespace std;

namespace __node_rocketmq__ {

    static Napi::FunctionReference constructor;

    RocketMQProducer::RocketMQProducer(const Napi::CallbackInfo &info) : Napi::ObjectWrap<RocketMQProducer>(info) {

        const char *group_id = info[0].ToString().Utf8Value().data();
        const char *instance_name = info[1].ToString().Utf8Value().data();
        Napi::Object options = info[2].ToObject();
        producer_ptr = CreateProducer(group_id);
        if (info[1].IsNull() ? NULL : instance_name) {
            SetProducerInstanceName(producer_ptr, info[1].IsNull() ? NULL : instance_name);
        }
        try {
            this->SetOptions(options);
        } catch (runtime_error e) {
            Napi::Error::New(info.Env(), e.what())
                    .ThrowAsJavaScriptException();
        }
    }

    RocketMQProducer::~RocketMQProducer() {
        try {
            ShutdownProducer(producer_ptr);
        }
        catch (...) {
            //
        }

        DestroyProducer(producer_ptr);
    }

    void RocketMQProducer::SetOptions(Napi::Object options) {
        // set name server
        Napi::Value _name_server_v = options.Get("nameServer");
        if (_name_server_v.IsString()) {
            SetProducerNameServerAddress(producer_ptr, _name_server_v.ToString().Utf8Value().data());
        }

        // set group name
        Napi::Value _group_name_v = options.Get("groupName");
        if (_group_name_v.IsString()) {
            SetProducerGroupName(producer_ptr, _group_name_v.ToString().Utf8Value().data());
        }


        // set log num & single log size
        int file_num = 3;
        int64 file_size = 104857600;
        Napi::Value _log_file_num_v = options.Get("logFileNum");
        Napi::Value _log_file_size_v = options.Get("logFileSize");
        if (_log_file_num_v.IsNumber()) {
            file_num = _log_file_num_v.ToNumber().Int32Value();
        }
        if (_log_file_size_v.IsNumber()) {
            file_size = _log_file_size_v.ToNumber().Int32Value();
        }
        SetProducerLogFileNumAndSize(producer_ptr, file_num, file_size);

        // set log level
        Napi::Value _log_level_v = options.Get("logLevel");
        if (_log_level_v.IsNumber()) {
            int level = _log_level_v.ToNumber().Int32Value();
            SetProducerLogLevel(producer_ptr, (CLogLevel) level);
        }

        // set compress level
        Napi::Value _compress_level_v = options.Get("compressLevel");
        if (_compress_level_v.IsNumber()) {
            int level = _compress_level_v.ToNumber().Int32Value();
            SetProducerCompressLevel(producer_ptr, level);
        }

        // set send message timeout
        Napi::Value _send_message_timeout_v = options.Get("sendMessageTimeout");
        if (_send_message_timeout_v.IsNumber()) {
            int timeout = _send_message_timeout_v.ToNumber().Int32Value();
            SetProducerSendMsgTimeout(producer_ptr, timeout);
        }

        // set max message size
        Napi::Value _max_message_size_v = options.Get("maxMessageSize");
        if (_max_message_size_v.IsNumber()) {
            int size = _max_message_size_v.ToNumber().Int32Value();
            SetProducerMaxMessageSize(producer_ptr, size);
        }
    }


    Napi::Object RocketMQProducer::Init(Napi::Env env, Napi::Object exports) {
        Napi::Function func =
                DefineClass(env,
                            "RocketMQProducer",
                            {
//                                InstanceMethod("New", &RocketMQProducer::New),
                                InstanceMethod("start", &RocketMQProducer::Start),
                                InstanceMethod("shutdown", &RocketMQProducer::Shutdown),
                                InstanceMethod("send", &RocketMQProducer::Send),
                                InstanceMethod("setSessionCredentials", &RocketMQProducer::SetSessionCredentials),
                        });
        Napi::FunctionReference *constructor = new Napi::FunctionReference();
        *constructor = Napi::Persistent(func);
        env.SetInstanceData<Napi::FunctionReference>(constructor);


        exports.Set("Producer", func);
        return exports;
    }

    Napi::Value RocketMQProducer::Start(const Napi::CallbackInfo &info) {
        Napi::Function callback = info[0].As<Napi::Function>();
        ProducerStartOrShutdownWorker *pssWorker = new ProducerStartOrShutdownWorker(callback, this->GetProducer(),
                                                                                     ProducerWorkerType::START_PRODUCER);

        pssWorker->Queue();
        return info.Env().Undefined();
    }

    Napi::Value RocketMQProducer::Shutdown(const Napi::CallbackInfo &info) {
        Napi::Function callback = info[0].As<Napi::Function>();
        ProducerStartOrShutdownWorker *pssWorker = new ProducerStartOrShutdownWorker(callback, this->GetProducer(),
                                                                                     ProducerWorkerType::SHUTDOWN_PRODUCER);
        pssWorker->Queue();
        return info.Env().Undefined();
    }

    Napi::Value RocketMQProducer::SetSessionCredentials(const Napi::CallbackInfo &info) {
        const char *access_key = info[0].ToString().Utf8Value().data();
        const char *secret_key = info[1].ToString().Utf8Value().data();
        const char *ons_channel = info[2].ToString().Utf8Value().data();
        int ret;
        try {
            ret = SetProducerSessionCredentials(this->GetProducer(), access_key, secret_key, ons_channel);
        } catch (runtime_error e) {
            Napi::Error::New(info.Env(), e.what())
                    .ThrowAsJavaScriptException();
            return info.Env().Null();
        }
        catch (std::exception &e) {
            Napi::Error::New(info.Env(), e.what())
                    .ThrowAsJavaScriptException();
            return info.Env().Null();
        }
        catch (rocketmq::MQException &e) {
            Napi::Error::New(info.Env(), e.what())
                    .ThrowAsJavaScriptException();
            return info.Env().Null();
        }
        return Napi::Number::New(info.Env(), ret);
    }

    Napi::Value RocketMQProducer::Send(const Napi::CallbackInfo &info) {
        Napi::String topic = info[0].ToString();
        Napi::Object options = info[2].ToObject();
        CMessage *msg = CreateMessage(topic.Utf8Value().data());

        Napi::Value _tags_to_be_checked = options.Get("tags");
        Napi::Value _keys_to_be_checked = options.Get("keys");
        if (_tags_to_be_checked.IsString()) {
            const char *tags = _tags_to_be_checked.ToString().Utf8Value().data();
            SetMessageTags(msg, tags);
        }

        if (_keys_to_be_checked.IsString()) {
            const char *keys = _keys_to_be_checked.ToString().Utf8Value().data();
            SetMessageKeys(msg, keys);
        }

        if (info[1].IsString()) {
            SetMessageBody(msg, info[1].ToString().Utf8Value().data());
        } else {
            Napi::Buffer<const char> buff = Napi::Buffer<const char>(info.Env(), info[1]);
            SetByteMessageBody(msg, buff.Data(), buff.Length());
        }
        if (info[3].IsFunction()) {
            Napi::Function callback = info[3].As<Napi::Function>();
            ProducerSendMessageWorker *pWorker = new ProducerSendMessageWorker(callback,
                                                                               this,
                                                                               msg);
            pWorker->Queue();
        }

        return info.Env().Undefined();
    }
}