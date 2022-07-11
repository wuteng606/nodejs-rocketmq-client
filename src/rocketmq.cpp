//
// Created by 武腾 on 2022/7/7.
//

#include "rocketmq.h"
#include "iostream"

using namespace Napi;
namespace __node_rocketmq__ {

#if defined(__APPLE__)
    uv_lib_t lib;

    void DLOpen(const Napi::CallbackInfo &info) {
        std::string filename = info[0].ToString().Utf8Value();
        uv_dlopen(filename.c_str(), &lib);
    }

#endif

    Object InitAll(Env env, Object exports) {
        RocketMQProducer::Init(env, exports);
        ConsumerAck::Init(env, exports);
        RocketMQPushConsumer::Init(env, exports);
#if defined(__APPLE__)
        exports.Set("macosDLOpen", Napi::Function::New(env, DLOpen));
#endif
        return exports;
    }


    NODE_API_MODULE(rocketmq, InitAll);
}