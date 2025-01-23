let config = {
  /* cs server info */
  //csHost: "192.168.123.102", // cs.thecella.kr  43.200.66.148   192.168.123.102
  csHost: "cs.thecella.kr",
  cpaymentPort: 12472,
  cpaymentProtocol: "ws",
  cproductProtocol: "http",
  cproductPort: 2471,
  cpaymentPath: "/api/v1/cpayment/ws/device/",
  cproductPath: "/api/v1/cproduct/device/",
  /* kicc server info */
  kiccPort: 5000,
  kicctitPort: 6000,
  memberShip: "1234567890",

  /* icebox info */
  senderPort: 3000,
  receiverPort: 3001,
  timesLimit: 3,
  closeDoorTimerLimit: 16 * 60 * 1000,

  /* vision server info */
  aiHost: "125.129.230.131", //59.9.248.4 -- 3gpu   125.129.230.131 --2gpu      59.6.209.205  -- new 3gpu
  // aiHost: "localhost",
  aiPort: 8999,
  aiTopPort: 9990,
  weightInfoPath: "/api/v1/sensorData/",

  /* kiosk info */
  kioskPort: 5001,

  // profiling
  top_upload_start: 0,
  top_upload_end: 0,
  compress_start: 0,
  compress_end: 0,
  side_upload_start: 0,
  side_upload_end: 0,
  door_closed: 0,
  top_done: 0,
  get_decision: 0,
  pay_start: 0,
  pay_end: 0,

  /* japan web server */
  webHost: "3.34.119.146", //3.34.119.146  aws, 221.147.16.192 openstore
  webPort: 20000,

  /* kakao adult cert server */
  kakaoCertUrl: "http://phasecommu.synology.me:1022/api/adult",
};

module.exports = config;
