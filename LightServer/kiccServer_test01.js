const { parentPort } = require("worker_threads");
const net = require("net");
var logger = require("./logger");
let iconv = require("iconv-lite");
var moment = require("moment");
var { playAudio } = require("./audioPlayer");

var EventEmitter = require("events").EventEmitter;
var kiccEmitter = new EventEmitter();

let {
  kicctitPort,
  memberShip,
  timesLimit,
  pay_start,
  pay_end,
} = require("./config");
const fs = require("fs");
const { deviceID, cancelMode } = JSON.parse(fs.readFileSync("deviceInfo.json"));
const { request } = require("http");
const { performance } = require("perf_hooks");
let payT0;
let payT1;
let cancelT0;
let cancelT1;

let kicctitClient;
let cnt = 0;
let tokenObj = null;
let clientInterval;
let errorTimes = 1;
let tasks = [];
let requestQueue = [];
let intervals = [];
let isBusy = false;
let isDoorOpened = false;
let requestErrorTimes = 1;
let adultMode = false;
let preType = "";
let paymentReqQueue = [];
let paymentData;
let isTimeout = false;

kiccEmitter.on("paymentRequest", paymentRequestHandler);
kiccEmitter.on("doorStatus", (status) => {
  isDoorOpened = status;
});
kiccEmitter.on("billCancel", billCancelHandler);
kiccEmitter.on("rebill", rebillHandler);

let tokenHandler = {
  TR: (data) => {
    createTokenHandler(data);
  },
  TD: (data) => {
    delTokenHandler(data);
  },
  D1: (data) => {
    confirmTokenHandler(data);
  },
  D4: (data) => {
    cancelTokenHandler(data);
  },
};

let responseHandler = {
  ack: (data) => {
    logger.log("from kicc client: ack ", data);
  },
  fb1409: (data) => {
    stateInfoHandler(data);
  },
  fb1404: (data) => {
    confirmHandler(data);
  },
  fb1402: (data) => {
    deviceInfoHandler(data);
  },
};

let server;

createKiccServer(socketHandler);

function createKiccServer(handler) {
  server = net.createServer(handler);
  server.maxConnections = 1;
  server.listen(kicctitPort);
  logger.log("listen 6000");
}

function socketHandler(socket) {
  logger.log(
    "Connection from",
    socket.remoteAddress,
    "port",
    socket.remotePort
  );

  socket.on("data", (data) => {
    logger.log("socketData length: ", data.length);
    let length = data.length;
    let index = 0;
    if (length > 3) {
      while (length > 0) {
        let packLength = data.readUIntBE(index + 1, 2) + 4;
        logger.log("packLength: ", packLength);
        let packData = data.slice(index, index + packLength);
        socketDataHandler(packData, socket);
        length -= packLength;
        index += packLength;
        logger.log("index: ", index);
      }
    } else {
      socketDataHandler(data, socket);
    }
  });
  socket.on("end", socketEndHandler);
  socket.on("error", (ex) => {
    logger.log("handle tcp socket error: ", ex);
  });
}

function socketDataHandler(data, socket) {
  logger.log("from kicc client: ", data);
  logger.log("data: ", data.toString("hex"));

  let type = getType(data);
  logger.log(type);
  if (responseHandler[type]) {
    kiccClient = socket;
    logger.log("set kiccClient");
    kiccClient.write(Buffer.from([0x06, 0x06, 0x06]));
    responseHandler[type](data);
  } else {
    logger.error("there is no handler: ", type);
    socket.end();
    socket.destroy();
  }
}

function socketEndHandler() {
  logger.log("Closed", kiccClient.remoteAddress, "port", kiccClient.remotePort);
}

function getType(data) {
  if (data.length <= 6) {
    if (data.toString("hex") == "060606") {
      return "ack";
    } else {
      return "unknown";
    }
  } else {
    logger.log("sent ack");
    setCnt(data);
    let eventType = data.readUIntBE(4, 3).toString(16);
    return eventType;
  }
}

