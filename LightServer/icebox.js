let {
  senderPort,
  receiverPort,
  timesLimit,
  closeDoorTimerLimit,
  door_closed,
} = require("./config");
var logger = require("./logger");

const fs = require("fs");
const { deviceID, vendor } = JSON.parse(fs.readFileSync("deviceInfo.json"));
var dateFormat = require("dateformat");
var moment = require("moment");
var path = require("path");
var EventEmitter = require("events").EventEmitter;
var iceboxEmitter = new EventEmitter();

var zmq = require("zeromq");
var zmqSender = zmq.socket("push");
var zmqReceiver = zmq.socket("pull");

let closeDoorTimeout;
let intervalQueue = [];

const { performance } = require("perf_hooks");
let rmImgT0;
let rmImgT1;
let rmCount = 0;
let zmqReceiverHandler = {
  DOOR_OPENED: (data) => {
    doorOpenedHandler(data);
  },
  DOOR_CLOSED: (data) => {
    doorClosedHandler(data);
  },
  RELEASE_INFO: (data) => {
    releaseInfoHandler(data);
  },
  deviceOpen: (data) => {
    deviceOpenedHandler(data);
  },
  sendImage: (data) => {
    sendImageHandler(data);
  },
  topviewDone: (data) => {},
  lockAllOpened: (data) => {
    lockAllOpenedHandler(data);
  },
  setTimer: (data) => {
    onSetTimerHandler(data);
  },
  lockState: (data) => {
    onLockStateHandler(data);
  },
  closeDoorError: (data) => {
    onCloseDoorErrorHandler(data);
  },
  deadboltError: (data) => {
    onDeadboltErrorHandler(data);
  },
  sensorError: (data) => {
    onSensorErrorHandler(data);
  },
};

let openDoorTime;
let openDoorTimestamp;

zmqSender.bindSync(`tcp://0.0.0.0:${senderPort}`);
zmqReceiver.bindSync(`tcp://0.0.0.0:${receiverPort}`);
logger.log(`mq bound to port ${senderPort}, ${receiverPort}`);

iceboxEmitter.on("openDoor", openDoorHandler);
iceboxEmitter.on("queryProduct", queryProductHandler);
iceboxEmitter.on("deviceOpen", deviceOpenHandler);
iceboxEmitter.on("setTimer", setTimerHandler);
iceboxEmitter.on("lockState", lockStateHandler);

zmqReceiver.on("message", function (data) {
  let index = data.lastIndexOf("}");
  if (index > 0) {
    let subBuf = data.subarray(0, index + 1);
    let response;
    try {
      response = JSON.parse(subBuf.toString("utf8"));
    } catch (error) {
      logger.error("JSON parse error: ", error);
    }
    logger.log("from sensor sytstem: ", subBuf.toString("utf8"));
    if (response && response.event_type) {
      if (response.user_id || response.deviceID) {
        zmqReceiverHandler[response.event_type](response);
      } else {
        logger.error("token is null: ", response);
      }
    } else {
      logger.error("from sensor sytstem without event_type: ", response);
    }
  }
});

function openDoorHandler(data) {
  logger.log("from main: ", data);
  openDoorTime = getUTCTime();
  openDoorTimestamp = performance.now();
  sendSystemRequest(data, "openDoorError");
}

function queryProductHandler(data) {
  let msg = JSON.stringify({ objs: data });
  logger.log(msg);
  let filePath = path.resolve("./products.json");

  fs.writeFile(filePath, msg, (err) => {
    if (err) {
      throw err;
    }
    logger.log("JSON data is saved.");
    /* query product list */
    let reqBuffer = formateZmqRequest({ type: "queryProduct", path: filePath });
    zmqSender.send(reqBuffer);
    logger.log("sent buffer to sensor system: ", reqBuffer);
  });
}

function deviceOpenHandler(data) {
  logger.log("deviceOpenHandler: ", data);
  sendSystemRequest(data, "deviceOpenError");
}

function setTimerHandler(data) {
  sendSystemRequest(data, "setTimerError");
}

