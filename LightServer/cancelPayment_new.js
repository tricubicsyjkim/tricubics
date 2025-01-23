const { parentPort } = require("worker_threads");
const net = require("net");
// var logger = require("tracer").console();
var logger = require("./logger");
let iconv = require("iconv-lite");

var EventEmitter = require("events").EventEmitter;
var kiccEmitter = new EventEmitter();

const { kiccPort, memberShip, timesLimit } = require("./config");
const fs = require("fs");
const deviceID = JSON.parse(fs.readFileSync("deviceInfo.json")).deviceID;

let kiccClient;
let cnt = 0;
let tokenObj = null;
let clientInterval;
let errorTimes = 1;
let tasks = [];
let requestQueue = [];
let intervals = [];

kiccEmitter.on("paymentRequest", paymentRequestHandler);

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

const server = net.createServer(socketHandler);
server.maxConnections = 20;
server.listen(kiccPort);
logger.log("listen 5000");

function socketHandler(socket) {
  logger.log(
    "Connection from",
    socket.remoteAddress,
    "port",
    socket.remotePort
  );

  kiccClient = socket;
  socket.on("data", socketDataHandler);
  socket.on("end", socketEndHandler);
}

function socketDataHandler(data) {
  logger.log("from kicc client: ", data);
  logger.log("data: ", data.toString("hex"));
  let type = getType(data);
  logger.log(type);
  if (responseHandler[type]) {
    responseHandler[type](data);
  } else {
    logger.error("there is no handler: ", type);
  }
}

function socketEndHandler() {
  logger.log("Closed", kiccClient.remoteAddress, "port", kiccClient.remotePort);
}

function getType(data) {
  if (data.length <= 6) {
    return "ack";
  } else {
    kiccClient.write(Buffer.from([0x06, 0x06, 0x06]));
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
  if (state === "3035" && resCode === 0) {
    /* should check 05 */
    // let buf = createTokenRequest();
    // sendKiccRequest(buf, "createToken");
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
    if (resCode == 0) {
      if (obj["S01"]) {
        let type = obj["S01"];
        tokenHandler[type](obj);
      }
    } else if (resCode == 255) {
      logger.error("request error: ", obj);
    }
  } else {
    logger.error("data length is 0");
  }
}

function deviceInfoHandler(data) {
  logger.log(data.toString("hex"));
  /* check IP address and port*/
  /* mock user agree */
  // let buf = createTokenRequest();
  // sendKiccRequest(buf, "createToken");

  // let cancelBuf = cancelPayment(
  //   "KI1AAF10004D7031F66F676C2B339",
  //   11,
  //   "48168920", //44637602   44537265
  //   "210624"
  // );
  // kiccClient.write(cancelBuf);

  let paylBuf = paymentRequest("xxx", 300);
  kiccClient.write(paylBuf);
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
    resObj[entry[0]] = entry[1];
  });
  return resObj;
}

function createTokenHandler(data) {
  let validity = data["R08"];
  let token = data["R19"];

  if (token && validity) {
    tokenObj = { token, validity, type: "openDoor" };
    kiccEmitter.emit("createdToken", tokenObj);
    resetInterval();
    logger.log(tokenObj);
  } else {
    logger.error("invalid token");
  }
}

function delTokenHandler(data) {
  logger.log(data);
}

function confirmTokenHandler(data) {
  logger.log(data);
  resetInterval();
  /* check response from kicc client */
  /* do something */
  let response = requestQueue.shift();
  logger.log(response);
  // response.approveResult = 1;
  // response.approveNo = data["R09"];
  // /* check it's bill or paymentRequest */
  // if (response.tokenID) {
  //   delete Object.assign(response, {
  //     ["approvePrice"]: response["paymentPrice"],
  //   })["paymentPrice"];
  //   kiccEmitter.emit("billResponse", response);
  // } else {
  //   delete Object.assign(response, { ["tokenID"]: response["token"] })["token"];
  //   kiccEmitter.emit("confirmToken", response);
  // }
  // logger.log(response);
}

function cancelTokenHandler(data) {
  logger.log(data);
}

function sendKiccRequest(data, type) {
  kiccClient.write(data);
  logger.log("sent buffer to kiccClient: ", data);
  tasks.push({ data, type });
  clientInterval = setInterval(function () {
    if (errorTimes < timesLimit) {
      errorTimes++;
      kiccClient.write(data);
      logger.log("start interval: ", errorTimes);
    } else {
      let obj = { data, type: `${type}Error` };
      kiccEmitter.emit(type, obj);
      logger.error(obj);
      resetInterval();
    }
  }, 10000);
  intervals.push(clientInterval);
}

function resetInterval() {
  clearInterval(clientInterval);
  times = 1;
  logger.log("clearInterval and reset times");
}

function paymentRequestHandler(data) {
  logger.log(data);
  requestQueue.push(data);
  let buf = paymentRequest(
    data.token || data.tokenID,
    data.approvePrice || data.paymentPrice
  );
  sendKiccRequest(buf, "confirmToken");
}
