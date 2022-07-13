const rocketmq = require("../index");

const nameServer =
    "rocketmq1-dev.yzw.cn:9876;rocketmq2-dev.yzw.cn:9876;rocketmq3-dev.yzw.cn:9876";

async function main() {
    try {

        const consumer = new rocketmq.PushConsumer(
            "Tower-Dev-Ops-Group",
            "Tower-Dev-Ops-Producer",
            {
                nameServer,
            });
        const end = consumer.subscribe("qa_tower_devops", "*");
        await consumer.start();
        consumer.on("message", function () {
            console.log(arguments);
        })

    } catch (e) {
        console.log(e)
    }
}

main();
