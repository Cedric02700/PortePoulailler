//Voir utilité erreurMode

#include <EEPROM.h>

/********** Adressage EEprom *********************************************************************************  

trajet: 0 à 3.
seuil: 4 à 5.
    
 */

//********** Variable à modifier *****************************************************************************

//int extra = 200;                             //Nombre de pas a faire en plus apres ouverture/fermeture
//unsigned long delaiFermeture = 1200000;      //Delai fermeture apres validation
//boolean hyst = 15;                           //Hysteresis
//long delai = 20000;                          //Delai de validation 

int extra = 50;                                //Debug
unsigned long delaiFermeture = 10000;          //Debug
boolean hyst = 15;                             //Debug
long delai = 15000;                            //Debug 

//********** Declaration des constantes **********************************************************************
const int photoResistance = A0;      //Photo resistance connecté sur Pin A0 de l'arduino
const int Step = 2;                  //Pin step du A4988 connecté sur Pin 2 de l'arduino
const int Dir = 3;                   //Pin dir du A4988 connecté sur Pin 3 de l'arduino
const int Enable = 4;                //Pin step du A4988 connecté sur Pin 4 de l'arduino
const int LED = 5;                   //LED connecté sur Pin 5 de l'arduino
const int FdcHaut = A1;              //Fin de course haut connecté sur Pin A1 de l'arduino
const int FdcBas = A2;               //Fin de course bas connecté sur Pin A2 de l'arduino

//********** Declaration des variables pour l'ouverture/fermeture de la porte ********************************
boolean etatPorte = 0;                       //0: ouverte; 1: fermee   
boolean etatFdcHaut;                         //memorise etat FDC haut
boolean etatFdcBas;                          //memorise etat FDC bas
unsigned long compteurTrajetMontant;         //Compteur trajet montant, utilisé pour valider la calibration
unsigned long compteurTrajetDescendant;      //Compteur trajet descendant, utilisé pour valider la calibration
unsigned long trajet;                        //Compteur trajet, utilisé pour detecter un probleme de moteur ou de FDC lor de l'utilisation
unsigned long difference;                    //Difference entre nombre de pas ouverture/fermeture durant la calibration
unsigned long toleranceCalibration = 1600;   //Erreur tolerée pour valider la calibration 
unsigned long toleranceErreur;                //Erreur tolerée lors d'une ouverture ou fermeture
unsigned long timerFermeture; 
boolean fermetureMode = 0;

//********** Declaration des variable pour la mesure de luminosité et traitement de l'info *******************
int seuil;                    //Seuil de declenchement ouverture/fermeture porte.
int lumi;                     //Variable qui stock la valeur de luminosité ambiante
unsigned long delaiTimer;     //Pour valider que le sueil est bien franchi. evite les ouv/ferm intempestives
boolean validation = 0;       //Indique que l'on est en attende de validation que le seuil franchi

//********** Declaration des variables pour la gestion de la LED *********************************************
int delaiOn = 0;         //Duree LED allumee
int delaiOff = 0;        //Duree LED eteinte
int nbCycles = 0;        //Nombre de cycles a realiser
unsigned long timerOn;   //Timer LED alluméé
unsigned long timerOff;  //Timer LED eteinte
boolean etatLed = 0;     //0 = LED eteinte, 1 = LED allumee
boolean flashMode = 0;   //Indique que la LED doit clignoter
int flashType = 1;       //Mode de clignotement
int pwm = 0;             //Utilisé pour alumage et extinction progressive
long timerProgressif;    //Timer 
long delaiProgressif;    //Delai pour passer de eteint a allume
int delaiOffCode;        //Delai entre deux repetition du code
long timerOffCode;       //Timer delai entre deux repetitions du code
int code;                //Nombre de flash du code a emettre
int compteurCode;        //Decompte le nombre de flash emis
boolean pause = 0;       //Sert à distinguer la pause entre deux emission du code

//********** Declaration des variables pour la gestion des erreurs d'ouverture/fermeture *********************
unsigned long compteurOuverture;       
unsigned long compteurFermeture;
boolean erreurMode = 0;

