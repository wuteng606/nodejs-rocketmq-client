//
// Created by 武腾 on 2022/7/7.
//

#include "consumer_ack.h"
#include <iostream>

namespace __node_rocketmq__ {

    Napi::FunctionReference ConsumerAck::constructor;

    ConsumerAck::ConsumerAck(const Napi::CallbackInfo &info)
            : Napi::ObjectWrap<ConsumerAck>(info) {
    }

    ConsumerAck::~ConsumerAck() noexcept {
        this->inner = NULL;
    }

    Napi::Object ConsumerAck::Init(Napi::Env env, Napi::Object exports) {
        Napi::Function func = DefineClass(
                env,
                "ConsumerAck",
                {
                        InstanceMethod("done", &ConsumerAck::Done)
                }
        );
        constructor = Napi::Persistent(func);
        constructor.SuppressDestruct();
        exports.Set("ConsumerAck", func);
        return exports;
    }

    Napi::Value ConsumerAck::Done(const Napi::CallbackInfo &info) {
        bool succ = true;
        if (info.Length() >= 1) {
            succ = (info[0].IsUndefined() || info[0].IsNull() || info[0].ToBoolean());
        }
        CConsumeStatus status = succ ? CConsumeStatus::E_CONSUME_SUCCESS : CConsumeStatus::E_RECONSUME_LATER;
        this->Ack(status);
    }

    void ConsumerAck::Ack(CConsumeStatus status) {
        if (this->inner) {
            this->inner->Ack(status);
            this->inner = NULL;
        }
    }


    Napi::Object ConsumerAck::NewInstance() {
        Napi::Object obj = constructor.New({});
        return obj;
    }


}