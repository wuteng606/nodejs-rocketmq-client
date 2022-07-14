//
// Created by 武腾 on 2022/7/7.
//

#include "push_consumer.h"
#include "map"
#include <iostream>


namespace __node_rocketmq__ {
    using namespace std;

    map<CPushConsumer *, RocketMQPushConsumer *> _push_consumer_map;

    HandleMessageWorker *hmw;

    struct MessageHandlerParam {
        RocketMQPushConsumer *consumer;
        ConsumerAckInner *ack;
        CMessageExt *msg;
    };

    char message_handler_param_keys[5][8] = {"topic", "tags", "keys", "body", "msgId"};

    uv_mutex_t _get_msg_ext_column_lock;
    Napi::FunctionReference RocketMQPushConsumer::constructor;


    RocketMQPushConsumer::RocketMQPushConsumer(const Napi::CallbackInfo &info) : Napi::ObjectWrap<RocketMQPushConsumer>(
            info) {
        string instance_name = info[1].ToString().Utf8Value();
        Napi::Object options = info[2].ToObject();

        consumer_ptr = CreatePushConsumer(info[0].ToString().Utf8Value().data());
        if (instance_name.data()) {
            SetPushConsumerInstanceName(this->GetConsumer(), instance_name.data());
        }
        _push_consumer_map[this->GetConsumer()] = this;
        RegisterMessageCallback(this->GetConsumer(), RocketMQPushConsumer::OnMessage);
        try {
            this->SetOptions(options);
        }
        catch (const runtime_error e) {
            Napi::TypeError::New(info.Env(), e.what()).ThrowAsJavaScriptException();
            return;
        }
        catch (const std::exception &e) {
            Napi::TypeError::New(info.Env(), e.what()).ThrowAsJavaScriptException();
            return;
        }
    }

    RocketMQPushConsumer::~RocketMQPushConsumer() {
        try {
            ShutdownPushConsumer(consumer_ptr);
            auto it = _push_consumer_map.find(consumer_ptr);
            if (it != _push_consumer_map.end()) {
                _push_consumer_map.erase(consumer_ptr);
            }
        }
        catch (...) {
            //
        }

        DestroyPushConsumer(consumer_ptr);
        consumer_ptr = NULL;
    }