void setup() //**********************************************************************************************
{
  
  Serial.begin(9600);                       //Utilisé pour debug  
  digitalWrite(Enable, HIGH);               //Desactive le moteur
  pinMode(Step, OUTPUT);
  pinMode(Dir, OUTPUT);
  pinMode(Enable, OUTPUT);
  pinMode(FdcHaut, INPUT);
  pinMode(FdcBas, INPUT);
  EEPROM.get(0, trajet);
  EEPROM.get(4, seuil);
  ouverture();
  etatFdcHaut = digitalRead(FdcHaut);
  etatFdcBas = digitalRead(FdcBas);
  if((etatFdcBas == LOW) && (etatFdcHaut == LOW)) 
  {
    analogWrite(LED, 255);
    while(etatFdcBas == LOW)
    {
      etatFdcBas = digitalRead(FdcBas);
    }
    analogWrite(LED, 0);
    delay(2500);
    calibration();
  }
  Serial.print("Trajet: ");
  Serial.println(trajet);
  Serial.print("Seuil: ");
  Serial.println(seuil);
  Serial.println("Setup OK");
}

// ledConfig(1, 500, 2500, (delaiFermeture / 3000));
/***********************************************************************************************************
*** Utilisation de la fonction ledConfig()                                                            
*** Mode: - 1 = LED clignotante, 'a': duree allumee, 'b': duree eteinte, 'c': nombre de clignotements
***       - 2 = LED allumage progressif, 'a': duree pour passer de  eteint a allumee, 'b': duree durant
***             laquelle le LED est eteinte entre deux cycles, 'c': duree du mode en ms
***       - 3 = LED a extinction progressive: idem allumage progressif mais a l'envers pour le 'a'.
***       - 4 = Code: 'a': nb. de flash du code, 'b': duree du flash, 'c':duree d'emission du code en ms.
//**********************************************************************************************************/
void ledConfig(int mode, int a, int b, unsigned long c)
{
  delaiOn = a;                      //Defini delaiOn suivant parametre de la fonction
  delaiOff = b;                     //Defini delaiOff suivant parametre de la fonction
  nbCycles = c;                     //Defini nb suivant parametre de la fonction
  flashType = mode;                 //Defini flashType suivant parametre de la fonction
  timerOn = millis();               //Demarre le timer On
  flashMode = 1;                    //Indique que la Led doit clignoter
  if(flashType == 1)                //Si mode Clignotement
  {
    analogWrite(LED, 255);          //Allume la LED
    etatLed = 1;                    //Defini son etat comme allumee
  }
  if((flashType == 2))              //Si mode allumage pregressif
  {
    delaiOn = (delaiOn / 255);      //Divise delaiOn par 255 pour calculer la duree du pas d'incrementation du rapport cyclique pour l'allumage progressif
    pwm = 0;                        //Initialise PWM a 0
    etatLed = 1;                    //Defini etat LED comme allumee
    delaiProgressif = c;            //Defini delaiProgressif  suivant parametre de la fonction
    timerProgressif = millis();     //Demarre le timer du mode progressif 
  }
  if((flashType == 3))              //Si mode extiction Progressive
  {
    delaiOn = (delaiOn / 255);      //Divise le delai Off pour calculer la duree du pas d'incrementation du rapport cyclique pour l'allumage progressif       
    pwm = 255;                      //Initialise le PWM a 255
    etatLed = 1;                    //Defini la LED comme allumee
    delaiProgressif = c;            //Defini delaiProgressif suivant parametre de la fonction   
    timerProgressif = millis();     //Demarre le timer du mode progressif
  }
  if(flashType == 4)                //Si mode code.
  {
    code = a;                       //Defini le nombre de lash du code
    compteurCode = a;               //Initialise le compteur du code
    delaiOn = b;                    //Defini la duree aluumee de la LED
    delaiOff = b;                   //Defini la duree eteinte entre deux flash de la LED 
    delaiOffCode = (b * 2);         //Defini la duree entre deux repetitions du code
    analogWrite(LED, 255);          //Alulume la LED
    etatLed = 1;                    //Defini l'etat LED comme allumee
    flashMode = 1;                  //Indique que la LED doit clignoter
  }
}

