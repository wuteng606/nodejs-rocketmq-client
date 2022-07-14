//
// Created by 武腾 on 2022/7/13.
//

#ifndef ROCKETMQ_HANDLE_MESSAGE_H
#define ROCKETMQ_HANDLE_MESSAGE_H

#include <napi.h>
#include <iostream>
#include "consumer_ack.h"
#include <uv.h>

using namespace std;
namespace __node_rocketmq__ {



    class HandleMessageWorker : public Napi::AsyncWorker {
    public:
        HandleMessageWorker(Napi::Function &callback) : AsyncWorker(callback) {
        }

        ~HandleMessageWorker() {}

        static string GetMessageColumn(char *name, CMessageExt *msg);

        void SetMessageParam(ConsumerAckInner *ack_inner, CMessageExt *msg);

        void SetCount(int count) {
            std::cout << "[sdk] SetCount :" << count << std::endl;
        }

        void Execute() {
            std::cout << "[sdk] HandleMessageWorker Execute" << std::endl;
        }

        void OnOK() {
            std::cout << "[sdk] OnOK" << std::endl;
//            char message_handler_param_keys[5][8] = {"topic", "tags", "keys", "body", "msgId"};
//
//            std::cout << "[sdk] HandleMessageWorker OnOK abc :"
//                      << HandleMessageWorker::GetMessageColumn(message_handler_param_keys[0],
//                                                               this->msg) << std::endl;
//            Napi::Object result = Napi::Object::New(Env());
//            for (int i = 0; i < 5; i++) {
//                result.Set(Napi::String::New(Env(), message_handler_param_keys[i]),
//                           Napi::String::New(Env(), HandleMessageWorker::GetMessageColumn(message_handler_param_keys[i],
//                                                                                          this->msg)));
//            }
//            ack = ConsumerAck::NewInstance();
//            ConsumerAck *tempAck = Napi::ObjectWrap<ConsumerAck>::Unwrap(ack);
//            tempAck->SetInner(this->ack_inner);
//            Callback().Call({Env().Undefined(), result, ack});
        }

    private:
        Napi::Object ack;
        ConsumerAckInner *ack_inner;
        CMessageExt *msg;
    };

}

#endif // ROCKETMQ_HANDLE_MESSAGE_H
