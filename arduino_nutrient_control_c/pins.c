/* Pin assignments version b
 
 Arduino Mega 2560 

VIN			
VIN			
GND			
GND			
5V			
5V			
3V3			
3V3			
AREF			
RST			
	 nc	0	RX			
	 nc	1	TX			
INT4	2	D2	PWM			INT0		FLOW1
INT5	3	D3	PWM			INT1		Interrupt
		4	D4	PWM	Q1	12V   7   BRs	Valve 1
		5	D5	PWM	Q2	12V   8   BR	Valve 2
		6	D6	PWM	Q3	12V   6   G		Valve 3
		7	D7	PWM	Q4	12V   4   B		Valve 4
		8	D8	PWM	Q5	12V   3   O		Valve 5
		9	D9	PWM	Q6 	12V   1   Os	Valve 6
		10	D10	PWM	Q7	12V   2   Gs	Bottle Mixer
	 nc	11	D11	PWM
	 	12	D12	PWM	EXT
	 nc	13	D13	PWM
	 nc	14	D14	TX
	 nc	15	D15	RX
	 nc	16	D16
	 nc	17	D17
INT3 nc	18	D18
INT2 nc	19	D19
INT1	20	D20		SDA		I2C Data
INT0	21	D21		SCL		I2C Clock
	 nc	22	D22		SS_RAM	SPI Ram chip select
		23	D23		SS_INT	SPI internal MMC chip select / SPI Ram chip select
	 nc	24	D24
	 nc	25	D25
	 nc	26	D26
	 nc	27	D27
	 nc	28	D28		
	 nc	29	D29		
	 	30	D30		J1-3	Motor 0 -3
		31	D31		J1-4	Motor 0 -4
		32	D32		J5-1	Motor 4 -1	Nutrient 3
		33	D33		J5-2	Motor 4 -2
		34	D34		J5-3	Motor 4 -3
		35	D35		J5-4	Motor 4 -4
		36	D36		J4-1	Motor 3 -1	Nutrient 2
		37	D37		J4-2	Motor 3 -2
		38	D38		J4-3	Motor 3 -3
		39	D39		J4-4	Motor 3 -4
		40	D40		J3-1	Motor 2 -1	Nutrient 1
		41	D41		J3-2	Motor 2 -2
		42	D42		J3-3	Motor 2 -3
		43	D43		J3-4	Motor 2 -4
		44	D44	PWM	J2-1	Motor 1 -1	pH
		45	D45	PWM	J2-2	Motor 1 -2
		46	D46	PWM	J2-3	Motor 1 -3
		47	D47		J2-4	Motor 1 -4
		48	D48		J1-2	Motor 0 -2
		49	D49		J1-1	Motor 0 -1	H2O
		50	D50		MISO	Slave out
		51	D51		MOSI	Slave in
		52	D52		SCK		Clock
	 nc	53	D53		CS						* Not used for routing reasons
		54	A0		JS2-7	Sense 0		Humid 1
		55	A1		JS2-6	Sense 1		Humid 2
		56	A2		JS2-5	Sense 2		Floor 1
		57	A3		JS2-4	Sense 3		Floor 2
		58	A4		JS2-3	Sense 4		Pan 1
		59	A5		JS2-2	Sense 5		Pan 2
		60	A6		JS1-7	Sense 6		Pan 3
		61	A7		JS1-6	Sense 7		Pan 4
		62	A8		JS1-5	Sense 8		Plant 1
		63	A9		JS1-4	Sense 9		Plant 2
		64	A10		JS1-3	Sense 10	Plant 3
		65	A11		JS1-2	Sense 11	Plant 4
		66	A12		J6-1	Motor 5 -1	Nutrient 4
		67	A13		J6-2	Motor 5 -2
		68	A14		J6-3	Motor 5 -3
		69	A15		J6-4	Motor 5 -4
 */
 
/* Pin assignments version e
 
 Arduino Mega 2560 

VIN			
VIN			
GND			
GND			
5V			
5V			
3V3			
3V3			
AREF			
RST			
	 nc	0	RX			
	 nc	1	TX			
INT4	2	D2	PWM		FLOW1
INT5	3	D3	PWM		Interrupt
		4	D4	PWM	Q1	12V   7   BRs	Valve 1
		5	D5	PWM	Q2	12V   8   BR	Valve 2
		6	D6	PWM	Q3	12V   6   G		Valve 3
		7	D7	PWM	Q4	12V   4   B		Valve 4
		8	D8	PWM	Q5	12V   3   O		Valve 5
		9	D9	PWM	Q6 	12V   1   Os	Valve 6
		10	D10	PWM	Q7	12V   2   Gs	Bottle Mixer
	 	11	D11	PWM 		LED1
		12	D12	PWM 		OPT(_EXT)
	 	13	D13	PWM	->	 	D47
		14	D14	TX	XDA		I2C auxiliary Data
		15	D15	RX			(OPT)
	 	16	D16		XCL		I2C auxiliary Clock
	 	17	D17		GPSRX	GPS recieve
INT3 	18	D18		PPS		GPS pulse per second
INT2 	19	D19		GPSTX	GPS transmit
INT1	20	D20		SDA		I2C Data
INT0	21	D21		SCL		I2C Clock
	 	22	D22		SS_EXT	SPI external chip select 
		23	D23		SS_INT	SPI internal MMC chip select / SPI Ram chip select
	 nc	24	D24
	 nc	25	D25
	 nc	26	D26
	 nc	27	D27
	 nc	28	D28		
	 nc	29	D29		
	 	30	D30		J2-3	Motor 0 -3
		31	D31		J2-4	Motor 0 -4
		32	D32		J6-1	Motor 4 -1	Nutrient 3
		33	D33		J6-2	Motor 4 -2
		34	D34		J6-3	Motor 4 -3
		35	D35		J6-4	Motor 4 -4
		36	D36		J3-1	Motor 3 -1	Nutrient 2
		37	D37		J3-2	Motor 3 -2
		38	D38		J3-3	Motor 3 -3
		39	D39		J3-4	Motor 3 -4
		40	D40		J4-1	Motor 2 -1	Nutrient 1
		41	D41		J4-2	Motor 2 -2
		42	D42		J4-3	Motor 2 -3
		43	D43		J4-4	Motor 2 -4
		44	D44	PWM	J1-1	Motor 1 -1	pH
		45	D45	PWM	J1-2	Motor 1 -2
		46	D46	PWM	J1-3	Motor 1 -3
		47	D47	D13	J1-4	Motor 1 -4
		48	D48		J2-2	Motor 0 -2
		49	D49		J2-1	Motor 0 -1	H2O
		50	D50		MISO	Slave out
		51	D51		MOSI	Slave in
		52	D52		SCK		Clock
	 nc	53	D53		CS						* Not used for routing reasons
		54	A0		JS2-7	Sense 0
		55	A1		JS2-6	Sense 1
		56	A2		JS2-5	Sense 2
		57	A3		JS2-4	Sense 3
		58	A4		JS2-3	Sense 4
		59	A5		JS2-2	Sense 5
		60	A6		JS1-7	Sense 6
		61	A7		JS1-6	Sense 7
		62	A8		JS1-5	Sense 8
		63	A9		JS1-4	Sense 9
		64	A10		JS1-3	Sense 10
		65	A11		JS1-2	Sense 11
		66	A12		J5-1	Motor 5 -1	Nutrient 4
		67	A13		J5-2	Motor 5 -2
		68	A14		J5-3	Motor 5 -3
		69	A15		J5-4	Motor 5 -4
 */
