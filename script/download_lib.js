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
"use strict";

const fs = require("fs");
const os = require("os");
const path = require("path");

const destroy = require("destroy");
const urllib = require("urllib");

const getLinuxDistroRoute = require("./get_linux_distro_route");
const pkg = require("../package");

let REGISTRY_MIRROR =
    process.env.NODE_ROCKETMQ_REGISTRY ||
    "https://opensource-rocketmq-client.oss-cn-hangzhou.aliyuncs.com";
if (!REGISTRY_MIRROR.endsWith("/")) REGISTRY_MIRROR += "/";

const CPP_SDK_VERSION = pkg.cppSDKVersion;
const LIB_DIR = path.join(__dirname, "..", "deps", "lib");
const URL_ROOT = `${REGISTRY_MIRROR}cpp-client`;

async function getUrlArray() {
    const platform = os.platform();
    const ret = [];
    let distro;

    switch (platform) {
        case "win32":
            ret.push(
                `${URL_ROOT}/windows/${CPP_SDK_VERSION}/rocketmq-client-cpp.dll`
            );
            ret.push(
                `${URL_ROOT}/windows/${CPP_SDK_VERSION}/rocketmq-client-cpp.lib`
            );
            break;

        case "darwin":
            ret.push(`${URL_ROOT}/mac/${CPP_SDK_VERSION}/librocketmq.dylib`);
            // ret.push(`${URL_ROOT}/mac/${CPP_SDK_VERSION}/librocketmq.a`);
            break;

        case "linux":
            distro = await getLinuxDistroRoute();
            ret.push(
                `${URL_ROOT}/linux/${CPP_SDK_VERSION}/${distro}/librocketmq.a`
            );
            break;

        default:
            throw new Error(`Unsupported platform ${platform}`);
    }

    return ret;
}

async function mkdir(dir) {
    try {
        const stat = await fs.promises.stat(dir);
        if (stat.isDirectory()) {
            const files = await fs.promises.readdir(dir);
            if (files.length > 0) {
                throw new Error("已下载文件，无需重复下载");
            }
        } else {
            await fs.promises.mkdir(dir);
        }
    } catch (error) {
        await fs.promises.mkdir(dir);
    }
}

async function main() {
    let urls;
    try {
        urls = await getUrlArray();
    } catch (e) {
        console.log(e);
        console.error(`[rocketmq sdk] [error] ${e.message}`);
        process.exit(4);
    }
    try {
        await mkdir(LIB_DIR);
    } catch (e) {
        console.log(e);
        console.error(`[rocketmq sdk] [error] ${e.message}`);
        process.exit(0);
    }
    let writeTimes = 0;
    for (const url of urls) {
        console.log(`[rocketmq sdk] [info] downloading [${url}]...`);

        const resp = await urllib.request(url, {
            timeout: 60000 * 5,
            followRedirect: true,
            streaming: true,
        });

        if (resp.status !== 200) {
            destroy(resp.res);
            console.error(
                `[rocketmq sdk] [error] error status ${resp.status} while downloading [${url}].`
            );
            process.exit(4);
        }

        const readStream = resp.res;
        const filename = path.join(LIB_DIR, path.basename(url));
        const writeStream = fs.createWriteStream(filename, {
            encoding: "binary",
        });

        // eslint-disable-next-line
        function handleDownladCallback(err) {
            if (err) {
                console.error(
                    `[rocketmq sdk] [error] error occurred while downloading [${url}] to [${filename}].`
                );
                console.error(err.stack);
                process.exit(4);
            }

            writeTimes++;
            destroy(resp.res);

            console.log(`[rocketmq sdk] [info] downloaded library [${url}].`);
            if (writeTimes === urls.length) {
                console.log(
                    "[rocketmq sdk] [info] all libraries have been written to disk."
                );
                process.exit(0);
            }
        }

        readStream.on("error", handleDownladCallback);
        writeStream.on("error", handleDownladCallback);
        writeStream.on("finish", handleDownladCallback);

        readStream.pipe(writeStream);
    }
}

main();
