const { exec } = require("child_process");
var logger = require("./logger");

function playAudio() {
  exec("sh play_audio.sh paymentLater.mp3", (err, stdout, stderr) => {
    if (err) {
      logger.error(err);
    } else {
      logger.log(`play audio: ${stdout}`);
    }
  });
}

function reboot() {
  exec("sudo reboot", (err, stdout, stderr) => {
    if (err) {
      logger.error(err);
    } else {
      logger.log(`reboot: ${stdout}`);
    }
  });
}

function playAudioWithName(fileName) {
  exec(`sh play_audio.sh ${fileName}`, (err, stdout, stderr) => {
    if (err) {
      logger.error(err);
    } else {
      logger.log(`play audio: ${stdout}`);
    }
  });
}

module.exports = { playAudio, reboot, playAudioWithName };
