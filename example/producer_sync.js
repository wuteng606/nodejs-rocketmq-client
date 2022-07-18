const rocketmq = require("../index.js");

const nameServer = ""

async function main() {

    try {
        const producer = new rocketmq.Producer(
            "group_name",
            "instance_name",
            {
                nameServer,
            });
        await producer.start();
        console.log("等待发送消息");

        try {
            console.log("开始发送消息");
            const ret = await producer.send("topic_name", `string 类型的 内容`, {
                keys: "key_name",
                tags: "tag_name",
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

