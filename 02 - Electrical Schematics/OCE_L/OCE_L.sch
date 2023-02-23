EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 4
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 650  1750 0    50   ~ 0
Teensy 4.1\n\nEthernet Out - DONE\n12 Thermistor In - DONE\n1 Vacuum Sense In - DONE\n2 Analog In (buffered with Op Amp) with variable gain - use the LM324 - DONE\n2 Analog Out (0 - 12 V)  - DONE\nHeater Drive Out to Level Shifter - DONE\nBlower Drive Out to Level Shifter - DONE\nAuxiliary Drive Out to Level Shifter - DONE\nOvenOK Drive Out to Level Shifter - DONE\n\nLCD Drive Out - I2C Interface\n5 buttons (in out up down run) - DONE\nState Indicator LEDs - temperature, vacuum, state - DONE
$Sheet
S 8300 4700 2300 1500
U 618DFD7C
F0 "Thermistor" 50
F1 "Thermistor.sch" 50
$EndSheet
$Sheet
S 8250 3500 1300 800 
U 6195EB8F
F0 "UserInterface" 50
F1 "UserInterface.sch" 50
$EndSheet
Text GLabel 4250 4650 2    50   Input ~ 0
TH02_IN
Text GLabel 4250 4750 2    50   Input ~ 0
TH01_IN
Text GLabel 2650 4650 0    50   Input ~ 0
TH03_IN
Text GLabel 2650 4750 0    50   Input ~ 0
TH04_IN
Text GLabel 2650 4850 0    50   Input ~ 0
TH05_IN
Text GLabel 2650 4950 0    50   Input ~ 0
TH06_IN
Text GLabel 2650 5050 0    50   Input ~ 0
TH07_IN
Text GLabel 2650 5150 0    50   Input ~ 0
TH08_IN
Text GLabel 2650 5250 0    50   Input ~ 0
TH09_IN
Text GLabel 2650 5350 0    50   Input ~ 0
TH10_IN
Text GLabel 4250 5350 2    50   Input ~ 0
SCL2
Text GLabel 4250 5250 2    50   Input ~ 0
SDA2
Text GLabel 2650 5450 0    50   Input ~ 0
TH11_IN
Text GLabel 4250 3450 2    50   Input ~ 0
SW4
Text GLabel 2650 2550 0    50   Input ~ 0
SW3
Text GLabel 2650 4250 0    50   Input ~ 0
SW2
Text GLabel 2650 4150 0    50   Input ~ 0
SW1
Text GLabel 2650 3250 0    50   Input ~ 0
SW5
Text GLabel 2650 3350 0    50   Input ~ 0
LED1
Text GLabel 4250 3550 2    50   Input ~ 0
LED2
Text GLabel 4250 4050 2    50   Input ~ 0
LED3
Text GLabel 4250 2850 2    50   Input ~ 0
ETH_RX+
Text GLabel 4250 2750 2    50   Input ~ 0
ETH_RX-
Text GLabel 4250 2650 2    50   Input ~ 0
ETH_TX+
Text GLabel 4250 2550 2    50   Input ~ 0
ETH_TX-
Text GLabel 4250 2450 2    50   Input ~ 0
ETH_LED
Text GLabel 4250 5450 2    50   Input ~ 0
TH12_IN
Text GLabel 4250 5050 2    50   Input ~ 0
VS1
Text GLabel 4250 4950 2    50   Input ~ 0
UA1
Text GLabel 4250 4850 2    50   Input ~ 0
UA2
$Comp
L Transistor_Array:ULQ2004A U2
U 1 1 62198034
P 8550 1400
F 0 "U2" H 8550 2067 50  0000 C CNN
F 1 "ULQ2004A" H 8550 1976 50  0000 C CNN
F 2 "Package_SO:SOIC-16_4.55x10.3mm_P1.27mm" H 8600 850 50  0001 L CNN
F 3 "http://www.ti.com/lit/ds/symlink/uln2003a.pdf" H 8650 1200 50  0001 C CNN
	1    8550 1400
	1    0    0    -1  