function stateInfoHandler(data) {
  logger.log(data);
  let resCode = data.readUIntBE(7, 1);
  let state = data.readUIntBE(8, 2).toString(16);
  logger.log("resCode: ", resCode, " , state: ", state);
  if (state == "3034") {
    logger.log("device gets into adult mode");
    adultMode = true;
  } else if (state == "3039") {
    logger.log("adult mode failed");
    adultMode = false;
  } else if (state === "3035" && resCode === 0) {
    if (adultMode == true) adultMode = false;
    /* should check 05 */
    if (!isDoorOpened) {
      let buf = createTokenRequest();
      if (isBusy) playAudio();
      preprocessRequest({}, buf, "createToken");
    } else {
      logger.log("door is open");
    }
  } else if (state === "3037") {
    logger.log("kicc client is ready");
    requestQueue.shift();
    logger.log("after 3037: ", requestQueue);
    resetInterval();
    if (requestQueue.length > 0) {
      if (requestQueue[0]) {
        let req = requestQueue[0];
        if (req.kiccType == "cancelToken") {
          cancelT0 = performance.now();
        }

        if (req.kiccType == "createToken" && isDoorOpened == true) {
          requestQueue.shift();
          logger.log("door is opened, shift createToken request");
        } else {
          logger.log("request: ", req);
          sendKiccRequest(req.buf, req.kiccType, req.token);
        }
      }
    } else {
      isBusy = false;
      logger.log("set isBusy to false");
    }
  } else if (state === "3038") {
    logger.log("kicc client restarted");
    resetInterval();
    if (requestQueue.length > 0) {
      if (requestQueue[0]) {
        let req = requestQueue[0];
        if (req.kiccType != "createToken") {
          logger.log("request: ", req);
          sendKiccRequest(req.buf, req.kiccType, req.token);
        } else {
          requestQueue.shift();
          if (requestQueue.length > 0) {
            let reqNext = requestQueue[0];
            if (reqNext.hasOwnProperty("token")) {
              sendKiccRequest(reqNext.buf, reqNext.kiccType, reqNext.token);
            } else {
              sendKiccRequest(reqNext.buf, reqNext.kiccType);
            }
            logger.log("send request after cancel a createToken reqeust");
          } else {
            isBusy = false;
            logger.log("set isBusy to false after receive 3038");
          }
        }
      }
    }
  } else {
    logger.log("user doesnot agree or there is something wrong about the card");
  }
}

function confirmHandler(data) {
  let length = data.readUIntBE(1, 2) - 7;
  logger.log(length);
  if (length > 0) {
    let resCode = data.readUInt8(7);
    logger.log("resCode: ", resCode);
    let obj = getResponseObj(data, length);
    let type = "unknown";
    if (obj["S01"]) type = obj["S01"];

    if (isTimeout == true) {
      isTimeout = false;
      logger.log("confirmHandler req[0]: ", requestQueue[0]);
      logger.log("obj: ", obj);
      if (type == getKiccType(requestQueue[0].kiccType) && type != "TR") {
        let reqToken = obj["S22"].slice(7);
        let price =
          requestQueue[0].approvePrice || requestQueue[0].paymentPrice;
        if (reqToken == requestQueue[0].token && obj["S10"] == price) {
          logger.log("it is last request: ", requestQueue[0]);
        } else {
          logger.log("price or token is not same");
          emitError(JSON.parse(JSON.stringify(requestQueue[0])));
          return;
        }
      } else {
        logger.log("kicc type is not same");
        emitError(JSON.parse(JSON.stringify(requestQueue[0])));
        return;
      }
    }

    if (resCode == 0) {
      if (obj["R04"] != "0000") {
        if (requestErrorTimes < 3) {
          requestErrorTimes++;
          requestQueue.unshift(requestQueue[0]);
          logger.log("retry request: ", requestQueue[0]);
        } else {
          logger.log("send 3times request, but all failed");
          requestErrorTimes = 1;
          tokenHandler[type](obj);
        }
      } else if (obj["R04"] == "0000") {
        tokenHandler[type](obj);
        requestErrorTimes = 1;
      }
    } else if (resCode == 255) {
      logger.error("request error: ", obj);
      handleFailedResponse(obj);
      obj.approveResult = 2;
      //tokenHandler[type](obj);
    }
  } else {
    logger.error("data length is 0");
  }
}

function deviceInfoHandler(data) {
  logger.log(data.toString("hex"));
}

function requestCmd(data) {
  let target = Buffer.alloc(7);
  target[0] = 2;
  target[3] = cnt;
  target[4] = 0xfb;
  target[5] = 0x14;
  target[6] = 0x04;
  let dataBuf = Buffer.from(data, "utf8");
  logger.log(dataBuf);
  let length = 6 + Buffer.byteLength(dataBuf, "utf8");
  logger.log(length);
  target.writeUInt16BE(length, 1);
  target = Buffer.concat([target, dataBuf, Buffer.from([0x03])]);
  let targetCopy = Buffer.alloc(Buffer.byteLength(target, "utf8") - 1);
  target.copy(targetCopy, 0, 1);
  let checkCode = checkCrc(targetCopy, Buffer.byteLength(targetCopy));
  logger.log(checkCode);
  let crc = Buffer.alloc(2);
  crc.writeUInt16BE(checkCode);
  target = Buffer.concat([target, crc]);
  return target;
}

