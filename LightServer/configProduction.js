const config = {
  /* cs server info */
  csHost: "cs.thecella.kr", //cs.thecella.kr  3.35.120.144
  cpaymentPort: 2472,
  cproductPort: 2471,
  deviceID: "CAR0T001",
  cpaymentPath: "/api/v1/cpayment/ws/device/",
  cproductPath: "/api/v1/cproduct/device/",
  /* kicc server info */
  kiccPort: 5000,
  memberShip: "1234567890",

  /* icebox info */
  senderPort: 3000,
  receiverPort: 3001,
  timesLimit: 3,
  closeDoorTimerLimit: 0.5 * 60 * 1000,
};

module.exports = config;
