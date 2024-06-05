#include <ESP8266WiFiMulti.h>
#include <Wire.h>
#include <SparkFun_SCD4x_Arduino_Library.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

ESP8266WiFiMulti wifiMulti;

// WiFi Device name
#define DEVICE "SenseairS8"
// WiFi AP SSID
#define WIFI_SSID "Wifi_SSID"
// WiFi password
#define WIFI_PASSWORD "Wifi_password"
// InfluxDB v2 server url, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_URL "http://your-influxdb-ip:8086"
// InfluxDB v2 server or cloud API token (Use: InfluxDB UI -> Data -> API Tokens -> Generate API Token)
#define INFLUXDB_TOKEN "your-influxdb_token"
// InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "your-influxdb-org"
// InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define INFLUXDB_BUCKET "your-influxdb-bucket"
// Timezone
#define TZ_INFO "GMT+0BST-1,M3.5.0/01:00:00,M10.5.0/02:00:00"

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Data point
Point sensor_db("SCD4x");

SCD4x mySensor;

void setup() {
  Serial.begin(115200);
  Serial.println(F("SCD4x Example"));

  // Setup WiFi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  // Wait for WiFi connection
  Serial.print("Connecting to WiFi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  // Initialize the SCD4x sensor
  Wire.begin();
  if (!mySensor.begin()) {
    Serial.println(F("Sensor not detected. Please check wiring. Freezing..."));
    while (1);
  }
}

void loop() {
  if (mySensor.readMeasurement()) {
    // Clear fields for reusing the point. Tags will remain untouched
    sensor_db.clearFields();

    // Store measured values into point, any can be deleted if not required
    sensor_db.addField("co2_ppm", mySensor.getCO2());
    sensor_db.addField("temperature_c", mySensor.getTemperature());
    sensor_db.addField("humidity_rh", mySensor.getHumidity());

    // Check WiFi connection and reconnect if needed
    if (wifiMulti.run() != WL_CONNECTED) {
      Serial.println("WiFi connection lost");
    }

    // Write point
    if (!client.writePoint(sensor_db)) {
      Serial.print("InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
    } else {
      Serial.println("Data sent to InfluxDB");
    }
  } else {
    Serial.print(F("."));
  }

  delay(60000); // Wait for 60 seconds between measurements
}
