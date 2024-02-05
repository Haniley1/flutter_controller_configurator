#include <EEPROM.h>
//#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DNSServer.h>
#include <WiFiManager.h>                        // https://github.com/tzapu/WiFiManager
#include <ESP8266SSDP.h>
//#include <aREST.h>
ESP8266WebServer HTTP(80);                      // Web интерфейс для устройства
WiFiServer SERVERaREST(8080);                   // aREST и сервер для него
//aREST rest = aREST();
#define ONE_WIRE_BUS D4                       // Шина данных подключена к выводу D4 на ESP8266. Пин подключения OneWire шины, 2 (D4)
OneWire oneWire(ONE_WIRE_BUS);                  // Настройка объекта oneWire для связи с любыми устройствами OneWire
DallasTemperature sensors(&oneWire);            // Передаем ссылку на объект oneWire объекту Dallas Temperature.
DeviceAddress Thermometer;                      // Переменная для хранения адресов устройств


float operatingMaximum = 0;
float operatingMinimum = 0;
float operatingMaximumEeprom = 0;
float operatingMinimumEeprom = 0;
String data, params;
float data1;
const char *host = "api.apinjener.ru";          // Адрес нашего веб сервера
const int httpPort = 80;                        // Адрес порта для HTTPS= 443 или HTTP = 80
//------------------------------------------------------------------------
int sensorId = 2;
const char *nameAP = "Drying Chamber_2"; 
//------------------------------------------------------------------------
WiFiManager wifiManager;   //Включаем WiFiManager

void setup()
{
  Serial.begin(115200);
  EEPROM.begin(200);   // для esp8266/esp32
  //wifiManager.resetSettings();
  pinMode(D6, OUTPUT);
  wifiManager.setConfigPortalTimeout(120); // auto close configportal after n seconds
  wifiManager.autoConnect(nameAP);    //Если не удалось подключиться клиентом запускаем режим AP. Доступ к настройкам по адресу http://192.168.4.1
  HTTP_init();                                 //настраиваем HTTP интерфейс
  SSDP_init();
}

void loop()
{
  HTTP.handleClient();
  delay(5000);    
  Serial.println("Ok"); 
  delay(1000);

         
  //AREST_init();                                //включаем aREST и сервер к нему
  sensors.begin();                             // определение устройств на шине
  sensors.getAddress(Thermometer,1);
  sensors.requestTemperatures();
  data1 = sensors.getTempC(Thermometer);// Получить значение температуры
  data = (String)data1;
  params = "datchId=" + (String)sensorId + "&data=" + data;
  Serial.print("params = ");
  Serial.println(params);
  transmit2();
  //HTTP.handleClient();
  //delay(1);
  //WiFiClient client = SERVERaREST.available();
  //rest.handle(client);
  EEPROM.get(50, operatingMaximumEeprom);
  Serial.print("operatingMaximumEeprom_67 = ");
  Serial.println(operatingMaximumEeprom);
  EEPROM.get(0, operatingMinimumEeprom);
  Serial.print("operatingMinimumEeprom_70 = ");
  Serial.println(operatingMinimumEeprom);
  
  if(data1 < operatingMinimumEeprom)
  {
    Serial.print("data1_75 = ");
    Serial.println(data1);
    digitalWrite(D6, LOW);
  }
  if(data1 > operatingMaximumEeprom)
  {
    Serial.print("data1_81 = ");
    Serial.println(data1);    
    digitalWrite(D6, HIGH);
  }
    Serial.print("data1_85 = ");
    Serial.println(data1);  
  transmit1();
  delay(1);
  //WiFiClient client = SERVERaREST.available();
  //rest.handle(client);
}

void HTTP_init(void)
{
  HTTP.on("/index.html", HTTP_GET, []() {
    HTTP.send(200, "text/plain", "Test aREST");
  });
  HTTP.on("/description.xml", HTTP_GET, []() {
    SSDP.schema(HTTP.client());
  });
  HTTP.on("/temp/get-current", HTTP_GET, []() {
    HTTP.send(200, "text/plain", data);
  });
  HTTP.on("/temp/get-limit", HTTP_GET, []() {
    HTTP.send(200, "text/plain", String(operatingMinimumEeprom) + "|" + String(operatingMaximumEeprom));
  });
  HTTP.on("/temp/set-limit", HTTP_GET, handleUpdateTempLimit);
  HTTP.begin();
}

void SSDP_init(void)
{
  SSDP.begin();
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName("ESP8266");
  SSDP.setSerialNumber("001788102201");
  SSDP.setURL("index.html");
  SSDP.setModelName("temp_checker_controller");
  SSDP.setModelNumber("000000000001");
  SSDP.setModelURL("http://esp8266-arduinoide.ru/wifimanager/");
  SSDP.setManufacturer("Alexandr Rudetski");
  SSDP.setManufacturerURL("http://www.esp8266-arduinoide.ru");
}

//void AREST_init(void)
//{
//  rest.set_id("1");                    // Определяем имя name и ИД ID устройства aREST
//  rest.set_name("aRest");
//  SERVERaREST.begin();                 // Запускаем сервер
//}

