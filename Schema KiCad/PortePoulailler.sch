EESchema Schematic File Version 4
LIBS:PortePoulailler-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MCU_Module:Arduino_Nano_v3.x A1
U 1 1 5C83E8FD
P 5000 3400
F 0 "A1" H 5600 4350 50  0000 C CNN
F 1 "Arduino_Nano_v3.x" H 4350 4400 50  0000 C CNN
F 2 "Module:Arduino_Nano" H 5150 2450 50  0001 L CNN
F 3 "http://www.mouser.com/pdfdocs/Gravitech_Arduino_Nano3_0.pdf" H 5000 2400 50  0001 C CNN
	1    5000 3400
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J4
U 1 1 5C83EDD5
P 8000 1700
F 0 "J4" H 8079 1692 50  0000 L CNN
F 1 "+12V" H 8079 1601 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_Altech_AK300-2_P5.00mm" H 8000 1700 50  0001 C CNN
F 3 "~" H 8000 1700 50  0001 C CNN
	1    8000 1700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 5C83EEE7
P 7250 4250
F 0 "#PWR0101" H 7250 4000 50  0001 C CNN
F 1 "GND" H 7255 4077 50  0000 C CNN
F 2 "" H 7250 4250 50  0001 C CNN
F 3 "" H 7250 4250 50  0001 C CNN
	1    7250 4250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5C83EF15
P 5050 4500
F 0 "#PWR0102" H 5050 4250 50  0001 C CNN
F 1 "GND" H 5055 4327 50  0000 C CNN
F 2 "" H 5050 4500 50  0001 C CNN
F 3 "" H 5050 4500 50  0001 C CNN
	1    5050 4500
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 5C83FF05
P 5300 1600
F 0 "#FLG0101" H 5300 1675 50  0001 C CNN
F 1 "PWR_FLAG" H 5300 1774 50  0000 C CNN
F 2 "" H 5300 1600 50  0001 C CNN
F 3 "~" H 5300 1600 50  0001 C CNN
	1    5300 1600
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 5C83FF37
P 5800 1600
F 0 "#FLG0102" H 5800 1675 50  0001 C CNN
F 1 "PWR_FLAG" H 5800 1774 50  0000 C CNN
F 2 "" H 5800 1600 50  0001 C CNN
F 3 "~" H 5800 1600 50  0001 C CNN
	1    5800 1600
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0103
U 1 1 5C83FF85
P 5300 1600
F 0 "#PWR0103" H 5300 1450 50  0001 C CNN
F 1 "+12V" H 5315 1773 50  0000 C CNN
F 2 "" H 5300 1600 50  0001 C CNN
F 3 "" H 5300 1600 50  0001 C CNN
	1    5300 1600
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 5C83FFE3
P 5800 1600
F 0 "#PWR0104" H 5800 1350 50  0001 C CNN
F 1 "GND" H 5805 1427 50  0000 C CNN
F 2 "" H 5800 1600 50  0001 C CNN
F 3 "" H 5800 1600 50  0001 C CNN
	1    5800 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	7150 4050 7150 4150
Wire Wire Line
	7150 4150 7250 4150
Wire Wire Line
	7350 4150 7350 4050
Wire Wire Line
	7250 4250 7250 4150
Connection ~ 7250 4150
Wire Wire Line
	7250 4150 7350 4150
Wire Wire Line
	5000 4400 5050 4400
Wire Wire Line
	6750 3550 6700 3550
Wire Wire Line
	6700 3550 6700 3650
Wire Wire Line
	6700 3750 6750 3750
Wire Wire Line
	6750 3650 6700 3650
Connection ~ 6700 3650
Wire Wire Line
	6700 3650 6700 3750
Wire Wire Line
	6700 3650 6650 3650
Wire Wire Line
	6650 3650 6650 4150
Wire Wire Line
	6650 4150 7150 4150