void flash()  //*********************************************************************************************
{
  if(flashType == 1)                                            //Mode clignotement de la LED
  {
    if(((millis() - timerOn) >= delaiOn) && (etatLed == 1))     //Si delai LED allumee depassé
    {
      analogWrite(LED, 0);                                      //Eteint la LED
      timerOff = millis();                                      //Demarre le Timer Led eteinte
      etatLed = 0;                                              //Defini etat led comme eteinte
      nbCycles--;                                               //Decremente le compteur de clignotements                                  
    }
    if(((millis() - timerOff) >= delaiOff) && (etatLed == 0))   //Si delai LED eteinte depassé
    {
      analogWrite(LED, 255);                                    //Allume la LED
      timerOn = millis();                                       //Demarre le timer led allumee                     
      etatLed = 1;                                              //Defini etat led comme allumee
    }
    if(nbCycles <= 0)flashMode = 0;                             //Si nombre de clignotement defini atteint, on arrete de faire clignoter la led
  }
  if(flashType == 2)                                            //Mode allumage preogressif
  {
    if(((millis() - timerOn) >= delaiOn) && (etatLed == 1))     //Quand la duree d'un pas d'incrementation est depassé
    {
      analogWrite(LED, pwm);                                    //Allume la LED au rapport cyclique defini au tour precedent
      pwm++;                                                    //Incremente le rapport cyclique
      timerOn = millis();                                       //Reinitialise le timer du pas d'incrementation
    }
    if((pwm == 255) && (etatLed == 1))                          //Si la LED est eclairee a son maximum
    {
      analogWrite(LED, 0);                                      //Eteint la LED
      timerOff = millis();                                      //Demarre le timer LED eteinte
      pwm = 0;                                                  //Intialise le rapport cyclique a 0
      etatLed = 0;                                              //Defini etat LED comme eteinte
    }
    if((etatLed == 0) && ((millis() - timerOff) >= delaiOff))   //duree LED eteinte atteinte
    {
      etatLed = 1;                                              //defini etat LED comme allumee
      timerOn = millis();                                       //Demarre le timer duree incrementation rapport cyclique
    }
    if((millis() - timerProgressif) >= delaiProgressif)         //La duree de clignotement de la led est atteinte
    {
      flashMode = 0;                                            //La LED ne doit plus clignoter
      analogWrite(LED, 0);                                      //Eteint la LED
      etatLed = 0;                                              //Etat LED defini comme eteinte
    }
  }
  if(flashType == 3)                                            //Mode extinction progressive
  {
    if(((millis() - timerOn) >= delaiOn) && (etatLed == 1))     //fonctionnement similaire a allumage progressif
    {                                                           //Voir commentaire ci dessus
      analogWrite(LED, pwm);
      pwm--;
      timerOn = millis();
    }
    if((pwm == 0) && (etatLed == 1))
    {
      analogWrite(LED, 0);
      timerOff = millis();
      pwm = 255;
      etatLed = 0;
    }
    if((etatLed == 0) && ((millis() - timerOff) >= delaiOff)) 
    {
      etatLed = 1;
      timerOn = millis();
    }
    if((millis() - timerProgressif) >= delaiProgressif) 
    {
      flashMode = 0;
      analogWrite(LED, 0);
      etatLed = 0;
    }
  }
  if(flashType == 4)                                                                                       //Mode code. 
  {
    if(((millis() - timerOn) >= delaiOn) && (etatLed == 1) && (compteurCode > 0))                          //Duree LED allumee atteinte
    {
      analogWrite(LED, 0);                                                                                 //eteint la LED
      timerOff = millis();                                                                                 //Demarre le timer LED eteinte
      etatLed = 0;                                                                                         //Defini etat LED comme eteinte
      compteurCode--;                                                                                      //Decremente le compteur du nombre de flas du code
    }
    if(((millis() - timerOff) >= delaiOff) && (etatLed == 0) && (compteurCode > 0))                        //Duree LED eteinte atteint
    {
      analogWrite(LED, 255);                                                                               //Allume la LED
      timerOn = millis();                                                                                  //Demarre le timer LED allumee 
      etatLed = 1;                                                                                         //Definil'etat LED comme allumee
    }
    if(((millis() - timerOff) >= delaiOff) && (etatLed == 0) && (compteurCode == 0) &&(pause == 0))        //Si le nombre de flas du code a ete emis
    {
      timerOffCode = millis();                                                                             //Demarre le timer de la pause entre deux emission du code
      pause = 1;                                                                                           //Defini que l'on se touve entre deux emission de code
    }
    if(((millis() - timerOffCode) >= delaiOffCode) && (pause == 1))                                        //Delai de pause entre deux emission du code ecoulee
    {
      analogWrite(LED, 255);                                                                               //Allume la LED
      timerOn = millis();                                                                                  //Demarre le timer LED allumee
      etatLed = 1;                                                                                         //defini etat LED comme allumee
      compteurCode = code;                                                                                 //Initialise le compteur de flash du code a emettre         
      pause = 0;                                                                                           //Defini la pause entre deux emission du code comme terminee
    }
  }
}

