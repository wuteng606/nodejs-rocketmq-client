//
// Created by 武腾 on 2022/7/13.
//

#ifndef ROCKETMQ_HANDLE_MESSAGE_H
#define ROCKETMQ_HANDLE_MESSAGE_H

#include <napi.h>
#include <iostream>
#include "consumer_ack.h"

using namespace std;
namespace __node_rocketmq__ {

    struct MessageHandlerParam {
        ConsumerAckInner *ack;
        CMessageExt *msg;
    };

    class HandleMessageWorker : public Napi::AsyncWorker {
    public:
        HandleMessageWorker(Napi::Function &callback) : AsyncWorker(callback) {}

        ~HandleMessageWorker() {}

        void SetMessageParam(CMessageExt *msg);


        void Execute() {
            std::cout << "[sdk] HandleMessageWorker Execute" << std::endl;
        }

        void OnOK() {
            std::cout << "[sdk] HandleMessageWorker OnOK" << std::endl;
            Napi::Object abc = Napi::Object::New(Env());
            abc.Set("a", Napi::String::New(Env(), "123"));
//            ConsumerAck *ack = Napi::ObjectWrap<ConsumerAck>::Unwrap({});
//            ack->SetInner(ack_inner);
//            abc.Set("done", )
            std::cout << "[sdk] HandleMessageWorker OnOK abc" << abc.Get("a").ToString().Utf8Value() << std::endl;

            Callback().Call({
                                    Env().Undefined(),
                                    Napi::String::New(Env(), "hello world"),
                                    abc
                            });
        }


    };

}

#endif //ROCKETMQ_HANDLE_MESSAGE_H
