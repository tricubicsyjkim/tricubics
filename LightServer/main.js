const { Worker } = require("worker_threads");
var logger = require("./logger");
const nodemailer = require("nodemailer");

let { door_closed, get_decision } = require("./config");
let { iceboxEmitter } = require("./icebox");

let { kiccEmitter } = require("./kiccServer_test01");


let { dbHelperEmitter, getProductList } = require("./dbHelper");
let { csClientEmitter } = require("./csClient");
let { visionClientEmitter } = require("./visionClient");
let { imageSenderEmitter } = require("./imageSender");
let { lotteClientEmitter } = require("./lotteClient");
let { webClientEmitter } = require("./webClient");
let { kakaoCertEmitter } = require("./kakaoCert");
var { reboot, playAudioWithName } = require("./audioPlayer");

var fs = require("fs");
const { deviceID, vendor, deviceName, adultMode } = JSON.parse(
  fs.readFileSync("deviceInfo.json")
);
var path = require("path");
var archiver = require("archiver");
const { performance } = require("perf_hooks");
const { kioskEmitter } = require("./kioskServer");
let totalT0;
let totalT1;

let productList = [];
let isDoorOpened = false;
let csDataQueue = [];
let sysErrorMap = {};
// var userID = 21;

let currentToken = "";
let isAdult = false;
let kakaoCertTimeout;

csClientEmitter.on("completeReq", (data) => {});
csClientEmitter.on("completeReqError", (data) => {
  logger.error("completeReqError: ", data);
  sendMail(data);
});
csClientEmitter.on("incompleteReqError", (data) => {
  logger.error("incompleteReqError: ", data);
  sendMail(data);
});
csClientEmitter.on("bill", (data) => {
  csBillHandler(data);
});
csClientEmitter.on("unpurchased", (data) => {
  unpurchasedHandler(data);
});

csClientEmitter.on("nobill", (data) => {
  nobillHandler(data);
});

function csBillHandler(data) {
  logger.log("from csClient bill event: ", data);
  if (vendor == "hysong") {
    let decisionData = changeID2KorName(JSON.parse(JSON.stringify(data)));
    kioskEmitter.emit("decision", decisionData);
  }

  if (vendor == "japan") {
    let decisionData = changeID2EngName(JSON.parse(JSON.stringify(data)));
    webClientEmitter.emit("decision", decisionData);
    return;
  }

  if (vendor == "lotte") {
    changeID2Barcode(data);
    logger.log("billData barcode: ", data);
    lotteClientEmitter.emit("decision", data);
  } else {
    kiccEmitter.emit("paymentRequest", data);
  }
}

function unpurchasedHandler(data) {
  logger.log("from csClient unpurchased event: ", data);
  if (vendor == "lotte") lotteClientEmitter.emit("unpurchased", data);
}

function nobillHandler(data) {
  logger.log("from csClient nobill event: ", data);
  if (vendor == "lotte") lotteClientEmitter.emit("nobill", data);
}

csClientEmitter.on("billCancel", (data) => {
  logger.log("from csClient billCancel event: ", data);
  kiccEmitter.emit("billCancel", data);
});

csClientEmitter.on("rebill", (data) => {
  logger.log("from csClient rebill event: ", data);
  kiccEmitter.emit("rebill", data);
});

csClientEmitter.on("deviceOpen", (data) => {
  logger.log("from csClient deviceOpen event: ", data);
  // if (!isDoorOpened) {
  iceboxEmitter.emit("deviceOpen", data);
  // } else {
  //   logger.log("the door is opened");
  // }
});

/* kicc tit server wokrer */
//kiccEmitter.on("createdToken", createdTokenHandler);
kiccEmitter.on("createdToken", createdTokenHandler);
kiccEmitter.on("createTokenError", createdTokenErrorHandler);
kiccEmitter.on("confirmToken", confirmTokenHandler);
kiccEmitter.on("confirmTokenError", confirmTokenErrorHandler);
kiccEmitter.on("billResponse", billResponseHandler);
kiccEmitter.on("cancelToken", cancelTokenHandler);
kiccEmitter.on("cancelTokenError", cancelTokenErrorHandler);
kiccEmitter.on("rebillResponse", rebillResponseHandler);
kiccEmitter.on("openDoorError", openDoorErrorHandler);