$EndComp
Text GLabel 9300 1000 2    50   Input ~ 0
12V_F
Text GLabel 8450 2200 0    50   Input ~ 0
GND
Wire Wire Line
	8450 2200 8550 2200
Wire Wire Line
	8550 2200 8550 2000
Wire Wire Line
	8950 1000 9300 1000
Text GLabel 9950 1400 2    50   Input ~ 0
HTR1
Text GLabel 9950 1200 2    50   Input ~ 0
BLW1
Text GLabel 9950 1500 2    50   Input ~ 0
IRO1
Text GLabel 9950 1300 2    50   Input ~ 0
AUX1
Text GLabel 4250 4350 2    50   Input ~ 0
IRO1_EN
Text GLabel 4250 4250 2    50   Input ~ 0
HTR1_EN
Text GLabel 2650 4450 0    50   Input ~ 0
BLW1_EN
Text GLabel 4250 3650 2    50   Input ~ 0
AUX1_EN
Text GLabel 7900 1500 0    50   Input ~ 0
IRO1_EN
Text GLabel 7900 1400 0    50   Input ~ 0
HTR1_EN
Text GLabel 7900 1200 0    50   Input ~ 0
BLW1_EN
Text GLabel 7900 1300 0    50   Input ~ 0
AUX1_EN
Wire Wire Line
	7900 1500 8150 1500
Wire Wire Line
	7900 1400 8150 1400
Wire Wire Line
	7900 1300 8150 1300
Wire Wire Line
	7900 1200 8150 1200
$Sheet
S 6950 3550 1150 800 
U 621A3A39
F0 "Mains" 50
F1 "Mains.sch" 50
$EndSheet
Text GLabel 7900 1600 0    50   Input ~ 0
LED3
Text GLabel 7900 1700 0    50   Input ~ 0
LED2
Text GLabel 7900 1800 0    50   Input ~ 0
LED1
Wire Wire Line
	7900 1600 8150 1600
Wire Wire Line
	7900 1700 8150 1700
Wire Wire Line
	7900 1800 8150 1800
Text GLabel 9950 1800 2    50   Input ~ 0
LED1_DR
Text GLabel 9950 1700 2    50   Input ~ 0
LED2_DR
Text GLabel 9950 1600 2    50   Input ~ 0
LED3_DR
Wire Wire Line
	8950 1700 9150 1700
Wire Wire Line
	8950 1800 9000 1800
Text GLabel 2650 3550 0    50   Input ~ 0
UA3_OUT
Text GLabel 2650 3450 0    50   Input ~ 0
UA4_OUT
Text GLabel 4250 5650 2    50   Input ~ 0
GND
Text GLabel 4250 5750 2    50   Input ~ 0
GND
$Comp
L Connector:Barrel_Jack J2
U 1 1 625CD79A
P 4950 1100
F 0 "J2" V 4961 1288 50  0000 L CNN
F 1 "Barrel_Jack" V 5052 1288 50  0000 L CNN
F 2 "OCE_L:Kycon-KLDX-0202-A-MFG" H 5000 1060 50  0001 C CNN
F 3 "~" H 5000 1060 50  0001 C CNN
	1    4950 1100
	0    1    1    0   
$EndComp
Text Notes 5250 850  0    50   ~ 0
12V in @ 0.8 A
Text GLabel 4850 1600 3    50   Input ~ 0
GND
Wire Wire Line
	4850 1600 4850 1500
Text GLabel 5050 1600 3    50   Input ~ 0
12V
Wire Wire Line
	4850 1500 4600 1500
Wire Wire Line
	4600 1500 4600 2050
Connection ~ 4850 1500
Wire Wire Line
	4850 1500 4850 1400
