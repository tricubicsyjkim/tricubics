const WebSocket = require("ws");
var fs = require("fs");
var path = require("path");
var EventEmitter = require("events").EventEmitter;
var kioskEmitter = new EventEmitter();

var logger = require("./logger");
const { kioskPort } = require("./config");

const wsServer = new WebSocket.Server({ port: kioskPort });
var websocket;

wsServer.on("connection", function connection(ws, req) {
  logger.log(req.url);
  logger.log("remoteAddress: ", req.socket.remoteAddress);

  let paths = req.url.split("/");
  let event = paths.pop();
  if (event == "kiosk") websocket = ws;
  wsServer.on("message", function incoming(message) {
    if (typeof message == "string") {
      logger.log("received: %s", message);
      let obj = JSON.parse(message);
      //   eventHandler[obj.type](obj);
    }
  });
});

kioskEmitter.on("doorOpened", doorOpenedHandler);
kioskEmitter.on("lockAllOpened", lockAllOpenedHandler);
kioskEmitter.on("doorClosed", doorClosedHandler);
kioskEmitter.on("decision", decisionHandler);

function doorOpenedHandler(data) {
  logger.log("doorOpenedHandler: ", data);
  let { is_opened, user_id } = data;
  if (is_opened == true && user_id) {
    let request = { type: "doorOpened", token: user_id };
    try {
      websocket.send(JSON.stringify(request));
    } catch (error) {
      logger.log("send error: ", error);
    }
  } else {
    logger.log("door doesnot open or token is null");
  }
}

function lockAllOpenedHandler(data) {
  logger.log("lockAllOpenedHandler: ", data);
  if (data.user_id) {
    let request = { type: "lockAllOpened", token: data.user_id };

    try {
      websocket.send(JSON.stringify(request));
    } catch (error) {
      logger.log("send lockallopened error: ", error);
    }
  } else {
    logger.log("there is no token");
  }
}

function doorClosedHandler(data) {
  logger.log("doorClosedHandler: ", data);
  let { token } = data;
  if (token) {
    let request = { type: "doorClosed", token };
    try {
      websocket.send(JSON.stringify(request));
    } catch (error) {
      logger.log("send door closed error: ", error);
    }
  }
}

function decisionHandler(data) {
  logger.log("decisionHandler: ", data);
  let token = data.token || data.tokenID;
  let { paymentPrice, productDetails } = data;
  if (token) {
    let request = { token, paymentPrice, productDetails, type: "decision" };
    try {
      websocket.send(JSON.stringify(request));
    } catch (error) {
      logger.log("send decision error: ", error);
    }
  } else {
    logger.log("there is no token");
  }
}

module.exports = { kioskEmitter };
