#include "WiFi.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"
#include <EEPROM.h>

char* wifi_ssid[3] = {"Subibroo","Chikka","JioFi3_126C36"};
char* wifi_pwd[3] = {"68729844","gbid24550","4p7ce3t8vv"};
//String cse_ip = "192.168.43.59";
//String cse_ip = "139.59.42.21";
String cse_ip = "onem2m.iiit.ac.in";
////////////////////////////////////////
//Sensor Setup
//------------------------------------//


StaticJsonBuffer<200> jsonBuffer;
JsonObject& user_data = jsonBuffer.createObject();
JsonObject& temp_user_data = jsonBuffer.createObject();
JsonObject& sensor_data = jsonBuffer.createObject();

//String cse_port = "8080";
String cse_port = "443";
String server = "https://"+cse_ip+":"+cse_port+"/~/in-cse/in-name/";

String createCI(String server, String ae, String cnt, String val)
{
  HTTPClient http;
  http.begin(server + ae + "/" + cnt + "/");
//  String postRequest = String() + "POST " + url + " HTTP/1.1\r\n" +
//                       "Host: " + CSE_IP + ":" + CSE_HTTP_PORT + "\r\n" +
//                       "X-M2M-Origin: " + CSE_M2M_ORIGIN + "\r\n" +
//                       "Content-Type: application/json;ty=" + ty + "\r\n" +
//                       "Content-Length: " + rep.length() + "\r\n"
//                       "Connection: close\r\n\n" +
//                       rep;
//                       
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");
  http.addHeader("Content-Length", "100");
  http.addHeader("Connection", "close");
  int code = http.POST("{\"m2m:cin\": {\"cnf\": \"text/plain:0\",\"con\": "+ String(val) +"}}");
  http.end();
  Serial.print(code);
  delay(300);
}

void setup()
{
  Serial.begin(115200);
  EEPROM.begin(8);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  int wifi_no = EEPROM.readInt(4);
  if((wifi_no<0) ||(wifi_no>2))
  {
    wifi_no=0;
  }
  EEPROM.writeInt(4,(wifi_no+1)%3);
  EEPROM.commit();
  Serial.println(wifi_no);
  WiFi.begin(wifi_ssid[wifi_no], wifi_pwd[wifi_no]);
  int timee=0;
  Serial.println("Code Started");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    timee++;
    if(timee>20)
    {
      ESP.restart();
    }
    Serial.println("Connecting to WiFi..");
  }
  pinMode(5,INPUT); 
}

int tot = 0;

void loop()
{
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connection lost.. trying to reconnect");
    ESP.restart();
  }
  //To GET values
  //http.begin("http://192.168.0.113:8080/~/in-cse/in-name/verte001/DATA/la");
  //http.begin("http://192.168.0.113:8080/~/in-cse");
  //int httpCode = http.GET();
 


  int Liquid_level=EEPROM.readInt(0);
  //int Liquid_level=digitalRead(5);
  Serial.print("Liquid_level= ");Serial.println(Liquid_level,DEC);


  //when single sensor gives single value
  String sensor2 = String(Liquid_level); 

  // Make it a single string
  String sensor_string = sensor2;

  // Make it OneM2M complaint
  sensor_string = "\"" + sensor_string + "\""; // DO NOT CHANGE THIS LINE

  // Send data to OneM2M server
  createCI(server, "Team17_Water_Level_Monitoring_in_Overhead_Tanks", "node_1", sensor_string);
  //delay(1000); // DO NOT CHANGE THIS LINE
  Liquid_level = digitalRead(5);
  EEPROM.writeInt(0,Liquid_level);
  EEPROM.commit();
  tot++;
  delay(300000); // DO NOT CHANGE THIS LINE/
//    delay(500); // DO NOT CHANGE THIS LINE/
  if(tot > 6)
  {
    ESP.restart();
  }
}
