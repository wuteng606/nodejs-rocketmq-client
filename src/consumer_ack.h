//
// Created by 武腾 on 2022/7/7.
//

#include "consumer_ack_inner.h"
#include "napi.h"

namespace __node_rocketmq__ {

    class ConsumerAck : public Napi::ObjectWrap<ConsumerAck> {
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);
        static Napi::Object NewInstance();

        ConsumerAck(const Napi::CallbackInfo &info);

        ~ConsumerAck();

    private:
        static Napi::FunctionReference constructor;

        Napi::Value Done(const Napi::CallbackInfo &info);

        void Ack(CConsumeStatus status);

    public:
        void SetInner(ConsumerAckInner* _inner){
            inner = _inner;
        }

    private:
        ConsumerAckInner* inner;

    };

}