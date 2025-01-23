var net = require("net");
var fs = require("fs");
var EventEmitter = require("events").EventEmitter;
var lotteClientEmitter = new EventEmitter();
var logger = require("tracer").console();
const { v4: uuidv4 } = require("uuid");
let { deviceID, side, vendor } = JSON.parse(fs.readFileSync("deviceInfo.json"));
let unlockMap = {};
let allOpenedMap = {};
let setTimerMap = {};
let lockStateMap = {};
logger.log(deviceID, " ,", side, " ,", vendor);
var client;

let eventHandler = {
  unlock: (data) => {
    unlockHandler(data);
  },
  setTimer: (data) => {
    setTimerHandler(data);
  },
  lockState: (data) => {
    lockStateHandler(data);
  },
};

var keypress = require("keypress"),
  tty = require("tty");

keypress(process.stdin);

// listen for the "keypress" event
let token = "";
process.stdin.on("keypress", function (ch, key) {
  if (ch) {
    logger.log("ch: ", ch);

    if (ch == 1) {
      token = uuidv4();
      setTimeout(() => {
        client.write(JSON.stringify({ type: "unLock", token }));
        logger.log(JSON.stringify({ type: "unLock", token }));
      }, 3000);
    } else if (ch == 2) {
      client.write(
        JSON.stringify({
          type: "setTimer",
          token,
          duration: 30000,
        })
      );
      logger.log(
        JSON.stringify({
          type: "setTimer",
          token,
          duration: 30000,
        })
      );
    } else if (ch == 3) {
      client.write(
        JSON.stringify({
          type: "setTimer",
          token,
          duration: 5000,
        })
      );
      logger.log(
        JSON.stringify({
          type: "setTimer",
          duration: 5000,
          token,
        })
      );
    } else if (ch == 4) {
      client.write(
        JSON.stringify({
          type: "lockState",
          token,
        })
      );

      logger.log(
        JSON.stringify({
          type: "lockState",
          token,
        })
      );
      // setInterval(function () {
      //   socketCopy.write(
      //     JSON.stringify({
      //       type: "lockState",
      //       token: index.toString(),
      //     })
      //   );
      // }, 5000);
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

function connect() {
  client = new net.Socket();
  client.connect(30000, "localhost", function () {
    logger.log("Connected");
  });

  client.on("data", function (data) {
    let msg;
    try {
      msg = JSON.parse(data);
      logger.log("Received: ", msg);
      if (msg.type && msg.type == "decision") {
        token = uuidv4();
      }
    } catch (error) {
      logger.log("parse data error: ", error);
    }
  });

  client.on("close", function () {
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

connect();
module.exports = { lotteClientEmitter };
