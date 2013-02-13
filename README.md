Konchiboard
===========

Demo software for Konchiboard

1. Blinks all 6 LEDs
2. Becomeas available as serial port (For windows use Arduino Leonardo drivers) at 115200bps 8N1, no flow control
3. Scans I2C bus and reports results to serial port
4. If  I2C device exists on address 0x1A assumes this is DS2482 for 1wire
5. Scans 1wire bus
6. For every 1wire device tries to read temperature and report this to serial port


$ sudo dfu-programmer atmega32u4 erasetonu@Inspiron-7520:~/Konchiboard$ ls -brt /tmp/ | grep build | tail -n 1build7356527073541884004.tmp
$ sudo dfu-programmer atmega32u4 flash /tmp/build7356527073541884004.tmp/*.hex
Validating...
9896 bytes used (34.51%)
$ sudo dfu-programmer atmega32u4 start
$ 

To compile sources you may want to use Arduino Leonardo board but make sure to set 8Mhz instead of 16Mhz. This is chip limitation while using 3,3V.


