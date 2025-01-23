var net = require("net");
var fs = require("fs");
var EventEmitter = require("events").EventEmitter;
var lotteClientEmitter = new EventEmitter();
var logger = require("./logger");
let { deviceID, side, vendor, socketMode, lotteDebug } = JSON.parse(
  fs.readFileSync("deviceInfo.json")
);
let unlockMap = {};
let allOpenedMap = {};
let setTimerMap = {};
let lockStateMap = {};
logger.log(deviceID, " ,", side, " ,", vendor, " , ", socketMode);
var client;
var socketCopy;
var responsorCopy;
let doorStatus = "closed";

let eventHandler = {
  unLock: (data) => {
    unlockHandler(data);
  },
  setTimer: (data) => {
    setTimerHandler(data);
  },
  lockState: (data) => {
    lockStateHandler(data);
  },
};

if (vendor == "lotte" && socketMode == "client") connect();

function connect() {
  socketCopy = new net.Socket();
  socketCopy.connect(30000, "localhost", function () {
    logger.log("connected lotte server");
  });

  socketCopy.on("data", function (data) {
    let msg;
    try {
      msg = JSON.parse(data);
      logger.log("Received: ", msg);
      if (msg.type) {
        eventHandler[msg.type](msg);
      }
    } catch (error) {
      logger.log("parse data error: ", error);
    }
  });

  socketCopy.on("close", function () {
    logger.log("Connection closed");
    setTimeout(function () {
      connect();
    }, 1000);
  });
  // client.on("error", function () {
  //   setTimeout(function () {
  //     connect();
  //   }, 1000);
  // });
}

function unlockHandler(data) {
  if (doorStatus == "closed") {
    if (data.token) {
      changeToken(data);
      data.vendor = vendor;
      unlockMap[data.token] = data;
      allOpenedMap[data.token] = data;
    }
    lotteClientEmitter.emit("unlock", data);
  } else if (doorStatus == "opened") {
    logger.log("door is opened");
    let response = { ...data, status: 3, error: "is using" };
    logger.log("unlock response: ", response);
    try {
      socketCopy.write(JSON.stringify(response));
      if (!lotteDebug) socketCopy.write("\n");
    } catch (error) {
      logger.error("socketCopy write error: is using, and: ", error);
    }
  }
}

function setTimerHandler(data) {
  if (data.token) {
    changeToken(data);
    data.vendor = vendor;
    setTimerMap[data.token] = data;
    lotteClientEmitter.emit("setTimer", data);
  }
}

function lockStateHandler(data) {
  if (data.token) {
    changeToken(data);
    data.vendor = vendor;
    lockStateMap[data.token] = data;
    lotteClientEmitter.emit("lockState", data);
  }
}

lotteClientEmitter.on("unlockRes", unlockResHandler);
lotteClientEmitter.on("lockAllOpened", lockAllOpenedHandler);
lotteClientEmitter.on("doorClosed", doorClosedHandler);
lotteClientEmitter.on("decision", decisionHander);
lotteClientEmitter.on("onSetTimer", onSetTimerHandler);
lotteClientEmitter.on("onLockState", onLockStateHandler);
lotteClientEmitter.on("lockStateError", lockStateErrorHandler);
lotteClientEmitter.on("setTimerError", setTimerErrorHandler);
lotteClientEmitter.on("unlockError", unlockErrorHandler);
lotteClientEmitter.on("doorClosedError", doorClosedErrorHandler);
lotteClientEmitter.on("unpurchased", unpurchasedHandler);
lotteClientEmitter.on("nobill", nobillHandler);

function unlockResHandler(data) {
  logger.log("unlockResHandler: ", data);
  let { user_id, is_opened, type, token } = data;
  if (user_id) {
    if (unlockMap[user_id]) {
      logger.log("find unlock");
      let oriToken = changeBackToken(user_id);
      let response = { token: oriToken, type: "unLock", side };
      response.status = is_opened ? 0 : 3;
      if (is_opened) doorStatus = "opened";
      logger.log("unlock response: ", response);
      try {
        socketCopy.write(JSON.stringify(response));
        if (!lotteDebug) socketCopy.write("\n");
      } catch (error) {
        logger.error("unlockRes error: ", error);
      }
      delete unlockMap[user_id];
    }
  }
}

