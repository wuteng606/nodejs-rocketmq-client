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
#ifndef __ROCKETMQ_SEND_MESSAGE_H__
#define __ROCKETMQ_SEND_MESSAGE_H__

#include <napi.h>
#include <CProducer.h>
#include <MQClientException.h>
#include <iostream>


using namespace std;
namespace __node_rocketmq__ {

    class ProducerSendMessageWorker : public Napi::AsyncWorker {
    public:
        ProducerSendMessageWorker(Napi::Function &callback, RocketMQProducer *producer, CMessage *msg) :
                AsyncWorker(callback),
                msg(msg),
                producer(producer) {
        }

        ~ProducerSendMessageWorker() {
            DestroyMessage(msg);
        }

        void Execute() {
            try {
                int status = SendMessageSync(producer->GetProducer(), msg, &send_ret);
            }
            catch (const std::runtime_error e) {
                SetError(e.what());
            }
            catch (const std::exception &e) {
                SetError(e.what());
            }
        }

        void OnOK() {
            std::cout << "offset:" << send_ret.offset << "\nsendStatus:" << send_ret.sendStatus << "\noffset:"
                      << send_ret.offset << std::endl;
            Callback().Call({
                                    Env().Undefined(),
                                    Napi::Number::New(Env(), (unsigned int) send_ret.sendStatus),
                                    Napi::String::New(Env(), send_ret.msgId),
                                    Napi::Number::New(Env(), (long long) send_ret.offset)
                            });
        }


    private:
        CMessage *msg;
        RocketMQProducer *producer;

        CSendResult send_ret;
    };
}

#endif
