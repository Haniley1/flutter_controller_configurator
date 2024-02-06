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
#include <Ticker.h>

#define ONE_WIRE_BUS D4                       // Шина данных подключена к выводу D4 на ESP8266. Пин подключения OneWire шины, 2 (D4)

ESP8266WebServer HTTP(80);                      // Web интерфейс для устройства
WiFiServer SERVERaREST(8080);                   // aREST и сервер для него

OneWire oneWire(ONE_WIRE_BUS);                  // Настройка объекта oneWire для связи с любыми устройствами OneWire
DallasTemperature sensors(&oneWire);            // Передаем ссылку на объект oneWire объекту Dallas Temperature.
DeviceAddress Thermometer;                      // Переменная для хранения адресов устройств

Ticker tempTicker;
Ticker tempLimitTicker;
Ticker delayTicker;

int httpCounter = 0;
float operatingMaximum = 0;
float operatingMinimum = 0;
float operatingMaximumEeprom = 0;
float operatingMinimumEeprom = 0;
String temperature, params;
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

  tempTicker.attach(2, test);
  // tempTicker.attach_ms(15000, collectTemperature);
  // tempLimitTicker.attach_ms(10000, getTempLimit);
}

void loop()
{
  HTTP.handleClient();
  delay(1);
}

void test() {
  WiFiClient httpClient;                                      //Обьявляем обьект класса WiFiClient
  Serial.println("Test1");
  Serial.println("Test2");
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
    HTTP.send(200, "text/plain", temperature);
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

void sendCurrentTemp()
{
  params = "datchId=" + (String)sensorId + "&temperature=" + temperature;
  Serial.print("params = ");
  Serial.println(params);

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
  Link = "/pokazaniya/temperature?" + params;                         //Формируем строку для GET запроса
  httpClient.print(String("GET ") + Link + " HTTP/1.1\r\n" + "Host: " + host + "\r\n\r\n"); //Отправляем GET запрос через ESP
}

void getTempLimit()
{
  WiFiClient httpClient;                                      //Обьявляем обьект класса WiFiClient
  httpClient.setTimeout(1000);                                //Присваиваем значение паузы (1 секундa)
  Serial.print("HTTP Connecting");                           //Пишем в UART: Соединяемся с нашим веб сервером
  int r = 0;                                                  //Обьявляем переменную счетчика попыток подключения
  while ((!httpClient.connect(host, httpPort)) && (r < 30)) {
    delay(100);
    Serial.print(".");
    r++;
  }                                                            //Пока пытаемся соединиться с веб сервером отправляем в UART точки
  if (r == 30) {
    Serial.println(" Connection failed");           //Если не получилось соединиться пишем в UART, что не получилось
  } else {
    Serial.println(" Connected to web");                    //Если получилось соединиться пишем в UART, что получилось
    delay(1000);                                                 //Ждем
    String Link2 = "/setting/setting?datchId=" + String(sensorId);                    //Формируем строку для GET запроса
    httpClient.print(String("GET ") + Link2 + " HTTP/1.1\r\n" + "Host: " + host + "\r\n\r\n"); //Отправляем GET запрос через ESP
    while (httpClient.connected()) {                             //Ловим ответ веб сервера
      String line2 = httpClient.readStringUntil('\n');
      if (line2 == "\r") break;
    }
    while(httpClient.available()) {                             //Ловим строку от веб сервера
      //Формируем строку для ответа веб сервера                 
      String line = httpClient.readStringUntil('\n');
      String line1 = line.substring(0, 30); 

      int index1 = line1.indexOf(":");
      int index2 = line1.indexOf(",");
      int index3 = line1.indexOf(":", index2);
      int index4 = line1.indexOf("}");

      String maxTempStr = line1.substring(index1+1, index2);
      float maximum = maxTempStr.toFloat();
      operatingMaximum = maximum - 2;
      Serial.print("operatingMaximum = ");
      Serial.println(operatingMaximum); 

      String minTempStr = line1.substring(index3+1, index4);
      float minimum =  minTempStr.toFloat();  
      operatingMinimum = minimum +2;
      Serial.print("operatingMinimum = ");
      Serial.println(operatingMinimum);

      EEPROM.get(50, operatingMaximumEeprom);
      if (operatingMaximumEeprom != operatingMaximum) {
        operatingMaximumEeprom = operatingMaximum;
        EEPROM.put(50, operatingMaximumEeprom);
        EEPROM.commit();     // для esp8266/esp32
        Serial.print("operatingMaximumEeprom = ");
        Serial.println(operatingMaximumEeprom);
      }

      EEPROM.get(0, operatingMinimumEeprom);
      if (operatingMinimumEeprom != operatingMinimum) {
        operatingMinimumEeprom = operatingMinimum;
        EEPROM.put(0, operatingMinimumEeprom);
        EEPROM.commit();     // для esp8266/esp32
        Serial.print("operatingMinimumEeprom = ");
        Serial.println(operatingMinimumEeprom);
      }
    }
  }
  
}

void collectTemperature() {
  Serial.println("Collecting temp...");
  sensors.begin();                             // определение устройств на шине
  sensors.getAddress(Thermometer,1);
  sensors.requestTemperatures();
  float currentTemp = sensors.getTempC(Thermometer);// Получить значение температуры
  temperature = (String) currentTemp;

  if (currentTemp < operatingMinimumEeprom) {
    Serial.print("Temperature is critical low! ");
    Serial.println(currentTemp);
    digitalWrite(D6, LOW);
  }
  if (currentTemp > operatingMaximumEeprom) {
    Serial.print("Temperature is critical high! ");
    Serial.println(currentTemp);    
    digitalWrite(D6, HIGH);
  }

  sendCurrentTemp();
}

void handleUpdateTempLimit() {
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