function lockAllOpenedHandler(data) {
  logger.log("lockAllOpenedHandler: ", data);
  let { user_id } = data;
  doorStatus = "opened";

  if (unlockMap[user_id]) {
    logger.log("find unlock in lockAllOpenedHandler");
    let oriToken = changeBackToken(user_id);
    let response = { token: oriToken, type: "unLock", side, status: 0 };
    logger.log("unlock response: ", response);
    try {
      socketCopy.write(JSON.stringify(response));
      if (!lotteDebug) socketCopy.write("\n");
    } catch (error) {
      logger.error("unlock too soon error: ", error);
    }
    logger.log("before unlockMap: ", unlockMap);
    delete unlockMap[user_id];
    logger.log("after unlockMap: ", unlockMap);
  }

  if (allOpenedMap[user_id]) {
    let oriToken = changeBackToken(user_id);
    let response = { token: oriToken, type: "doorOpened", side };
    logger.log("doorOpened response: ", response);
    try {
      socketCopy.write(JSON.stringify(response));
      if (!lotteDebug) socketCopy.write("\n");
    } catch (error) {
      logger.error("lockAllOpened error: ", error);
    }

    logger.log("before allOpenedMap: ", allOpenedMap);
    delete allOpenedMap[user_id];
    logger.log("after allOpenedMap: ", allOpenedMap);
  }
}

function doorClosedHandler(data) {
  logger.log("doorClosedHandler: ", data);
  doorStatus = "closed";
  let { token, type } = data;
  if (type != "doorClosed") type = "doorClosed";
  let oriToken = changeBackToken(token);
  let response = { token: oriToken, type, side, status: 2 };
  logger.log("doorClosed response: ", response);
  try {
    socketCopy.write(JSON.stringify(response));
    if (!lotteDebug) socketCopy.write("\n");
  } catch (error) {
    logger.error("doorClosed error: ", error);
  }

  // let decisionResult = {
  //   type: "payment",
  //   deviceID: "21112406590497202535",
  //   token,
  //   state: 1,
  //   // productDetails: [
  //   //   {
  //   //     productID: "8801007071046",
  //   //     productCount: 1,
  //   //     trayID: "22050301133637872603",
  //   //   },
  //   // ],
  //   productDetails: [],
  //   paymentAt: "2022-05-10T01:27:06Z",
  //   paymentPrice: 4000,
  //   paymentMode: 2,
  // };
  // setTimeout(function () {
  //   decisionHander(decisionResult);
  // }, 3000);
}

function decisionHander(data) {
  logger.log("decisionHander: ", data);
  if (data.type) data.type = "decision";
  if (data.productDetails) {
    data.productDetails.forEach((product) => {
      if (product.trayID) delete product.trayID;
    });
  } else {
    data.productDetails = [];
  }
  logger.log("decisionHander: ", data);
  let { type, token, productDetails, tokenID } = data;
  token = token || tokenID;
  let oriToken = changeBackToken(token);
  let response = { type, token: oriToken, productDetails, side };
  try {
    socketCopy.write(JSON.stringify(response));
    if (!lotteDebug) socketCopy.write("\n");
  } catch (error) {
    logger.error("decision error: ", error);
  }
  logger.log("decision response: ", response);
}

function onSetTimerHandler(data) {
  logger.log("onSetTimerHandler: ", data);
  let type = data.event_type;
  let token = data.user_id;
  let oriToken = changeBackToken(token);
  let response = { type, token: oriToken, duration: data.duration, side };
  try {
    socketCopy.write(JSON.stringify(response));
    if (!lotteDebug) socketCopy.write("\n");
  } catch (error) {
    logger.error("setTimer error: ", error);
  }
}

