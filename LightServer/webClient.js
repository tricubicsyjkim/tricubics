const WebSocket = require("ws");
var fs = require("fs");
var path = require("path");
var EventEmitter = require("events").EventEmitter;
var webClientEmitter = new EventEmitter();
var logger = require("./logger");

let { webHost, webPort } = require("./config");
const { deviceID, vendor } = JSON.parse(fs.readFileSync("deviceInfo.json"));
let webClientWS;

let webClientHandler = {
  openDoor: (data) => {
    openDoorHandler(data);
  },
  decision: (data) => {
    decisionHandler(data);
  },
};

try {
  if (vendor == "japan") connect();
} catch (error) {
  logger.error("connect error: ", error);
}

function connect() {
  webClientWS = new WebSocket(`ws://${webHost}:${webPort}/ls/${deviceID}`);
  logger.log("web server: ", `ws://${webHost}:${webPort}/ls/${deviceID}`);

  webClientWS.on("open", function open() {
    logger.log("connected to web server");
  });

  webClientWS.on("message", function incoming(data) {
    logger.log("from web server: ", data);
    let response = JSON.parse(data);
    let type = response.type;
    logger.log(type);
    if (webClientHandler[type]) {
      webClientHandler[type](response);
    }
  });

  webClientWS.on("close", function (e) {
    logger.log(
      "Socket is closed. Reconnect will be attempted in 1 second.",
      e.reason
    );
    setTimeout(function () {
      connect();
    }, 1000);
  });

  webClientWS.on("error", function (err) {
    logger.error("Socket encountered error: ", err.message, "Closing socket");
    webClientWS.close();
  });
}

function openDoorHandler(data) {
  logger.log("openDoorHandler: ", data);
  data.vendor = "japan";
  webClientEmitter.emit("openDoor", data);
}

webClientEmitter.on("doorOpened", (data) => {
  logger.log("opendoor res: ", data);

  if (!data.user_id && !data.token) {
    logger.log("there is no token");
  } else {
    let res = {
      token: data.user_id || data.token,
      type: "doorOpened",
      isOpened: data.is_opened || false,
      deviceID,
    };
    webClientWS.send(JSON.stringify(res));
  }
});

webClientEmitter.on("doorClosed", (data) => {
  logger.log("doorclosed res: ", data);
  if (!data.user_id && !data.token) {
    logger.log("there is no token");
  } else {
    let { token, user_id, is_closed } = data;
    let res = {
      token: token || user_id,
      type: "doorClosed",
      deviceID,
      isClosed: is_closed,
    };
    webClientWS.send(JSON.stringify(res));
  }
});

webClientEmitter.on("decision", (data) => {
  logger.log("decision res: ", data);
  let { token, tokenID, paymentPrice, paymentDetails, productDetails } = data;
  if (!tokenID && !token) {
    logger.log("there is no token");
  } else {
    let res = {
      type: "decision",
      deviceID,
      token: token || tokenID,
      paymentPrice,
      paymentDetails: paymentDetails || productDetails,
    };
    webClientWS.send(JSON.stringify(res));
  }
});

// setTimeout(function () {
//   let res = {
//     type: "decision",
//     deviceID,
//     token: "cd3de323-eed7-4a6f-9da3-beb3211b6ca1",
//     paymentPrice: 2000,
//     paymentDetails: [
//       {
//         productID: "BOTTLE_HY_BARIDREAM_ORGANIC_SOYMILK_190ML", //BOTTLE_HY_MECHNIKOV_APPLE_140ML BOTTLE_HY_BARIDREAM_ORGANIC_SOYMILK_190ML
//         productCount: 1,
//         price: 1500,
//       },
//     ],
//   };
//   webClientWS.send(JSON.stringify(res));
// }, 5000);

module.exports = { webClientEmitter };
