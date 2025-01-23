var net = require("net");
var logger = require("tracer").console();

var net = require("net");

var keypress = require("keypress"),
  tty = require("tty");

let socketCopy;
let index = 552;
// make `process.stdin` begin emitting "keypress" events
keypress(process.stdin);

// listen for the "keypress" event
process.stdin.on("keypress", function (ch, key) {
  if (ch) {
    logger.log("ch: ", ch);
    if (ch == 1) {
      index++;
      setTimeout(() => {
        socketCopy.write(
          JSON.stringify({ type: "unLock", token: index.toString() })
        );
      }, 3000);
    } else if (ch == 2) {
      socketCopy.write(
        JSON.stringify({
          type: "setTimer",
          token: index.toString(),
          duration: 30000,
        })
      );
    } else if (ch == 3) {
      socketCopy.write(
        JSON.stringify({
          type: "setTimer",
          token: index.toString(),
          duration: 5000,
        })
      );
    } else if (ch == 4) {
      socketCopy.write(
        JSON.stringify({
          type: "lockState",
          token: index.toString(),
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

var server = net.createServer(function (socket) {
  socketCopy = socket;
  socket.on("data", (data) => {
    let msg = JSON.parse(data);
    logger.log("msg: ", msg);
  });
});

server.listen(30000, "127.0.0.1");