function createdTokenHandler(data) {
  if (adultMode) {
    if (isAdult) {
      isAdult = false;
      if (kakaoCertTimeout) clearTimeout(kakaoCertTimeout);
      iceboxEmitter.emit("openDoor", data);
    } else {
      playAudioWithName("qr.mp3");
    }
  } else {
    iceboxEmitter.emit("openDoor", data);
  }
  logger.log("openDoor: ", data);
  getProductList();
}

function createdTokenErrorHandler(data) {
  logger.log("openDoorError: ", data);
  data.token = "unknown";
  sendMail(data);
}

function confirmTokenHandler(data) {
  logger.log("confirmToken: ", data);
  if (data.token) {
    let csData = findDataInQueue(data.token);
    csData.data = data;
    logger.log("csData: ", csData);
    csClientEmitter.emit("cpayment", csData);
  }
}

function confirmTokenErrorHandler(data) {
  logger.log("confirmTokenError: ", data);
  if (data.token) {
    let csData = findDataInQueue(data.token);
    logger.log("csData: ", csData);
    csClientEmitter.emit("cpayment", csData);
  }
  sendMail(data);
}

function billResponseHandler(data) {
  logger.log(data);
  delete data.kiccType;
  csClientEmitter.emit("billResponse", data);
}

function cancelTokenHandler(data) {
  logger.log(data);
  if (data.kiccType) {
    delete data.kiccType;
  }
  csClientEmitter.emit("billCancelResponse", data);
}

function cancelTokenErrorHandler(data) {
  logger.log("cancelTokenErrorHandler: ", data);
  sendMail(data);
}

function rebillResponseHandler(data) {
  logger.log(data);
  delete data.kiccType;
  csClientEmitter.emit("rebillResponse", data);
}

/* smart icebox wokrer */
iceboxEmitter.on("doorOpened", doorOpenedHandler);
iceboxEmitter.on("openDoorError", openDoorErrorHandler);
iceboxEmitter.on("doorClosed", doorClosedHandler);
iceboxEmitter.on("closeDoorError", closeDoorErrorHandler);
iceboxEmitter.on("decisionError", decisionErrorHandler);
iceboxEmitter.on("lockStateError", lockStateErrorHandler);
iceboxEmitter.on("setTimerError", setTimerErrorHandler);
iceboxEmitter.on("releaseInfo", releaseInfoHandler);
iceboxEmitter.on("releaseError", releaseErrorHandler);
iceboxEmitter.on("deviceOpened", deviceOpenedHandler);
iceboxEmitter.on("sendImage", sendImageHandler);
iceboxEmitter.on("topviewDone", topviewDoneHandler);
iceboxEmitter.on("lockAllOpened", lockAllOpenedHandler);
iceboxEmitter.on("onSetTimer", onSetTimerHandler);
iceboxEmitter.on("onLockState", onLockStateHandler);
iceboxEmitter.on("deadboltError", onDeadboltHandler);
iceboxEmitter.on("sensorError", sensorErrorHandler);
iceboxEmitter.on("kiosk", (data) => {
  logger.log("kiosk close door");
  kioskEmitter.emit("doorClosed", data);
});

function doorOpenedHandler(data) {
  logger.log("data: ", data);
  isDoorOpened = true;
  kiccEmitter.emit("doorStatus", isDoorOpened);
  if (vendor == "lotte") {
    lotteClientEmitter.emit("unlockRes", data);
  }

  if (vendor == "hysong") kioskEmitter.emit("doorOpened", data);
  if (vendor == "japan") webClientEmitter.emit("doorOpened", data);
}

function openDoorErrorHandler(data) {
  logger.log("openDoorErrorHandler: ", data);
  if (vendor == "lotte") {
    lotteClientEmitter.emit("unlockError", data);
  }
  if (vendor == "japan") {
    webClientEmitter.emit("doorOpened", data);
  }
}

function doorClosedHandler(data) {
  logger.log("data: from maindoorClosedHandler", data);
  door_closed = performance.now();
  totalT0 = performance.now();
  isDoorOpened = false;
  kiccEmitter.emit("doorStatus", isDoorOpened);
  if (data.changedWeightInfo.length > 0) {
    visionClientEmitter.emit("sensorData", data);
    /* setTimeout to send request */
    createSysErrorTimer(JSON.parse(JSON.stringify(data)));
  }

  if (vendor == "lotte") {
    lotteClientEmitter.emit("doorClosed", JSON.parse(JSON.stringify(data)));
    if (data.changedWeightInfo.length <= 0) {
      lotteClientEmitter.emit("decision", data);
    }
  }

  if (vendor == "japan") {
    webClientEmitter.emit("doorClosed", JSON.parse(JSON.stringify(data)));
  }
}

