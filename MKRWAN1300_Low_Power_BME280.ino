#include <MKRWAN.h>
#include <ArduinoLowPower.h>


#include <BME280I2C.h>
#include <Wire.h>

BME280I2C bme;


LoRaModem modem;

#define tiempo_tx 900   //data upload time

//OTAA TTN network access keys

String appEui = "70B3D57ED0042B4E";
String appKey = "2f1c6deb99099e89de3e600083f3d451";

//TPN ABP network access keys

String devAddr = "xxxxxxxxxxxxxxxxxx";
String nwkSKey = "xxxxxxxxxxxxxxxxxx";
String appSKey = "xxxxxxxxxxxxxxxxxx";

void setup() {

  Wire.begin();
  bme.begin();

  
  
  modem.begin(EU868); //European frequency TTN
  delay(1000);

  int connected = modem.joinOTAA(appEui, appKey); //uncomment if you want to use OTAA
  //int connected = modem.joinABP(devAddr, nwkSKey, appSKey);  //uncomment if you want to use ABP



  modem.minPollInterval(10);
  modem.dataRate(5);
  // Set poll interval to 60 seconds.
  Serial.begin(115200);
}

void loop()
{
  float temp(NAN), hum(NAN), pres(NAN);

  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_Pa);

  bme.read(pres, temp, hum, tempUnit, presUnit);

  float celsius = temp;
 // float relativehum = hum;
  float pressure = pres;

  Serial.println("Temperature: ");
  Serial.println(celsius);
  Serial.println("Pressure: ");
  Serial.println(pressure);
  // We convert values to bytes

  int16_t temperatura = (int16_t)(celsius * 10); //pending to add +40 so as not to transmit negative part
  int16_t presion = (int16_t)(pressure / 10);
  
  //We create the data array, as many as has been defined in brackets, counting zero, with bitshift and zero padding up to 16 bits

  byte datos[6];
  datos[0] = (temperatura >> 8) & 0xff;
  datos[1] = temperatura & 0xff;
//  datos[2] = humedad >> 8;
 // datos[3] = humedad & 0xFF;
  datos[4] = (presion >> 8) & 0xff;
  datos[5] = presion & 0xff;

  //Sending the data through the LORA modem
  int err;
  modem.beginPacket();
  modem.write(datos, sizeof(datos));
  err = modem.endPacket();

  LowPower.sleep(tiempo_tx * 1000);

}
