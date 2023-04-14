#include <Arduino.h>
#include <Wire.h>

// Master sda = 21, scl = 22
// Slave sda = 4, scl = 5
const byte sda_M = 21;
const byte scl_M = 22;
const byte sda_S = 4;
const byte scl_S = 5;

#define Left_Plate
// Left Plate Slave address = 0x0b
// Right Plate Slave address = 0x09

#ifdef Left_Plate
#define I2C_DEV_ADDR_M 0x09
#define I2C_DEV_ADDR_S 0x0b
uint32_t dt = 1000;
#else
#define I2C_DEV_ADDR_M 0x0b
#define I2C_DEV_ADDR_S 0x09
uint32_t dt = 2000;
#endif

// Connect other I2C device to master for testing the function of the master.
#define I2C_TEST_ADDR 0x3C

// ======================================================
// TwoWire(0) work as Master
TwoWire WireM = TwoWire(0);

void Master_setup()
{
  WireM.begin(sda_M, scl_M, 0);
}

void Master_loop()
{
  Serial.println();
  // Test master functioning.
  WireM.beginTransmission(I2C_TEST_ADDR);
  byte error_test = WireM.endTransmission();
  if(error_test != 0)
    Serial.printf("[Master] Check %u\n", error_test);

  // Write message to the esp32 slave
  WireM.beginTransmission(I2C_DEV_ADDR_M);
  WireM.printf("Hello  %02x!", I2C_DEV_ADDR_M);
  uint8_t error = WireM.endTransmission(true);
  if (error != 0)
    Serial.printf("[Master] 0x%02x endTransmission. error = %u\n", I2C_DEV_ADDR_M, error);

  // Read 16 bytes from the esp32 slave
  uint8_t bytesReceived = WireM.requestFrom(I2C_DEV_ADDR_M, 16);
  int start = millis();
  Serial.printf("[Master] requestFrom 0x%02x: %u\n", I2C_DEV_ADDR_M, bytesReceived);
  if ((bool)bytesReceived)
  { // If received more than zero bytes
    uint8_t temp[bytesReceived];
    WireM.readBytes(temp, bytesReceived);
    for (int i = 1; i < bytesReceived; i++)
    {
      if (temp[i] == 0xff)
      {
        temp[i] = 0x00;
        bytesReceived = i;
        break;
      }
    }
    char str[bytesReceived + 1];
    memcpy(str, temp, sizeof(str));
    Serial.print("[Master] ");
    Serial.print(str);
    Serial.print(" dt = ");
    Serial.println(millis()-start);
  }
}
// ====================================================================
// TwoWire(1) work as slave
TwoWire WireS = TwoWire(1);

int s = 0;

void onRequest()
{
  WireS.print(s);
  WireS.print(" Packets.");

  Serial.print("[Slave] onRequest. Print: ");
  Serial.print(s);
  Serial.println(" Packets.");
  s++;
}

void onReceive(int len)
{
  Serial.printf("[Slave] onReceive(len = %d): ", len);
  while (WireS.available())
  {
    Serial.write(WireS.read());
  }
  Serial.println();
}

void Slave_setup()
{
  WireS.onReceive(onReceive);
  WireS.onRequest(onRequest);
  WireS.begin(I2C_DEV_ADDR_S, sda_S, scl_S, 0);
#if CONFIG_IDF_TARGET_ESP32
  char message[64];
  snprintf(message, 64, "%u Packets.", s++);
  WireS.slaveWrite((uint8_t *)message, strlen(message));
#endif
}

//===========================================================================

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Master_setup();
  Slave_setup();
}

void loop()
{
  Master_loop();
  delay(dt);
}