function createTokenRequest() {
  let data = `S01=TR;S02=TK;S10=G;S11=${memberShip};S23=${deviceID};`;
  logger.log("data", data);
  let buffer = requestCmd(data);
  logger.log(buffer.toString("hex"), Buffer.byteLength(buffer));
  return buffer;
}

function delTokenRequest() {
  let data = `S01=TD;S02=TK;S06=@;S22=${token};S23=${deviceID};`;
  let buffer = requestCmd(data);
  logger.log(buffer.toString("hex"));
}

function paymentRequest(token, amount) {
  let tax = Math.round(amount / 11);
  let data = `S01=D1;S02=40;S06=@;S09=00;S10=${amount};S15=0;S16=${tax};S22=TOKNKIC${token};S23=${deviceID};`;
  let buffer = requestCmd(data);
  logger.log(buffer.toString("hex"));
  return buffer;
}

function cancelPayment(token, amount, orderNum, date) {
  let tax = Math.round(amount / 11);
  let data = `S01=D4;S02=40;S06=@;S09=00;S10=${amount};S12=${orderNum};S13=${date};S15=0;S16=${tax};S22=TOKNKIC${token};S23=12345678901234567890;`;
  let buffer = requestCmd(data);
  logger.log(buffer.toString("hex"));
  return buffer;
}

function checkCrc(data, length) {
  logger.log(data.toString("hex"), "length: ", length);
  let index = 0;
  let temp;
  let SEED = Uint16Array.from([0x8005]);

  let crc = Uint16Array.from([0xffff]);
  do {
    temp = data[index++] & 0xff;
    for (i = 0; i < 8; i++) {
      if ((crc[0] & 0x0001) ^ (temp & 0x01)) crc[0] = (crc[0] >> 1) ^ SEED[0];
      else crc[0] >>= 1;
      temp >>= 1;
    }
  } while (--length);
  crc[0] = ~crc[0];
  temp = crc[0];
  crc[0] = (crc[0] << 8) | ((temp >> 8) & 0xff);
  return crc[0];
}

function setCnt(data) {
  let tmpCnt = data.readUIntBE(3, 1);
  cnt = tmpCnt >= 255 ? 1 : tmpCnt + 1;
}

function getResponseObj(data, length) {
  let resData = iconv.decode(data.slice(8, 8 + length), "euc-kr").toString();
  logger.log("resData: ", resData);
  let resArr = resData.split(";");
  logger.log(resArr);
  let resObj = {};
  resArr.forEach((ele) => {
    let entry = ele.split("=");
    if (entry.length <= 2) {
      resObj[entry[0]] = entry[1];
    } else {
      let entrykey = entry.shift();
      resObj[entrykey] = entry.join("'");
    }
  });
  return resObj;
}

function createTokenHandler(data) {
  let validity = data["R08"];
  let token = data["R19"];

  if (token && validity) {
    tokenObj = { token, validity, type: "openDoor" };
    kiccEmitter.emit("createdToken", tokenObj);
    logger.log(tokenObj);
  } else {
    logger.error("invalid token");
    let data = requestQueue[0];
    sendKiccRequest(data.buf, data.kiccType);
  }
}

function delTokenHandler(data) {
  logger.log(data);
}

