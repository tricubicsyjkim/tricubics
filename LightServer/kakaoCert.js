const { SerialPort } = require("serialport");
var EventEmitter = require("events").EventEmitter;
var kakaoCertEmitter = new EventEmitter();
var fs = require("fs");
var dateFormat = require("dateformat");
var request = require("async-request");
const logger = require("./logger");
const { storeId, storeName, deviceType } = JSON.parse(
  fs.readFileSync("deviceInfo.json")
);
const { kakaoCertUrl } = require("./config");

const port = new SerialPort({ path: "/dev/ttyACM0", baudRate: 115200 });

port.on("open", function (err) {
  if (err) {
    logger.log("error opening scanner port: ", err.message);
  } else {
    logger.log("conneted barcode scanner");
  }
});

port.on("data", async function (data) {
  logger.log("Data:", data);
  logger.log("String:", data.toString());
  if (data.toString().startsWith("KAKAO")) {
    const res = await kakaoAdultCert(data.toString());
    logger.log("kakao res: ", res.statusCode, typeof res.statusCode);
    logger.log("kakao res: ", res.body);
    let obj = JSON.parse(res.body);
    if (
      res.statusCode &&
      res.statusCode.toString().startsWith("20") &&
      obj.resultCode &&
      obj.resultCode.startsWith("20")
    ) {
      if (obj.resultCode == "200") {
        logger.log("kakao cert:  adult");
        kakaoCertEmitter.emit("kakaoCert", "adult");
      } else if (obj.resultCode == "201") {
        logger.log("kakao cert: not adult");
        kakaoCertEmitter.emit("kakaoCert", "child");
      }
    } else if (res.body.resultCode) {
      logger.log("kako cert failed");
    }
  }
});

port.on("error", function (err) {
  logger.log("barcode scanner error: ", err.message);
});

async function kakaoAdultCert(data) {
  const adultRequest = {
    storeName,
    storeId,
    deviceId: deviceType,
    orderId: getOrderIdByTime(),
    type: "1",
    barcode: data,
  };

  const requestOptions = {
    method: "POST",
    data: adultRequest,
  };

  try {
    const response = await request(kakaoCertUrl, requestOptions);
    return response;
  } catch (error) {
    logger.log("kakao adult cert failed: ", error);
  }
}

getOrderIdByTime();
function getOrderIdByTime() {
  const now = new Date();
  let orderId = dateFormat(now, "yyyymmddHMMss");
  logger.log("orderId: ", orderId);
  return orderId;
}

module.exports = { kakaoCertEmitter };
