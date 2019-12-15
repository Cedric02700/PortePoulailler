/********** Adressage EEprom *********************************************************************************  

trajet: 0 à 3.
seuil: 4 à 5.
autoCalibration: 6

**************************************************************************************************************/

 
 #include <EEPROM.h>

//********** Valeur a modifier ******************************************************************************
//unsigned long trajet = 27115;
//int seuil = 45;
unsigned long trajet = 20;     //Debug
int seuil = 600;                   //Debug

unsigned long valeurTrajet;
int valeurSeuil;
//boolean autoCal = 0;
//boolean autoCalibration;

void setup() 
{
  EEPROM.put(0, trajet);
  EEPROM.put(4, seuil);
//  EEPROM.put(6, autoCal);
  Serial.begin(9600);
}

void loop() 
{
  EEPROM.get(0, valeurTrajet);
  EEPROM.get(4, valeurSeuil);
//  EEPROM.get(6, autoCalibration);
  Serial.print("Trajet: ");
  Serial.println(valeurTrajet);
  Serial.print("Seuil: ");
  Serial.println(valeurSeuil);
//  Serial.print("autoCalibration: ");
//  Serial.println(autoCalibration);
  delay(100000);
}
