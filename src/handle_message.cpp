#include "handle_message.h"

namespace __node_rocketmq__ {

    uv_mutex_t _get_msg_ext_column_lock;

    void HandleMessageWorker::SetMessageParam(uv_async_t *async) {
        MessageHandlerParam *param = (MessageHandlerParam *) (async->data);
        ConsumerAckInner *ack_inner = param->ack;
        CMessageExt *msg = param->msg;
        std::cout << "[sdk] SetMessageParam :" << GetMessageBody(msg) << std::endl;
        this->ack->SetInner(ack_inner);
        this->msg = msg;
    }

    string HandleMessageWorker::GetMessageColumn(char *name, CMessageExt *msg) {
        const char *orig = NULL;

//        uv_mutex_lock(&_get_msg_ext_column_lock);
        switch (name[0]) {
            // topic / tags
            case 't':
                orig = name[1] == 'o' ? GetMessageTopic(msg) : GetMessageTags(msg);
                break;

                // keys
            case 'k':
                orig = GetMessageKeys(msg);
                break;

                // body
            case 'b':
                orig = GetMessageBody(msg);
                break;

                // msgId
            case 'm':
                orig = GetMessageId(msg);
                break;

            default:
                orig = NULL;
                break;
        }

//        uv_mutex_unlock(&_get_msg_ext_column_lock);

        if (!orig)
            return "";
        return orig;
    }
}