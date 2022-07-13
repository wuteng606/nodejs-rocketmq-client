const rocketmq = require("../index.js");

const nameServer = "rocketmq1-dev.yzw.cn:9876;rocketmq2-dev.yzw.cn:9876;rocketmq3-dev.yzw.cn:9876"

async function main() {
    // setTimeout(() => {
    //     console.log("123")
    // }, 1000)

    try {
        const producer = new rocketmq.Producer(
            "Tower-Dev-Ops-Group",
            "Tower-Dev-Ops-Producer",
            {
                nameServer,
            });
        await producer.start();
        console.log("等待发送消息");

        try {
            console.log("开始发送消息");
            const ret = await producer.send("qa_tower_devops", `{"devLang":"nodejs","mid":198,"appName":"test-rocketmq-client","appCode":"test-rocketmq-client","appType":"sdk","projectCode":"ttttttt","appLeader":"star.x.lan,wuteng","remarks":"test-rocketmq-client","gitAddr":"git@gitlab.yzw.cn:mobile/tower/tower-api.git","createUser":"star.x.lan"}`, {
                keys: "test-rocketmq-client",
                tags: "updateProject",
            });
            console.log("消息结果 ret", ret);
        } catch (e) {
            console.log(e);
        }

    } catch (e) {
        console.log(e)
        // process.exit(4);
    }
}


main();

// ldap_rocketmq_server=
// ldap_rocketmq_topic=qa_tower_devops

