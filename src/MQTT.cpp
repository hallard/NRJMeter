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
// Written by Sylvain REMY / Charles-Henri Hallard
//
// History : V1.00 2017-09-05 - First release
//           V2.00 2019-06-02 - Use AsyncMQTT library
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#include "NRJMeter.h"

#ifdef USE_MQTT

String outTopic;
String inTopic;

AsyncMqttClient mqttClient;

void subscribeTopic(char * topic, uint8_t qos) 
{
  Debugf(PSTR("Subscribing qos:%d on '%s'\r\n"), qos, topic);
  mqttClient.subscribe(topic, qos);
}

void onMqttConnect(bool sessionPresent) 
{
  char topic[64];
  uint32_t id = ESP_getChipId() & 0xFFFF ;

  inTopic = config.host;
  inTopic += F("/in");

  outTopic = config.host;
  outTopic += F("/out");

  Debugf(PSTR("Connected to MQTT. Session present:%d\r\n"), sessionPresent) ;

  // you can subscribe some topics here if needed
  //strcpy_P(topic, PSTR("xiaomi/from/#") );
  //subscribeTopic(topic, 0);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) 
{
  DebuglnF("Disconnected from MQTT.");
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) 
{
  Debugf(PSTR("Subscribe acknowledged, packetId:%d  qos:%d\r\n"), packetId, qos) ;
}

void onMqttUnsubscribe(uint16_t packetId) 
{
  Debugf(PSTR("Unsubscribe acknowledged, packetId:%d\r\n"), packetId) ;
}

// Message received, take care we're in async mode and frame may need to be re assembled
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) 
{
  static uint8_t tinfo_display_state = 0;
  static char * pbuff = nullptr;

  //Debugf(PSTR("Received[%d/%d][%d] %s\r\n"), len, total, index, topic) ;

  // 1st Frame or single frame
  if (index == 0 ) {
      if (pbuff) {
        free(pbuff);
      }
      pbuff = (char *) calloc(total+1, sizeof(char));
      if (pbuff) {
        memcpy(pbuff, payload, len);
      }
  }

  // Last frame
  if ( pbuff && (len+index) == total ) {
    memcpy(pbuff+index, payload, len);
  }

  // Be sure we have a full frame
  if ( pbuff && (((len+index) == total) || ((len==total) && index ==0)) ) {

    Debugf(PSTR("Received[%d] %s\r\n"), total, pbuff) ;

    if (pbuff) {
      free(pbuff);
      pbuff = nullptr;
    }

  } // Len == total

}

void onMqttPublish(uint16_t packetId) 
{
  Debugf(PSTR("Publish acknowledged, packetId:%d\r\n"), packetId) ;
}


bool mqttPost(const char * topic, const char* message)
{
  Debugf("MQTT sending \"%s\" to %s\r\n", message, outTopic.c_str());
  
  //Post on general NRJMeter topic QOS 0, no retain
  mqttClient.publish(outTopic.c_str(), MQTT_QOS, MQTT_RETAIN, message);

    //Post on targeted topic if different
  if (strcmp(topic,outTopic.c_str()) !=  0)
  {
    Debugf("and to %s\r\n", topic);
    mqttClient.publish(topic, MQTT_QOS, MQTT_RETAIN, message);
  }
}

void handle_MQTT()
{
  if (config.config & CFG_MQTT) {
    if ( WiFi.isConnected() ) {
      if ( (config.config & CFG_MQTT) && !mqttClient.connected() ) {
        DebuglnF("Connecting to MQTT...");
        mqttClient.connect();
      }
    }
  }
}

void MQTT_setup()
{
  if (config.config & CFG_MQTT) {
    mqttClient.onConnect(onMqttConnect);
    mqttClient.setMaxTopicLength(MQTT_MAX_TOPIC_LEN);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(config.mqtt.host, config.mqtt.port);
  }
}

void MQTT_close()
{
  if (config.config & CFG_MQTT) {
    mqttClient.unsubscribe(inTopic.c_str());
    mqttClient.disconnect();
  }
}


#else // USE_MQTT

bool mqttPost(const char * topic, const char* message) { return false;}
void handle_MQTT() {}
void MQTT_setup() {}
void MQTT_close() {}

#endif