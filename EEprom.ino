/********** Adressage EEprom *********************************************************************************  

trajet: 0 à 3.
seuil: 5 à 6.
    
 */

 
 #include <EEPROM.h>

unsigned long trajet = 26652;
unsigned long valeurTrajet;
int seuil = 20;
int valeurSeuil;

void setup() 
{
  EEPROM.put(0, trajet);
  EEPROM.put(5, seuil);
  Serial.begin(9600);
}

void loop() 
{
  EEPROM.get(0, valeurTrajet);
  EEPROM.get(5, valeurSeuil);
  Serial.print("Trajet: ");
  Serial.println(valeurTrajet);
  Serial.print("Seuil: ");
  Serial.println(valeurSeuil);
  delay(100000);
}