function confirmTokenHandler(data) {
  logger.log(data);
  let response = requestQueue[0];
  logger.log(response);
  payT1 = performance.now();
  pay_end = performance.now();
  logger.log("profiling--- confirmToken takes : " + (payT1 - payT0) + " ms");
  logger.log("profiling payment: " + (pay_end - pay_start) + " ms");
  let paymentTime = moment().format("YYYY-MM-DDTHH:mm:ss.SSS");
  logger.log("profiling payment time: ", paymentTime);
  if (response.buf) {
    delete response.buf;
  }

  if (!data.approveResult && data["R04"] == "0000") {
    response.approveResult = 1; //check whether get some error message
    response.resultCode = "0000";
    response.resultMsg = "create OK";
  } else {
    response.approveResult = 2;
    response.resultCode = "8521";
    response.resultMsg = `${data["R17"]}-${data["R04"]}`;
  }

  if (data["R09"]) {
    response.approveNo = data["R09"];
  }

  /* check aprrove time */
  if (data["R07"]) {
    response.approveAt = formateDateToUtc(data["R07"]);
  }

  if (response.type == "bill" || response.type == "rebill") {
    delete response.paymentPrice;
    if (data["S10"]) {
      response.approvePrice = parseInt(data["S10"]);
    }
    /* cancel payment */
    if (data["S22"] && data["S10"] && data["R09"] && data["R07"]) {
      let token = data["S22"].substring(7);
      let amount = data["S10"];
      let orderNum = data["R09"];
      let date = data["R07"].substring(0, 6);
      logger.log("cancelMode: ", cancelMode);
      if (cancelMode) {
        let buf = cancelPayment(token, amount, orderNum, date);
        preprocessRequest({}, buf, "cancelToken", token);
      }
    } else {
      logger.log("s22, s10, r09, r07 not found");
    }

    if (response.type == "bill") {
      kiccEmitter.emit("billResponse", response);
    } else if (response.type == "rebill") {
      kiccEmitter.emit("rebillResponse", response);
    }
  } else {
    if (response.state == 1) {
      if (data["S10"]) {
        response.approvePrice = parseInt(data["S10"]);
      }
      /* cancel payment */
      if (data["S22"] && data["S10"] && data["R09"] && data["R07"]) {
        let token = data["S22"].substring(7);
        let amount = data["S10"];
        let orderNum = data["R09"];
        let date = data["R07"].substring(0, 6);
        if (cancelMode) {
          let buf = cancelPayment(token, amount, orderNum, date);
          preprocessRequest({}, buf, "cancelToken", token);
        }
      } else {
        logger.log("s22, s10, r09, r07 not found");
      }
      kiccEmitter.emit("confirmToken", response);
    } else if (response.state == 2) {
    }
    logger.log(response);
  }
}

function cancelTokenHandler(data) {
  logger.log(data);
  cancelT1 = performance.now();
  logger.log(
    "profiling--- cancelToken takes : " + (cancelT1 - cancelT0) + " ms"
  );
  let response = requestQueue[0];
  logger.log(response);
  if (response.buf) {
    delete response.buf;
  }

  if (response.type && response.type == "billCancel") {
    if (data["R04"] == "0000") {
      response.approveResult = 1;
      response.resultCode = "0000";
      response.resultMsg = "create OK";
      if (data["R07"]) {
        response.approveAt = formateDateToUtc(data["R07"]);
      }
    } else {
      response.approveResult = 2;
      response.resultCode = data["R04"];
      response.resultMsg = data["R17"];
      if (response.approveAt) {
        delete response.approveAt;
      }
      if (response.approveNo) {
        delete response.approveNo;
      }
      if (response.approvePrice) {
        delete response.approvePrice;
      }
    }

    if (data["R09"]) {
      response.approveNo = data["R09"];
    }

    kiccEmitter.emit("cancelToken", response);
  }
}

function sendKiccRequest(data, type, token) {
  kiccClient.write(data);
  logger.log("sent buffer to kiccClient: ", data.toString());
  preType = type;
  clientInterval = setTimeout(function () {
    isTimeout = true;
    resetInterval();
    if (token) {
      let index = paymentReqQueue.length;
      while (index--) {
        if (paymentReqQueue[index].token == token) {
          logger.log("paymentReqQueue index: ", index);
          break;
        }
      }
      if (index != -1) {
        paymentData = paymentReqQueue[index];
        if (!paymentData.hasOwnProperty("resultCode")) {
          paymentData.resultCode = "8521";
          paymentData.resultMsg = "connection error";
        }

        if (
          !paymentData.hasOwnProperty("approvePrice") &&
          paymentData.paymentPrice
        ) {
          paymentData.approvePrice = paymentData.paymentPrice;
        }

        if (!paymentData.hasOwnProperty("approveResult"))
          paymentData.approveResult = 2;

        let obj = { data: paymentData, type: `${type}Error`, token };
        logger.log("paymentData: ", obj);
        kiccClient.end();
        setTimeout(function () {
          checkLastCmd();
        }, 1000);

        logger.log("remove data from paymentReqQueue");
        paymentReqQueue.splice(index, 1);
        logger.log("requestQueue: ", paymentReqQueue);
        return;
      } else {
        logger.log("not found index: ", requestQueue);
        kiccClient.end();
        setTimeout(function () {
          checkLastCmd();
        }, 1000);
      }
    } else {
      logger.log("not found token, close socket and send LA");
      kiccClient.end();
      setTimeout(function () {
        checkLastCmd();
      }, 1000);
    }
  }, 70000);
}