function lockStateHandler(data) {
  sendSystemRequest(data, "lockStateError");
}

function sendSystemRequest(data, type) {
  logger.log(data);
  let reqBuffer = formateZmqRequest(data);
  zmqSender.send(reqBuffer);

  let times = 1;
  let interval = setInterval(function () {
    if (times < timesLimit) {
      times++;
      zmqSender.send(reqBuffer);
      logger.log("start interval: ", times, " , data: ", data);
    } else {
      data.type = type;
      iceboxEmitter.emit(type, data);
      logger.error(type);
      resetInterval(data.token || data.user_id || data.deviceID);
    }
  }, 5000);
  intervalQueue.push({
    user_id: data.token || data.user_id || data.deviceID,
    interval,
  });
}

function resetInterval(user_id) {
  logger.log("resetInterval user_id: ", user_id);
  if (user_id) {
    intervalQueue = intervalQueue.filter(function (item) {
      if (item.user_id == user_id) {
        logger.log("found interval");
        clearInterval(item.interval);
      }
      return item.user_id != user_id;
    });
  } else {
    logger.log("not found this user_id: ", user_id);
  }
  logger.log("intervalQueue after: ", intervalQueue);
  logger.log("clearInterval");
}

//define the data format will be sent to system
function formateZmqRequest(data) {
  let reqBuffer = Buffer.alloc(2048);
  let reqData = {
    event_type: data.type,
    user_id: data.token || data.deviceID,
    inventory_list: {},
    ...data,
  };
  reqData = JSON.stringify(reqData);
  reqBuffer.write(reqData);
  logger.log("sent buffer to icebox: ", reqData);
  return reqBuffer;
}

function startCloseDoorTimer(data) {
  logger.log("startCloseDoorTimer");
  closeDoorTimeout = setTimeout(() => {
    logger.log("sensor system cannot close door");
    clearTimeout(closeDoorTimeout);
    data.type = "closeDoorError";
    iceboxEmitter.emit("closeDoorError", data);
  }, closeDoorTimerLimit);
}

function doorOpenedHandler(data) {
  logger.log("from sensor system: ", data);
  logger.log(data.hasOwnProperty("is_opened"));
  if (data.hasOwnProperty("is_opened")) {
    data.type = "doorOpened";
    iceboxEmitter.emit("doorOpened", data);
    startCloseDoorTimer(data);
  } else {
    logger.log("from sensor system without is_opened: ", data);
    data.errorMsg = "Lock is broken";
    data.type = "openDoorError";
    iceboxEmitter.emit("openDoorError", data);
  }
  resetInterval(data.user_id);
}

function doorClosedHandler(data) {
  logger.log("from sensor system: ", data);
  door_closed = performance.now();
  clearTimeout(closeDoorTimeout);
  let interval = performance.now() - openDoorTimestamp;
  logger.log("shopping interval is: ", interval, " ms");

  data.paymentAt = openDoorTime;
  data.token = data.user_id;
  let obj = {
    type: "releaseInfo",
    token: data.user_id,
    paymentAt: data.paymentAt,
  };
  sendSystemRequest(obj, "releaseInfoError");
  if (vendor == "hysong") iceboxEmitter.emit("kiosk", data);

  if (data.hasOwnProperty("errorList")) {
    data.type = "decisionError";
    data.deviceID = deviceID;
    iceboxEmitter.emit(data.type, data);
    return;
  } else {
    data.type = "doorClosed";
  }
  if (!data.hasOwnProperty("weightInfo")) {
    data.weightInfo = [];
  }

  /* check changed info*/
  checkChangedInfo(data);
  if (data.hasOwnProperty("changedInfo")) {
    logger.log("checkChangedInfo");
    // checkChangedInfo(data);
    delete data.changedInfo;
  }
  if (data.hasOwnProperty("changedWeightInfo")) {
    if (data.changedWeightInfo.length > 0) {
      data.changedWeightInfo.forEach((info) => {
        formatTimestamp4Vision(info);
      });
    }
  } else {
    data.changedWeightInfo = [];
  }
  logger.log("before send to main", data);
  iceboxEmitter.emit("topviewDone", data); //change
  let totalWeight = data.weightInfo.reduce(
    (pre, ele) => pre + ele.WeightValue,
    0
  );
  if (totalWeight > 20 && interval > 5000) {
    data.type = "decisionError";
    data.deviceID = deviceID;
    data.errorType = "return case";
    logger.log("catch return case");
  }
  logger.log("data.type: ", data.type);
  iceboxEmitter.emit(data.type, data);
}