function closeDoorErrorHandler(data) {
  logger.log("data: ", data);
  /* do something if the door cannot close */
  isDoorOpened = false;
  kiccEmitter.emit("doorStatus", isDoorOpened);
  if (vendor == "lotte") {
    lotteClientEmitter.emit("doorClosedError", data);
  }
  sendMail(JSON.parse(JSON.stringify(data)));
}

function decisionErrorHandler(data) {
  logger.log("data: ", data);

  if (data.errorType && vendor == "lotte") {
    delete data["errorType"];
    lotteClientEmitter.emit("doorClosed", JSON.parse(JSON.stringify(data)));
    logger.log("send doorClosed in decisionErrorHandler");
  }

  if (data.errorType && vendor == "japan") {
    delete data["errorType"];
    webClientEmitter.emit("doorClosed", JSON.parse(JSON.stringify(data)));
    logger.log("send doorClosed in decisionErrorHandler");
  }

  if (data.type != "sysError") {
    isDoorOpened = false;
    kiccEmitter.emit("doorStatus", isDoorOpened);
  }

  if (data.token || data.user_id) {
    // imageSenderEmitter.emit("clearQueue", JSON.parse(JSON.stringify(data)));
  }

  /* c/s  */
  if (data) {
    data.type = "incomplete";
    sendMail(data);
  }
  compressFolder(data);
}

function lockStateErrorHandler(data) {
  logger.log("lockStateErrorHandler: ", data);
  lotteClientEmitter.emit("lockStateError", data);
}

function setTimerErrorHandler(data) {
  logger.log("setTimerErrorHandler: ", data);
  lotteClientEmitter.emit("setTimerError", data);
}

function releaseInfoHandler(data) {
  logger.log("data: ", data);
}

function releaseErrorHandler(data) {
  logger.log("data: ", data);
  /* do something if the sensor system cannot release info */
}

function deviceOpenedHandler(data) {
  logger.log("deviceOpenedHandler: ", data);
  csClientEmitter.emit("deviceOpened", data);
}

function sendImageHandler(data) {
  // logger.log("sendImageHandler: ", data);
  imageSenderEmitter.emit("sendImage", data);
}

function topviewDoneHandler(data) {
  logger.log("topviewDoneHandler: ", data);
  let request = { ...data };
  request.type = "topviewDone";
  logger.log("topvoewDone request: ", request);
  imageSenderEmitter.emit("topviewDone", request);
}

function lockAllOpenedHandler(data) {
  logger.log("lockAllOpenedHandler: ", data);
  if (vendor == "lotte") {
    lotteClientEmitter.emit("lockAllOpened", data);
  }
  if (vendor == "hysong") {
    kioskEmitter.emit("lockAllOpened", data);
  }
}

function onSetTimerHandler(data) {
  lotteClientEmitter.emit("onSetTimer", data);
}

function onLockStateHandler(data) {
  lotteClientEmitter.emit("onLockState", data);
}

function onDeadboltHandler(data) {
  logger.log("onDeadboltHandler: ", data);
  sendMail(data);
}

function sensorErrorHandler(data) {
  logger.log("sensorErrorHandler: ", data);
  sendMail(data);
}

/* dbHelper */
dbHelperEmitter.on("queryProduct", queryProductHandler);
dbHelperEmitter.on("queryProductError", queryProductErrorHandler);

getProductList();

function queryProductHandler(data) {
  logger.log("query product list: ", JSON.stringify(data));
  if (data.length > 0) productList = [...data];
  iceboxEmitter.emit("queryProduct", data);
  imageSenderEmitter.emit("queryProduct", data);
}

function queryProductErrorHandler() {
  logger.error("query product list error");
}

