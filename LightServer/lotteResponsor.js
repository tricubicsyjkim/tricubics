var net = require("net");
var fs = require("fs");
var logger = require("tracer").console();
var client;

function connect() {
  client = new net.Socket();
  client.connect(30001, "localhost", function () {
    logger.log("Connected responsor server");
  });

  client.on("data", function (data) {
    let msg;
    try {
      msg = JSON.parse(data);
      logger.log("Received: ", msg);
      //   if (msg.type) {
      //     eventHandler[msg.type](msg);
      //   }
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

client.write(
  JSON.stringify({
    token: "xxxxxxxxxxx123e",
    isOpened: true,
    type: "sendUnlock",
  })
);

// client.write(
//   JSON.stringify({
//     token: "xxxxxxxxxxx123e",
//     isClosed: true,
//     type: "sendDoorClosed",
//   })
// );

// client.write(
//   JSON.stringify({
//     token: "xxxxxxxxxxx123e",
//     type: "sendLockAllOpened",
//   })
// );

// client.write(
//   JSON.stringify({
//     token: "xxxxxxxxxxx123e",
//     duration: 5000,
//     type: "sendSetTimer",
//   })
// );

// client.write(
//   JSON.stringify({
//     token: "xxxxxxxxxxx123e",
//     status: 1,
//     type: "sendLockState",
//   })
// );

// client.write(
//   JSON.stringify({
//     token: "xxxxxxxxxxx123e",
//     productDetails: [
//       {
//         productID: "8801007071046",
//         productCount: 1,
//       },
//     ],
//     type: "sendDecision",
//   })
// );
