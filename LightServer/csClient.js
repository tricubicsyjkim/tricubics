const WebSocket = require("ws");
var fs = require("fs");
var path = require("path");
var archiver = require("archiver");
var dateFormat = require("dateformat");
var moment = require("moment");
var EventEmitter = require("events").EventEmitter;
var csClientEmitter = new EventEmitter();
var logger = require("./logger");
const {
  cpaymentProtocol,
  csHost,
  cpaymentPort,
  cpaymentPath,
  timesLimit,
} = require("./config");
const { deviceID, vendor } = JSON.parse(fs.readFileSync("deviceInfo.json"));
let cpaymentWS, paymentID;
let clientInterval;
let errorTimes = 1;
let tasks = [];
let uploadFileQueue = [];
const { performance } = require("perf_hooks");
let t0;
let t1;
let t2;
let isBusy = false;
let compressDoneMap = {};
let decisionDoneMap = {};
let isStartUpload = false;
let retrySetTimeout;

connect();

function connect() {
  cpaymentWS = new WebSocket(
    `${cpaymentProtocol}://${csHost}:${cpaymentPort}${cpaymentPath}${deviceID}`
  );
  //`wss://${csHost}:${cpaymentPort}${cpaymentPath}${deviceID}`
  logger.log(
    "ws server: ",
    `${cpaymentProtocol}://${csHost}:${cpaymentPort}${cpaymentPath}${deviceID}`
  );

  cpaymentWS.on("open", function open() {
    loginRequest();
  });

  cpaymentWS.on("message", function incoming(data) {
    logger.log("from cs server: ", data);
    let response = JSON.parse(data);
    let type = response.type;
    logger.log(type);
    if (cpaymentWSHandler[type]) {
      cpaymentWSHandler[type](response);
    }
  });

  cpaymentWS.on("close", function (e) {
    logger.log(
      "Socket is closed. Reconnect will be attempted in 1 second.",
      e.reason
    );
    setTimeout(function () {
      connect();
    }, 1000);
  });

  cpaymentWS.on("error", function (err) {
    logger.error("Socket encountered error: ", err.message, "Closing socket");
    cpaymentWS.close();
  });
}

function loginRequest() {
  let loginRequest = { deviceID, type: "login", token: "" };
  cpaymentWS.send(JSON.stringify(loginRequest));
}

let cpaymentWSHandler = {
  login: (data) => {
    loginHandler(data);
  },
  payment: (data) => {
    paymentHandler(data);
  },
  bill: (data) => {
    billHandler(data);
  },
  billCancel: (data) => {
    billCancelHandler(data);
  },
  rebill: (data) => {
    rebillHandler(data);
  },
  deviceOpen: (data) => {
    deviceOpenHandler(data);
  },
  unpurchased: (data) => {
    unpurchasedHandler(data);
  },
  nobill: (data) => {
    nobillHandler(data);
  },
};

function loginHandler(data) {
  logger.log(data);
}

function paymentHandler(data) {
  logger.log(data, data.paymentID, data.dataURL);
  if (data.tokenID) {
    tasks = tasks.filter(function (item) {
      if (item.data.tokenID == data.tokenID) {
        resetInterval(item.interval);
        item.paymentID = data.paymentID;
        item.dataURL = data.dataURL;
        const clone = JSON.parse(JSON.stringify(item));
        csClientEmitter.emit("uploadFile", clone);
      }
      return item.originalData.token == data.tokenID;
    });
  } else {
    logger.log("there is no tokenID filed in response");
  }
}

function billHandler(data) {
  logger.log(data);

  if (data.paymentPrice == 0) {
    let trID = getTransactionID();
    const billResponse = {
      type: "bill",
      resultCode: "0000",
      resultMsg: "create OK",
      trID,
      paymentID: data.paymentID,
      approveResult: 1,
      approvePrice: data.paymentPrice,
    };
    logger.log(billResponse);
    cpaymentWS.send(JSON.stringify(billResponse));
  } else {
    if (vendor == "lotte" || vendor == "japan") {
      let response = makeBillRes(data);
      billResponseHandler(response);
    }
    csClientEmitter.emit("bill", data);
  }
}

function billCancelHandler(data) {
  logger.log("billCancelHandler: ", data);
  csClientEmitter.emit("billCancel", data);
}