Text GLabel 6350 1550 2    50   Input ~ 0
3V3
Text GLabel 4250 1950 2    50   Input ~ 0
5V
Text GLabel 4250 5150 2    50   Input ~ 0
PIT1
$Comp
L OCE_L:J1B1211CCD J9
U 1 1 61D07300
P 5050 6050
F 0 "J9" H 5578 5553 60  0000 L CNN
F 1 "J1B1211CCD" H 5578 5447 60  0000 L CNN
F 2 "OCE_L:J1B1211CCD" H 5450 5390 60  0001 C CNN
F 3 "" H 5050 6050 60  0000 C CNN
	1    5050 6050
	1    0    0    -1  
$EndComp
Text GLabel 4150 6150 0    50   Input ~ 0
ETH_RX+
Text GLabel 4100 6950 0    50   Input ~ 0
ETH_LED
Text GLabel 4150 6450 0    50   Input ~ 0
ETH_TX+
Text GLabel 4150 6550 0    50   Input ~ 0
ETH_TX-
Text GLabel 3950 6850 0    50   Input ~ 0
GND
Text GLabel 4150 6250 0    50   Input ~ 0
ETH_RX-
$Comp
L Device:C C22
U 1 1 61D0C513
P 4700 6350
F 0 "C22" V 4448 6350 50  0000 C CNN
F 1 "0.1uF" V 4539 6350 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 4738 6200 50  0001 C CNN
F 3 "~" H 4700 6350 50  0001 C CNN
	1    4700 6350
	0    1    1    0   
$EndComp
Wire Wire Line
	4100 6950 5050 6950
Wire Wire Line
	3950 6850 4950 6850
Wire Wire Line
	4550 6050 4550 6350
Wire Wire Line
	4550 6050 5050 6050
Wire Wire Line
	4550 6650 4550 6350
Wire Wire Line
	4550 6650 5050 6650
Connection ~ 4550 6350
Wire Wire Line
	4850 6350 4950 6350
Wire Wire Line
	4950 6350 4950 6600
Wire Wire Line
	4950 6600 4900 6600
Wire Wire Line
	4900 6600 4900 6700
Wire Wire Line
	4900 6700 4950 6700
Wire Wire Line
	4950 6700 4950 6850
Connection ~ 4950 6350
Wire Wire Line
	4950 6350 5050 6350
Connection ~ 4950 6850
Wire Wire Line
	4950 6850 5050 6850
$Comp
L Regulator_Linear:AZ1117-3.3 U1
U 1 1 61E719DC
P 5900 1550
F 0 "U1" H 5900 1792 50  0000 C CNN
F 1 "AZ1117-3.3" H 5900 1701 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 5900 1800 50  0001 C CIN
F 3 "https://www.diodes.com/assets/Datasheets/AZ1117.pdf" H 5900 1550 50  0001 C CNN
	1    5900 1550
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:AZ1117-5.0 U5
U 1 1 61E72DD7
P 5900 2400
F 0 "U5" H 5900 2642 50  0000 C CNN
F 1 "AZ1117-5.0" H 5900 2551 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 5900 2650 50  0001 C CIN
F 3 "https://www.diodes.com/assets/Datasheets/AZ1117.pdf" H 5900 2400 50  0001 C CNN
	1    5900 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5900 1850 5900 2050
Text GLabel 5300 2400 0    50   Input ~ 0
12V_F
Text GLabel 6400 2400 2    50   Input ~ 0
5V
Wire Wire Line
	6400 2400 6250 2400
Text GLabel 5900 2850 3    50   Input ~ 0
GND
Wire Wire Line
	5900 2850 5900 2700
Wire Wire Line
	6200 1550 6250 1550
$Comp
L Device:C C1
U 1 1 61E9034E
P 5450 1800
F 0 "C1" H 5565 1846 50  0000 L CNN
F 1 "10 uF" H 5565 1755 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5488 1650 50  0001 C CNN
F 3 "~" H 5450 1800 50  0001 C CNN
	1    5450 1800
	1    0    0    -1  
