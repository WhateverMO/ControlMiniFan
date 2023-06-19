/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   创建一个BLE服务器，一旦我们收到连接，将会周期性发送通知

   T使用步骤：
   1. 创建一个 BLE Server
   2. 创建一个 BLE Service
   3. 创建一个 BLE Characteristic
   4. 创建一个 BLE Descriptor
   5. 开始服务
   6. 开始广播


*/
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Robojax_L298N_DC_motor.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient_Generic.h>
uint8_t txValue = 0;
BLEServer *pServer = NULL;                   // BLEServer指针 pServer
BLECharacteristic *pTxCharacteristic = NULL; // BLECharacteristic指针 pTxCharacteristic
BLECharacteristic *reTxCharacteristic = NULL;//读取数据的特征值
                                             //write的写在了下面
bool deviceConnected = false;                //本次连接状态
bool oldDeviceConnected = false;             //上次连接状态
String commandResult;
int command;
int speed;
int seconds;
bool method;
String WIFIName;
String PassWord;

//-------------------------------------------------------------------------------------------------
//此处定义风扇马达
#define CHA 0
#define ENA 17 //如果使用Arduino板，这个引脚必须是PWM启用引脚
#define IN1 25
#define IN2 26
#define motor1 1

const int CCW = 2; //此变量名不要改变,逆时针旋转
const int CW = 1;  //此变量名不要改变，顺时针旋转

Robojax_L298N_DC_motor motor(IN1, IN2, ENA, CHA, true); // true为串口调试是否开启
//-------------------------------------------------------------------------------------------------

// See the following for generating UUIDs: https://www.uuidgenerator.net/
//蓝牙服务的 UUID
#define SERVICE_UUID "0b466f5e-1c11-413e-92cf-986e42af4861" // UART service UUID
//发送数据特征值的UUID
#define CHARACTERISTIC_UUID_RX "19d8d9e0-7d45-430e-aa67-2f4169eb2e22"
//接收数据特征值的UUID
#define CHARACTERISTIC_UUID_TX "7ee6c5c4-dbb2-4981-8e22-700846a4f83f"
//读取数据的特征值
#define CHARACTERISTIC_UUID_RETX "e1841616-4da8-4ea8-8407-ac1d2439ef51"
//设备的独有ID
// #define DEVICE_UUID "3C:E9:0E:88:4A:EE"
#define DEVICE_UUID "E0:5A:1B:A8:01:A6"
//-------------------------------------------------------------------------------------------------

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
    }
};
//-------------------------------------------------------------------------------------------------
//定义WebSocket相关变量
#define USE_SERIAL Serial
#define DEBUG_ESP_PORT Serial
// #define WS_SERVER "192.168.1.121"
#define WS_SERVER "172.20.10.3"
#define WS_PORT 5000
// #define WS_SSID "dzkj-2020"
// #define WS_PASSWORD "123abc.."
#define WS_SSID "MINH-iPhone"
#define WS_PASSWORD "02xl02xl"
#define USE_SSL false


WiFiMulti wifiMulti;
WebSocketsClient webSocket;
void connectWIFI();
void WIFI_Command();
//-------------------------------------------------------------------------------------------------
//定义前端传过来的键值对
JsonObject inputValue;

