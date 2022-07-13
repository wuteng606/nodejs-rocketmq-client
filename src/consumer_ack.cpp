//
// Created by 武腾 on 2022/7/7.
//

#include "consumer_ack.h"


namespace __node_rocketmq__ {

    ConsumerAck::ConsumerAck(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ConsumerAck>(info) {
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

        Napi::FunctionReference *constructor = new Napi::FunctionReference();
        *constructor = Napi::Persistent(func);
        env.SetInstanceData(constructor);
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


    Napi::Object ConsumerAck::NewInstance(Napi::Env env, Napi::Value arg) {
        Napi::EscapableHandleScope scope(env);
        Napi::Object obj = env.GetInstanceData<Napi::FunctionReference>()->New({arg});
        return scope.Escape(napi_value(obj)).ToObject();
    }


}