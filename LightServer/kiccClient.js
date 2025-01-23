const net = require("net");
var logger = require("tracer").console();
let iconv = require("iconv-lite");
const crypto = require("crypto");

let index = 1;
let client = new net.Socket();
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
  LA: (data) => {
    checkLastCmd(data);
  },
};

let lastBuffer, lastData;
let adultMode = false;

function socketDataHandler(data) {
  logger.log("from kicc client: ", data);
  logger.log("data: ", data.toString("hex"));

  let type = getType(data);
  logger.log(type);
  if (responseHandler[type]) {
    logger.log("set kiccClient");
    client.write(Buffer.from([0x06, 0x06, 0x06]));
    responseHandler[type](data);
  } else {
    logger.error("there is no handler: ", type);
  }
}

client.connect(5002, "localhost", function () {
  logger.log("Connected");

  //send state info
  let stateInfo = Uint8Array.from([
    0x02, 0x00, 0x53, 0x00, 0xfb, 0x14, 0x02, 0x00, 0x30, 0x34, 0x36, 0x36,
    0x31, 0x32, 0x30, 0x33, 0x33, 0x34, 0x36, 0x36, 0x32, 0x32, 0x30, 0x31,
    0x37, 0x30, 0x33, 0x34, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x45, 0x44,
    0x2d, 0x37, 0x38, 0x35, 0x30, 0x30, 0x30, 0x31, 0x20, 0x20, 0x20, 0x32,
    0x30, 0x37, 0x38, 0x30, 0x32, 0x31, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2f,
    0x03, 0xe6, 0x4b,
  ]);
  client.write(stateInfo);
});

client.on("data", function (data) {
  // logger.log("Received: " + data.toString("hex"));
  //   client.destroy(); // kill client after server's response
  let length = data.length;
  let index = 0;
  if (length > 3) {
    while (length > 0) {
      let packLength = data.readUIntBE(index + 1, 2) + 4;
      logger.log("packLength: ", packLength);
      let packData = data.slice(index, index + packLength);
      socketDataHandler(packData);
      length -= packLength;
      index += packLength;
      logger.log("index: ", index);
    }
  } else {
    logger.log("get ack");
  }
});

client.on("close", function () {
  logger.log("Connection closed");
  client.connect(5002, "localhost", function () {
    logger.log("Connected");

    //send state info
    let stateInfo = Uint8Array.from([
      0x02, 0x00, 0x53, 0x00, 0xfb, 0x14, 0x02, 0x00, 0x30, 0x34, 0x36, 0x36,
      0x31, 0x32, 0x30, 0x33, 0x33, 0x34, 0x36, 0x36, 0x32, 0x32, 0x30, 0x31,
      0x37, 0x30, 0x33, 0x34, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x45, 0x44,
      0x2d, 0x37, 0x38, 0x35, 0x30, 0x30, 0x30, 0x31, 0x20, 0x20, 0x20, 0x32,
      0x30, 0x37, 0x38, 0x30, 0x32, 0x31, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2f,
      0x03, 0xe6, 0x4b,
    ]);
    client.write(stateInfo);
  });
});

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

function setCnt(data) {
  let tmpCnt = data.readUIntBE(3, 1);
  cnt = tmpCnt >= 255 ? 1 : tmpCnt + 1;
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
          sendKiccRequest(req.buf, req.kiccType);
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
          sendKiccRequest(req.buf, req.kiccType);
        } else {
          requestQueue.shift();
          if (requestQueue.length > 0) {
            sendKiccRequest(req.buf, req.kiccType);
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
    let obj = getResponseObj(data, length);
    let type = "unknown";
    if (obj["S01"]) type = obj["S01"];
    logger.log("tpye: ", type);
    tokenHandler[type](obj);
  } else {
    logger.error("data length is 0");
  }
}

function createTokenHandler() {
  let seed = generateHexNumber(13);
  let token = `KI1${seed}`;

  let myData = `S01=TR;R01=P;R02=;R03=0234;R04=0000;R05=;R06=;R07=2302031539175;R08=230210153900;R09=;R10=;R12=029;R13=;R14=;R15=;R16=d;R17=;R18=;R19=${token};R20=031552691223;R21=Y;R22=;R23=;R24=;R25=;R26=;R27=;R28=;R29=;R30=;`;
  logger.log("createTokenHandler: ", myData);

  let buffer = requestCmd(myData);
  // if (index % 2 == 1) {
  // lastBuffer = buffer;
  // lastData = myData;
  client.write(buffer);
  setTimeout(function () {
    sendNext();
  }, 500);
  // }
  // index++;
}

function sendNext() {
  let next = Uint8Array.from([
    0x02, 0x00, 0x09, 0x00, 0xfb, 0x14, 0x09, 0x00, 0x30, 0x37, 0x03, 0x2f,
    0xcb,
  ]);
  client.write(next);
}