JsonObject get_jsoncomm(std::string rxvalue)
{
  //设置json字符串的size
  const size_t capacity = 2048;
  DynamicJsonDocument doc(capacity);
  String input = rxvalue.c_str();
  deserializeJson(doc, input);

  JsonObject tmp = doc.as<JsonObject>();
  //JsonObject root = doc.as<JsonObject>();
  commandResult = tmp[String("command")].as<String>();
  command = commandResult.toInt();
  speed = tmp[String("speed")].as<int>();
  seconds = tmp[String("seconds")].as<int>();
  method = tmp["method"];
  WIFIName = tmp[String("params")][String("wifiname")].as<String>();
  PassWord = tmp[String("params")][String("password")].as<String>();
  Serial.print("get_jsoncomm_command:");
  Serial.println(command);
  return tmp;
}
//-------------------------------------------------------------------------------------------------
void StartScanWIFI()
{
    int n = WiFi.scanNetworks();
    int WIFI_SSID = 0;
    if (n==0)
    {
        Serial.println("no networks were found");
    }
    else
    {
        Serial.print(n);
        Serial.println("networks were found");
        for (int i = 0; i < n; ++i)
      {
        // 串口输出WiFi的名字、信号强度、是否加密
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(")");
        Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");

        //每收集到一条wifi信息发送一条通知
        String ssid = WiFi.SSID(i);
        Serial.println(ssid);
        Serial.println(ssid.length());
        try
        {
          const char *a = ssid.c_str();

          Serial.println("before type change");
          std::string tmpContent = a;
          Serial.println("end type change");
          Serial.println(tmpContent.length());

          pTxCharacteristic->setValue(tmpContent);
          pTxCharacteristic->notify();
          Serial.println("设备wifi通知已发出");
        }
        catch (std::exception e)
        {
          Serial.println(e.what());
        }
      }
    }
}
//-------------------------------------------------------------------------------------------------
//二进制转字符串
void hexdump(const void *mem, uint32_t len, uint8_t cols = 16)
{
  const uint8_t *src = (const uint8_t *)mem;

  Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);

  for (uint32_t i = 0; i < len; i++)
  {
    if (i % cols == 0)
    {
      Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
    }

    Serial.printf("%02X ", *src);
    src++;
  }
  Serial.printf("\n");
}
//-------------------------------------------------------------------------------------------------
//注册设备
void registerDevice()
{
  DynamicJsonDocument doc(1024);

  doc["MessageType"] = "Register";
  doc["Params"][0] = DEVICE_UUID;
  

  String result;
  serializeJson(doc, result);
  webSocket.sendTXT(result);
  Serial.println("-------------------------------------");
}
//-------------------------------------------------------------------------------------------------
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    Serial.println("webSocketEvent");
    Serial.println(type);

    // String array_convet_string(char *arr);
  switch (type)
  {
  case WStype_DISCONNECTED:
  {
      Serial.println("[WSc] Disconnected!");
    break;
  }
  case WStype_CONNECTED:
  {
    Serial.print("[WSc] Connected to url: ");
    Serial.println((char *)payload);
    //连接成功后发送注册设备的请求
    registerDevice();
    Serial.print("already sent RegisterMessage!");
  }
  break;
  case WStype_TEXT:
  {
    Serial.printf("[WSc] get text: %s\n", payload);
    get_jsoncomm(std::string((char *)payload));
    WIFI_Command();
    //0603 shen

    // send message to server
    //webSocket.sendTXT("message here");
  }
  break;
  case WStype_BIN:
  {
    Serial.printf("[WSc] get binary length: %u\n", length);
    hexdump(payload, length);

    // send data to server
    webSocket.sendBIN(payload, length);
  }
  break;

  case WStype_PING:
    // pong will be send automatically
    Serial.printf("[WSc] get ping\n");
    break;
  case WStype_PONG:
    // answer to a ping we send
    Serial.printf("[WSc] get pong\n");
    break;
  case WStype_ERROR:
    Serial.printf("[WSc] get ERROR\n");
    break;
  case WStype_FRAGMENT_TEXT_START:
    Serial.printf("[WSc] get FRAGMENT_TEXT\n");
    break;
  case WStype_FRAGMENT_BIN_START:
    Serial.printf("[WSc] get FRAGMENT_BIN\n");
    break;
  case WStype_FRAGMENT:
    Serial.printf("[WSc] get FRAGMENT\n");
    break;
  case WStype_FRAGMENT_FIN:
    Serial.printf("[WSc] get FRAGMENT_FIN\n");
    break;

  default:
    Serial.printf("[WSc] default\n");
    break;
  }
}
//-------------------------------------------------------------------------------------------------
void shift_gear(){
  Serial.println("fan has been on (shift gear continuously)");
  motor.brake(1);
  for(int i = 1;i<=100;i++){
    delay(10);
    motor.rotate(motor1, i, CCW);
  }
  motor.brake(1);
  for(int i = 100;i>0;i--){
    delay(10);
    motor.rotate(motor1, i, CCW);
  }
  motor.brake(1);
}

void wait_run(){
  motor.brake(1);
  Serial.println("fan has been off");
  double start = millis();
  while (true)
  {
    Serial.print(".");
    delay(300);
    if (millis() - start > 1000 * seconds)
    {
      Serial.println("time is up");
      break;
   }
  }
  motor.rotate(motor1, 10, CCW);
  Serial.println("fan has been on");
}

