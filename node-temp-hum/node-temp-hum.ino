#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>;
#include <string.h>;

// Temperatures Pin Configuration

#define DHTPIN1 5     // what pin we're connected to
#define DHTPIN2 4
#define DHTPIN3 0

#define DHTTYPE1 DHT22   // DHT 22  (AM2302)
#define DHTTYPE2 DHT22   // DHT 22  (AM2302)
#define DHTTYPE3 DHT11   // DHT 22  (AM2302)
DHT dht(DHTPIN1, DHTTYPE1); //// Initialize DHT sensor for normal 16mhz Arduino
DHT dht2(DHTPIN2, DHTTYPE2);
DHT dht3(DHTPIN3, DHTTYPE3);

// Update these with values suitable for your network.
const char* ssid = "elpropio";
const char* password = "d798fd798f";
//const char* ssid = "LAS CHICHIS";
//const char* password = "140805092105";
//const char* mqtt_server = "192.168.1.11";
const char* mqtt_server = "192.168.0.148";
const char* clientID = "temphum1";
const char* sensor_name = "";
const char* separator = ":";

//Variables
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
char data[50];


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topico, byte* payload, unsigned int length) {
  // Conver the incoming byte array to a string
  payload[length] = '\0'; // Null terminator used to terminate the char array
  String message = (char*)payload;
  String topic = String(topico);

  Serial.print("Message arrived on topic: [");
  Serial.print(topic);
  Serial.print("], ");
  Serial.println(message);

  if(topic == "testing" && message == "testing"){
    client.publish("testing-response", "alive");
  }
  
  if(topic == "humidity" && message == "data"){
   // -------- SENSOR 1 -------------------------
    memset(msg, 0, sizeof(msg));
    memset(data, 0, sizeof(data));
    hum = dht.readHumidity();
    strcat(msg, clientID);
    strcat(msg, separator);
    sensor_name = "h1";
    strcat(msg, sensor_name);
    dtostrf(hum , 3, 2, data);
    strcat(msg, separator);
    strcat(msg, data);
    client.publish("humidity-response", msg);
    // -------------------------------------------
    
    // -------- SENSOR 2 -------------------------
    memset(msg, 0, sizeof(msg));
    memset(data, 0, sizeof(data));
    hum = dht2.readHumidity();
    strcat(msg, clientID);
    strcat(msg, separator);
    sensor_name = "h2";
    strcat(msg, sensor_name);
    dtostrf(hum , 3, 2, data);
    strcat(msg, separator);
    strcat(msg, data);
    client.publish("humidity-response", msg);
    // -------------------------------------------

    // -------- SENSOR 3 -------------------------
    memset(msg, 0, sizeof(msg));
    memset(data, 0, sizeof(data));
    hum = dht3.readHumidity() + 23;
    strcat(msg, clientID);
    strcat(msg, separator);
    sensor_name = "h-out";
    strcat(msg, sensor_name);
    dtostrf(hum , 3, 2, data);
    strcat(msg, separator);
    strcat(msg, data);
    client.publish("humidity-response", msg);
    // -------------------------------------------

        
  }
  if(topic == "temperature" && message == "data"){
    // -------- SENSOR 1 -------------------------
    memset(msg, 0, sizeof(msg));
    memset(data, 0, sizeof(data));
    temp= dht.readTemperature();
    strcat(msg, clientID);    
    strcat(msg, separator);
    sensor_name = "t1";
    strcat(msg, sensor_name);
    dtostrf(temp , 3, 2, data);
    strcat(msg, separator);
    strcat(msg, data);
    client.publish("temperature-response", msg);    
    // -------------------------------------------
    
    // -------- SENSOR 2 -------------------------
    memset(msg, 0, sizeof(msg));
    memset(data, 0, sizeof(data));
    temp= dht2.readTemperature();
    strcat(msg, clientID);    
    strcat(msg, separator);
    sensor_name = "t2";
    strcat(msg, sensor_name);
    dtostrf(temp , 3, 2, data);
    strcat(msg, separator);
    strcat(msg, data);
    client.publish("temperature-response", msg);    
    // -------------------------------------------

    // -------- SENSOR 3 -------------------------
    memset(msg, 0, sizeof(msg));
    memset(data, 0, sizeof(data));
    temp= dht3.readTemperature();
    strcat(msg, clientID);    
    strcat(msg, separator);
    sensor_name = "t-out";
    strcat(msg, sensor_name);
    dtostrf(temp , 3, 2, data);
    strcat(msg, separator);
    strcat(msg, data);
    client.publish("temperature-response", msg);    
    // -------------------------------------------
    
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("conexions", clientID);
      // ... and resubscribe
      client.subscribe("testing");
      client.subscribe("temperature");
      client.subscribe("humidity");
      // ---------------- Subscruptions
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}


void loop() {     
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
