#include <DS18B20.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define TEMP_SENSOR 13 // Input pin to read temp
#define HEAT_MAT 22    // Output pin to toggle heat mat
#define UPPER 23.0     // Upper limit for temp (in degC)
#define LOWER 22.0     // Lower limit for temp (in degC)
#define DELAY 7000     // Loop delay (in ms)

// Setup a oneWire instance to communicate with a OneWire device
OneWire oneWire(TEMP_SENSOR);
DallasTemperature sensors(&oneWire);

// Declare arrays to hold device address
DeviceAddress tempSensorAddress;

/*
  - Update temp value from sensor
  - Make sure temp is in range
  - Wait DELAY ms and do it again
*/
void loop(void) // loops after setup()
{
  Serial.println("");

  checkTemp(tempSensorAddress);
  delay(DELAY);
}

/*
  - Open serial port
  - Find temp sensor
  - Turn on heat mat
*/
void setup(void) // runs once then hands it over to loop()
{
  Serial.begin(9600);

  // locate devices on the bus
  Serial.print("Locating devices...");
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // Assign address manually. The addresses below will need to be changed
  // to valid device addresses on your bus. Device address can be retrieved
  // by using either oneWire.search(deviceAddress) or individually via
  // sensors.getAddress(deviceAddress, index)
  // Note that you will need to use your specific address here
  // insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };

  // Method 1:
  // Search for devices on the bus and assign based on an index. Ideally,
  // you would do this to initially discover addresses on the bus and then
  // use those addresses and manually assign them (see above) once you know
  // the devices on your bus (and assuming they don't change).
  if (!sensors.getAddress(tempSensorAddress, 0))
    Serial.println("Unable to find address for Device 0");

  Serial.print("Device 0 Address: ");
  printAddress(tempSensorAddress);
  Serial.println();

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(tempSensorAddress, 9);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(tempSensorAddress), DEC);
  Serial.println();

  pinMode(HEAT_MAT, OUTPUT);
  digitalWrite(HEAT_MAT, HIGH);
}

// Check to see temp is within bounds. Adjust accordingly
void checkTemp(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  printTemp(tempC);
  if (tempC > UPPER)
  {
    Serial.println("++++++  This one is too hot!  +++++++");
    digitalWrite(HEAT_MAT, LOW);
  }
  else if (tempC < LOWER)
  {
    Serial.println("------  This one is too cold!  ------");
    digitalWrite(HEAT_MAT, HIGH);
  }
  else
  {
    Serial.println("===  This one is juuuuuust right  ===");
  }
}

// Print temp (in degC) to serial
void printTemp(float temp)
{
  if (temp == DEVICE_DISCONNECTED_C)
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  char buffer[70];
  sprintf(buffer, "Temp is %.1f degC", temp);
  Serial.println(buffer);
}

// Print device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16)
      Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}