void run_wait(){
  motor.rotate(motor1, 10, CCW);
  Serial.println("fan has been on");
  double start = millis();
  while (true)
  {
    Serial.print(".");
    delay(300);
    if (millis() - start > 1000 * seconds)
    {
      Serial.println("time is up");
      break;
   }
  }
  motor.brake(1);
  Serial.println("fan has been off");
}

void process_other(){

  switch(command)
  {
    case 5:
    {
      Serial.println("shift gear continuously");
      shift_gear();
    }
    break;
    case 6:
    {
      Serial.print("wait ");
      Serial.print(seconds);
      Serial.println(" seconds to make the fan on");
      wait_run();
    }
    break;
    case 7:
    {
      Serial.print("make the fan on ");
      Serial.print(seconds);
      Serial.println(" seconds continuously");
      run_wait();
    }
    break;
    default:
    {
      Serial.println("not effect1");
    }
    break;
  }
}
//-------------------------------------------------------------------------------------------------
void WIFI_Command()
{
  Serial.println(command);
  Serial.println("\nWIFI");
  switch (command)
  {
    //风扇开关控制
  case 3:
  {
    if (method)
    {
      Serial.println("fan has been on");
      motor.rotate(motor1, 10, CCW);
      /*开电扇*/
    }
    else
    {
      Serial.println("fan has been off");
      motor.brake(1);
      /*关电扇*/
    }
  }
  break;
    //风扇档速调节
  case 4:
  {
    Serial.println("WIFI CONTROL");
    Serial.println(speed);
    if (speed == 1)
    {
      Serial.println("1 Gear");
      motor.rotate(motor1, 40, CCW);
    }
    else if (speed == 2)
    {
      Serial.println("2 Gear");
      motor.rotate(motor1, 70, CCW);
    }
    else if (speed == 3)
    {
      Serial.println("3 Gear");
      motor.rotate(motor1, 100, CCW);
    }
    else
    {
      Serial.println("not effect");
    }
  }
  break;
  default:
  {
    Serial.println("");
    process_other();
  }
  break;
  }
}
//-------------------------------------------------------------------------------------------------
void BLE_command()
{
  Serial.println("");
  Serial.println("BLE");
  Serial.print("BLE_command:");
  Serial.println(command);

  switch (command)
  {
  //扫描有那些WIFI
  case 1:
  {
    StartScanWIFI();
  }
  break;
  //连接WIFI
  case 2:
  {
    Serial.println("------------------------------------------------");
    Serial.println(WIFIName);
    Serial.println(PassWord);
    Serial.println("------------------------------------------------");
    //接收从手机端传过来的WIFI名称密码
    //c_str() 函数可以将 const string* 类型 转化为 cons char* 类型
    const char *name = WIFIName.c_str();
    const char *psw = PassWord.c_str();
    WiFi.begin(name,psw);
    //millis函数可以用来获取Arduino开机后运行的时间长度，该时间长度单位是毫秒,最长可记录接近50天左右的时间。如果超出记录时间上限，记录将从0重新开始。
    double start = millis();
    //isOverTime用来判断WIFI连接是否已经超时
    int isOverTime = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WIFI CONNECTING...");
        delay(1000);
        if (millis() - start >10000)
        {
            isOverTime = 1;
            break;
        }
    }
    if (!isOverTime)
    //如果是否连接超时这个参数为0，则表示为没有超时，即连接成功
    {
        pTxCharacteristic -> setValue("true");
        Serial.println("SUCCESS!");
        reTxCharacteristic -> setValue("WIFI CONNECTED!");
        connectWIFI();
    }
    else
    {
        pTxCharacteristic -> setValue("false");
        Serial.println("Fail to connect!");
        reTxCharacteristic -> setValue("WIFI NOT CONNECTED!");
    }
    pTxCharacteristic -> notify();
  }
  break;
   //风扇开关控制
  case 3:
  {
    if (method)
    {
      Serial.println("fan has been on");
      motor.rotate(motor1, 10, CCW);
      /*开电扇*/
    }
    else
    {
      Serial.println("fan has been off");
      motor.brake(1);
      /*关电扇*/
    }
       //fanControl(fanState);
  }
  break;
    //风扇档速调节
  case 4:
  {
    if (speed == 1)
    {
      Serial.println("1 gear");
      motor.rotate(motor1, 30, CCW);
    }
    else if (speed == 2)
    {
      Serial.println("2 gear");
      motor.rotate(motor1, 75, CCW);
    }
    else if (speed == 3)
    {
      Serial.println("3 gear");
      motor.rotate(motor1, 100, CCW);
    }
    else
    {
      Serial.println("无效命令");
    }
  }
  break;
  default:
    process_other();
  break;
 }
}
//-------------------------------------------------------------------------------------------------
//打开websocket长链接
void connectWIFI()
{
    wifiMulti.addAP(WS_SSID, WS_PASSWORD);
    double start = millis();
    bool OverTime = false;

    while (wifiMulti.run() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
        if (millis() - start > 10000)//如果成立则连接失败
        {
            Serial.println("fail");
            return;
        }
    }
    //客户端地址
    Serial.print("WebSockets Client Start at IP : ");
    Serial.print(WiFi.localIP());
    //服务器地址和端口
    Serial.print("Connecting to Server at:");
    Serial.println(WS_SERVER);

    //第一：这几个宏定义只是决定代码块是否被编译！
    //第二：别忘了#endif
    //#if和#endif是一组同时使用的，叫做条件编译指令。#if与#define、#include等指令一样是由预处理器这个强大的工具处理的，预处理器可以在编译前处理c程序。
    #if USE_SSL
    webSocket.beginSSL(WS_SERVER,WS_PORT);
    #else
    webSocket.begin(WS_SERVER,WS_PORT,"/ws");
    #endif
    Serial.println("here we are");
    // event handler
    webSocket.onEvent(webSocketEvent);

    Serial.println("hello!!");

    // try every 5000 if connection has failed
    webSocket.setReconnectInterval(5000);

    webSocket.disableHeartbeat();

    Serial.print("Connected to WebSockets Server at IP address: ");
    Serial.println(WS_SERVER);
    
}
//-------------------------------------------------------------------------------------------------

