import fetch from 'node-fetch';
import { SerialPort } from 'serialport';

const portName = '/dev/ttyACM0';
const baudRate = 115200;

const serialPort = new SerialPort({ path: portName, baudRate });

serialPort.on('open', async () => {
  console.log(`Serial Port: ${portName} - ${baudRate} bauds`);

  try {
    const response = await fetch('https://lichess.org/api/tv/feed');

    if (!response.body) {
      throw new Error("Unable to conect to lichess.org");
    }

    response.body.on('data', (chunk) => {
      const text = chunk.toString();
      console.log("Data :", text);

      serialPort.write(text + "\n", (err) => {
        if (err) {
          console.error("UART Error:", err.message);
        }
      });
    });

    response.body.on('end', () => {
      console.log("Connection closed.");
    });

  } catch (error) {
    console.error(error.message);
  }
});
