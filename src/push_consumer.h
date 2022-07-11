//
// Created by 武腾 on 2022/7/7.
//

#include <CPushConsumer.h>
#include <uv.h>
#include <napi.h>
#include <string>

namespace __node_rocketmq__ {

    class RocketMQPushConsumer : public Napi::ObjectWrap<RocketMQPushConsumer> {
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);

        static int OnMessage(CPushConsumer *consumer_ptr, CMessageExt *msg_ext);

        static std::string GetMessageColumn(char *name, CMessageExt *msg);

        RocketMQPushConsumer(const Napi::CallbackInfo &info);

        ~RocketMQPushConsumer();

    private:
//        Napi::Value New(const Napi::CallbackInfo &info);
        Napi::Value Start(const Napi::CallbackInfo &info);

        Napi::Value Shutdown(const Napi::CallbackInfo &info);

        Napi::Value Subscribe(const Napi::CallbackInfo &info);

        Napi::Value SetListener(const Napi::CallbackInfo &info);

        Napi::Value SetSessionCredentials(const Napi::CallbackInfo &info);

        void SetOptions(Napi::Object options);

        static void HandleMessageInEventLoop(uv_async_t* async);

    protected:
        CPushConsumer* GetConsumer(){
            return consumer_ptr;
        }

        Napi::Function* GetListenFunction(){
            Napi::Function* cb;
            cb = &listener_func;
            return cb;
        }

    private:
        CPushConsumer* consumer_ptr;
        Napi::Function listener_func;
    };


}