Connection ~ 7150 4150
$Comp
L power:+5V #PWR0105
U 1 1 5C840BE8
P 5200 2400
F 0 "#PWR0105" H 5200 2250 50  0001 C CNN
F 1 "+5V" H 5215 2573 50  0000 C CNN
F 2 "" H 5200 2400 50  0001 C CNN
F 3 "" H 5200 2400 50  0001 C CNN
	1    5200 2400
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0106
U 1 1 5C840C84
P 7150 2550
F 0 "#PWR0106" H 7150 2400 50  0001 C CNN
F 1 "+5V" H 7165 2723 50  0000 C CNN
F 2 "" H 7150 2550 50  0001 C CNN
F 3 "" H 7150 2550 50  0001 C CNN
	1    7150 2550
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0107
U 1 1 5C841279
P 7350 2550
F 0 "#PWR0107" H 7350 2400 50  0001 C CNN
F 1 "+12V" H 7365 2723 50  0000 C CNN
F 2 "" H 7350 2550 50  0001 C CNN
F 3 "" H 7350 2550 50  0001 C CNN
	1    7350 2550
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0108
U 1 1 5C8412BB
P 7500 1700
F 0 "#PWR0108" H 7500 1550 50  0001 C CNN
F 1 "+12V" V 7515 1828 50  0000 L CNN
F 2 "" H 7500 1700 50  0001 C CNN
F 3 "" H 7500 1700 50  0001 C CNN
	1    7500 1700
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 5C84131A
P 7800 1800
F 0 "#PWR0109" H 7800 1550 50  0001 C CNN
F 1 "GND" V 7805 1672 50  0000 R CNN
F 2 "" H 7800 1800 50  0001 C CNN
F 3 "" H 7800 1800 50  0001 C CNN
	1    7800 1800
	0    1    1    0   
$EndComp
Text GLabel 4500 3000 0    50   Input ~ 0
Step
Text GLabel 6750 3250 0    50   Input ~ 0
Step
Text GLabel 4500 3100 0    50   Input ~ 0
Dir.
Text GLabel 6750 3350 0    50   Input ~ 0
Dir.
Text GLabel 4500 3200 0    50   Input ~ 0
Enable
Text GLabel 6750 3150 0    50   Input ~ 0
Enable
$Comp
L Device:CP C1
U 1 1 5C8416D8
P 6400 1700
F 0 "C1" H 6518 1746 50  0000 L CNN
F 1 "100uF 25V" H 6518 1655 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 6438 1550 50  0001 C CNN
F 3 "~" H 6400 1700 50  0001 C CNN
	1    6400 1700
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0110
U 1 1 5C841884
P 6400 1550
F 0 "#PWR0110" H 6400 1400 50  0001 C CNN
F 1 "+12V" H 6415 1723 50  0000 C CNN
F 2 "" H 6400 1550 50  0001 C CNN
F 3 "" H 6400 1550 50  0001 C CNN
	1    6400 1550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0111
U 1 1 5C8418CD
P 6400 1850
F 0 "#PWR0111" H 6400 1600 50  0001 C CNN
F 1 "GND" H 6405 1677 50  0000 C CNN
F 2 "" H 6400 1850 50  0001 C CNN
F 3 "" H 6400 1850 50  0001 C CNN
	1    6400 1850
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x05_Male J5
U 1 1 5C853CF7
P 8400 3250
F 0 "J5" H 8200 3100 50  0000 C CNN
F 1 "Connecteur moteur" H 7950 3250 50  0000 C CNN
F 2 "Connector_JST:JST_XH_B05B-XH-A_1x05_P2.50mm_Vertical" H 8400 3250 50  0001 C CNN
F 3 "~" H 8400 3250 50  0001 C CNN
	1    8400 3250
	-1   0    0    1   
$EndComp
Text GLabel 7650 3150 2    50   Input ~ 0
1B
Text GLabel 7650 3350 2    50   Input ~ 0
1A
Text GLabel 7650 3250 2    50   Input ~ 0
2A
Text GLabel 7650 3450 2    50   Input ~ 0
2B
Text GLabel 8200 3150 0    50   Input ~ 0
1B
Text GLabel 8200 3350 0    50   Input ~ 0
1A
Text GLabel 8200 3250 0    50   Input ~ 0
2A
Text GLabel 8200 3450 0    50   Input ~ 0
2B
$Comp
L power:+12V #PWR0112
U 1 1 5C855633
P 4900 2400
F 0 "#PWR0112" H 4900 2250 50  0001 C CNN
F 1 "+12V" H 4915 2573 50  0000 C CNN
F 2 "" H 4900 2400 50  0001 C CNN
F 3 "" H 4900 2400 50  0001 C CNN
	1    4900 2400
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J2
U 1 1 5C83EB57
P 3450 2650
F 0 "J2" H 3529 2642 50  0000 L CNN
F 1 "FdcHaut" H 3550 2550 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_Altech_AK300-2_P5.00mm" H 3450 2650 50  0001 C CNN
F 3 "~" H 3450 2650 50  0001 C CNN
	1    3450 2650
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x04_Male J3
U 1 1 5C855F56
P 3450 1550
F 0 "J3" H 3422 1430 50  0000 R CNN
F 1 "ConnecteurLed/Photoresistance" H 3422 1521 50  0000 R CNN
F 2 "Connector_JST:JST_XH_B04B-XH-A_1x04_P2.50mm_Vertical" H 3450 1550 50  0001 C CNN
F 3 "~" H 3450 1550 50  0001 C CNN
	1    3450 1550
	-1   0    0    1   