void ledOff()
{
  analogWrite(LED, 0);
  etatLed = 0;
  flashMode = 0;
}

void calibration()  //*************************************************************************************** 
{
  compteurTrajetMontant = 0;                  //Passe le compteur de pas montant a 0
  compteurTrajetDescendant = 0;               //Passe le compteur de pas descendant a 0
  digitalWrite(Enable, LOW);                  //Active l'alimentation du moteur
  etatFdcHaut = digitalRead(FdcHaut);         //Lecture etat FDC Haut
  if(etatFdcHaut == LOW)                      //Si le FDC haut activé
  {
    digitalWrite(Dir, LOW);                   //Moteur dans le sens descente
    while(etatFdcHaut == LOW)                 //Tant que le FDC haut est activé
    {
      avance1pas();                           //Fait avancer le moteur de 1 pas
      etatFdcHaut = digitalRead(FdcHaut);     //Lecture etat FDC haut
    } 
    for(int i = extra; i > 0; i--)            //Fait descendre la porte de 5mm supplementaire
    {
      avance1pas();                           //Fait avancer le moteur de 1 pas
    }
  }
  delay(1000);
  digitalWrite(Dir, HIGH);                    //Moteur dans le sens monter
  while(etatFdcHaut == HIGH)                  //Place la porte en position de debut de calibration en position ouverte)
  {
    avance1pas();                             //Fait avancer le moteur de 1 pas
    etatFdcHaut = digitalRead(FdcHaut);       //Lecture etat FDC haut
  }
  delay(1000);
  digitalWrite(Dir, LOW);                     //Moteur dans le sens descendre
  etatFdcBas = digitalRead(FdcBas);           //Lecture etat FDC Bas
  while(etatFdcBas == HIGH)                   //Debut de calibration
  {
    avance1pas();                             //Fait avancer le moteur de 1 pas
    etatFdcBas = digitalRead(FdcBas);         //Lecture etat FDC haut
    compteurTrajetDescendant++;               //Incremente le compteur de trajet descendant
  }
  delay(1000);
  digitalWrite(Dir, HIGH);                    //Moteur dans le sens monter
  etatFdcHaut = digitalRead(FdcHaut);
  while(etatFdcHaut == HIGH)                  //Tant que le FDC haut est desactivé
  {
    avance1pas();                             //Avance le moteur de 1 pas
    compteurTrajetMontant++;                  //Increment le compteur de trajet montant
    etatFdcHaut = digitalRead(FdcHaut);       //Lecture etat FDC haut
  }
  for(int i = extra; i > 0; i--)              //Movement supplementaire pour assurer l'activation du fin de course   
  {
    avance1pas();                             //avance le moteur de 1 pas
  }
  etatPorte = 0;                              //Indique que la porte est ouverte
  digitalWrite(Enable, HIGH);                 //Coupe l'alimentation du moteur
  
  //********** Detection erreurs et validadation calibration **********
  
  difference = (compteurTrajetMontant - compteurTrajetDescendant);
  difference = abs(difference);
  if(difference > toleranceCalibration)        //Ecart trop important, rejet de la calibration
  {
    ledConfig(4, 1, 300, 5000);
  }
  if(difference < toleranceCalibration)        //Calibration reussie, enregistre le nb de pas d'un trajet
  {
    trajet = (((compteurTrajetMontant + compteurTrajetDescendant) / 2) + extra);
    EEPROM.put(0, trajet);             //Memorise nb de pas necessaire pour ouvrir/fermer la porte
    EEPROM.put(6, 0);                          //Desactive la calibration automatique             
  }
}

