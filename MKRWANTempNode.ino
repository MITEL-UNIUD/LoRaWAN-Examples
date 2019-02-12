

/*
  MKR WAN 1300 generic temperature Node
  for DHT11, DHT22, DS18B20
  
  This sketch demonstrates how to send and receive temperature and humidity 
  with the MKR WAN 1300 LoRa module plus a temperature sensor (DHT-11 or DHT-22 or DS18B20), 
  and send them to TTN using OTAA or ABP (configurable), ADR or not, setting a specific data rate, etc.
  The sketch is derived from the original LoraSendAndReceive example.
  This example code is in the public domain.
  MITEL - Medical Informatics, Telemedicine & eHealth Lab, University of Udine, Italy
  http://mitel.dimi.uniud.it

*/

#include <MKRWAN.h>
#include <SimpleDHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "ArduinoLowPower.h"


////////////////// SETTINGS ////////////////////////
//Set type of Sensor: DHT11, DHT22, DS18B20
#define DHT11

// Set sensor pin
#define SENSORPIN 7

//Set Join method
//#define ABP
#define OTAA 

// Set ADR
#define ADR true

//Set Data rate 0=SF12, 5=SF7
#define DATARATE 5

//set delay between transmissions (minutes)
#define DELAY 10

/////////////////////////////////////////////////////

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

String devAddr = DEVADDR;
String nwkSKey = NWSKEY;
String appSKey = APPSKEY;


#ifdef DS18B20
  OneWire oneWire(SENSORPIN);
  DallasTemperature sensors(&oneWire);
#endif
#ifdef DHT11
  SimpleDHT11 dht11;
#endif
#ifdef DHT22
  SimpleDHT22 dht11;
#endif
#if defined(DHT11) || defined(DHT22)
  int pinDHT11 = SENSORPIN;
#endif

byte DR=DATARATE;  //0=SF12, 5=SF7
float ftemp=0;
LoRaModem modem;

unsigned long wait= 1000*60*DELAY; //delay
byte downlink = 0; 


void setup() {
  analogReference(AR_INTERNAL1V0);
  LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, alarmEvent0, CHANGE);

  Serial.begin(9600);
  while (!Serial && millis() < 10000);


  if (!modem.begin(EU868)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());
  Serial.println("MKR WAN 1300 - temperature node by MITEL-UNIUD");

#ifdef DS18B20
  sensors.begin();
  Serial.println("Using DS18B20 temperature sensor.");
#endif
#ifdef DHT11
  Serial.println("Using DHT-11 temperature & humidity sensor.");
#endif
#ifdef DHT22
  Serial.println("Using DHT-221 temperature & humidity sensor.");
#endif


#ifdef OTAA
  int connected = modem.joinOTAA(appEui, appKey);
  Serial.println("Joining with OTAA...");
#else
  int connected = modem.joinABP(devAddr, nwkSKey, appSKey);
  Serial.println("Joining with ABP...");
#endif

  if (!connected) {
    Serial.println("OTAA: Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  }

  Serial.print("DevAddr: "); Serial.println(modem.getDevAddr());
  Serial.print("NwkSKey: "); Serial.println(modem.getNwkSKey());
  Serial.print("AppSKey: "); Serial.println(modem.getAppSKey());
   
  // Set poll interval to 60 secs.
  modem.minPollInterval(60);

  modem.setADR(ADR);
  modem.dataRate(DR);
 
}

void loop() {
  byte temperature = 0;
  byte humidity = 0;
  #ifdef DS18B20 
    sensors.requestTemperatures(); 
    ftemp=sensors.getTempCByIndex(0);
    temperature= byte(ftemp+128); 
  #endif
  
  #if defined(DHT11) || defined(DHT22)
  int errs = SimpleDHTErrSuccess;
  if ((errs = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(errs);
    return;
  }
  #endif
  
  Serial.print("ADR:"); Serial.print(modem.getADR());
  Serial.print("; DR:"); Serial.println(modem.getDataRate());
 
  Serial.print("Sending: H " + String(humidity) + "% - Temperature " + String(temperature) );
  Serial.println();

  int err;
  modem.beginPacket();
  modem.write(humidity);
  modem.write(temperature );
  modem.write(volt());
  err = modem.endPacket(false);
 
  if (err > 0) {
    Serial.println("Message sent correctly!");
  } else {
    Serial.println("Error sending message :(");
   }

  Serial.println();

//When you are happy with your code, you may switch to LowPower.sleep() instead of delay() (it disables the serial output)
  delay(wait);
  //LowPower.sleep(wait);
}

byte volt() {
  // if 1.65V internal reference: 680
  // if 1.0: 1024
  return byte(35 * analogRead(ADC_BATTERY) / 1024);
}

void alarmEvent0() {
  int  alarm_source = 0;
}
