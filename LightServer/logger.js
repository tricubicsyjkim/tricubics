var logger = require("tracer").dailyfile({
  root: "./log",
  maxLogFiles: 365,
  allLogsFileName: "server",
  dateformat: "HH:MM:ss.l",
});

// var logger = require("tracer").console();

module.exports = logger;