void avance1pas()  //*****************************************************************************************
{
  digitalWrite(Step, HIGH);                 //Fait avancer le moteur de 1 pas
  delay(1);                                 //Fait avancer le moteur de 1 pas
  digitalWrite(Step, LOW);                  //Fait avancer le moteur de 1 pas
  delay(1);                                 //Fait avancer le moteur de 1 pas
}

void ouverture() //*******************************************************************************************
{
  etatFdcHaut = digitalRead(FdcHaut);       //lecture etat fin de course haut
  digitalWrite(Dir, HIGH);                  //moteur sens monter
  digitalWrite(Enable, LOW);                //active le moteur
  compteurOuverture = 0;                    //Remise a 0 du compteur de pas ouverture (detection d'erreur FDC)
  while(etatFdcHaut == HIGH)                //Tant que fin de course haut desactivé
  {
    avance1pas();                           //avance le moteur de 1 pas
    etatFdcHaut = digitalRead(FdcHaut);     //lecture etat fin de course haut 
    compteurOuverture++;
//    if(compteurOuverture > (trajet + toleranceErreur))   //Si probleme de FDC
//    {
//      erreurMode = 1;
//      ledConfig(4, 1, 300, 86400000);       //Signale l'erreur jusqu'a la fermeture
//      digitalWrite(Dir, LOW);               //met le moteur en sens descendre
//      for(int i = (extra + toleranceErreur); i > 0; i--) //Met la porte en bonne position
//      {
//        avance1pas();                           //avance le moteur de 1 pas
//      }
//      digitalWrite(Dir, HIGH);               //met le moteur en sens monter      
//    }
  } 
  for(int i = extra; i > 0; i--)            //Movement supplementaire pour assurer l'activation du fin de course   
  {
    avance1pas();                           //avance le moteur de 1 pas
  }
  etatPorte = 0;
  digitalWrite(Enable, HIGH);               //desactive le moteur
  Serial.println("Porte Ouverte");          //Debug
}  

void fermeture() //*******************************************************************************************
{
  etatFdcBas= digitalRead(FdcBas);        //lecture fin de course bas
  digitalWrite(Dir, LOW);                 //moteur sens descente
  digitalWrite(Enable, LOW);              //active le moteur
  compteurFermeture = 0;                  //Remise a 0 du compteur de pas fermeture (detection d'erreur FDC)
  while(etatFdcBas == HIGH)               //tant que fin de course bas deasctivé 
  {
    avance1pas();                         //avance le moteur de 1 pas
    etatFdcBas = digitalRead(FdcBas);
    compteurFermeture++;
//    if(compteurFermeture > (trajet + toleranceErreur))     //Si probleme de FDC
//    {
//      erreurMode = 1;
//      ledConfig(4, 2, 300, 86400000);                    //Signale l'erreur jusqu'a l'ouverture
//      digitalWrite(Dir, HIGH);                           //met le moteur en sens monter
//      for(int i = (extra + toleranceErreur); i > 0; i--) //Met la porte en bonne position
//      {
//        avance1pas();                                    //avance le moteur de 1 pas
//      }
//      digitalWrite(Dir, LOW);                            //met le moteur en sens descendre      
//    }     
  }   
  for(int i = extra; i>0; i--)                             //mouvement supplementaire pour assurer l'activation du fin de course
  {
    avance1pas();                                          //avance de 1 pas
  }
  etatPorte = 1;
  digitalWrite(Enable, HIGH);                              //desactive le moteur
  Serial.println("Porte Fermee");
}