$EndComp
$Comp
L Device:C C23
U 1 1 61E90A7C
P 6250 1800
F 0 "C23" H 6365 1846 50  0000 L CNN
F 1 "22 uF" H 6365 1755 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6288 1650 50  0001 C CNN
F 3 "~" H 6250 1800 50  0001 C CNN
	1    6250 1800
	1    0    0    -1  
$EndComp
$Comp
L Device:C C24
U 1 1 61E90E81
P 6250 2250
F 0 "C24" H 6365 2296 50  0000 L CNN
F 1 "22 uF" H 6365 2205 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6288 2100 50  0001 C CNN
F 3 "~" H 6250 2250 50  0001 C CNN
	1    6250 2250
	1    0    0    -1  
$EndComp
Connection ~ 6250 2400
Wire Wire Line
	6250 2400 6200 2400
Wire Wire Line
	5300 2400 5450 2400
Connection ~ 5450 2400
Wire Wire Line
	5450 2400 5600 2400
Wire Wire Line
	5450 2100 5450 2050
Connection ~ 5450 2050
Wire Wire Line
	5450 2050 5900 2050
Wire Wire Line
	5900 2050 6250 2050
Wire Wire Line
	6250 2050 6250 1950
Connection ~ 5900 2050
Wire Wire Line
	6250 2050 6250 2100
Connection ~ 6250 2050
Wire Wire Line
	6250 1650 6250 1550
Connection ~ 6250 1550
Wire Wire Line
	6250 1550 6350 1550
$Comp
L Device:C C2
U 1 1 61E91396
P 5450 2250
F 0 "C2" H 5565 2296 50  0000 L CNN
F 1 "10 uF" H 5565 2205 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5488 2100 50  0001 C CNN
F 3 "~" H 5450 2250 50  0001 C CNN
	1    5450 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 6150 5050 6150
Wire Wire Line
	4150 6250 5050 6250
Wire Wire Line
	4150 6450 5050 6450
Wire Wire Line
	4150 6550 5050 6550
$Comp
L OCE_L:TJA1044 U6
U 1 1 61D0D3D9
P 5600 4850
F 0 "U6" H 5600 5225 50  0000 C CNN
F 1 "TJA1044" H 5600 5134 50  0000 C CNN
F 2 "OCE_L:TJA1044" H 5600 5200 50  0001 C CNN
F 3 "" H 5600 5200 50  0001 C CNN
	1    5600 4850
	1    0    0    -1  
$EndComp
Text GLabel 5300 4800 0    50   Input ~ 0
GND
Text GLabel 5300 4900 0    50   Input ~ 0
5V
Text GLabel 2650 3850 0    50   Input ~ 0
CAN_RX
Text GLabel 2650 3950 0    50   Input ~ 0
CAN_TX
Text GLabel 5900 4700 2    50   Input ~ 0
GND
$Comp
L Connector:Conn_01x03_Female J6
U 1 1 61D18AC0
P 7200 4900
F 0 "J6" H 7228 4926 50  0000 L CNN
F 1 "Conn_01x03_Female" H 7228 4835 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x03_P2.54mm_Vertical" H 7200 4900 50  0001 C CNN
F 3 "~" H 7200 4900 50  0001 C CNN
	1    7200 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 4800 6200 4600
Wire Wire Line
	6200 4600 5900 4600
Wire Wire Line
	5900 4600 5900 4700
Wire Wire Line
	5900 4800 6100 4800
Wire Wire Line
	6100 4800 6100 4900
Wire Wire Line
	5900 4900 6050 4900
Wire Wire Line
	6050 4900 6050 5000
$Comp
L Device:R R3
U 1 1 61D46477
P 6550 4550
F 0 "R3" V 6343 4550 50  0000 C CNN
F 1 "60" V 6434 4550 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6480 4550 50  0001 C CNN
F 3 "~" H 6550 4550 50  0001 C CNN
	1    6550 4550
	0    1    1    0   