void transmit1()
{
  WiFiClient httpClient;                                      //Обьявляем обьект класса WiFiClient
  httpClient.setTimeout(1000);                                //Присваиваем значение паузы (1 секундa)
  Serial.print("HTTPS Connecting");                           //Пишем в UART: Соединяемся с нашим веб сервером
  int r = 0;                                                  //Обьявляем переменную счетчика попыток подключения
  while ((!httpClient.connect(host, httpPort)) && (r < 30))
  {
    delay(100);
    Serial.print(".");
    r++;
  }                                                            //Пока пытаемся соединиться с веб сервером отправляем в UART точки
  if (r == 30) Serial.println(" Connection failed");           //Если не получилось соединиться пишем в UART, что не получилось
  else Serial.println(" Connected to web");                    //Если получилось соединиться пишем в UART, что получилось
  delay(1000);                                                 //Ждем
  httpClient.connect(host, httpPort);
  String Link;
  Link = "/pokazaniya/data?" + params;                         //Формируем строку для GET запроса
  httpClient.print(String("GET ") + Link + " HTTP/1.1\r\n" + "Host: " + host + "\r\n\r\n"); //Отправляем GET запрос через ESP
}

void transmit2()
{
  WiFiClient httpClient;                                      //Обьявляем обьект класса WiFiClient
  httpClient.setTimeout(1000);                                //Присваиваем значение паузы (1 секундa)
  Serial.print("HTTP Connecting");                           //Пишем в UART: Соединяемся с нашим веб сервером
  int r = 0;                                                  //Обьявляем переменную счетчика попыток подключения
  while ((!httpClient.connect(host, httpPort)) && (r < 30))
  {
    delay(100);
    Serial.print(".");
    r++;
  }                                                            //Пока пытаемся соединиться с веб сервером отправляем в UART точки
  if (r == 30) 
  {
    Serial.println(" Connection failed");           //Если не получилось соединиться пишем в UART, что не получилось
  }
  else 
  {
    Serial.println(" Connected to web");                    //Если получилось соединиться пишем в UART, что получилось
    delay(1000);                                                 //Ждем
    String Link2 = "/setting/setting?datchId=" + String(sensorId);                    //Формируем строку для GET запроса
    httpClient.print(String("GET ") + Link2 + " HTTP/1.1\r\n" + "Host: " + host + "\r\n\r\n"); //Отправляем GET запрос через ESP
    while (httpClient.connected())                              //Ловим ответ веб сервера
    {
      String line2 = httpClient.readStringUntil('\n');
      if (line2 == "\r") break;
    }
    while(httpClient.available())                              //Ловим строку от веб сервера
    {                
      //Формируем строку для ответа веб сервера                 
      String line = httpClient.readStringUntil('\n');
      //string.substring(from, to)
      String line1 = line.substring(0, 30) ; 
      Serial.println(line1);
      int index1 = line1.indexOf(":");
      int index2 = line1.indexOf(",");
      int index3 = line1.indexOf(":", index2);
      int index4 = line1.indexOf("}");
      String stroka1 = line1.substring(index1+1, index2);
      float maximum = stroka1.toFloat();
      operatingMaximum = maximum - 2;
      Serial.print("operatingMaximum = ");
      Serial.println(operatingMaximum); 
      String stroka2 = line1.substring(index3+1, index4);
      float minimum =  stroka2.toFloat();  
      operatingMinimum = minimum +2;
      Serial.print("operatingMinimum = ");
      Serial.println(operatingMinimum);
      EEPROM.get(50, operatingMaximumEeprom);
      if(operatingMaximumEeprom != operatingMaximum)
      {
        operatingMaximumEeprom = operatingMaximum;
        EEPROM.put(50, operatingMaximumEeprom);
        EEPROM.commit();     // для esp8266/esp32
        Serial.print("operatingMaximumEeprom = ");
        Serial.println(operatingMaximumEeprom);
      }
      EEPROM.get(0, operatingMinimumEeprom);
      if(operatingMinimumEeprom != operatingMinimum)
      {
        operatingMinimumEeprom = operatingMinimum;
        EEPROM.put(0, operatingMinimumEeprom);
        EEPROM.commit();     // для esp8266/esp32
        Serial.print("operatingMinimumEeprom = ");
        Serial.println(operatingMinimumEeprom);
      }
    }
  }
  
}

void handleUpdateTempLimit() {
  Serial.println("updateTempLimit");
  Serial.println(HTTP.hasArg("minTemp"));

  if (HTTP.hasArg("minTemp")) {
    String minTemp = HTTP.arg("minTemp");
    float minimum = minTemp.toFloat();
    Serial.println(minimum);
    operatingMinimum = minimum + 2;

    EEPROM.get(0, operatingMinimumEeprom);
    if (operatingMinimumEeprom != operatingMinimum) {
      operatingMinimumEeprom = operatingMinimum;
      EEPROM.put(0, operatingMinimumEeprom);
      EEPROM.commit();
    }
  }

  if (HTTP.hasArg("maxTemp")) {
    String maxTemp = HTTP.arg("maxTemp");
    float maximum = maxTemp.toFloat();
    operatingMaximum = maximum - 2;

    EEPROM.get(50, operatingMaximumEeprom);
    if (operatingMaximumEeprom != operatingMaximum) {
      operatingMaximumEeprom = operatingMaximum;
      EEPROM.put(50, operatingMaximumEeprom);
      EEPROM.commit();     // для esp8266/esp32
    }
  }

  HTTP.send(200, "text/plain", HTTP.arg("minTemp"));
}
