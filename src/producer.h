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
#include <CProducer.h>
#include <napi.h>

namespace __node_rocketmq__ {

    class RocketMQProducer : public Napi::ObjectWrap<RocketMQProducer> {
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);

        RocketMQProducer(const Napi::CallbackInfo &info);

        ~RocketMQProducer();

    public:
        CProducer *GetProducer() { return producer_ptr; }

    private:

//    Napi::Value New(const Napi::CallbackInfo &info);

        Napi::Value Start(const Napi::CallbackInfo &info);

        Napi::Value Shutdown(const Napi::CallbackInfo &info);

        Napi::Value Send(const Napi::CallbackInfo &info);

        Napi::Value SetSessionCredentials(const Napi::CallbackInfo &info);


        static Napi::FunctionReference constructor;

    private:
        void SetOptions(Napi::Object options);

    private:
        CProducer *producer_ptr;
    };

}
