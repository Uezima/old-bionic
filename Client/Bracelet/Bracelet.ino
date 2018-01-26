/******************************************************
 ******************************************************
 ** / _ \| |   |  _ \  | __ )_ _/ _ \| \ | |_ _/ ___|**
 **| | | | |   | | | | |  _ \| | | | |  \| || | |    **
 **| |_| | |___| |_| | | |_) | | |_| | |\  || | |___ **
 ** \___/|_____|____/  |____/___\___/|_| \_|___\____|**
 ******************************************************                                                  
 ******************************************************
 **Created 22 Jan 2018*********************************
 **By Guilherme Uezima*********************************
 ******************************************************
 ======================================================
 * Monitoring of sensors (accelerometer) by the Wemos *
 *     D1 Mini and communication with the server      *
 ======================================================*/

#include <ESP8266WiFi.h>  //WIFI Connection
#include <Wire.h>         //I2C  Connection
#include <ArduinoJson.h>  //JSON Communication with server
#include <PubSubClient.h> //MQTT Communication

/* CONST BLOCK ======= */
const char* SSID = "***";
const char* WIFI_PASSWORD = "***";

const int SDA_PIN = 4; //I2C SDA - GPIO (WEMOS D1)
const int SCL_PIN = 5; //I2C SCL - GPIO (WEMOS D2)

const int LED_PIN = 13;

const int MPU_ADDR =      0x68; // MPU6050 ADDRESS (0x68)
const int MPU_ID =        0x75; // MPU6050 IDENTIFICATION
const int PWR_MPU =       0x6B; // ENERGY MANAGEMENT
const int GYRO_CONFIG =   0x1B; // GYROSCOPE CONFIG REGISTRY
const int ACCEL_CONFIG =  0x1C; // ACCELEROMETER CONFIG REGISTRY
const int ACCEL_XOUT =    0x3B; // ACCELEROMETER ACCEL X CONFIG REGISTRY 

const char* MQTT_SERVER = "***";
const int   MQTT_PORT = 000;
const char* MQTT_USER = "***";
const char* MQTT_PASS = "***";
const char* MQTT_TOPIC = "***";


/* VARS BLOCK ======= */
bool boardLed = false;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; 

WiFiClient client;
PubSubClient clientMQTT(client);

/* LOG BLOCK ========= */
void infoLog(String msg) {
  Serial.println("[INFO] " + msg);
}

/* WIFI BLOCK ======== */
void configureWiFi() {
  infoLog("Connection to network");
  Serial.println(SSID);

  if(WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, WIFI_PASSWORD);

  while(WiFi.status() != WL_CONNECTED){
    delay(100);
    Serial.print("|");
  }
  Serial.println("");
  infoLog("Connected in network");
}

/* CALLBACK MQTT BLOCK ======== */
void callback(char* topic, byte* payload, unsigned int length) {
  infoLog("==========================================");
  infoLog("Message arrived from MQTT Server - Topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for(int i = 0; i < length; i++){
    Serial.print((char)payload[i]);
  }

  Serial.println();
  infoLog("==========================================");
}

/* PUBSUB MQTT BLOCK ======== */
void configureMQTT() {
  clientMQTT.setServer(MQTT_SERVER, MQTT_PORT);
  clientMQTT.setCallback(callback);

  while(!clientMQTT.connected()){
    if(clientMQTT.connect("WEMOSD1Client", MQTT_USER, MQTT_PASS)){
      infoLog("Connected in MQTT server");
    }else{
      infoLog("Connection to MQTT server failed");
      Serial.println(clientMQTT.state());
    }
    delay(2000);
    Serial.print("|");
  }
}

void sendJSONMQTT(int jsonBufferSize, JsonObject& JSONencoder, char* topic) {
  char JSONmessageBuffer[jsonBufferSize];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

  if(clientMQTT.publish(topic, JSONmessageBuffer)){
    infoLog("Success sending message");
  }else{
    infoLog("Error sending message");
    infoLog("RESET");
    setup();
  }
}

/* I2C BLOCK ======= */
void configureI2C() {
  Wire.begin(SDA_PIN, SCL_PIN);
}

