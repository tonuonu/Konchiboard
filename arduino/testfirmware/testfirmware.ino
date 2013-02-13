
#include <Wire.h>
#include <DS2482.h>

DS2482      ds(0x1a);

int         led1Pin = 13;
int         led2Pin = 7;
int         led3Pin = 5;

void
setup()
{
	pinMode(led1Pin, OUTPUT);
	pinMode(led2Pin, OUTPUT);
	pinMode(led3Pin, OUTPUT);

	Wire.begin();
	Serial.begin(115200);
	Serial.println("Launching tests");
	ds.reset();
}

double
GetTemp(void)
{
	unsigned int wADC;
	double      t;

#if 1
	analogReference(INTERNAL);
	delay(200);		// wait for voltages to become stable.
	wADC = analogRead(8);	// Channel 8 is temperature sensor.
#else
	// Set the internal reference and mux.
	ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
	ADCSRA |= _BV(ADEN);	// enable the ADC

	delay(200);		// wait for voltages to become stable.

	ADCSRA |= _BV(ADSC);	// Start the ADC

	// Detect end-of-conversion
	while (bit_is_set(ADCSRA, ADSC));

	// Reading register "ADCW" takes care of how to read ADCL and ADCH.
#if defined (__AVR_ATmega32U4__)
	wADC = ADC;		// For Arduino Leonardo
#else
	wADC = ADCW;		// 'ADCW' is preferred over 'ADC'
#endif
#endif

	// FXIME: Right formula is unclear
	t = (wADC - 0.0) /* / 1.22 */ ;

	return (t);
}

void
onewirescan()
{
	byte        addr[8];
	int         HighByte, LowByte, SignBit, Whole, Fract, TReading, Tc_100,
		FWhole;

	Serial.println("Scanning 1wire...");
	bool        x = ds.wireReset();

	Serial.print("wireReset returned ");
	Serial.println(x, HEX);
	ds.wireResetSearch();
	bool        y;

	do {
		y = ds.wireSearch(addr);

		Serial.print("wireSearch returned ");
		Serial.println(y, HEX);
		if (!y) {
			Serial.println("No more addresses.");
			ds.wireResetSearch();
		} else {
			delay(10);
			Serial.print("Found device ");
			delay(10);
			switch (addr[0]) {
				case 0x23:
					Serial.print("DS24B33");
					break;
				case 0x28:
					Serial.print("DS18B20");
					break;
				case 0x42:
					Serial.print("DS28EA00");
					break;
				case 0x3B:
					Serial.print("MAX31826");
					break;
				default:
					Serial.print("unknown");
					break;


			}
			delay(10);
			Serial.print(" on ");
			delay(10);
			Serial.print(addr[0], HEX);
			Serial.print(":");
			Serial.print(addr[1], HEX);
			Serial.print(":");
			Serial.print(addr[2], HEX);
			Serial.print(":");
			Serial.print(addr[3], HEX);
			Serial.print(":");
			Serial.print(addr[4], HEX);
			Serial.print(":");
			Serial.print(addr[5], HEX);
			Serial.print(":");
			Serial.print(addr[6], HEX);
			Serial.print(":");
			Serial.println(addr[7], HEX);

			ds.wireReset();
			ds.wireSelect(addr);
			ds.wireWriteByte(0x44);	// Tell MAX31826 to start temperature AD conversion

			delay(100);	// wait a little

			ds.wireReset();
			ds.wireSelect(addr);
			ds.wireWriteByte(0xBE);	// Read scratchpad 1 for data
			LowByte = ds.wireReadByte();
			HighByte = ds.wireReadByte();
			Serial.print("Temperature is: ");
			TReading = (HighByte << 8) + LowByte;
			SignBit = TReading & 0x8000;	// test most sig bit
			if (SignBit) {
				TReading = (TReading ^ 0xffff) + 1;	// 2's comp
			}
			Tc_100 = (6 * TReading) + TReading / 4;	// multiply by (100 * 0.0625) or 6.25

			Whole = Tc_100 / 100;	// separate off the whole and fractional portions
			Fract = Tc_100 % 100;
			if (SignBit) {	// If its negative
				Serial.print("-");
			}
			Serial.print(Whole);
			Serial.print(".");
			if (Fract < 10) {
				Serial.print("0");
			}
			Serial.println(Fract);
		}
	} while (y);
}

void
loop()
{
	byte        error, address;
	int         nDevices;

//      Serial.print("atmega32u4 internal temperatue: ");
//      Serial.print(GetTemp(),1);
//      Serial.println(" C");

	Serial.println("Scanning I2C...");

	nDevices = 0;
	for (address = 0; address <= 127; address++) {
		// Use the return value of
		// the Write.endTransmisstion to see if
		// a device did acknowledge to the address.
		Wire.beginTransmission(address);
		error = Wire.endTransmission();

		if (error == 0) {
			delay(10);
			Serial.print("Found device ");
			delay(10);
			Serial.print("at address 0x");
			delay(10);
			if (address < 16)
				Serial.print("0");
			Serial.print(address, HEX);
			nDevices++;
			if (address == 0x1a) {
				Serial.println(", assuming DS2482");
				onewirescan();
			} else {
				Serial.println("");
			}

		} else if (error == 4) {
			Serial.print("Unknow error at address 0x");
			if (address < 16)
				Serial.print("0");
			Serial.println(address, HEX);
		}
	}
	if (nDevices == 0) {
		Serial.println("No I2C devices found");
	} else {
		Serial.println("done");
	}

	digitalWrite(led1Pin, HIGH);	// sets the LED 1 on
	delay(50);
	digitalWrite(led2Pin, HIGH);	// sets the LED 2 on
	delay(50);
	digitalWrite(led3Pin, HIGH);	// sets the LED 3 on
	delay(50);
	digitalWrite(led1Pin, LOW);	// sets the LED 4 on
	delay(50);
	digitalWrite(led2Pin, LOW);	// sets the LED 5 on
	delay(50);
	digitalWrite(led3Pin, LOW);	// sets the LED 6 on
	delay(50);
}
