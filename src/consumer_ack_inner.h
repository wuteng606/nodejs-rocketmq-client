//
// Created by 武腾 on 2022/7/7.
//


#include <uv.h>
#include <CPushConsumer.h>

namespace __node_rocketmq__ {

    class ConsumerAckInner {
    public:
        ConsumerAckInner();

        ~ConsumerAckInner();

        void Ack(CConsumeStatus _status);

        CConsumeStatus WaitResult();

    private:
        bool acked;
        CConsumeStatus status;
        uv_mutex_t mutex;
        uv_cond_t cond;
    };

}