$EndComp
$Comp
L Device:R R3
U 1 1 5C856740
P 2950 1300
F 0 "R3" V 2950 1250 50  0000 L CNN
F 1 "10K" H 3050 1300 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 2880 1300 50  0001 C CNN
F 3 "~" H 2950 1300 50  0001 C CNN
	1    2950 1300
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR0113
U 1 1 5C8567EF
P 3150 1350
F 0 "#PWR0113" H 3150 1200 50  0001 C CNN
F 1 "+5V" H 3150 1500 50  0000 C CNN
F 2 "" H 3150 1350 50  0001 C CNN
F 3 "" H 3150 1350 50  0001 C CNN
	1    3150 1350
	1    0    0    -1  
$EndComp
Text GLabel 2850 1450 0    50   Input ~ 0
PhotoResistance
Text GLabel 5500 3400 2    50   Input ~ 0
PhotoResistance
$Comp
L power:GND #PWR0114
U 1 1 5C8570A8
P 3250 1550
F 0 "#PWR0114" H 3250 1300 50  0001 C CNN
F 1 "GND" V 3255 1422 50  0000 R CNN
F 2 "" H 3250 1550 50  0001 C CNN
F 3 "" H 3250 1550 50  0001 C CNN
	1    3250 1550
	0    1    1    0   
$EndComp
Text GLabel 2950 1650 0    50   Input ~ 0
LED
Text GLabel 4500 3300 0    50   Input ~ 0
LED
$Comp
L Device:C_Small C2
U 1 1 5C857229
P 3100 2700
F 0 "C2" H 3000 2900 50  0000 L CNN
F 1 "10nF" H 3150 2900 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D5.0mm_W2.5mm_P2.50mm" H 3100 2700 50  0001 C CNN
F 3 "~" H 3100 2700 50  0001 C CNN
	1    3100 2700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 5C85496B
P 2750 2350
F 0 "R1" H 2820 2396 50  0000 L CNN
F 1 "10K" H 2950 2400 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 2680 2350 50  0001 C CNN
F 3 "~" H 2750 2350 50  0001 C CNN
	1    2750 2350
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0115
U 1 1 5C854A01
P 2750 2200
F 0 "#PWR0115" H 2750 2050 50  0001 C CNN
F 1 "+5V" H 2765 2373 50  0000 C CNN
F 2 "" H 2750 2200 50  0001 C CNN
F 3 "" H 2750 2200 50  0001 C CNN
	1    2750 2200
	1    0    0    -1  
$EndComp
Text GLabel 2650 2600 0    50   Input ~ 0
FdcHaut
Wire Wire Line
	2750 2600 2750 2500
Wire Wire Line
	2650 2600 2750 2600
Wire Wire Line
	2750 2600 3100 2600
Connection ~ 2750 2600
Wire Wire Line
	3100 2600 3250 2600
Wire Wire Line
	3250 2600 3250 2650
Connection ~ 3100 2600
Wire Wire Line
	3250 2750 3250 2800
Wire Wire Line
	3250 2800 3100 2800
$Comp
L power:GND #PWR0116
U 1 1 5C855DED
P 3100 2800
F 0 "#PWR0116" H 3100 2550 50  0001 C CNN
F 1 "GND" H 3105 2627 50  0000 C CNN
F 2 "" H 3100 2800 50  0001 C CNN
F 3 "" H 3100 2800 50  0001 C CNN
	1    3100 2800
	1    0    0    -1  
$EndComp
Connection ~ 3100 2800
$Comp
L Connector:Screw_Terminal_01x02 J1
U 1 1 5C8563B5
P 3450 3900
F 0 "J1" H 3300 3650 50  0000 L CNN
F 1 "FdcBas" H 3200 4150 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_Altech_AK300-2_P5.00mm" H 3450 3900 50  0001 C CNN
F 3 "~" H 3450 3900 50  0001 C CNN
	1    3450 3900
	-1   0    0    1   
$EndComp
$Comp
L Device:C_Small C3
U 1 1 5C8563BC
P 3100 3850
F 0 "C3" H 3000 4050 50  0000 L CNN
F 1 "10nF" H 3150 4050 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D5.0mm_W2.5mm_P2.50mm" H 3100 3850 50  0001 C CNN
F 3 "~" H 3100 3850 50  0001 C CNN
	1    3100 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 5C8563C3
