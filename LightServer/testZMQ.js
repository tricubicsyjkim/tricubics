var zmq = require("zeromq");
var zmqSender = zmq.socket("push");

var data = {
  folderPath:
    "/home/trc/Downloads/workspace/visionServer/images/A000006/2021_5_19/KI1AAF100045C460E4E174A36BCCF",
  weightInfo: [
    { WeightID: 6, WeightValue: -229 },
    { WeightID: 7, WeightValue: -427 },
    { WeightID: 8, WeightValue: -550 },
  ],
  token: "KI1AAF100045C460E4E174A36BCCF",
  paymentAt: "2021-06-19T02:31:47Z",
  type: "sensorData",
  deviceID: "A000006",
};

zmqSender.bindSync("tcp://0.0.0.0:9998");
var dataStr = JSON.stringify(data);
console.log(dataStr);
console.log(typeof dataStr);
zmqSender.send(dataStr);
