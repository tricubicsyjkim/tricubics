const {
  cproductProtocol,
  csHost,
  cproductPort,
  cproductPath,
  timesLimit,
} = require("./config");
const fs = require("fs");
const deviceID = JSON.parse(fs.readFileSync("deviceInfo.json")).deviceID;
var logger = require("./logger");

var EventEmitter = require("events").EventEmitter;
var dbHelperEmitter = new EventEmitter();

const request = require("request");

let requestInterval;
let times = 1;

const options = {
  url: `${cproductProtocol}://${csHost}:${cproductPort}${cproductPath}${deviceID}/product/list?page=1&limit=100`,
  headers: {
    method: "GET",
    Authorization:
      "Bearer eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1SUQiOiIwMDAwMDAwMDAwMDAwMDAwMDAwMCIsInVzZXJJRCI6InRyaWN1YmljcyIsInJvbGUiOjEsInNjb3BlIjoyLCJ0b2tlblVzZSI6ImFjY2VzcyIsImV4cCI6MTkzOTA5ODkwNn0.K7u25x5T34f0blvd_j77ZwtBWirhb2j1fsGxFyeSC0RQnk9xBPD8Vwt64bLJ8KHgtjIxw_27pcQiRBYNsPquqHeTZzTrbaVi2ipKRnYR86ijY5OQlXqZ9AEKqmWA30VhrjjKHAADXXLXr_Hg3sazeCZee-BaorUaBRjcT84P1wMhefKLQGtTxDVT6yoSLkYHLsZebfDL5jCsPP40ynLBN7wra8ylXPoEbt8FMXq48ZcKnrhPbsv1EiYuh1-lO4q_OMKa1hI5EVOrQ8C6t7m4YdlxPv7RyxWTU_LJXquhIhrhnFZT9qCzozsBAwGHXGSz-CiQD14lXJswAqE_Gyrmc_2-441DAuEYWJoKJXCF2Z5mVppa6eMTy2HfCBY4eu-RdFRxepl4Dr5-X6p5M5pvs9DwQAk7wmOrY5KphQNmR5fq_TBZ5EBVvtUof-mcT5pX11oO23qpZexGRz8e6DFWL0YRZMF6o1X4LX3saE9VCSDgoDyIzzPLuxJJ-f_0mZZlaosWPoIL8w2q1kNg8G5JVB-edKoCOZG__Jr9sIXMYxKp8VDExN-jfHVv8z8Ttl6_JqgMhi0dT0gomGowH_PcXqjUCpmwWPrEObw0iz_zGTXzDGN627QeO9ILUJA-K9j6k4L0PpYVQq2CzoS-fdm0MSx3uOAtS4PVMZAB5N-zzCI",
    headers: {
      "Content-Type": "application/json",
    },
  },
};

function callback(error, response, body) {
  if (!error && response.statusCode == 200) {
    const data = JSON.parse(body);
    dbHelperEmitter.emit("queryProduct", data.products);
    resetInterval();
  } else {
    logger.log("error: ", error);
    logger.log("response: ", response);
  }
}

function getProductList() {
  requestInterval = setInterval(() => {
    if (times < timesLimit) {
      times++;
      request(options, callback);
      logger.log("option url: ", options.url);
      logger.log("start interval: ", times);
    } else {
      logger.error("request cproduct list error");
      dbHelperEmitter.emit("queryProductError", deviceID);
      resetInterval();
    }
  }, 3000);
}

function resetInterval() {
  clearInterval(requestInterval);
  times = 1;
  logger.log("clearInterval and reset request cproduct list interval");
}

module.exports = { dbHelperEmitter, getProductList };