$EndComp
$Comp
L Device:R R6
U 1 1 61D45575
P 6550 4700
F 0 "R6" V 6400 4950 50  0000 C CNN
F 1 "60" V 6500 4950 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 6480 4700 50  0001 C CNN
F 3 "~" H 6550 4700 50  0001 C CNN
	1    6550 4700
	0    1    1    0   
$EndComp
Wire Wire Line
	6350 4800 6350 4700
Wire Wire Line
	6350 4700 6400 4700
Wire Wire Line
	6200 4800 6350 4800
Wire Wire Line
	6350 4700 6350 4550
Wire Wire Line
	6350 4550 6400 4550
Connection ~ 6350 4700
Wire Wire Line
	6100 4900 6750 4900
Wire Wire Line
	6350 4800 7000 4800
Connection ~ 6350 4800
Wire Wire Line
	6700 4700 6700 5000
Wire Wire Line
	6050 5000 6700 5000
Connection ~ 6700 5000
Wire Wire Line
	6700 5000 7000 5000
Wire Wire Line
	6700 4550 6750 4550
Wire Wire Line
	6750 4550 6750 4900
Connection ~ 6750 4900
Wire Wire Line
	6750 4900 7000 4900
$Comp
L OCE_L:DEV-16771 IC1
U 1 1 61E4961D
P 3450 3850
F 0 "IC1" H 3450 6017 50  0000 C CNN
F 1 "DEV-16771" H 3450 5926 50  0000 C CNN
F 2 "OCE_L:MODULE_DEV-16771" H 3450 3850 50  0001 L BNN
F 3 "" H 3450 3850 50  0001 L BNN
F 4 "Manufacturer recommendations" H 3450 3850 50  0001 L BNN "STANDARD"
F 5 "4.1" H 3450 3850 50  0001 L BNN "PARTREV"
F 6 "SparkFun Electronics" H 3450 3850 50  0001 L BNN "MANUFACTURER"
F 7 "4.07mm" H 3450 3850 50  0001 L BNN "MAXIMUM_PACKAGE_HEIGHT"
	1    3450 3850
	1    0    0    -1  
$EndComp
Text GLabel 4250 3350 2    50   Input ~ 0
CAN_TX2
Text GLabel 5300 5000 0    50   Input ~ 0
CAN_RX
Text GLabel 5300 4700 0    50   Input ~ 0
CAN_TX
$Comp
L OCE_L:BK-912-TR B1
U 1 1 61EA7928
P 5250 3400
F 0 "B1" H 5550 3687 60  0000 C CNN
F 1 "BK-912-TR" H 5550 3581 60  0000 C CNN
F 2 "OCE_L:BK-912-TR" H 5400 3040 60  0001 C CNN
F 3 "" H 5250 3400 60  0000 C CNN
	1    5250 3400
	1    0    0    -1  
$EndComp
Text GLabel 5850 3400 2    50   Input ~ 0
GND
Text GLabel 5200 3400 0    50   Input ~ 0
VBAT
Wire Wire Line
	5250 3500 5250 3400
Wire Wire Line
	5200 3400 5250 3400
Connection ~ 5250 3400
Text GLabel 4250 2250 2    50   Input ~ 0
VBAT
$Comp
L Connector:Screw_Terminal_01x02 J1
U 1 1 61EA5286
P 4400 1150
F 0 "J1" V 4364 962 50  0000 R CNN
F 1 "Screw_Terminal_01x02" V 4273 962 50  0000 R CNN
F 2 "OCE_L:oce" H 4400 1150 50  0001 C CNN
F 3 "~" H 4400 1150 50  0001 C CNN
	1    4400 1150
	0    -1   -1   0   