function getApprovePrice(data) {
  logger.log("orderList: ", data.orderList);
  let totalPrice = 0;
  if (data.orderList) {
    totalPrice = data.orderList.reduce((total, product) => {
      logger.log("product: ", product);
      let obj = productList.filter((goods) => {
        if (goods.productID === product.productID) {
          product.trayID = goods.trayID;
          return true;
        }
        return false;
      });
      logger.log("obj: ", obj);
      if (obj.length === 1) {
        return total + product.productCount * obj[0].salePrice;
      } else {
        return total;
      }
    }, 0);
    logger.log(totalPrice);
  }
  return totalPrice;
}

visionClientEmitter.on("decision", (data, originalData) => {
  logger.log("originalData: ", originalData);
  if (!data.paymentAt && originalData.paymentAt) {
    data.paymentAt = originalData.paymentAt;
    logger.log("data.paymentAt is empty: ", data.paymentAt);
  }
  totalT1 = performance.now();
  logger.log(
    "profiling--- totoal time without payment: " + (totalT1 - totalT0) + " ms"
  );
  if (sysErrorMap[data.token]) {
    csDataQueue.push({ originalData, data });
    logger.log("find token, add originalData: ", data, originalData);
  } else {
    logger.log("not found token, donnot add originalData");
  }
  decisionHandler(data);
});

function decisionHandler(data) {
  logger.log("main decision Handler: ", data);
  get_decision = performance.now();
  logger.log(
    "profiling total decision: " + (get_decision - door_closed) + " ms"
  );
  if (sysErrorMap[data.token]) {
    clearTimeout(sysErrorMap[data.token].timeout);
    logger.log("found token and cleartimeout");
  } else {
    logger.log("not found token, have sent to C/S");
    return;
  }

  if (!data.hasOwnProperty("state")) {
    data.state = 1;
  }
  if (data.state == 1) {
    /* do something for lotte*/
    if (vendor == "lotte") {
      let decisionData = changeID2Barcode(JSON.parse(JSON.stringify(data)));
      lotteClientEmitter.emit("decision", decisionData);
    }

    if (vendor == "hysong") {
      let decisionData = changeID2KorName(JSON.parse(JSON.stringify(data)));
      kioskEmitter.emit("decision", decisionData);
    }

    if (vendor == "japan") {
      let decisionData = changeID2EngName(JSON.parse(JSON.stringify(data)));
      webClientEmitter.emit("decision", decisionData);
    }

    /* complete payment */
    if (data.paymentPrice > 0) {
      data.paymentMode = 2;
      if (vendor == "lotte" || vendor == "japan") {
        makeConfirmToken(data);
        confirmTokenHandler(data);
      } else {
        kiccEmitter.emit("paymentRequest", data);
      }
    } else {
      /* shift the csData when the paymentPrice is less than 0 */
      let csData = findDataInQueue(data.token);
      csClientEmitter.emit("rmImgFolder", csData.originalData.folderPath);
    }
  } else if (data.state == 2) {
    /* incomplete payment */
    let cpData = JSON.parse(JSON.stringify(data));
    if (cpData.type) cpData.type = "incomplete";
    sendMail(cpData);
    let csData = findDataInQueue(data.token);
    logger.log("findDataInQueue: ", csData);
    if (csData.data && csData.data.hasOwnProperty("productDetails")) {
      delete csData.data.productDetails;
      logger.log("delete productDetails");
    }
    logger.log("check csData: ", csData);
    csClientEmitter.emit("cpayment", csData);
  }
}

visionClientEmitter.on("compressDone", (sensorData) => {
  csClientEmitter.emit("compressDone", sensorData);
});

// setTimeout(function () {
//   let data = {
//     token: userID.toString(),
//     validity: "210706134100",
//     type: "openDoor",
//   };
//   userID++;
//   createdTokenHandler(data);
// }, 5000);

async function compressFolder(data) {
  logger.log("compressFolder: ", data);
  let folder = data.folderPath;
  logger.log("folder: ", folder);
  try {
    if (fs.existsSync(`${folder}_top.zip`)) {
      logger.log("top.zip file have exist");
      let csData = makeCpaymentData(data);
      logger.log("make csData: ", csData);
      let sensorData = makeSensorData(data);
      logger.log("make sensorData: ", sensorData);
      visionClientEmitter.emit("compressDone", sensorData);
      csClientEmitter.emit("cpayment", csData);
    } else {
      var output = fs.createWriteStream(folder + "_top.zip");
      var archive = archiver("zip");
      output.on("close", async function () {
        logger.log(archive.pointer() + " total bytes");
        logger.log(
          "archiver has been finalized and the output file descriptor has closed."
        );
        let csData = makeCpaymentData(data);
        logger.log("make csData: ", csData);
        let sensorData = makeSensorData(data);
        logger.log("make sensorData: ", sensorData);
        visionClientEmitter.emit("compressDone", sensorData);
        csClientEmitter.emit("cpayment", csData);
      });
      archive.on("error", function (err) {
        throw err;
      });
      archive.pipe(output);
      archive.directory(folder, false);
      archive.finalize();
    }
  } catch (err) {
    logger.error(err);
  }
}

