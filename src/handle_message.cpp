//
// Created by 武腾 on 2022/7/13.
//

#include "handle_message.h"

namespace __node_rocketmq__ {

    char message_handler_param_keys[5][8] = {"topic", "tags", "keys", "body", "msgId"};

    void HandleMessageWorker::SetMessageParam(CMessageExt *msg) {
//        CMessageExt *msg = param->msg;
//        ConsumerAck *ack = Napi::ObjectWrap<ConsumerAck>::Unwrap({});
//        ack->SetInner(ack_inner);
        std::cout << "[sdk] HandleMessageWorker SetMessageParam" << GetMessageBody(msg) << std::endl;

    }

}