P 2750 3500
F 0 "R2" H 2820 3546 50  0000 L CNN
F 1 "10K" H 2950 3550 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 2680 3500 50  0001 C CNN
F 3 "~" H 2750 3500 50  0001 C CNN
	1    2750 3500
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0117
U 1 1 5C8563CA
P 2750 3350
F 0 "#PWR0117" H 2750 3200 50  0001 C CNN
F 1 "+5V" H 2765 3523 50  0000 C CNN
F 2 "" H 2750 3350 50  0001 C CNN
F 3 "" H 2750 3350 50  0001 C CNN
	1    2750 3350
	1    0    0    -1  
$EndComp
Text GLabel 2650 3750 0    50   Input ~ 0
FdcBas
Wire Wire Line
	2750 3750 2750 3650
Wire Wire Line
	2650 3750 2750 3750
Wire Wire Line
	2750 3750 3100 3750
Connection ~ 2750 3750
Wire Wire Line
	3100 3750 3250 3750
Connection ~ 3100 3750
Wire Wire Line
	3250 3950 3100 3950
$Comp
L power:GND #PWR0118
U 1 1 5C8563DA
P 3100 3950
F 0 "#PWR0118" H 3100 3700 50  0001 C CNN
F 1 "GND" H 3105 3777 50  0000 C CNN
F 2 "" H 3100 3950 50  0001 C CNN
F 3 "" H 3100 3950 50  0001 C CNN
	1    3100 3950
	1    0    0    -1  
$EndComp
Connection ~ 3100 3950
Text GLabel 5500 3500 2    50   Input ~ 0
FdcHaut
Text GLabel 5500 3600 2    50   Input ~ 0
FdcBas
Wire Wire Line
	5050 4500 5050 4400
Connection ~ 5050 4400
Wire Wire Line
	5050 4400 5100 4400
NoConn ~ 5100 2400
NoConn ~ 5500 2800
NoConn ~ 4500 2800
NoConn ~ 4500 2900
NoConn ~ 5500 2900
NoConn ~ 5500 3200
NoConn ~ 5500 3700
NoConn ~ 5500 3800
NoConn ~ 5500 3900
NoConn ~ 5500 4000
NoConn ~ 5500 4100
NoConn ~ 4500 3600
NoConn ~ 4500 3700
NoConn ~ 4500 3800
NoConn ~ 4500 3900
NoConn ~ 4500 4000
NoConn ~ 4500 4100
NoConn ~ 8200 3050
$Comp
L Driver_Motor:Pololu_Breakout_A4988 A2
U 1 1 5C83E9CD
P 7150 3250
F 0 "A2" H 7750 3650 50  0000 C CNN
F 1 "A4988" H 7750 3750 50  0000 C CNN
F 2 "Module:Pololu_Breakout-16_15.2x20.3mm" H 7425 2500 50  0001 L CNN
F 3 "https://www.pololu.com/product/2980/pictures" H 7250 2950 50  0001 C CNN
	1    7150 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	6750 2850 6750 2950
NoConn ~ 4500 3500
NoConn ~ 4500 3400
$Comp
L Device:R R4
U 1 1 5C88D5CF
P 3100 1650
F 0 "R4" V 3100 1650 50  0000 C CNN
F 1 "180ohm" V 3200 1650 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0414_L11.9mm_D4.5mm_P15.24mm_Horizontal" V 3030 1650 50  0001 C CNN
F 3 "~" H 3100 1650 50  0001 C CNN
	1    3100 1650
	0    1    1    0   
$EndComp
Wire Wire Line
	3650 3800 3650 3700
Wire Wire Line
	3650 3700 3250 3700
Wire Wire Line
	3250 3700 3250 3750
Wire Wire Line
	3650 3900 3650 4000
Wire Wire Line
	3650 4000 3250 4000
Wire Wire Line
	3250 4000 3250 3950
$Comp
L power:GND #PWR0119
U 1 1 5C8918D9
P 2950 1150
F 0 "#PWR0119" H 2950 900 50  0001 C CNN
F 1 "GND" H 3050 1000 50  0000 R CNN
F 2 "" H 2950 1150 50  0001 C CNN
F 3 "" H 2950 1150 50  0001 C CNN
	1    2950 1150
	-1   0    0    1   
$EndComp
Wire Wire Line
	2850 1450 2950 1450
Wire Wire Line
	3150 1350 3250 1350
Wire Wire Line
	2950 1450 3250 1450
Connection ~ 2950 1450
$Comp
L Diode:1N5407 D1
U 1 1 5D6A7FA8
P 7650 1700
F 0 "D1" H 7650 1916 50  0000 C CNN
F 1 "1N5407" H 7650 1825 50  0000 C CNN
F 2 "Diode_THT:D_DO-201AD_P5.08mm_Vertical_AnodeUp" H 7650 1525 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88516/1n5400.pdf" H 7650 1700 50  0001 C CNN
	1    7650 1700
	1    0    0    -1  
$EndComp
$EndSCHEMATC
