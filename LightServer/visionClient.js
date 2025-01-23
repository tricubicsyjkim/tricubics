const WebSocket = require("ws");
var fs = require("fs");
var path = require("path");
var archiver = require("archiver");
var dateFormat = require("dateformat");
var EventEmitter = require("events").EventEmitter;
var visionClientEmitter = new EventEmitter();
var logger = require("./logger");

let {
  // aiHost,
  aiPort,
  weightInfoPath,
  compress_start,
  compress_end,
  side_upload_start,
  side_upload_end,
} = require("./config");
const { deviceID, shelfNum, aiHost } = JSON.parse(
  fs.readFileSync("deviceInfo.json")
);
let visionWS;
let clientInterval;
let errorTimes = 1;
let tasks = [];
let uploadFileQueue = [];
const { performance } = require("perf_hooks");
let t0;
let t1;
let t2;
let compressT0;
let compressT1;

let isBusy = false;
let sensorDataQueue = [];
let decisionQueue = [];
let sensorDataResMap = {};

try {
  connect();
} catch (error) {
  logger.error("connect error: ", error);
}

function connect() {
  visionWS = new WebSocket(
    `ws://${aiHost}:${aiPort}${weightInfoPath}${deviceID}`
  );
  logger.log(
    "ws server: ",
    `ws://${aiHost}:${aiPort}${weightInfoPath}${deviceID}`
  );

  visionWS.on("open", function open() {
    logger.log("connected to ai server");
  });

  visionWS.on("message", function incoming(data) {
    logger.log("from vision server: ", data);
    let response = JSON.parse(data);
    let type = response.type;
    logger.log(type);
    if (visionWSHandler[type]) {
      visionWSHandler[type](response);
    }
  });

  visionWS.on("close", function (e) {
    logger.log(
      "Socket is closed. Reconnect will be attempted in 1 second.",
      e.reason
    );
    setTimeout(function () {
      connect();
    }, 1000);
  });

  visionWS.on("error", function (err) {
    logger.error("Socket encountered error: ", err.message, "Closing socket");
    visionWS.close();
  });
}

let visionWSHandler = {
  sensorData: (data) => {
    sensorDataHandler(data);
  },
  decision: (data) => {
    decisionHandler(data);
  },
};

function sensorDataHandler(data) {
  logger.log(data.dataURL);
  sensorDataResMap[data.token] = true;
  let index = sensorDataQueue.length;
  logger.log("index: ", index);
  while (index--) {
    if (sensorDataQueue[index].token == data.token) {
      break;
    }
  }
  if (index == -1) {
    logger.log("not found index");
  } else {
    let sensorData = sensorDataQueue[index];
    sensorDataQueue.splice(index, 1);
    sensorData.dataURL = data.dataURL;
    compressFolder(sensorData);
    compressFolderHandler(sensorData);
  }
}

function decisionHandler(data) {
  logger.log(data);
  const decisionResponse = {
    type: "decision",
    resultCode: "0000",
    resultMsg: "decision OK",
    token: data.token,
  };
  logger.log(decisionResponse);
  try {
    visionWS.send(JSON.stringify(decisionResponse));
  } catch (error) {
    logger.error("visionWS send error: ", error.message);
  }
  /*check completePayment or incompletePayment*/
  logger.log("check type of payment");
  data.type = "payment";
  let index = decisionQueue.length;
  logger.log("index: ", index);
  while (index--) {
    if (decisionQueue[index].token == data.token) {
      visionClientEmitter.emit("decision", data, decisionQueue[index]);
      decisionQueue.splice(index, 1);
      break;
    }
  }
  if (index == -1) logger.log("not found index");
}

function sendSensorData(data) {
  logger.log(data);
  let token = data.token;
  let folderPath = data.folderPath;
  let weightInfo = data.weightInfo;
  let paymentAt = data.paymentAt;
  let changedWeightInfo = data.changedWeightInfo;
  let sensorData = {
    folderPath,
    weightInfo,
    token,
    paymentAt,
    type: "sensorData",
    deviceID,
    changedWeightInfo,
  };
  logger.log("sensorData: ", sensorData);
  sensorDataQueue.push(sensorData);

  try {
    visionWS.send(JSON.stringify(sensorData));
  } catch (error) {
    logger.error("visionWS send sensordata error: ", error.message);
  }

  setTimeout(function () {
    if (!sensorDataResMap[sensorData.token]) {
      logger.log("resend sensor");
      try {
        visionWS.send(JSON.stringify(sensorData));
      } catch (error) {
        logger.error("visionWS send sensordata error: ", error.message);
      }
    } else {
      delete sensorDataResMap[sensorData.token];
    }
    logger.log("after sensorDataResMap: ", sensorDataResMap);
  }, 3000);
}

