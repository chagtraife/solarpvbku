#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <time.h> 



#define PIN_LED 16 //gpio16
#define PIN_BUTTON 0//gpio0
//#define PIN_BUTTON 4//gpio4
#define LED_ON() digitalWrite(PIN_LED, HIGH)
#define LED_OFF() digitalWrite(PIN_LED, LOW)
#define LED_TOGGLE() digitalWrite(PIN_LED, digitalRead(PIN_LED) ^ 0x01)

#define DEBUG

Ticker ticker;


String buf;

bool longPress()
{
  static int lastPress = 0;
  if (millis() - lastPress > 3000 && digitalRead(PIN_BUTTON) == 0) {
    return true;
  } else if (digitalRead(PIN_BUTTON) == 1) {
    lastPress = millis();
  }
  return false;
}

void tick()
{
  //toggle state
  int state = digitalRead(PIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(PIN_LED, !state);     // set pin to the opposite state
}

bool in_smartconfig = false;
void enter_smartconfig()
{
  if (in_smartconfig == false) {
    in_smartconfig = true;
    ticker.attach(0.1, tick);
    WiFi.beginSmartConfig();
  }
}

void exit_smart()
{
  ticker.detach();
  LED_ON();
  in_smartconfig = false;
}

void send_data()
{

  // We now create a URI for the request
  //  const char* host = "192.168.1.50";
  const char* host = "192.168.43.196";
  String url = "http://192.168.43.196/solarpvbku/input/post";
  String apikey  = "&apikey=c9b99dfe3a1f5d356ec3d956b0d633cd";


  //  String client_print = "http://192.168.43.79/solarpvbku/input/post?node=emontx&fulljson={\"power1\":100,\"power2\":200}&apikey=6cb16350050a05a4c1ed849640cfe0b7";
  //    String client_print ="http://192.168.43.79";

  buf.trim(); // lam sach chuoi
  String client_print = url + buf + apikey;
  //  String client_print = buf;
  Serial.println(client_print);
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  // This will send the request to the server
  //   client.print(String("GET ") + url + buf + apikey + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  //  client.print(client_print);
  client.print(String("GET ") + client_print + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 1000) {
      Serial.println(">>> Client Timeout !\n");
      client.stop();
      return;
    }
  }
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
}



void setup() {
  //setup uart
  Serial.begin(9600);
  //Serial.setDebugOutput(true);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  ticker.attach(1, tick);
  Serial.println("Setup done");


#ifdef DEBUG
  const char* ssid     = "Solarpv";
  const char* password = "9qca0n04";
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
#endif

}

void loop() {
#ifndef DEBUG
  {
    if (longPress()) {
      enter_smartconfig();
      Serial.println("Enter smartconfig\n");
    }
    if (WiFi.status() == WL_CONNECTED && in_smartconfig && WiFi.smartConfigDone()) {
      exit_smart();
      Serial.println("Connected, Exit smartconfig\n");
    }
  }
#endif
  if (WiFi.status() == WL_CONNECTED) {
    //get aplikey from user

    Serial.println("req\n");// send request to mcu


    //wait data
    unsigned long timeout = millis();
    while (Serial.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Serial Timeout !\n");
        //client.stop();
        return;
      }
    }


    //get data
    while (Serial.available() > 0) {
      ;
      buf = Serial.readStringUntil('\n');
      Serial.print(buf);
    }




    //send data to server
    send_data();
    delay(5000);

  }
}
