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
#ifndef __ROCKETMQ_PRODUCER_START_OR_SHUTDOWN_H__
#define __ROCKETMQ_PRODUCER_START_OR_SHUTDOWN_H__

#include <napi.h>
#include <CProducer.h>
#include <MQClientException.h>

using namespace std;
namespace __node_rocketmq__ {

    enum ProducerWorkerType {
        START_PRODUCER = 0,
        SHUTDOWN_PRODUCER
    };

    class ProducerStartOrShutdownWorker : public Napi::AsyncWorker {
    public:
        ProducerStartOrShutdownWorker(Napi::Function &callback, CProducer *producer_ptr, ProducerWorkerType type) :
                AsyncWorker(callback),
                producer_ptr(producer_ptr),
                ret(0),
                type(type) {
        }

        ~ProducerStartOrShutdownWorker() {
        }

        void Execute() {
            try {
                switch (type) {
                    case START_PRODUCER:
                        ret = StartProducer(this->producer_ptr);
                        break;
                    case SHUTDOWN_PRODUCER:
                        ret = ShutdownProducer(this->producer_ptr);
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
            Callback().Call({
                                    Env().Undefined(),
                                    Napi::Number::New(Env(), (int) ret)
                            });
        }

    private:
        CProducer *producer_ptr;
        int ret;
        ProducerWorkerType type;
    };

}
#endif