function releaseInfoHandler(data) {
  logger.log("from sensor system: ", data);
  resetInterval(data.user_id);
  iceboxEmitter.emit("releaseInfo", data);
}

function deviceOpenedHandler(data) {
  logger.log("deviceOpenedHandler: ", data);
  resetInterval(data.deviceID);
  let response = {};
  response.type = data.event_type;
  response.deviceID = data.deviceID;
  if (data.is_opened) {
    response.resultCode = "0000";
    response.resultMsg = "success";
  } else {
    response.resultCode = "0001";
    response.resultMsg = "failure";
  }
  response.trID = getTransactionID();
  iceboxEmitter.emit("deviceOpened", response);
}

function sendImageHandler(data) {
  // logger.log("sendImageHandler: ", data);
  iceboxEmitter.emit("sendImage", data); //change
}

function lockAllOpenedHandler(data) {
  logger.log("lockAllOpenedHandler: ", data);
  iceboxEmitter.emit("lockAllOpened", data);
}

function onSetTimerHandler(data) {
  logger.log("onSetTimerHandler ", data);
  resetInterval(data.user_id);
  iceboxEmitter.emit("onSetTimer", data);
}

function onLockStateHandler(data) {
  logger.log("onLockStateHandler ", data);
  resetInterval(data.user_id);
  iceboxEmitter.emit("onLockState", data);
}

function onCloseDoorErrorHandler(data) {
  logger.log("onCloseDoorErrorHandler ", data);
  iceboxEmitter.emit("closeDoorError", data);
}

function onDeadboltErrorHandler(data) {
  logger.log("onDeadboltErrorHandler");
  iceboxEmitter.emit("deadboltError", data);
}

function onSensorErrorHandler(data) {
  logger.log("onSensorErrorHandler");
  iceboxEmitter.emit("sensorError", data);
}

function getUTCTime() {
  let now = new Date();
  let date = dateFormat(now, "UTC:yyyy-mm-dd'T'HH:MM:ss'Z'").toString();
  logger.log("get open door time :", date);
  return date;
}

function checkChangedInfo(data) {
  let timeInfoMap = getTimeInfoMap(data);
  // logger.log("timeInfoMap: ", timeInfoMap);
  rmImgT0 = performance.now();
  checkImgFolder(timeInfoMap, data);
  rmImgT1 = performance.now();
  logger.log("remove image takes: ", rmImgT1 - rmImgT0 + " ms");
}

function getTimeInfoMap(data) {
  let timeInfoMap = {};
  let changedInfo = data.changedInfo;
  if (changedInfo) {
    changedInfo.forEach((info) => {
      let shelfId = info.shelf_id;
      let d = new Date(info.start_time);
      logger.log("start time: ", d);
      if (!timeInfoMap[shelfId]) {
        timeInfoMap[shelfId] = [];
      }
      timeInfoMap[shelfId].push(info);
    });
  }
  return timeInfoMap;
}

function checkImgFolder(timeInfoMap, data) {
  let folderPath = data.folderPath;
  logger.log("folderPath: ", folderPath);
  let dirs = getNameList(folderPath, "dir");
  dirs.forEach((dir) => {
    imageFilter(timeInfoMap, folderPath, dir);
  });
}

