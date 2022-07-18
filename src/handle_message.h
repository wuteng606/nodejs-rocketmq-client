//
// Created by 武腾 on 2022/7/13.
//

#ifndef ROCKETMQ_HANDLE_MESSAGE_H
#define ROCKETMQ_HANDLE_MESSAGE_H

#include <napi.h>
#include <iostream>
#include <uv.h>
#include <CPushConsumer.h>


using namespace std;
namespace __node_rocketmq__
{

    class HandleMessage
    {
    public:
        HandleMessage()
        {
        }

        ~HandleMessage() {}

        static string GetMessageColumn(char *name, CMessageExt *msg);

    private:
    };

}

#endif // ROCKETMQ_HANDLE_MESSAGE_H
