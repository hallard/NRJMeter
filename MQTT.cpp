// **********************************************************************************
// ESP8266 NRJMeter MQTT Publisher
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// For any explanation about teleinfo ou use, see my blog
// http://hallard.me/category/tinfo
//
// This program works with the Wifinfo board
// see schematic here https://github.com/hallard/teleinfo/tree/master/Wifinfo
//
// Written by Sylvain REMY
//
// History : V1.00 2017-09-05 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#include "MQTT.h"
#include "NRJMeter.h"
#include "Debug.h"
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient


WiFiClient espClient;
PubSubClient mqttclient(espClient);
String outTopic;
String inTopic;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  DebugF("Message arrived [");
  Debug(topic);
  DebuglnF("]");
  DebugF("[");
  for (int i = 0; i < length; i++) {
    Debug((char)payload[i]);
  }
  DebuglnF("]");
}

void mqttReconnect() {
  // Loop until we're reconnected
  while (!mqttclient.connected()) {
    DebuglnF("Attempting MQTT connection...");

    //client.connect(config.host, "testuser", "testpass")
    // Attempt to connect
    if (mqttclient.connect(config.host)) {
      DebuglnF("connected");
      // Once connected, publish an announcement...
      mqttclient.publish(outTopic.c_str(), "Here i am");
      // ... and resubscribe
      mqttclient.subscribe(inTopic.c_str());
    } else {
      DebugF("failed, rc=");
      Debug(mqttclient.state());
      DebuglnF(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

bool mqttPost(const char * topic, const char* message)
{
  DebugF("MQTT sending \"");
  Debug(message);
  DebugF("\" to ");
  Debug(outTopic.c_str());
  
  //Post on general NRJMeter topic
  mqttclient.publish(outTopic.c_str(), message);
  
  //Post on targeted topic if different
  if (strcmp(topic,outTopic.c_str()) !=  0)
  {
    DebugF(" and ");
    Debugln(topic);
    mqttclient.publish(topic, message);
  }
}

void handle_MQTT()
{
  if (!mqttclient.connected() && WiFi.status() == WL_CONNECTED) {
    mqttReconnect();
  }

  mqttclient.loop();

  //mqttclient.publish(outTopic.c_str(), "ESPMQTTTestMessage!!");

}

void MQTT_setup()
{
  inTopic = config.host;
  inTopic += F("/in");

  outTopic = config.host;
  outTopic += F("/out");

  mqttclient.setServer(config.mqtt.host, config.mqtt.port);
  mqttclient.setCallback(mqttCallback);
}

