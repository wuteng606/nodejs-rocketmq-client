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

    struct MessageHandlerParam {
        ConsumerAckInner *ack;
        CMessageExt *msg;
    };

    class HandleMessageWorker : public Napi::AsyncWorker {
    public:
        HandleMessageWorker(Napi::Function &callback, ConsumerAck *ack, Napi::Object ackObject) : AsyncWorker(callback), ack(ack), ackObject(ackObject) {
        }

        ~HandleMessageWorker() {}

        static string GetMessageColumn(char *name, CMessageExt *msg);

        void SetMessageParam(uv_async_t *async);

        void SetCount(int count) {
            std::cout << "[sdk] SetCount :" << count << std::endl;
        }

        void Execute() {
            std::cout << "[sdk] HandleMessageWorker Execute" << std::endl;
        }

        void OnOK() {
            char message_handler_param_keys[5][8] = {"topic", "tags", "keys", "body", "msgId"};

            std::cout << "[sdk] HandleMessageWorker OnOK abc" << HandleMessageWorker::GetMessageColumn(message_handler_param_keys[0],
                                                                                                       this->msg) << std::endl;
            Napi::Object result = Napi::Object::New(Env());
            for (int i = 0; i < 5; i++) {
                result.Set(Napi::String::New(Env(), message_handler_param_keys[i]),
                           Napi::String::New(Env(), HandleMessageWorker::GetMessageColumn(message_handler_param_keys[i],
                                                                                          this->msg)));
            }
            Callback().Call({Env().Undefined(), result, ackObject});
//            Callback().Call(reinterpret_cast<napi_value>(2), {
//                    result,
//                    reinterpret_cast<Napi::Value &&>(ack)});
        }

    private:
        Napi::Object ackObject;
        ConsumerAck *ack;
        CMessageExt *msg;
    };

}

#endif // ROCKETMQ_HANDLE_MESSAGE_H