//设备接收手机端命令的Callback
class MyCallbacks : public BLECharacteristicCallbacks
{
  
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    Serial.println("got Ble");
    std::string rxvalue = pCharacteristic->getValue();
    Serial.println("begin");
    get_jsoncomm(rxvalue);
    Serial.print("onWriteCommand:");
    Serial.println(command);
    Serial.println("end");
    BLE_command();
  }
};
//-------------------------------------------------------------------------------------------------

void setup()
{
    Serial.begin(115200); //设置串口为115200

    //------------------------------------------------------
    //风扇马达
    motor.begin();

    Serial.println("start self-test");
    
		seconds = 1;
		shift_gear();
		delay(1000);
		run_wait();
		delay(1000);
		wait_run();
		motor.brake(1);
		seconds = 0;

    Serial.println("self-test done");

    // 创建一个 BLE 设备
    BLEDevice::init("jj and mw's BLE Device");

    // 创建一个 BLE 服务
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks()); //设置回调
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // 创建一个 BLE 特征
    pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
    pTxCharacteristic->addDescriptor(new BLE2902());
    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
    pRxCharacteristic->setCallbacks(new MyCallbacks()); //设置回调
    reTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RETX, BLECharacteristic::PROPERTY_READ);//设置WIFI

    pService->start();                  // 开始服务
    pServer->getAdvertising()->start(); // 开始广播
    Serial.println(" Waiting... ");

    Serial.begin(115200);
    //------------------------------------------------------
    //WIFI
    connectWIFI();
    //设置wifi的连接状态
    if(wifiMulti.run() != WL_CONNECTED)
    {
        reTxCharacteristic->setValue("WIFI Not Connected");
    }else{
        reTxCharacteristic->setValue("WIFI Connected");
    }
}

void loop()
{
    // deviceConnected 已连接
    if (deviceConnected)
    {
        pTxCharacteristic->setValue(&txValue, 1); // 设置要发送的值为1
        pTxCharacteristic->notify();              // 广播
        txValue++;                                // 指针地址自加1
        delay(500);                              // 如果有太多包要发送，蓝牙会堵塞
    }

    // disconnecting  断开连接
    if (!deviceConnected && oldDeviceConnected)
    {
        delay(500);                  // 留时间给蓝牙缓冲
        pServer->startAdvertising(); // 重新广播
        Serial.println(" Start Advertising... ");
        oldDeviceConnected = deviceConnected;
    }

    // connecting  正在连接
    if (deviceConnected && !oldDeviceConnected)
    {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }

    webSocket.loop();
}
