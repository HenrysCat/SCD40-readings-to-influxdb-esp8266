With this code, you can send CO2, temperature, and humidity readings to InfluxDB using an ESP8266 dev board and an SCD40/41 sensor

install this library in ArduinoIDE first (it is also available directly from the Arduino IDE library manager)

https://github.com/sparkfun/SparkFun_SCD4x_Arduino_Library

Now write the code to your esp device with Arduino IDE, this will send readings every 60 seconds, you can change this by editing the line 'Serial.println("Wait 60s"); delay(60000);;' to whatever you want.

The SCD4x sensors are calibrated to 400ppm, if you want to bring closer to real world CO2 level (around 420ppm) you can edit line 73 as below, in this example Ihave added 20 to the logged reading

sensor_db.addField("co2_ppm", mySensor.getCO2() + 20);