$EndComp
Text GLabel 4500 1350 3    50   Input ~ 0
12V
Text GLabel 4400 1350 3    50   Input ~ 0
GND
Text GLabel 2650 3150 0    50   Input ~ 0
SEN_IL
$Comp
L OCE_L:RAC10-12SK_277 PS1
U 1 1 61E71BD8
P 6500 1000
F 0 "PS1" H 6500 633 50  0000 C CNN
F 1 "RAC10-12SK_277" H 6500 724 50  0000 C CNN
F 2 "OCE_L:CONV_RAC10-12SK_277" H 6500 1000 50  0001 L BNN
F 3 "" H 6500 1000 50  0001 L BNN
F 4 "8/2020" H 6500 1000 50  0001 L BNN "PARTREV"
F 5 "23.5mm" H 6500 1000 50  0001 L BNN "MAXIMUM_PACKAGE_HEIGHT"
F 6 "RECOM" H 6500 1000 50  0001 L BNN "MANUFACTURER"
F 7 "Manufacturer Recommendations" H 6500 1000 50  0001 L BNN "STANDARD"
	1    6500 1000
	-1   0    0    1   
$EndComp
Wire Wire Line
	7300 1100 7200 1100
Wire Wire Line
	7300 900  7200 900 
Text GLabel 5750 900  0    50   Input ~ 0
GND
Text GLabel 5750 1100 0    50   Input ~ 0
12V
Wire Wire Line
	5750 1100 5800 1100
Wire Wire Line
	5750 900  5800 900 
$Comp
L Connector:Screw_Terminal_01x02 J8
U 1 1 61E8CF6E
P 7550 950
F 0 "J8" V 7514 762 50  0000 R CNN
F 1 "Screw_Terminal_01x02" V 7423 762 50  0000 R CNN
F 2 "OCE_L:oce" H 7550 950 50  0001 C CNN
F 3 "~" H 7550 950 50  0001 C CNN
	1    7550 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	7300 900  7300 950 
Wire Wire Line
	7300 950  7350 950 
Wire Wire Line
	7300 1100 7300 1050
Wire Wire Line
	7300 1050 7350 1050
$Comp
L Device:Fuse F1
U 1 1 62128E95
P 3900 800
F 0 "F1" V 3703 800 50  0000 C CNN
F 1 "1 A 1206  Fuse" V 3794 800 50  0000 C CNN
F 2 "Fuse:Fuse_1206_3216Metric" V 3830 800 50  0001 C CNN
F 3 "~" H 3900 800 50  0001 C CNN
	1    3900 800 
	0    1    1    0   
$EndComp
Text GLabel 3650 800  0    50   Input ~ 0
12V
Text GLabel 4150 800  2    50   Input ~ 0
12V_F
Wire Wire Line
	4150 800  4100 800 
Wire Wire Line
	3750 800  3650 800 
Wire Wire Line
	5050 1400 5050 1600
Text GLabel 5350 1550 0    50   Input ~ 0
12V_F
Wire Wire Line
	4600 2050 5450 2050
Wire Wire Line
	5450 1950 5450 2050
Wire Wire Line
	5450 1650 5450 1550
Wire Wire Line
	5450 1550 5600 1550
Wire Wire Line
	5350 1550 5450 1550
Connection ~ 5450 1550
$Comp
L Device:LED D4
U 1 1 620E9C71
P 9000 2100
F 0 "D4" V 9100 2050 50  0000 R CNN
F 1 "LED" V 9200 2100 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 9000 2100 50  0001 C CNN
F 3 "~" H 9000 2100 50  0001 C CNN
	1    9000 2100
	0    1    1    0   
$EndComp
$Comp
L Device:R R51
U 1 1 621014DA
P 9000 2550
F 0 "R51" H 9000 2400 50  0000 L CNN
F 1 "1k" H 9000 2700 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 8930 2550 50  0001 C CNN
F 3 "~" H 9000 2550 50  0001 C CNN
	1    9000 2550
	-1   0    0    1   
$EndComp
Wire Wire Line
	10050 2750 9900 2750
Wire Wire Line
	9000 2750 9000 2700