function makeCpaymentData(originalData) {
  let path_array = originalData.folderPath.split(path.sep);
  let fileName = `${path_array.pop()}.zip`;
  originalData.fileName = fileName;
  let data = {
    type: "payment",
    token: originalData.token,
    deviceID: originalData.deviceID,
    paymentAt: originalData.paymentAt,
    state: 2,
    paymentPrice: 0,
  };
  return { originalData, data };
}

function makeSensorData(data) {
  let { folderPath, weightInfo, token, paymentAt, deviceID } = data;

  let sensorData = {
    folderPath,
    weightInfo,
    token,
    paymentAt,
    deviceID,
    type: "sensorData",
  };
  return sensorData;
}

function createSysErrorTimer(data) {
  if (!data.deviceID) data.deviceID = deviceID;
  let timeout = setTimeout(function () {
    timeoutHandler(data);
  }, 99000);
  data.timeout = timeout;
  sysErrorMap[data.token] = data;
}

function timeoutHandler(data) {
  logger.log("timeout handler: ", data);
  if (sysErrorMap[data.token]) {
    delete data.timeout;
    data.type = "sysError";
    delete sysErrorMap[data.token];
    decisionErrorHandler(data);
  }
}

function changeID2Barcode(data) {
  data.productDetails = data.productDetails || data.paymentDetails;
  logger.log("productDetails: ", data.productDetails);
  logger.log("productList: ", productList);

  if (data.productDetails && data.productDetails.length > 0) {
    data.productDetails.forEach((product) => {
      Object.keys(product).forEach((key) => {
        if (key !== "productID" && key !== "productCount") {
          logger.log("delete key: ", key);
          delete product[key];
        }
      });

      let obj = productList.filter((goods) => {
        //logger.log(goods);
        if (goods.productID === product.productID) {
          product.productID = goods.barcode;
          return true;
        }
        return false;
      });
      logger.log("obj: ", obj);
    });
  } else {
    logger.log("paymentDetails is null");
  }
  return data;
}

function changeID2EngName(data) {
  data.productDetails = data.productDetails || data.paymentDetails;
  logger.log("productDetails: ", data.productDetails);
  logger.log("productList: ", productList);

  if (data.productDetails && data.productDetails.length > 0) {
    data.productDetails.forEach((product) => {
      Object.keys(product).forEach((key) => {
        if (key !== "productID" && key !== "productCount") {
          logger.log("delete key: ", key);
          delete product[key];
        }
      });

      let obj = productList.filter((goods) => {
        if (goods.productID === product.productID) {
          product.productID = goods.productEngName;
          // product.price = product.productCount * goods.salePrice;
          product.price = goods.salePrice;
          return true;
        }
        return false;
      });
      logger.log("obj: ", obj);
    });
  } else {
    logger.log("paymentDetails is null");
  }
  return data;
}

function changeID2KorName(data) {
  data.productDetails = data.productDetails || data.paymentDetails;
  logger.log("productDetails: ", data.productDetails);
  logger.log("productList: ", productList);

  if (data.productDetails && data.productDetails.length > 0) {
    data.productDetails.forEach((product) => {
      Object.keys(product).forEach((key) => {
        if (key !== "productID" && key !== "productCount") {
          logger.log("delete key: ", key);
          delete product[key];
        }
      });

      let obj = productList.filter((goods) => {
        if (goods.productID === product.productID) {
          product.productID = goods.productName;
          product.price = product.productCount * goods.salePrice;
          return true;
        }
        return false;
      });
      logger.log("obj: ", obj);
    });
  } else {
    logger.log("paymentDetails is null");
  }
  return data;
}

