//
// Created by 武腾 on 2022/7/12.
//


/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include "CCommon.h"
#include "CMessageExt.h"
#include "CPushConsumer.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <memory.h>
#include <unistd.h>
#endif

void thread_sleep(unsigned milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);  // takes microseconds
#endif
}

int doConsumeMessage(struct CPushConsumer* consumer, CMessageExt* msgExt) {
    printf("Hello,doConsumeMessage by Application!\n");
    printf("Msg Topic:%s\n", GetMessageTopic(msgExt));
    printf("Msg Tags:%s\n", GetMessageTags(msgExt));
    printf("Msg Keys:%s\n", GetMessageKeys(msgExt));
    printf("Msg Body:%s\n", GetMessageBody(msgExt));
    return E_CONSUME_SUCCESS;
}

int main(int argc, char* argv[]) {
    int i = 0;
    printf("PushConsumer Initializing....\n");
    CPushConsumer* consumer = CreatePushConsumer("Tower-Dev-Ops-Group");
    SetPushConsumerNameServerAddress(consumer, "rocketmq1-dev.yzw.cn:9876;rocketmq2-dev.yzw.cn:9876;rocketmq3-dev.yzw.cn:9876");
    Subscribe(consumer, "qa_tower_devops", "*");
    RegisterMessageCallback(consumer, doConsumeMessage);
    StartPushConsumer(consumer);
    printf("Push Consumer Start...\n");
    for (i = 0; i < 10; i++) {
        printf("Now Running : %d S\n", i * 10);
        thread_sleep(10000);
    }
    ShutdownPushConsumer(consumer);
    DestroyPushConsumer(consumer);
    printf("PushConsumer Shutdown!\n");
    return 0;
}