function rebillHandler(data) {
  logger.log("rebillHandler: ", data);
  csClientEmitter.emit("rebill", data);
}

function deviceOpenHandler(data) {
  logger.log("deviceOpenHandler: ", data);
  csClientEmitter.emit("deviceOpen", data);
}

function unpurchasedHandler(data) {
  logger.log("unpurchasedHandler: ", data);
  // let trID = getTransactionID();
  // let response = { ...data, resultCode: "0000", resultMsg: "create OK", trID };
  // logger.log("unpurchased response: ", response);
  // cpaymentWS.send(JSON.stringify(response));
  csClientEmitter.emit("unpurchased", data);
}

function nobillHandler(data) {
  logger.log("nobillHandler: ", data);
  let trID = getTransactionID();
  if (data.paymentPrice) delete data.paymentPrice;
  let response = { ...data, resultCode: "0000", resultMsg: "create OK", trID };
  logger.log("nobill response: ", response);
  cpaymentWS.send(JSON.stringify(response));
  csClientEmitter.emit("nobill", data);
}

function cpaymentHandler(csData) {
  let token = csData.data.token || csData.originalData.token;
  if (compressDoneMap[token]) {
    logger.log("compress finished at first, sendCpaymentReq");
    sendCpaymentReq(csData);
    delete compressDoneMap[token];
  } else {
    decisionDoneMap[token] = csData;
    logger.log("decisionDone finished");
  }
}

function billResponseHandler(data) {
  data.trID = getTransactionID();
  logger.log(data);
  cpaymentWS.send(JSON.stringify(data));
}

function billCancelResponseHandler(data) {
  data.trID = getTransactionID();
  logger.log(data);
  cpaymentWS.send(JSON.stringify(data));
}

function rebillResponseHandler(data) {
  data.trID = getTransactionID();
  data.resultCode = "0000";
  data.resultMsg = "create OK";
  logger.log(data);
  cpaymentWS.send(JSON.stringify(data));
}

function rmImgFolderHandler(dir) {
  let pathArray = dir.split("/");
  pathArray.pop();
  dir = pathArray.join("/");
  logger.log("image folder: ", dir);
  //fs.rmSync(dir, { recursive: true });
}

function deviceOpenedHandler(data) {
  logger.log("deviceOpenedHandler: ", data);
  cpaymentWS.send(JSON.stringify(data));
}

function sendCompleteRequest(csData, type) {
  cpaymentWS.send(JSON.stringify(csData.data));
  logger.log("sent complete request: ", csData);
  let interval = setInterval(function () {
    if (errorTimes < 3) {
      errorTimes++;
      cpaymentWS.send(JSON.stringify(csData.data));
      logger.log("start interval: ", errorTimes, " , csData: ", csData);
    } else {
      let obj = { data: csData.data, type: `${type}Error` };
      csClientEmitter.emit(`${type}Error`, obj);
      logger.error(obj);
      resetInterval(csData.interval);
    }
  }, 120000);
  csData.interval = interval;
  tasks.push(csData);
}

function resetInterval(interval) {
  clearInterval(interval);
  errorTimes = 1;
  logger.log("clearInterval and reset times");
}

function paymentFileUpload(req) {
  logger.log(JSON.stringify(req, null, 2));
  let paymentID = req.paymentID;
  let folder_path = req.originalData.folderPath;
  let path_array = folder_path.split(path.sep);
  let fileName = `${path_array.pop()}.zip`;
  logger.log(
    "paymentID: ",
    paymentID,
    "folder_path: ",
    folder_path,
    "fileName: ",
    fileName
  );
  const paymentUploadWS = new WebSocket(req.dataURL);

  paymentUploadWS.on("open", function open() {
    logger.log("connected upload server");
    isStartUpload = true;
    clearTimeout(retrySetTimeout);
    paymentUploadWS.send(JSON.stringify({ type: "binaryStart", fileName }));
    t0 = performance.now();
    const fileStream = fs.createReadStream(`${folder_path}_top.zip`, {
      highWaterMark: 10 * 1024,
    });

    fileStream
      .on("data", async function (chunk) {
        paymentUploadWS.send(chunk);
      })
      .on("end", function () {
        t1 = performance.now();
        logger.log(
          "profiling--- upload file took " + (t1 - t0) + " milliseconds."
        );
        paymentUploadWS.send(JSON.stringify({ type: "binaryEnd" }));
        isStartUpload = false;
      })
      .on("error", function (err) {
        logger.log("read file error: ", err);
      });
  });

  paymentUploadWS.on("message", function incoming(data) {
    logger.log("from server: ", data);
    let result = JSON.parse(data);
    logger.log("upload ws: ", result);
    if (result.resultCode === "0000") {
      logger.log("upload file successed");
      t2 = performance.now();
      logger.log("get response from server: ", t2 - t0);
      csClientEmitter.emit("uploadNextFile");
      csClientEmitter.emit("rmImgFolder", folder_path);
      paymentUploadWS.close();
    } else {
      logger.log("upload file faild");
      /* upload file again */
      paymentFileUpload(req);
    }
  });

  paymentUploadWS.on("close", function close(data) {
    logger.log("disconnected: ", data);
  });
  paymentUploadWS.on("error", (data) => {
    logger.log("error: ", data);
  });
}

