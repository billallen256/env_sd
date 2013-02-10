// 
//   FILE:  dht11_test1.pde
// PURPOSE: DHT11 library test sketch for Arduino
//

#include<stdlib.h>
#include <dht11.h>
#include <SD.h>

#define DHT11PIN 2

const int chipSelect = 4;

dht11 DHT11;

//Celsius to Fahrenheit conversion
double Fahrenheit(double celsius)
{
	return 1.8 * celsius + 32;
}

// fast integer version with rounding
//int Celcius2Fahrenheit(int celcius)
//{
//  return (celsius * 18 + 5)/10 + 32;
//}


//Celsius to Kelvin conversion
double Kelvin(double celsius)
{
	return celsius + 273.15;
}

// dewPoint function NOAA
// reference: http://wahiduddin.net/calc/density_algorithms.htm 
double dewPoint(double celsius, double humidity)
{
	double A0= 373.15/(273.15 + celsius);
	double SUM = -7.90298 * (A0-1);
	SUM += 5.02808 * log10(A0);
	SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/A0)))-1) ;
	SUM += 8.1328e-3 * (pow(10,(-3.49149*(A0-1)))-1) ;
	SUM += log10(1013.246);
	double VP = pow(10, SUM-3) * humidity;
	double T = log(VP/0.61078);   // temp var
	return (241.88 * T) / (17.558-T);
}

// delta max = 0.6544 wrt dewPoint()
// 5x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
	double a = 17.271;
	double b = 237.7;
	double temp = (a * celsius) / (b + celsius) + log(humidity/100);
	double Td = (b * temp) / (a - temp);
	return Td;
}

void setup()
{
  //Serial.begin(115200);
  Serial.begin(9600);
  Serial.println("DHT11 TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT11LIB_VERSION);
  Serial.println();
  
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  
  Serial.println("card initialized.");
}

void loop()
{
  // make a string for assembling the data to log:
  String dataString = "";
  
  Serial.println("\n");

  int chk = DHT11.read(DHT11PIN);

  Serial.print("Read sensor: ");
  
  switch (chk)
  {
    case DHTLIB_OK: 
		Serial.println("AOK"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		Serial.println("ECS"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		Serial.println("ETO"); 
		break;
    default: 
		Serial.println("UNK"); 
		break;
  }
  
  dataString += String(chk) + ",";
  char temp[10];

  float humidity = (float)DHT11.humidity;
  dtostrf(humidity, 6, 2, temp);
  Serial.print("Humidity (%): ");
  Serial.println(humidity, 2);
  dataString += String(temp) + ",";

  float temperatureC = (float)DHT11.temperature;
  dtostrf(temperatureC, 6, 2, temp);
  Serial.print("Temperature (oC): ");
  Serial.println(temperatureC, 2);
  dataString += String(temp) + ",";

  float temperatureF = Fahrenheit(DHT11.temperature);
  dtostrf(temperatureF, 6, 2, temp);
  Serial.print("Temperature (oF): ");
  Serial.println(temperatureF, 2);
  dataString += String(temp) + ",";

  float temperatureK = Kelvin(DHT11.temperature);
  dtostrf(temperatureK, 7, 2, temp);
  Serial.print("Temperature (K): ");
  Serial.println(temperatureK, 2);
  dataString += String(temp) + ",";

  float dew = dewPoint(temperatureC, humidity);
  dtostrf(dew, 6, 2, temp);
  Serial.print("Dew Point (oC): ");
  Serial.println(dew);
  dataString += String(temp);

  Serial.println(dataString);
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  
   // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    Serial.println("write complete");
  } else {
    Serial.println("error opening datalog.txt");
  }

  delay(30000);
}
//
// END OF FILE
//