    void RocketMQPushConsumer::SetOptions(Napi::Object options) {
        // set name server
        Napi::Value _name_server_v = options.Get("nameServer");

        if (_name_server_v.IsString()) {
            SetPushConsumerNameServerAddress(consumer_ptr, _name_server_v.ToString().Utf8Value().data());
        }

        // set thread count
        Napi::Value _thread_count_v = options.Get("threadCount");

        if (_thread_count_v.IsNumber()) {
            int thread_count = _thread_count_v.ToNumber().Int32Value();
            if (thread_count > 0) {
                SetPushConsumerThreadCount(this->GetConsumer(), thread_count);
            }
        }

        // set message batch max size
        Napi::Value _max_batch_size_v = options.Get("maxBatchSize");

        if (_max_batch_size_v.IsNumber()) {
            int max_batch_size = _max_batch_size_v.ToNumber().Int32Value();
            if (max_batch_size > 0) {
                SetPushConsumerMessageBatchMaxSize(this->GetConsumer(), max_batch_size);
            }
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
        SetPushConsumerLogFileNumAndSize(this->GetConsumer(), file_num, file_size);

        // set log level
        Napi::Value _log_level_v = options.Get("logLevel");

        if (_log_level_v.IsNumber()) {
            int level = _log_level_v.ToNumber().Int32Value();
            SetPushConsumerLogLevel(this->GetConsumer(), (CLogLevel) level);
        }
    }

    Napi::Object RocketMQPushConsumer::Init(Napi::Env env, Napi::Object exports) {
        Napi::Function func = DefineClass(env, "RocketMQPushConsumer", {
                InstanceMethod("start", &RocketMQPushConsumer::Start),
                InstanceMethod("shutdown", &RocketMQPushConsumer::Shutdown),
                InstanceMethod("subscribe", &RocketMQPushConsumer::Subscribe),
                InstanceMethod("setListener", &RocketMQPushConsumer::SetListener),
                InstanceMethod("setSessionCredentials", &RocketMQPushConsumer::SetSessionCredentials),
        });

//        Napi::FunctionReference *constructor = new Napi::FunctionReference();
        constructor = Napi::Persistent(func);
//        env.SetInstanceData(constructor);
        constructor.SuppressDestruct();
        exports.Set("PushConsumer", func);
        return exports;
    }

    Napi::Value RocketMQPushConsumer::Start(const Napi::CallbackInfo &info) {
        Napi::Function callback = info[0].As<Napi::Function>();
        PushConsumerStartOrShutdownWorker *pcsosd = new PushConsumerStartOrShutdownWorker(callback, this->GetConsumer(),
                                                                                          PushConsumerWorkerType::START_PUSH_CONSUMER);
        pcsosd->Queue();
    }

    Napi::Value RocketMQPushConsumer::Shutdown(const Napi::CallbackInfo &info) {
        Napi::Function callback = info[0].As<Napi::Function>();
        PushConsumerStartOrShutdownWorker *pcsosd = new PushConsumerStartOrShutdownWorker(callback, this->GetConsumer(),
                                                                                          PushConsumerWorkerType::SHUTDOWN_PUSH_CONSUMER);
        pcsosd->Queue();
    }

    Napi::Value RocketMQPushConsumer::Subscribe(const Napi::CallbackInfo &info) {
        int ret;
        try {
            ret = ::Subscribe(this->GetConsumer(), info[0].ToString().Utf8Value().data(),
                              info[1].ToString().Utf8Value().data());
        }
        catch (const runtime_error e) {
            Napi::Error::New(info.Env(), e.what()).ThrowAsJavaScriptException();
            return info.Env().Null();
        }
        catch (const std::exception &e) {
            Napi::Error::New(info.Env(), e.what()).ThrowAsJavaScriptException();
            return info.Env().Null();
        }
        return Napi::Number::New(info.Env(), ret);
    }

    Napi::Value RocketMQPushConsumer::SetListener(const Napi::CallbackInfo &info) {
        jsFnRef = Napi::Persistent(info[0].As<Napi::Function>());
        if (!listener_func.IsEmpty()) {
            Reset(listener_func);
        }
        if (info[0].IsFunction()) {
            listener_func = info[0].As<Napi::Function>();
        }
        std::cout << "[sdk] SetListener" << std::endl;
//        hmw = new HandleMessageWorker(this->GetListenFunction());
    }

    Napi::Value RocketMQPushConsumer::SetSessionCredentials(const Napi::CallbackInfo &info) {
        const char *access_key = info[0].ToString().Utf8Value().data();
        const char *secret_key = info[1].ToString().Utf8Value().data();
        const char *ons_channel = info[2].ToString().Utf8Value().data();
        int ret;
        try {
            ret = SetPushConsumerSessionCredentials(this->GetConsumer(), access_key, secret_key, ons_channel);
        }
        catch (const runtime_error e) {
            Napi::Error::New(info.Env(), e.what()).ThrowAsJavaScriptException();
            return info.Env().Null();
        }
        catch (const std::exception &e) {
            Napi::Error::New(info.Env(), e.what()).ThrowAsJavaScriptException();
            return info.Env().Null();
        }
        return Napi::Number::New(info.Env(), ret);
    }


    string RocketMQPushConsumer::GetMessageColumn(char *name, CMessageExt *msg) {
        const char *orig = NULL;

        uv_mutex_lock(&_get_msg_ext_column_lock);
        switch (name[0]) {
            // topic / tags
            case 't':
                orig = name[1] == 'o' ? GetMessageTopic(msg) : GetMessageTags(msg);
                break;

                // keys
            case 'k':
                orig = GetMessageKeys(msg);
                break;

                // body
            case 'b':
                orig = GetMessageBody(msg);
                break;

                // msgId
            case 'm':
                orig = GetMessageId(msg);
                break;

            default:
                orig = NULL;
                break;
        }

        uv_mutex_unlock(&_get_msg_ext_column_lock);

        if (!orig) return "";
        return orig;
    }

    void close_async_done(uv_handle_t *handle) {
        free(handle);
    }

    void RocketMQPushConsumer::Test(Napi::Env env, ConsumerAckInner *ack_inner, CMessageExt *msg) {
        Napi::HandleScope scope(env);
        Napi::Object ack_obj = ConsumerAck::NewInstance();
        std::cout << "[RocketMQPushConsumer Test] 1" << std::endl;
        ConsumerAck *ack = Napi::ObjectWrap<ConsumerAck>::Unwrap(ack_obj);
        Napi::Object result = Napi::Object::New(env);
        for (int i = 0; i < 5; i++) {
            result.Set(Napi::String::New(Env(), message_handler_param_keys[i]),
                       Napi::String::New(Env(), HandleMessageWorker::GetMessageColumn(message_handler_param_keys[i],
                                                                                      msg)));
        }
        std::cout << "[RocketMQPushConsumer Test] 2" << std::endl;
        ack->SetInner(ack_inner);
        jsFnRef.Call({env.Undefined(), result, ack_obj});

//        hmw = new HandleMessageWorker(jsFn);

//        jsFnRef.Call({env.Undefined(), result, ack_obj});
//        this->GetListenFunction().Call({env.Undefined(), result, ack_obj});
    }

    void RocketMQPushConsumer::HandleMessageInEventLoop(uv_async_t *async) {
        std::cout << "[sdk] HandleMessageInEventLoop 1" << std::endl;
        MessageHandlerParam *param = (MessageHandlerParam *) (async->data);
        RocketMQPushConsumer *consumer = param->consumer;
        ConsumerAckInner *ack_inner = param->ack;
        CMessageExt *msg = param->msg;
        consumer->Test(consumer->Env(), ack_inner, msg);
//        consumer->GetListenFunction().Call({consumer->Env().Undefined()});
//        hmw->SetMessageParam(ack_inner, msg);
//        hmw->Queue();
        uv_close((uv_handle_t *) async, close_async_done);
    }

    int RocketMQPushConsumer::OnMessage(CPushConsumer *consumer_ptr, CMessageExt *msg_ext) {
        std::cout << "[sdk] OnMessage :" << GetMessageBody(msg_ext) << std::endl;

        RocketMQPushConsumer *consumer = _push_consumer_map[consumer_ptr];
        if (!consumer) {
            // TODO: error handle
            return CConsumeStatus::E_RECONSUME_LATER;
        }

        ConsumerAckInner ack_inner;

        // create async parameter
        MessageHandlerParam param;
        param.consumer = consumer;
        param.ack = &ack_inner;
        param.msg = msg_ext;

        // create a new async handler and bind with `RocketMQPushConsumer::HandleMessageInEventLoop`
        uv_async_t *async = (uv_async_t *) malloc(sizeof(uv_async_t));
        uv_async_init(uv_default_loop(), async, RocketMQPushConsumer::HandleMessageInEventLoop);
        async->data = (void *) (&param);

        // send async handler
        uv_async_send(async);

        // wait for result
        CConsumeStatus status = ack_inner.WaitResult();
        std::cout << "[sdk] CConsumeStatus status" << status << std::endl;
        return status;
    }
}