async function compressFolder(sensorData) {
  logger.log("compressFolder: ", sensorData);
  let folder = sensorData.folderPath;
  logger.log("folder: ", folder);
  compressT0 = performance.now();
  compress_start = performance.now();
  const output = fs.createWriteStream(folder + ".zip");
  const archive = archiver("zip", {
    zlib: { level: 9 }, // Sets the compression level.
  });
  output.on("close", async function () {
    logger.log(archive.pointer() + " total bytes");
    logger.log(
      "archiver has been finalized and the output file descriptor has closed."
    );
    // paymentFileUpload(req);
    compressT1 = performance.now();
    compress_end = performance.now();
    logger.log(
      "profiling--- compress folder takes : " +
        (compressT1 - compressT0) +
        " milliseconds."
    );
    logger.log(
      "profiling compress side image: " +
        (compress_end - compress_start) +
        " ms"
    );
    visionClientEmitter.emit("uploadFile", sensorData);
  });
  output.on("end", function () {
    console.log("Data has been drained");
  });

  archive.on("warning", function (err) {
    if (err.code === "ENOENT") {
      // log warning
    } else {
      // throw error
      throw err;
    }
  });

  archive.on("error", function (err) {
    throw err;
  });
  archive.pipe(output);
  // archive.directory(`${folder}/video2`, "video2");
  // archive.directory(`${folder}/video4`, "video4");
  // archive.directory(`${folder}/video6`, "video6");
  // archive.directory(`${folder}/video8`, "video8");
  for (let i = 1; i <= shelfNum; i++) {
    archive.directory(`${folder}/video${i * 2}`, `video${i * 2}`);
    logger.log("archive.directory");
  }
  archive.finalize();
}

function paymentFileUpload(req) {
  logger.log("paymentFileUpload: ", req);
  let deviceID = req.deviceID;
  let folderPath = req.folderPath;
  let pathArray = folderPath.split(path.sep);
  let fileName = `${pathArray.pop()}.zip`;
  req.fileName = fileName;
  decisionQueue.push(req);

  let paymentUploadWS;
  try {
    paymentUploadWS = new WebSocket(req.dataURL);
  } catch (error) {
    logger.error("paymentUploadWS error: ", error.message);
    return;
  }

  paymentUploadWS.on("open", function open() {
    logger.log("connected upload server");
    side_upload_start = performance.now();
    paymentUploadWS.send(
      JSON.stringify({
        type: "binaryStart",
        fileName,
        token: req.token,
        deviceID,
      })
    );
    t0 = performance.now();
    const fileStream = fs.createReadStream(`${folderPath}.zip`, {
      highWaterMark: 10 * 1024,
    });

    fileStream
      .on("data", async function (chunk) {
        paymentUploadWS.send(chunk);
      })
      .on("end", function () {
        t1 = performance.now();
        logger.log("upload file took " + (t1 - t0) + " milliseconds.");
        paymentUploadWS.send(JSON.stringify({ type: "binaryEnd", deviceID }));
        //fs.unlinkSync(`${folderPath}.zip`);
      })
      .on("error", function (err) {
        logger.log("read file error: ", err);
      });
  });

  paymentUploadWS.on("message", function incoming(data) {
    logger.log("from server: ", data);
    let result = JSON.parse(data);
    logger.log("upload ws: ", result);
    if (result.resultCode === "0000") {
      logger.log("upload file successed");
      fs.unlinkSync(`${folderPath}.zip`);
      t2 = performance.now();
      logger.log("profiling--- get response from server: ", t2 - t0);
      side_upload_end = performance.now();
      logger.log(
        "profiling upload side image: " +
          (side_upload_end - side_upload_start) +
          " ms"
      );
      visionClientEmitter.emit("uploadNextFile");
      paymentUploadWS.close();
    } else {
      logger.log("upload file faild");
      /* upload file again */
      paymentFileUpload(req);
    }
  });

  paymentUploadWS.on("close", function close(data) {
    logger.log("disconnected: ", data);
  });
  paymentUploadWS.on("error", (data) => {
    logger.log("error: ", data);
  });
}

function uploadFileHandler(data) {
  setTimeout(() => {
    if (!isBusy) {
      isBusy = true;
      logger.log("is no busy, start upload file");
      paymentFileUpload(data);
    } else {
      uploadFileQueue.push(data);
      logger.log("is busy, push request to queue");
    }
  }, 300);
}

function uploadNextFileHandler() {
  setTimeout(() => {
    let length = uploadFileQueue.length;
    if (length > 0) {
      paymentFileUpload(uploadFileQueue.shift());
    } else {
      isBusy = false;
      logger.log("uploadfile queue is empty");
    }
  }, 400);
}

function compressFolderHandler(sensorData) {
  let folderPath = sensorData.folderPath;
  logger.log(folderPath);
  fs.unlink(folderPath, () => {
    const output = fs.createWriteStream(folderPath + "_top.zip");
    const archive = archiver("zip", {
      zlib: { level: 9 }, // Sets the compression level.
    });
    output.on("close", async function () {
      logger.log(archive.pointer() + " total bytes");
      logger.log(
        "archiver has been finalized and the output file descriptor has closed."
      );
      visionClientEmitter.emit("compressDone", sensorData);
    });
    output.on("end", function () {
      logger.log("Data has been drained");
    });

    archive.on("warning", function (err) {
      if (err.code === "ENOENT") {
        // log warning
      } else {
        // throw error
        throw err;
      }
    });

    archive.on("error", function (err) {
      throw err;
    });
    archive.pipe(output);
    archive.directory(folderPath, false);
    archive.finalize();
  });
}

visionClientEmitter.on("sensorData", (data) => sendSensorData(data));
visionClientEmitter.on("uploadFile", (data) => uploadFileHandler(data));
visionClientEmitter.on("uploadNextFile", () => {
  uploadNextFileHandler();
});
visionClientEmitter.on("compressFolder", (folderPath) =>
  compressFolderHandler(folderPath)
);
module.exports = { visionClientEmitter };
