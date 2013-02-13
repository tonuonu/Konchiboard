Konchiboard
===========

Demo software for Konchiboard

1. Blinks all 6 LEDs
2. Becomeas available as serial port (For windows use Arduino Leonardo drivers) at 115200bps 8N1, no flow control
3. Scans I2C bus and reports results to serial port
4. If  I2C device exists on address 0x1A assumes this is DS2482 for 1wire
5. Scans 1wire bus
6. For every 1wire device tries to read temperature and report this to serial port