function onLockStateHandler(data) {
  logger.log("onLockStateHandler: ", data);
  let type = data.event_type;
  let token = data.user_id;
  let status = data.status;
  let oriToken = changeBackToken(token);
  let response = { type, token: oriToken, status, side };
  try {
    socketCopy.write(JSON.stringify(response));
    if (!lotteDebug) socketCopy.write("\n");
  } catch (error) {
    logger.error("lockState error: ", error);
  }
}

function lockStateErrorHandler(data) {
  logger.log("lockStateErrorHandler: ", data);
  let { token } = data;
  let oriToken = changeBackToken(token);
  let response = { type: "lockState", side, token: oriToken, status: 3 };
  try {
    socketCopy.write(JSON.stringify(response));
    if (!lotteDebug) socketCopy.write("\n");
  } catch (error) {
    logger.error("lockStateError error: ", error);
  }
}

function setTimerErrorHandler(data) {
  logger.log("setTimerErrorHandler: ", data);
  let { token } = data;
  let oriToken = changeBackToken(token);
  let response = { type: "setTimer", side, token: oriToken, duration: 0 };
  try {
    socketCopy.write(JSON.stringify(response));
    if (!lotteDebug) socketCopy.write("\n");
  } catch (error) {
    logger.error("setTimerError error: ", error);
  }
}

function unlockErrorHandler(data) {
  logger.log("unlockErrorHandler: ", data);
  let { token } = data;
  let oriToken = changeBackToken(token);
  let response = { type: "unLock", token: oriToken, side, status: 3 };
  try {
    socketCopy.write(JSON.stringify(response));
    if (!lotteDebug) socketCopy.write("\n");
  } catch (error) {
    logger.error("unlockError error: ", error);
  }
}

function doorClosedErrorHandler(data) {
  logger.log("doorClosedErrorHandler: ", data);
  let token = data.token || data.user_id;
  let oriToken = changeBackToken(token);
  let response = { type: "doorClosed", token: oriToken, side, status: 3 };
  try {
    socketCopy.write(JSON.stringify(response));
    if (!lotteDebug) socketCopy.write("\n");
  } catch (error) {
    logger.error("doorClosedError error: ", error);
  }
}

function unpurchasedHandler(data) {
  logger.log("lotteClient unpurchasedHandler: ", data);
  if (data.type) data.type = "decision";
  data.token = data.tokenID;
  if (data.productDetails) {
    data.productDetails.forEach((product) => {
      if (product.trayID) delete product.trayID;
    });
  } else {
    data.productDetails = [];
  }

  let { type, token, productDetails, tokenID } = data;
  token = token || tokenID;
  let oriToken = changeBackToken(token);
  let response = { type, token: oriToken, productDetails, side };
  try {
    socketCopy.write(JSON.stringify(response));
    if (!lotteDebug) socketCopy.write("\n");
  } catch (error) {
    logger.error("unpurchase error: ", error);
  }

  logger.log("decision response: ", response);
}

function nobillHandler(data) {
  logger.log("lotteClient nobillHandler: ", data);
  if (data.type) data.type = "decision";
  data.token = data.tokenID;
  if (data.productDetails) {
    data.productDetails.forEach((product) => {
      if (product.trayID) delete product.trayID;
    });
  } else {
    data.productDetails = [];
  }

  let { type, token, productDetails, tokenID } = data;
  token = token || tokenID;
  let oriToken = changeBackToken(token);
  let response = { type, token: oriToken, productDetails, side };
  try {
    socketCopy.write(JSON.stringify(response));
    if (!lotteDebug) socketCopy.write("\n");
  } catch (error) {
    logger.error("nobill error: ", error);
  }

  logger.log("decision response: ", response);
}

function changeToken(data) {
  data.token = data.token || data.tokenID;
  data.token = data.token + "_" + side;
  return data;
}

function changeBackToken(token) {
  let lastIndex = token.lastIndexOf(`_${side}`);
  return token.substring(0, lastIndex);
}