Wire Wire Line
	9150 2700 9150 2750
Connection ~ 9150 2750
Wire Wire Line
	9150 2750 9000 2750
Wire Wire Line
	9300 2700 9300 2750
Connection ~ 9300 2750
Wire Wire Line
	9300 2750 9150 2750
Wire Wire Line
	9450 2700 9450 2750
Connection ~ 9450 2750
Wire Wire Line
	9450 2750 9300 2750
Wire Wire Line
	9600 2700 9600 2750
Connection ~ 9600 2750
Wire Wire Line
	9600 2750 9450 2750
Wire Wire Line
	9750 2700 9750 2750
Connection ~ 9750 2750
Wire Wire Line
	9750 2750 9600 2750
Wire Wire Line
	9900 2700 9900 2750
Connection ~ 9900 2750
Wire Wire Line
	9900 2750 9750 2750
$Comp
L Device:LED D5
U 1 1 621400C8
P 9150 2100
F 0 "D5" V 9250 2050 50  0000 R CNN
F 1 "LED" V 9350 2100 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 9150 2100 50  0001 C CNN
F 3 "~" H 9150 2100 50  0001 C CNN
	1    9150 2100
	0    1    1    0   
$EndComp
$Comp
L Device:LED D6
U 1 1 621404DD
P 9300 2100
F 0 "D6" V 9400 2050 50  0000 R CNN
F 1 "LED" V 9500 2100 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 9300 2100 50  0001 C CNN
F 3 "~" H 9300 2100 50  0001 C CNN
	1    9300 2100
	0    1    1    0   
$EndComp
$Comp
L Device:LED D7
U 1 1 621407AE
P 9450 2100
F 0 "D7" V 9550 2050 50  0000 R CNN
F 1 "LED" V 9650 2100 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 9450 2100 50  0001 C CNN
F 3 "~" H 9450 2100 50  0001 C CNN
	1    9450 2100
	0    1    1    0   
$EndComp
$Comp
L Device:LED D8
U 1 1 62140A27
P 9600 2100
F 0 "D8" V 9700 2050 50  0000 R CNN
F 1 "LED" V 9800 2100 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 9600 2100 50  0001 C CNN
F 3 "~" H 9600 2100 50  0001 C CNN
	1    9600 2100
	0    1    1    0   
$EndComp
$Comp
L Device:LED D9
U 1 1 62140CAE
P 9750 2100
F 0 "D9" V 9850 2050 50  0000 R CNN
F 1 "LED" V 9950 2100 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 9750 2100 50  0001 C CNN
F 3 "~" H 9750 2100 50  0001 C CNN
	1    9750 2100
	0    1    1    0   
$EndComp
$Comp
L Device:LED D10
U 1 1 62140E69
P 9900 2100
F 0 "D10" V 10000 2050 50  0000 R CNN
F 1 "LED" V 10100 2100 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 9900 2100 50  0001 C CNN
F 3 "~" H 9900 2100 50  0001 C CNN
	1    9900 2100
	0    1    1    0   
$EndComp
Wire Wire Line
	9000 2250 9000 2400
$Comp
L Device:R R52
U 1 1 62154CB0
P 9150 2550
F 0 "R52" H 9150 2400 50  0000 L CNN
F 1 "1k" H 9150 2700 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9080 2550 50  0001 C CNN
F 3 "~" H 9150 2550 50  0001 C CNN
	1    9150 2550
	-1   0    0    1   
$EndComp
$Comp
L Device:R R53
U 1 1 62154ECE
P 9300 2550
F 0 "R53" H 9300 2400 50  0000 L CNN
F 1 "1k" H 9300 2700 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9230 2550 50  0001 C CNN
F 3 "~" H 9300 2550 50  0001 C CNN
	1    9300 2550
	-1   0    0    1   
