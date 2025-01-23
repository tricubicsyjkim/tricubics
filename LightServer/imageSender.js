/*
 * @Author: your Name
 * @Date: 2021-11-15 15:29:05
 * @LastEditors: trc-turing gg.jiang@tricubics.com
 * @LastEditTime: 2023-04-19 11:44:47
 * @Description:
 */
var fs = require("fs");
var path = require("path");
var dateFormat = require("dateformat");
var EventEmitter = require("events").EventEmitter;
var imageSenderEmitter = new EventEmitter();
var logger = require("./logger");
let {
  // aiHost,
  aiTopPort,
  top_upload_start,
  top_upload_end,
  compress_start,
  compress_end,
  side_upload_start,
  side_upload_end,
  door_closed,
  top_done,
  get_decision,
} = require("./config");
const { deviceID, aiHost } = JSON.parse(fs.readFileSync("deviceInfo.json"));
const WebSocket = require("ws");
let products = [];
let infos = {};
let topviewDoneData = {};
imageSenderEmitter.on("sendImage", (data) => {
  logger.log("sendImage: ", data);
  if (data.user_id) {
    if (!infos[data.user_id]) {
      infos[data.user_id] = {};
      infos[data.user_id].uploadFileQueue = [];
      infos[data.user_id].isBusy = false;
      infos[data.user_id].isDone = false;
    }
    infos[data.user_id].uploadFileQueue.push(data);
  }
  setTimeout(() => {
    if (!infos[data.user_id].isBusy) {
      infos[data.user_id].isBusy = true;
      logger.log("is no busy, start upload topview image");
      if (infos[data.user_id].uploadFileQueue.length > 0) {
        sendImageHandler(infos[data.user_id].uploadFileQueue.shift());
      }
    } else {
      logger.log("is busy, push request to queue");
    }
  }, 0);
});

imageSenderEmitter.on("topviewDone", (data) => {
  logger.log("topviewDone: ", data);
  if (!infos[data.token]) {
    logger.log("there is no top image");
    return;
  }

  if (infos[data.token].uploadFileQueue.length == 0) {
    logger.log("sent all top images at first: ", data);
    topviewDoneHandler(data);
  } else {
    infos[data.token].isDone = true;
    topviewDoneData[data.token] = data;
  }
});

imageSenderEmitter.on("queryProduct", (data) => {
  products = data;
});

function sendImageHandler(data) {
  let { event_type, path, user_id } = data;
  let fileName = path.split("/").pop();
  const paymentUploadWS = new WebSocket(
    `ws://${aiHost}:${aiTopPort}/upload/${deviceID}/${fileName}`
  );

  setTimeout(() => {
    logger.log("readyState in timeout: ", paymentUploadWS.readyState);
    logger.log("data: ", data);
    if (paymentUploadWS.readyState == 0) {
      infos[user_id].uploadFileQueue.unshift(data);
      imageSenderEmitter.emit("uploadNextFile", { token: user_id });
    }
  }, 1000);

  paymentUploadWS.on("open", function open() {
    top_upload_start = performance.now();
    const websocketStream = WebSocket.createWebSocketStream(paymentUploadWS, {
      decodeStrings: false,
    });
    fs.stat(`${path}`, function (err, stats) {
      websocketStream.write(
        JSON.stringify({
          type: event_type,
          deviceID,
          token: user_id,
          path,
          size: stats.size,
        })
      );
      const fileStream = fs.createReadStream(`${path}`);
      fileStream.pipe(websocketStream, { end: false });
    });
  });

  paymentUploadWS.on("message", function incoming(data) {
    logger.log("from server: ", data);
    let result = JSON.parse(data);
    logger.log("upload ws: ", result);
    if (result.resultCode === "0000") {
      logger.log("upload file successed");
      imageSenderEmitter.emit("uploadNextFile", result);
      paymentUploadWS.close();
    } else {
      logger.log("upload file faild");
      /* upload file again */
      sendImageHandler(result);
    }
  });

  paymentUploadWS.on("close", function close(data) {
    logger.log("disconnected: ", data);
    logger.log(typeof data);
    if (data == 1006) {
      imageSenderEmitter.emit("uploadNextFile", { token: user_id });
      logger.log("readyState close: ", paymentUploadWS.readyState);
    }
    paymentUploadWS.close();
  });

  paymentUploadWS.on("error", (data) => {
    logger.log("error: ", data);
  });
}

function topviewDoneHandler(data) {
  const paymentUploadWS = new WebSocket(
    `ws://${aiHost}:${aiTopPort}/upload/${deviceID}/topviewDone`
  );
  let { type, token, paymentAt, weightInfo, changedWeightInfo } = data;
  paymentUploadWS.on("open", function open() {
    logger.log("connected upload server");
    logger.log("check topviewDone data: ", data);
    let request = {
      type,
      token,
      deviceID,
      paymentAt,
      weightInfo,
      changedWeightInfo,
      products,
    };
    paymentUploadWS.send(JSON.stringify(request));
  });

  paymentUploadWS.on("message", function incoming(data) {
    logger.log("from server: ", data);
    top_done = performance.now();
    logger.log("topview done: " + (top_done - door_closed));
  });

  paymentUploadWS.on("close", function close(data) {
    logger.log("disconnected: ", data);
  });
  paymentUploadWS.on("error", (data) => {
    logger.log("error: ", data);
  });
}

imageSenderEmitter.on("uploadNextFile", (data) => {
  if (infos[data.token] && infos[data.token].uploadFileQueue) {
    uploadNextFileHandler(data);
  } else {
    logger.log("uploadFileQueue is empty, cannot uploadNext: ", data);
  }
});

function uploadNextFileHandler(data) {
  let length = infos[data.token].uploadFileQueue.length;
  logger.log("uploadNextFileHandler: ", data.token);
  if (length > 0) {
    setTimeout(() => {
      sendImageHandler(infos[data.token].uploadFileQueue.shift());
    }, 0);
  } else {
    infos[data.token].isBusy = false;
    logger.log("uploadfile queue is empty: ", data.token);
    logger.log("check isDone: ", infos[data.token].isDone);
    if (infos[data.token].isDone) {
      topviewDoneHandler(topviewDoneData[data.token]);
      delete infos[data.token];
      delete topviewDoneData[data.token];
    }
  }
}

imageSenderEmitter.on("clearQueue", (data) => {
  let token = data.token || data.user_id;
  if (token && infos[token]) {
    if (infos[token].uploadFileQueue) {
      infos[token].uploadFileQueue = [];
      logger.log("clear top image queue");
    }
  }
});

module.exports = { imageSenderEmitter };
