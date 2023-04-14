# I2C_Master_and_Slave

This practice is based on the Esp32_I2C_Slave example.

This practice aims to make a Esp32 work as Master and Slave at the same time.

# Wiring
The left esp32 master's I2C IO is connected to a tested I2C device and the right esp32 slave IO.  
The right esp32 master's I2C IO is connected to a tested I2C device and the left esp32 slave IO.

# Coding
The Esp32 works as Master in TwoWire(0) and works as Slave in TwoWire(1).
