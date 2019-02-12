# LoRaWAN-Examples
This repository maintains sample code for working with LoRaWAN:
* MKRWAN-DHT11: Arduino sketch for sending temperature and humidity acquired with DHT-11 to TTN. To be used with MKRWAN-DHT11-decoder.js. 
* MKRWANTempNode: more generic version of the former. It is configurable for using DHT-11, DHT22 or DS18B20 sensors, using ABP or OTAA, etc. To be used with MKRWANTempNode-decoder.js. 
In both cases you need to create an application and register your device on TTN, and you have to copy&paste the decoder code in the Payload Formats tab, under "Decoder".