function confirmTokenHandler(data) {
  logger.log("confirmTokenHandler: ", data);
  let token = data["S10"];
  let price = data["S22"];
  let strObj = JSON.stringify(data);
  strObj = strObj.replace(/\{/g, "");
  strObj = strObj.replace(/\}/g, "");
  strObj = strObj.replace(/\:/g, "=");
  strObj = strObj.replace(/\"/g, "");
  strObj = strObj.replace(/\,/g, ";");
  strObj += ";";
  logger.log("strObj: ", strObj);
  let confirmNum = generateHexNumber(4);
  let strBack = `R01=Q;R02=A;R03=0156;R04=0000;R05=016;R06=0000;R07=2302010916073;R08=;R09=${confirmNum};R10=;R11=Y;R12=016;R13=KBcard;R14=00099752493;R15=KBcard;R16=d;R17=;R18=00000000;R19=KICC;R20=010942591407;R21=;R22=Y;R23=527289;R24=;R25=;R26=;R27=;R28=;R29=;R30=;`;
  strObj += strBack;
  let buffer = requestCmd(strObj);
  // if (index % 2 == 1) {
  lastBuffer = buffer;
  lastData = strObj;
  // }

  client.write(buffer);
  setTimeout(function () {
    sendNext();
  }, 500);
  index++;
}

function cancelTokenHandler(data) {
  let token = data["S10"];
  let price = data["S22"];
  let strObj = JSON.stringify(data);
  strObj = strObj.replace(/\{/g, "");
  strObj = strObj.replace(/\}/g, "");
  strObj = strObj.replace(/\:/g, "=");
  strObj = strObj.replace(/\"/g, "");
  strObj = strObj.replace(/\,/g, ";");
  strObj += ";";
  logger.log("strObj: ", strObj);
  // let confirmNum = generateHexNumber(4);
  let strBack = `R01=P;R02=A;R03=0157;R04=0000;R05=016;R06=0000;R07=2302010916123;R08=;R09=30011915;R10=;R11=Y;R12=016;R13=KBcard;R14=00099752493;R15=KBcard;R16=d;R17=;R18=00000000;R19=KICC;R20=010963116815;R21=;R22=Y;R23=527289;R24=;R25=;R26=;R27=;R28=;R29=;R30=;`;
  strObj += strBack;
  let buffer = requestCmd(strObj);
  if (index % 2 == 1) {
    lastBuffer = buffer;
    lastData = strObj;
  }
  client.write(buffer);
  setTimeout(function () {
    sendNext();
  }, 500);
  // index++;
}

function checkLastCmd(data) {
  logger.log("lastcmd: ", lastData);
  client.write(lastBuffer);
  sendNext();
}

function generateHexNumber(length) {
  return crypto.randomBytes(length).toString("hex");
}

function requestCmd(data) {
  let target = Buffer.alloc(8);
  target[0] = 2;
  target[3] = cnt;
  target[4] = 0xfb;
  target[5] = 0x14;
  target[6] = 0x04;
  target[7] = 0;
  let dataBuf = Buffer.from(data, "utf8");
  // logger.log(dataBuf);
  let length = 7 + Buffer.byteLength(dataBuf, "utf8");
  // logger.log(length);
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

function checkCrc(data, length) {
  // logger.log(data.toString("hex"), "length: ", length);
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

function deviceInfoHandler(data) {
  logger.log(data.toString("hex"));
}

function getResponseObj(data, length) {
  let resData = iconv.decode(data.slice(7, 7 + length), "euc-kr").toString();
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

var keypress = require("keypress"),
  tty = require("tty");
const Json = require("archiver/lib/plugins/json");

keypress(process.stdin);

// listen for the "keypress" event
process.stdin.on("keypress", function (ch, key) {
  if (ch) {
    logger.log("ch: ", ch);

    if (ch == 1) {
      setTimeout(() => {
        let pressBtn = Uint8Array.from([
          0x02, 0x00, 0x09, 0x00, 0xfb, 0x14, 0x09, 0x00, 0x30, 0x35, 0x03,
          0x04, 0x5c,
        ]);
        client.write(pressBtn);
      }, 1500);
    }
  }

  if (key && key.ctrl && key.name == "c") {
    process.stdin.pause();
    process.exit(1);
  }
});

if (typeof process.stdin.setRawMode == "function") {
  process.stdin.setRawMode(true);
} else {
  tty.setRawMode(true);
}
process.stdin.resume();

/* create token */
// let token = `KI`;
// let myData = `S01=TR;R01=P;R02=;R03=0234;R04=0000;R05=;R06=;R07=2302031539175;R08=230210153900;R09=;R10=;R12=029;R13=;R14=;R15=;R16=d;R17=;R18=;R19=${token};R20=031552691223;R21=Y;R22=;R23=;R24=;R25=;R26=;R27=;R28=;R29=;R30=;`;

// myData = myData.replace(/\s/g, "");
// myData = myData.replace(/\'/g, "");
// myData = myData.replace(/\,/g, ";");
// logger.log("myData: ", myData);

/* confirm token */
// let myData = {
//   S01: "D1",
//   S02: "40",
//   S06: "@",
//   S09: "00",
//   S10: "900",
//   S15: "0",
//   S16: "91",
//   S22: "TOKNKICKI175207b50b4779a5ceed987a8d0",
//   S23: "22090504085284914975",
// };

// let strObj = JSON.stringify(myData);
// strObj = strObj.replace(/\{/g, "");
// strObj = strObj.replace(/\}/g, "");
// strObj = strObj.replace(/\:/g, "=");
// strObj = strObj.replace(/\,/g, ";");

// logger.log(strObj);

/* make confirm response  back part */
// let myData = `'R01=P',
//   'R02=A',
//   'R03=0157',
//   'R04=0000',
//   'R05=016',
//   'R06=0000',
//   'R07=2302010916123',
//   'R08=',
//   'R09=30011915    ',
//   'R10=',
//   'R11=Y',
//   'R12=016',
//   'R13=KB체크카드          ',
//   'R14=00099752493 ',
//   'R15=KB국민카드          ',
//   'R16=d',
//   'R17=',
//   'R18=00000000',
//   'R19=KICC로제출',
//   'R20=010963116815',
//   'R21=',
//   'R22=Y',
//   'R23=527289',
//   'R24=',
//   'R25=',
//   'R26=',
//   'R27=',
//   'R28=',
//   'R29=',
//   'R30=',`;
// myData = myData.replace(/\s/g, "");
// myData = myData.replace(/\'/g, "");
// myData = myData.replace(/\,/g, ";");
// logger.log("myData: ", myData);
