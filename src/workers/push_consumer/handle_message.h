//
// Created by 武腾 on 2022/7/13.
//

#ifndef ROCKETMQ_HANDLE_MESSAGE_H
#define ROCKETMQ_HANDLE_MESSAGE_H

#include <napi.h>

using namespace std;
namespace __node_rocketmq__ {
    class HandleMessageWorker : public Napi::AsyncWorker {
    public:
        HandleMessageWorker(Napi::Function &callback) : AsyncWorker(callback) {}

        ~HandleMessageWorker() {}

        void Execute() {
        }

        void OnOK() {
            Callback().Call({
                                    Env().Undefined(),
                                    Napi::String::New(Env(), "hello world")
                            });
        }
    };

}

#endif //ROCKETMQ_HANDLE_MESSAGE_H
