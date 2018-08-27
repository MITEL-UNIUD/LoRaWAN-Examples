/*
  MKR WAN 1300 - LoraWAN Send DHT-11 Temperature and Humidity
  This sketch demonstrates how to send and receive temperature and humidity 
  with the MKR WAN 1300 LoRa module plus a DHT-11, and send them to TTN using ABP.
  The sketch is derived from the original LoraSendAndReceive example.
  This example code is in the public domain.

  MITEL - Medical Informatics, Telemedicine & eHealth Lab, University of Udine, Italy
  http://mitel.dimi.uniud.it
*/

#include <MKRWAN.h>
#include <SimpleDHT.h>

///////////////////////////////////////////////////////////////////////////////////////
//CONFIGURATION
///////////////////////////////////////////////////////////////////////////////////////

// comment next line out when you don't need to debug (smaller sketch)
#define DEBUG 1

// Sensitive data from TTN
String devAddr = "XXXXXX";
String nwkSKey = "YYYYYYYYYYYYYYYYYYYYYY";
String appSKey = "ZZZZZZZZZZZZZZZZZZZZZZ";

// change this to your regional band (eg. US915, AS923, ...)
#define REGION EU868

//DHT-11 sensor pin
int pinDHT11 = 7;

//delay between sends
int incr=1000*30*1; //minimum 2 minutes, but 10 minutes makes sense

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

//Main objects
LoRaModem modem;
SimpleDHT11 dht11;
int counter=0; 

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  //yellow led on during setup

#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial&& millis() < 15000);
#endif
  
  if (!modem.begin(REGION)) {
    #ifdef DEBUG
    Serial.println("Failed to start module");
    #endif
    while (1) {}
  };

#ifdef DEBUG
  Serial.println("Send temperature and humidity to TTN with Arduino MKR WAN 1300");
#endif
  
  int connected = modem.joinABP(devAddr, nwkSKey, appSKey);
 
  if (!connected) {
    #ifdef DEBUG
    Serial.println("ABP joining failed.");
   #endif
    while (1) {}
  }

  // Set poll interval to 60 secs.
  modem.minPollInterval(60);
  // NOTE: independently by this setting the modem will
  // not allow to send more than one message every 2 minutes,
  // this is enforced by firmware and can not be changed.

  // set datarate to 5= SF7125; 3 = SF9125 NB default 0=SF12125. Change this if needed.
  //modem.dataRate(5);
  digitalWrite(LED_BUILTIN, LOW);

}

void loop() {
 
  
  counter=counter+1;
  delay(incr);

  byte temperature = 0;
  byte humidity = 0;
  
  int errs = SimpleDHTErrSuccess;
  if ((errs = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    #ifdef DEBUG
    Serial.print("Read DHT11 failed, err="); Serial.println(errs);
    #endif
    return;
  }

#ifdef DEBUG
  String msg=String(counter)+":"+String((int)temperature)+"C "+String((int)humidity)+"%H";
  Serial.println();
  Serial.print("Sending: " + msg );
  Serial.println();
#endif

  digitalWrite(LED_BUILTIN, HIGH); //led on during send 
  int err;
  modem.beginPacket(); 
  //we send two bytes, one for humidity and one for temperature
  //they will be decoded and transformed to JSON by the encoder to be put on TTN
  modem.write(humidity);
  modem.write(temperature );
  err = modem.endPacket(false); //unconfirmed: true is confirmed. With true, most messages are sent even if err
  if (err > 0) {
    #ifdef DEBUG
    Serial.println("Message sent correctly :)");
    #endif
  } else {
    #ifdef DEBUG
    Serial.println("Error sending message :(");
    Serial.println(err);
    #endif
   }
    digitalWrite(LED_BUILTIN, LOW); //led on during send 

}
