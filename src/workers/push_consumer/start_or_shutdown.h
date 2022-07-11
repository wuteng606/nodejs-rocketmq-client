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
#ifndef __ROCKETMQ_PUSH_CONSUMER_START_OR_SHUTDOWN_H__
#define __ROCKETMQ_PUSH_CONSUMER_START_OR_SHUTDOWN_H__

#include <napi.h>
#include <CPushConsumer.h>
#include <MQClientException.h>

using namespace std;
namespace __node_rocketmq__ {

    enum PushConsumerWorkerType {
        START_PUSH_CONSUMER = 0,
        SHUTDOWN_PUSH_CONSUMER
    };

    class PushConsumerStartOrShutdownWorker : public Napi::AsyncWorker {
    public:
        PushConsumerStartOrShutdownWorker(Napi::Function& callback, CPushConsumer *consumer_ptr,
                                          PushConsumerWorkerType type)
                :
                AsyncWorker(callback),
                consumer_ptr(consumer_ptr),
                ret(0),
                type(type) {
        }

        ~PushConsumerStartOrShutdownWorker() {
        }

        void Execute() {
            try {
                switch (type) {
                    case START_PUSH_CONSUMER:
                        ret = StartPushConsumer(consumer_ptr);
                        break;
                    case SHUTDOWN_PUSH_CONSUMER:
                        ret = ShutdownPushConsumer(consumer_ptr);
                        break;
                    default:
                        break;
                }
            }
            catch (const runtime_error e) {
                SetError(e.what());
            }
            catch (const exception &e) {
                SetError(e.what());
            }
        }

        void OnOK() {
//            Napi::HandleScope scope(Env());
            Callback().Call({
                                    Env().Undefined(),
                                    Napi::Number::New(Env(), (int) ret)
                            });
        }

    private:
        CPushConsumer *consumer_ptr;
        int ret;
        PushConsumerWorkerType type;
    };

}
#endif