var server = net.createServer(function (socket) {
  socketCopy = socket;
  logger.log("get a connection");
  socket.on("data", (data) => {
    logger.log("check data: ", data.toString());
    let msg;
    try {
      msg = JSON.parse(data);
      logger.log("Received: ", msg);
      if (msg.type) {
        eventHandler[msg.type](msg);
      }
    } catch (error) {
      logger.log("parse data error: ", error);
    }
  });
});

if (socketMode == "server") {
  server.listen(30000, "0.0.0.0");
  logger.log("lotte server listen 30000");
}

var responseHandler = {
  sendUnlock: (data) => {
    sendUnlock(data);
  },
  sendLockAllOpened: (data) => {
    sendLockAllOpened(data);
  },
  sendDoorClosed: (data) => {
    sendDoorClosed(data);
  },
  sendDecision: (data) => {
    sendDecision(data);
  },
  sendSetTimer: (data) => {
    sendSetTimer(data);
  },
  sendLockState: (data) => {
    sendLockState(data);
  },
};

var resServer = net.createServer(function (socket) {
  responsorCopy = socket;
  logger.log("responsor server get a connection");
  socket.on("data", (data) => {
    logger.log("responsor server check data: ", data.toString());
    let msg;
    try {
      msg = JSON.parse(data);
      logger.log("responsor server Received: ", msg);
      if (msg.type) {
        responseHandler[msg.type](msg);
      }
    } catch (error) {
      logger.log("parse data error: ", error);
    }
  });
});

resServer.listen(30001, "0.0.0.0");
logger.log("lotte responsor server listen 30001");

function sendUnlock(data) {
  logger.log("sendUnlockRes: ", data);
  let { token, isOpened } = data;
  let response = { token, type: "unLock", side };
  response.status = isOpened ? 0 : 3;
  logger.log("unLock response: ", response);
  try {
    socketCopy.write(JSON.stringify(response));
    if (!lotteDebug) socketCopy.write("\n");
  } catch (error) {
    logger.error("sendUnlock error: ", error);
  }
}

function sendLockAllOpened(data) {
  logger.log("sendLockAllOpened: ", data);
  let response = { token: data.token, type: "doorOpened", side };
  logger.log("doorOpened response: ", response);
  try {
    socketCopy.write(JSON.stringify(response));
    if (!lotteDebug) socketCopy.write("\n");
  } catch (error) {
    logger.error("sendLockAllOpened error: ", error);
  }
}

function sendDoorClosed(data) {
  logger.log("sendDoorClosed: ", data);
  let { token, isClosed } = data;
  let response = { token, type: "doorClosed", side };
  response.status = isClosed ? 2 : 3;
  logger.log("doorClosed response: ", response);
  try {
    socketCopy.write(JSON.stringify(response));
    if (!lotteDebug) socketCopy.write("\n");
  } catch (error) {
    logger.error("sendDoorClosed error: ", error);
  }
}

function sendDecision(data) {
  logger.log("sendDecision: ", data);
  let { token, productDetails } = data;
  let response = { type: "decision", token, productDetails, side };
  logger.log("decision response: ", response);
  try {
    socketCopy.write(JSON.stringify(response));
    if (!lotteDebug) socketCopy.write("\n");
  } catch (error) {
    logger.error("sendDecision error: ", error);
  }
}

function sendSetTimer(data) {
  logger.log("onSetTimer: ", data);
  let { token, duration } = data;
  let response = { type: "setTimer", token, duration, side };
  logger.log("sendSetTimer response: ", response);
  try {
    socketCopy.write(JSON.stringify(response));
    if (!lotteDebug) socketCopy.write("\n");
  } catch (error) {
    logger.error("sendSetTimer error: ", error);
  }
}

function sendLockState(data) {
  logger.log("sendLockState: ", data);
  let { token, status } = data;
  let response = { type: "lockState", token, status, side };
  logger.log("sendLockState response: ", response);
  try {
    socketCopy.write(JSON.stringify(response));
    if (!lotteDebug) socketCopy.write("\n");
  } catch (error) {
    logger.error("sendLockState error: ", error);
  }
}

module.exports = { lotteClientEmitter };
