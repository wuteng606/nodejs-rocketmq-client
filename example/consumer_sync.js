const rocketmq = require("../index");

const nameServer =
    "";

async function main() {
    try {

        const consumer = new rocketmq.PushConsumer(
            "group_name",
            "instance_name",
            {
                nameServer,
            });
        const end = consumer.subscribe("topic_name", "*");
        await consumer.start();
        consumer.on("message", function () {
            console.log("message", arguments);
            let obj = arguments[2];
            obj.done();
        })


    } catch (e) {
        console.log(e)
    }
}

main();