void luminosite() //******************************************************************************************
{
  lumi = analogRead(photoResistance);                             //Mesure la luminosité ambiante
  if((etatPorte == 0) && (fermetureMode == 0))                    //Si porte ouverte
  {
    if((lumi <= (seuil - hyst)) && (validation == 0))             //Si la luminosité ambiante passe sous le seuil pour la premiere fois
    {
      erreurMode = 0;
      Serial.println("entree valid fermeture");
      delaiTimer = millis();                                      //Demarre le chrono de validation de depassement de seuil
      validation = 1;                                             //Indique que la vailidation de depassement de seuil est active
      ledConfig(3, 1500, 1000, delai);
    }
    if((lumi >= (seuil - hyst)) && (validation == 1))              //Si la luminosité remonte au dessus du seuil avant la fin de la validation
    {
      validation = 0;                                             //Annule la validation et la fermeture de la porte
      ledOff();
    }
    if(((lumi <= (seuil-hyst)) && (validation == 1) && (millis() - delaiTimer) >= delai) && (fermetureMode == 0)) //Si delai de validation ecoulé
    { 
      Serial.println("Timer fermeture");
      ledConfig(1, 150, 2850, (delaiFermeture / 3000));
      fermetureMode = 1;                                          //Demarre la temporisatio de fermeture
      timerFermeture = millis();                                  //Ferme la porte
    }
  }
  
  if(etatPorte == 1)                                              //Si la porte est fermee
  {
    if((lumi >= (seuil + hyst)) && (validation == 0))             //Si la luminosité ambiante passe au dessus le seuil pour la premiere fois
    {
      erreurMode = 0;
      Serial.println("entree valid ouverture");
      delaiTimer = millis();                                      //Demarre le chrono de validation de depassement de seuil
      validation = 1;                                             //Indique que la vailidation de depassement de seuil est active
      ledConfig(2, 1500, 1000, delai);
    }
    if((lumi <= (seuil + hyst)) && (validation == 1))             //Si la luminosité descend sous le seuil avant la fin de la validation
    {
      validation = 0;                                             //Annule la validation et l'ouverture de la porte
      ledOff();
    }
    if(((lumi >= (seuil + hyst)) && (validation == 1)) && ((millis() - delaiTimer) > delai))  //Si delai validation ecoulé
    { 
      Serial.println("ouverture");
      validation = 0;                                             //Sort du mode validation
      ledOff();
      ouverture();
    }
  }
//  Serial.println(lumi);
}

void manuelle()
{
  //********** En stand by *****************************************
  etatFdcBas = digitalRead(FdcBas);
  if((etatPorte == 0) && (etatFdcBas == LOW))
  {
    if(validation == 1)
    {
      validation =0;
      ledOff();
    }
 
    analogWrite(LED, 255);                      //allume la LED
    while(etatFdcBas == LOW)
    {
      etatFdcBas = digitalRead(FdcBas);
    }
    Serial.println("Fermeture manuelle");
    analogWrite(LED, 0);
//    fermeture();
    digitalWrite(Dir, LOW);                          //moteur sens descente
    digitalWrite(Enable, LOW);                       //active le moteur
    for(int i = (trajet + extra); i > 0; i--)
    {
      avance1pas();
    }
    digitalWrite(Enable, HIGH);                     //Desactive le moteur
    etatPorte = 1;                                  //etat porte fermée 
  }
  etatFdcHaut = digitalRead(FdcHaut);
  if((etatPorte == 1) && (etatFdcHaut == LOW))
  {
    if(validation == 1)
    {
      validation =0;
      ledOff();
    }
    
    analogWrite(LED, 255);                      //allume la LED
    while(etatFdcHaut == LOW)
    {
      etatFdcHaut = digitalRead(FdcHaut);
    }
    Serial.println("Ouverture manuelle");
    analogWrite(LED, 0);
//    ouverture();
    digitalWrite(Dir, HIGH);                          //moteur sens montee
    digitalWrite(Enable, LOW);                       //active le moteur
    for(int i = (trajet + extra); i > 0; i--)
    {
      avance1pas();
    }
    digitalWrite(Enable, HIGH);                     //Desactive le moteur
    etatPorte = 0;                                  //etat porte ouverte 
  }
}

void loop() //***************************************************************************************************
{
  manuelle();
  luminosite();
  if(flashMode == 1) flash();
  if(fermetureMode == 1)
  {
    if((millis() - timerFermeture) > delaiFermeture) 
    {
      Serial.println("fermeture");
      fermetureMode = 0;
      validation = 0;                                             //Sort du mode validation
      fermeture();
    }
  }
}
