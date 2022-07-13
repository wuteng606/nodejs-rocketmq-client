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
        console.log("consumer", consumer);
        const end = consumer.subscribe("qa_tower_devops", "*");
        console.log("consumer subscribe ", end);
        await consumer.start();
        consumer.on("message", (msg, ack) => {
            console.log(msg);
            ack.done();
        })
        console.log("consumer", consumer);

    } catch (e) {
        console.log(e)
    }
}

main();