function getTransactionID(max) {
  let trID =
    dateFormat(new Date(), "yymmddHHMMss") +
    Math.floor(Math.random() * 99999999);
  return trID;
}

function uploadFileHandler(data) {
  setTimeout(() => {
    if (!isBusy) {
      isBusy = true;
      logger.log("is no busy, start upload file");
      retrySetTimeout = setTimeout(function () {
        if (isStartUpload == false) {
          logger.log("can not connect NAS, retry: ", data);
          // paymentFileUpload(data);
          csClientEmitter.emit("uploadNextFile");
        }
      }, 500);
      paymentFileUpload(data);
    } else {
      uploadFileQueue.push(data);
      logger.log("is busy, push request to queue");
    }
  }, 50);
}

function uploadNextFileHandler() {
  setTimeout(() => {
    let length = uploadFileQueue.length;
    if (length > 0) {
      paymentFileUpload(uploadFileQueue.shift());
    } else {
      isBusy = false;
      logger.log("uploadfile queue is empty");
    }
  }, 300);
}

function compressDoneHandler(data) {
  logger.log("data: ", data);
  let token = data.token;
  if (decisionDoneMap[token]) {
    logger.log("decision finished at first, sendCpaymentReq");
    const clone = JSON.parse(JSON.stringify(decisionDoneMap[token]));
    logger.log("clone: ", clone);
    sendCpaymentReq(clone);
    delete decisionDoneMap[token];
  } else {
    compressDoneMap[token] = data;
    logger.log("compressDone finished");
  }
}

function makeBillRes(data) {
  let response = {
    approveResult: 1,
    resultCode: "0000",
    resultMsg: "create OK",
    approveNo: "111111  ",
  };
  response.type = data.type;
  response.tokenID = data.tokenID;
  response.paymentID = data.paymentID;
  response.approvePrice = data.paymentPrice;
  response.approveAt = moment(Date.now()).utc().format();
  return response;
}

function sendCpaymentReq(csData) {
  logger.log(JSON.stringify(csData, null, 2));
  csData.data.paymentMode = 2;
  let state = csData.data.state;
  if (csData.data.token) {
    csData.data.tokenID = csData.data.token;
    delete csData.data.token;
    if (csData.data.kiccType) delete csData.data.kiccType;
  }

  if (state == 1) {
    sendCompleteRequest(csData, "completeReq");
  } else if (state == 2) {
    logger.log("incomplete case: ", csData.data.tokenID);
    sendCompleteRequest(csData, "incompleteReq");
  }
}

csClientEmitter.on("cpayment", (data) => cpaymentHandler(data));
csClientEmitter.on("billResponse", (data) => {
  billResponseHandler(data);
});
csClientEmitter.on("uploadFile", (data) => uploadFileHandler(data));
csClientEmitter.on("uploadNextFile", () => {
  uploadNextFileHandler();
});
csClientEmitter.on("billCancelResponse", (data) =>
  billCancelResponseHandler(data)
);
csClientEmitter.on("rebillResponse", (data) => rebillResponseHandler(data));
csClientEmitter.on("rmImgFolder", (data) => rmImgFolderHandler(data));
csClientEmitter.on("deviceOpened", (data) => deviceOpenedHandler(data));
csClientEmitter.on("compressDone", (data) => compressDoneHandler(data));
module.exports = { csClientEmitter };