function checkLastCmd() {
  let data = `S01=LA;`;
  let buffer = requestCmd(data);
  logger.log(buffer.toString("hex"));
  kiccClient.write(buffer);
}

function resetInterval() {
  clearTimeout(clientInterval);
  errorTimes = 1;
  logger.log("clearInterval and reset times");
}

function paymentRequestHandler(data) {
  logger.log(data);
  paymentReqQueue.push(data);
  payT0 = performance.now();
  pay_start = performance.now();
  let buf = paymentRequest(
    data.token || data.tokenID,
    data.approvePrice || data.paymentPrice
  );
  if (data.type == "bill") {
    preprocessRequest(data, buf, "confirmToken", data.token || data.tokenID);
  } else {
    preprocessRequest(data, buf, "confirmToken", data.token || data.tokenID);
  }
}

function billCancelHandler(data) {
  logger.log(data);
  let approveAt;
  if (data.approveAt) {
    approveAt = formateUtcToKst(data.approveAt);
  }

  let buf = cancelPayment(
    data.token || data.tokenID,
    data.approvePrice || data.paymentPrice,
    data.approveNo,
    approveAt
  );
  preprocessRequest(data, buf, "cancelToken", data.token || data.tokenID);
}

function rebillHandler(data) {
  logger.log(data);
  let buf = paymentRequest(
    data.token || data.tokenID,
    data.approvePrice || data.paymentPrice
  );
  preprocessRequest(data, buf, "confirmToken", data.token || data.tokenID);
}

function preprocessRequest(data, buf, kiccType, token) {
  data.buf = buf;
  data.kiccType = kiccType;
  if (token) data.token = token;
  logger.log("before push: ", data);
  requestQueue.push(data);
  if (!isBusy && adultMode == false) {
    logger.log("kicc server is not busy, ready to send request");
    isBusy = true;
    sendKiccRequest(buf, kiccType, token);
  } else {
    logger.log("kicc server is busy");
  }
}

function handleFailedResponse(obj) {
  if (obj.hasOwnProperty("CANCELED")) {
    logger.log("user canceled request");
  } else if (obj.hasOwnProperty("NOT AVAILABLE")) {
    logger.log("kicc server is not ready");
    resetInterval();
    if (requestQueue[0]) {
      let req = requestQueue[0];
      if (req.kiccType == "createToken") {
        logger.log("request: ", req);
        requestQueue.shift();
        if (requestQueue.length == 0) {
          isBusy = false;
          logger.log("receive NOT AVAILABLE, remove a createToken request");
        }
      }
    }
  } else if (obj.hasOwnProperty("FAIL")) {
    logger.log(requestQueue[0]);
    if (requestQueue[0] && requestQueue[0].kiccType != "createToken") {
      logger.log("handler fail: ", obj);
      logger.log("requestQueue: ", requestQueue[0]);
      if (requestErrorTimes < 3) {
        requestErrorTimes++;
        requestQueue.unshift(requestQueue[0]);
        logger.log("retry request: ", requestQueue[0]);
      } else {
        logger.log("send 3times request, but all failed");
        logger.log(obj);
        logger.log(requestQueue[0]);
      }
    }
  }
}

function formateDateToUtc(date) {
  let data = [];
  for (let i = 0; i < date.length; i += 2) {
    data.push(date.slice(i, i + 2));
  }
  let dateString = `20${data[0]}-${data[1]}-${data[2]} ${data[3]}:${data[4]}:${data[5]}`;
  let utcDate = moment(dateString).utc().format();
  return utcDate;
}

function formateUtcToKst(date) {
  logger.log("utc date: ", date);
  var utcDate = moment(date);
  let year = utcDate.year().toString().slice(2);
  let month = utcDate.month() + 1;
  if (month < 10) {
    month = "0" + month;
  }
  let day = utcDate.date();
  if (day < 10) {
    day = "0" + day;
  }
  let kstDate = `${year}${month}${day}`;
  logger.log("kstDate: ", kstDate);
  return kstDate;
}

function getKiccType(type) {
  let kiccType = "unknown";
  switch (type) {
    case "createToken":
      kiccType = "TR";
      break;
    case "cancelToken":
      kiccType = "D4";
      break;
    case "confirmToken":
      kiccType = "D1";
      break;
  }
  return kiccType;
}

function emitError(data) {
  logger.log("emitError: ", data);
  data.kiccType = data.kiccType + "Error";
  data.type = data.kiccType;
  kiccEmitter.emit(data.kiccType, data);
}

module.exports = { kiccEmitter };