$EndComp
$Comp
L Device:R R54
U 1 1 62155094
P 9450 2550
F 0 "R54" H 9450 2400 50  0000 L CNN
F 1 "1k" H 9450 2700 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9380 2550 50  0001 C CNN
F 3 "~" H 9450 2550 50  0001 C CNN
	1    9450 2550
	-1   0    0    1   
$EndComp
$Comp
L Device:R R55
U 1 1 62155232
P 9600 2550
F 0 "R55" H 9600 2400 50  0000 L CNN
F 1 "1k" H 9600 2700 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9530 2550 50  0001 C CNN
F 3 "~" H 9600 2550 50  0001 C CNN
	1    9600 2550
	-1   0    0    1   
$EndComp
$Comp
L Device:R R56
U 1 1 6215547B
P 9750 2550
F 0 "R56" H 9750 2400 50  0000 L CNN
F 1 "1k" H 9750 2700 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9680 2550 50  0001 C CNN
F 3 "~" H 9750 2550 50  0001 C CNN
	1    9750 2550
	-1   0    0    1   
$EndComp
$Comp
L Device:R R57
U 1 1 621556B9
P 9900 2550
F 0 "R57" H 9900 2400 50  0000 L CNN
F 1 "1k" H 9900 2700 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 9830 2550 50  0001 C CNN
F 3 "~" H 9900 2550 50  0001 C CNN
	1    9900 2550
	-1   0    0    1   
$EndComp
Wire Wire Line
	9150 2400 9150 2250
Wire Wire Line
	9300 2400 9300 2250
Wire Wire Line
	9450 2400 9450 2250
Wire Wire Line
	9600 2400 9600 2250
Wire Wire Line
	9750 2400 9750 2250
Wire Wire Line
	9900 2400 9900 2250
Wire Wire Line
	9000 1950 9000 1800
Connection ~ 9000 1800
Wire Wire Line
	9150 1950 9150 1700
Connection ~ 9150 1700
Wire Wire Line
	9000 1800 9950 1800
Wire Wire Line
	9150 1700 9950 1700
Wire Wire Line
	8950 1600 9300 1600
Wire Wire Line
	8950 1500 9450 1500
Wire Wire Line
	8950 1400 9600 1400
Wire Wire Line
	8950 1300 9750 1300
Wire Wire Line
	8950 1200 9900 1200
Wire Wire Line
	9300 1950 9300 1600
Connection ~ 9300 1600
Wire Wire Line
	9300 1600 9950 1600
Wire Wire Line
	9450 1950 9450 1500
Connection ~ 9450 1500
Wire Wire Line
	9450 1500 9950 1500
Wire Wire Line
	9600 1950 9600 1400
Connection ~ 9600 1400
Wire Wire Line
	9600 1400 9950 1400
Wire Wire Line
	9750 1950 9750 1300
Connection ~ 9750 1300
Wire Wire Line
	9750 1300 9950 1300
Wire Wire Line
	9900 1950 9900 1200
Connection ~ 9900 1200
Wire Wire Line
	9900 1200 9950 1200
$Comp
L Device:LED D3
U 1 1 621E1A81
P 4100 1550
F 0 "D3" V 4200 1500 50  0000 R CNN
F 1 "LED" V 4300 1550 50  0000 R CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 4100 1550 50  0001 C CNN
F 3 "~" H 4100 1550 50  0001 C CNN
	1    4100 1550
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R48
U 1 1 621E1A87
P 4100 1100
F 0 "R48" H 4100 950 50  0000 L CNN
F 1 "1k" H 4100 1250 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 4030 1100 50  0001 C CNN
F 3 "~" H 4100 1100 50  0001 C CNN
	1    4100 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 1250 4100 1400
Wire Wire Line
	4100 950  4100 800 
Connection ~ 4100 800 
Wire Wire Line
	4100 800  4050 800 
Text GLabel 4000 1700 0    50   Input ~ 0
GND
Wire Wire Line
	4000 1700 4100 1700
Text GLabel 10050 2750 2    50   Input ~ 0
12V_IL
$EndSCHEMATC
