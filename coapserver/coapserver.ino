#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <coap-simple.h>
#include <Wire.h>
#include "DHT.h"


#define DHTPIN 0
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

float lux = 0;
float humi= 0;
float temp= 0;


byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
IPAddress ip(10,0,0,1);

// UDP and CoAP class
EthernetUDP Udp;
Coap coap(Udp);

// CoAP client response callback
void callback_tmp(CoapPacket &packet, IPAddress ip, int port);

// CoAP server endpoint url callback
void callback_hum(CoapPacket &packet, IPAddress ip, int port);

void callback_lth(CoapPacket &packet, IPAddress ip, int port);



// LED STATE
bool LEDSTATE;

// CoAP server endpoint URL
void callback_lth(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("[Light] ON/OFF");
  
  // send response
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  String message(p);
  
  lux = analogRead(A0);
  char result[128];
  sprintf(result, "lux %f ", lux);
  coap.sendResponse(ip, port, packet.messageid, result);
}


void callback_tmp(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("[Light] ON/OFF");
  
  // send response
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  String message(p);
  
  temp = dht.readTemperature();
  char result[128];
  sprintf(result, "temp %f ", temp);
  coap.sendResponse(ip, port, packet.messageid, result);



}

void callback_hum(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("[Light] ON/OFF");
  
  // send response
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  String message(p);
  
  humi  = dht.readHumidity();
  char result[128];
  sprintf(result, "humi %f ", humi);
  coap.sendResponse(ip, port, packet.messageid, result);

  


}

void callback_all(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("[Light] ON/OFF");
  
  // send response
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  String message(p);
  
  lux = analogRead(A0);

    // read humidity
            humi  = dht.readHumidity();
            // read temperature as Celsius
            temp = dht.readTemperature();
  
    char result[128];
    sprintf(result, "temp %f ; hum %f ; lux %f \n",lux, humi,temp);

    coap.sendResponse(ip, port, packet.messageid, result);



}




void setup() {
  Serial.begin(9600);

  Ethernet.begin(mac,ip);
  Serial.print("My IP address: ");
  Serial.print(Ethernet.localIP());
  Serial.println();
  Serial.println("Starting DHT sensor...");
  dht.begin();


  
  Serial.println("Starting endpoints...");
  coap.server(callback_hum, "hum");         
  coap.server(callback_tmp, "tmp");
  coap.server(callback_lth, "lth");
  coap.server(callback_all, "");

  // start coap server/client
  coap.start();
  Serial.println("Ready.");
}

void loop() {
  // send GET or PUT coap request to CoAP server.
  // To test, use libcoap, microcoap server...etc
  // int msgid = coap.put(IPAddress(10, 0, 0, 1), 5683, "light", "1");
  Serial.println("Send Request");
 // int msgid = coap.get(IPAddress(XXX, XXX, XXX, XXX), 5683, "time");

  coap.loop();
}
/*
if you change LED, req/res test with coap-client(libcoap), run following.
coap-client -m get coap://(arduino ip addr)/light
coap-client -e "1" -m put coap://(arduino ip addr)/light
coap-client -e "0" -m put coap://(arduino ip addr)/light
*/