function makeConfirmToken(data) {
  logger.log("makeConfirmToken: ", data);
  data.paymentMode = 2;
  data.kiccType = "confirmToken";
  data.approveResult = 1;
  data.resultCode = "0000";
  data.resultMsg = "create OK";
  data.approveNo = "111111  ";
  data.approveAt = data.paymentAt;
  data.approvePrice = data.paymentPrice;
  logger.log("makeConfirmToken: ", data);
}

function findDataInQueue(token) {
  let index = csDataQueue.length;
  let data = {};
  while (index--) {
    if (csDataQueue[index].data && csDataQueue[index].data.token == token) {
      logger.log("findDataInQueue: ", csDataQueue[index]);
      data = JSON.parse(JSON.stringify(csDataQueue[index]));
      csDataQueue.splice(index, 1);
      break;
    }
  }
  return data;
}

async function sendMail(data) {
  logger.log("sendmail: ", data);
  let transporter = nodemailer.createTransport({
    host: "smtps.hiworks.com",
    port: 465,
    secure: true,
    auth: {
      user: "gg.jiang@tricubics.com",
      pass: "eztc1234@@",
    },
  });
  let htmlData = `<h3>Type: ${
    data.type || data.event_type
  }</h3> <h3> deviceID: ${deviceID}</h3> <h3>device name: ${deviceName}</h3><h3>token: ${
    data.token || data.user_id || data.tokenID || data.data.tokenID
  }</h3>`;
  logger.log("data.dev_path: ", data.dev_path);
  logger.log("data.sensor_type: ", data.sensor_type);
  logger.log("data.dev_path: ", typeof data.dev_path);
  logger.log("data.sensor_type: ", typeof data.sensor_type);
  if (data.dev_path) {
    htmlData += `<h3>device path: ${data.dev_path}</h3>`;
  }
  if (data.sensor_type) {
    htmlData += `<h3>sensor type: ${data.sensor_type}</h3>`;
  }
  logger.log("htmlData: ", htmlData);
  //, jun.park@tricubics.com
  let To;
  if (data.type && data.type == "incomplete") {
    To = "gg.jiang@tricubics.com, jgqandshh87@hotmail.com";
  } else {
    To = "gg.jiang@tricubics.com, jgqandshh87@hotmail.com";
  }
  let info = await transporter.sendMail({
    from: "gg.jiang@tricubics.com",
    to: To,
    subject: `error type: ${data.type || data.event_type}`,
    text: `error type: ${data.type || data.event_type}`,
    html: htmlData,
  });
  logger.log("Message sent: %s", info.messageId);
  setTimeout(() => {
    if (data.type == "closeDoorError") reboot();
  }, 5000);
}

lotteClientEmitter.on("unlock", (data) => {
  data.type = "openDoor";
  iceboxEmitter.emit("openDoor", data);
  logger.log("openDoor: ", data);
  getProductList();
});

lotteClientEmitter.on("setTimer", (data) => {
  logger.log("setTImer: ", data);
  iceboxEmitter.emit("setTimer", data);
});

lotteClientEmitter.on("lockState", (data) => {
  logger.log("lockState: ", data);
  iceboxEmitter.emit("lockState", data);
});

webClientEmitter.on("openDoor", (data) => {
  logger.log("webpage opendoor: ", data);
  iceboxEmitter.emit("openDoor", data);
});

kakaoCertEmitter.on("kakaoCert", (data) => {
  logger.log("kakao cert: ", data);
  if (data == "adult") {
    isAdult = true;
    playAudioWithName("agree.mp3");
    kakaoCertTimeout = setTimeout(() => {
      logger.log("kakaoCertTimeout");
      if (isAdult) {
        isAdult = false;
        playAudioWithName("qr.mp3");
      }
    }, 20000);
  } else if (data == "child") {
    isAdult = false;
    playAudioWithName("child.mp3");
  }
});

process.on("SIGTERM", (signal) => {
  logger.log(`Process ${process.pid} received a SIGTERM signal `, signal);
});

process.on("SIGINT", (signal) => {
  logger.log(`Process ${process.pid} has been interrupted, `, signal);
  process.exit(1);
});

process.on("uncaughtException", (err) => {
  logger.log(`Uncaught Exception: ${err.message}`);
  logger.log(err);
});

process.on("unhandledRejection", (err, promise) => {
  logger.log("Unhandled rejection at ", promise, `reason: ${err.message}`);
  logger.log(err);
});