function getNameList(folder, type) {
  if (!fs.existsSync(folder)) {
    fs.mkdirSync(`${folder}/video0`, { recursive: true });
  }

  let files = fs.readdirSync(folder);
  if (type == "dir") {
    let dics = files.filter((file) =>
      fs.statSync(`${folder}/${file}`).isDirectory()
    );
    return dics;
  } else if (type == "file") {
    let images = [];
    files.forEach((file) => {
      let stats = fs.statSync(`${folder}/${file}`);
      if (stats.isFile()) {
        /* check file name and covert to date*/
        images.push(`${folder}/${file}`);
      }
    });
    return images;
  }
}

function getShelfId(dic) {
  let shelfId = parseInt(dic.slice(5)) / 2;
  return shelfId;
}

function imageFilter(timeInfoMap, folderPath, dir) {
  let shelfId = getShelfId(dir);
  if (timeInfoMap[shelfId] || shelfId == 0) {
    // if (timeInfoMap[shelfId]) {
    rmImgWithTimestamp(folderPath, dir, timeInfoMap, shelfId);
  } else {
    logger.log("loadcell doesnot change: ", shelfId);
    logger.log(`${folderPath}/$${dir}`);
    fs.rmSync(`${folderPath}/${dir}`, { recursive: true });
  }
}

function getTimeWithName(fileName) {
  let file = fileName.split("/").pop();
  let dateStr = file.split(".")[0];
  let dateArr = dateStr.split("_");
  let year = dateArr[0].slice(0, 4);
  let month = dateArr[0].slice(4, 6);
  let day = dateArr[0].slice(6, 8);
  let hour = dateArr[1].slice(0, 2);
  let minite = dateArr[1].slice(2, 4);
  let second = dateArr[1].slice(4, 6);
  let mill = dateArr[2];
  let date = new Date(
    `${year}-${month}-${day}T${hour}:${minite}:${second}.${mill}Z`
  );
  let milliseconds = date.getTime() - 9 * 60 * 60 * 1000;
  return milliseconds;
}

function checkTimeStamp(timeInfoMap, shelfId, timeStamp) {
  let isRemove = true;
  let timeInfoData = [];
  if (shelfId == 0) {
    timeInfoData = timeInfoMap;
  } else {
    timeInfoData = timeInfoMap[shelfId];
  }
  timeInfoData.forEach((info) => {
    if (timeStamp >= info.start_time && timeStamp <= info.end_time) {
      isRemove = false;
    }
  });
  return isRemove;
}

function formatTimestamp4Vision(info) {
  let startChangeObj = moment(info.startChange);
  let stopChangeObj = moment(info.stopChange);
  info.startChange = startChangeObj.format("YYYYMMDD_HHmmss_SSS");
  info.stopChange = stopChangeObj.format("YYYYMMDD_HHmmss_SSS");
}

function rmImgWithTimestamp(folderPath, dir, timeInfoMap, shelfId) {
  let fileList = getNameList(`${folderPath}/${dir}`, "file");
  let totoalTimeInfo = [];
  let timeinfoArr = Object.values(timeInfoMap);
  let minStartTime = Number.MAX_SAFE_INTEGER;
  let maxEndTime = 0;
  timeinfoArr.forEach((info) => {
    totoalTimeInfo.push(...info);
  });

  totoalTimeInfo.forEach((info) => {
    if (info.start_time < minStartTime) minStartTime = info.start_time;
    if (info.end_time > maxEndTime) maxEndTime = info.end_time;
  });
  totoalTimeInfo = [{ start_time: minStartTime, end_time: maxEndTime + 400 }];

  // logger.log("minStartTime: ", minStartTime, " , maxEndTime: ", maxEndTime);

  fileList.forEach((file) => {
    let timeStamp = getTimeWithName(file);
    let isRemove = true;
    if (shelfId == 0) {
      isRemove = false;
    } else {
      isRemove = checkTimeStamp(timeInfoMap, shelfId, timeStamp);
    }
    if (isRemove) {
      rmCount++;
      fs.unlinkSync(file);
    }
  });
}

function getTransactionID() {
  let trID =
    dateFormat(new Date(), "yymmddHHMMss") +
    Math.floor(Math.random() * 99999999);
  return trID;
}

module.exports = { iceboxEmitter };