/* MPU6050 BLOCK ======= */
void readRawMPU() {  
  Wire.beginTransmission(MPU_ADDR);      
  Wire.write(ACCEL_XOUT);                      
  Wire.endTransmission(false);         
  Wire.requestFrom(MPU_ADDR, 14);        

  AcX = Wire.read() << 8; //Most significant bit                
  AcX |= Wire.read();     //Bit least significant                 
  AcY = Wire.read() << 8;
  AcY |= Wire.read();
  AcZ = Wire.read() << 8;
  AcZ |= Wire.read();

  Tmp = Wire.read() << 8;
  Tmp |= Wire.read();

  GyX = Wire.read() << 8;
  GyX |= Wire.read();
  GyY = Wire.read() << 8;
  GyY |= Wire.read();
  GyZ = Wire.read() << 8;
  GyZ |= Wire.read(); 

  boardLed = !boardLed;
  digitalWrite(LED_BUILTIN, boardLed);
  digitalWrite(LED_PIN, boardLed);
  delay(200);                                        
}

void sendJSONMPU() {
  StaticJsonBuffer<500> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();

  JSONencoder["AcX"] = AcX; 
  JSONencoder["AcY"] = AcY;
  JSONencoder["AcZ"] = AcZ;
  JSONencoder["Tmp"] = Tmp;
  JSONencoder["GyX"] = GyX;
  JSONencoder["GyY"] = GyY;
  JSONencoder["GyZ"] = GyZ;

  sendJSONMQTT(500, JSONencoder, "oldbionic/accelerometer");
}

uint8_t readRegMPU(uint8_t reg) {
  uint8_t data;
  Wire.beginTransmission(MPU_ADDR);     
  Wire.write(reg);                     
  Wire.endTransmission(false);          
  Wire.requestFrom(MPU_ADDR, 1);        
  data = Wire.read();                 
  return data;                          
}

void writeRegMPU(int reg, int val) {
  Wire.beginTransmission(MPU_ADDR);     
  Wire.write(reg);                     
  Wire.write(val);                      
  Wire.endTransmission(true);           
}

void findMPU(int mpu_addr) {
  Wire.beginTransmission(MPU_ADDR);
  int data = Wire.endTransmission(true);

  if(data == 0)
  {
    infoLog("MPU6050 located");
  }
  else 
  {
    infoLog("MPU6050 not found");
  }
}

void checkMPU(int mpu_addr) {
  findMPU(MPU_ADDR);
    
  int data = readRegMPU(MPU_ID);
  
  if(data == 104) 
  {
    infoLog("MPU6050 response OK! (104)");

    data = readRegMPU(PWR_MPU); 

    if(data == 64) infoLog("MPU6050 INFO MOD (64)");
    else infoLog("MPU6050 ACTIVE MOD");
  }
  else infoLog("MPU6050 ERROR");
}

void configureMPU6050() {
  //SET SENSOR MODE ACTIVE
  writeRegMPU(PWR_MPU, 0);     

 // SET GYROSCOPE SCALE
 /* FS_SEL  Full Scale Range
  *0        ± 250 °/s      0b00000000
  *1        ± 500 °/s      0b00001000
  *2        ± 1000 °/s     0b00010000
  *3        ± 2000 °/s     0b00011000
 */
  writeRegMPU(GYRO_CONFIG, 0); 

 // SET ACCELEROMETER SCALE
 /* AFS_SEL   Full Scale Range
  *0           ± 2g            0b00000000
  *1           ± 4g            0b00001000
  *2           ± 8g            0b00010000
  *3           ± 16g           0b00011000
 */
  writeRegMPU(ACCEL_CONFIG, 0); 
}

/* SETUP BLOCK ======= */
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);

  infoLog("Establishing WiFi connection");
  configureWiFi();
  configureMQTT();
  clientMQTT.subscribe("oldbionic/accelerometer/WEMOSD1");

  infoLog("Establishing MPU6050 connection");
  configureI2C();
  configureMPU6050();
  checkMPU(MPU_ADDR);

  infoLog("Configuration completed!");
}

void loop() {
  readRawMPU();
  sendJSONMPU();
  clientMQTT.loop();
  delay(2000);
}
