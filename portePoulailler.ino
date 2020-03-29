#include <EEPROM.h>

/********** Adressage EEprom *********************************************************************************  

trajet: 0 à 3.
seuil: 5 à 6.
    
 */

//********** Variable à modifier *****************************************************************************

unsigned long delaiFermeture = 1500000;      //Delai fermeture apres validation en ms
unsigned long delai = 210000;                //Delai de validation en ms .
unsigned long toleranceErreur = 500;         //Erreur tolerée lors d'une ouverture ou fermeture
unsigned long toleranceCalibration = 1000;   //Erreur tolerée pour valider la calibration
int dureePas = 1;                            //Duree d'un pas pur le moteur pas a a pas
unsigned long delaiEntreOuverte = 300000;    //delai ou la porte reste entre ouverte en ms

        //Valeurs pour test sur maquette
        
//unsigned long delaiFermeture = 2000;           //Delai fermeture apres validation en ms
//unsigned long delai = 200000;                  //Delai de validation en ms .
//unsigned long toleranceErreur = 500;           //Erreur tolerée lors d'une ouverture ou fermeture
//unsigned long toleranceCalibration = 5;        //Erreur tolerée pour valider la calibration
//int dureePas = 2;                              //Duree d'un pas pur le moteur pas a a pas
//unsigned long delaiEntreOuverte = 10000;       //delai ou la porte reste entre ouverte en ms

//********** Declaration des constantes **********************************************************************
const int photoResistance = A0;      //Photo resistance connecté sur Pin A0 de l'arduino
const int Step = 2;                  //Pin step du A4988 connecté sur Pin 2 de l'arduino
const int Dir = 3;                   //Pin dir du A4988 connecté sur Pin 3 de l'arduino
const int Enable = 4;                //Pin step du A4988 connecté sur Pin 4 de l'arduino
const int LED = 5;                   //LED connecté sur Pin 5 de l'arduino
const int FdcHaut = A1;              //Fin de course haut connecté sur Pin A1 de l'arduino
const int FdcBas = A2;               //Fin de course bas connecté sur Pin A2 de l'arduino

//********** Declaration des variables pour l'ouverture/fermeture de la porte ********************************
int etatPorte;                               //0: ouverte; 1: fermee; 2: entre ouverte   
boolean etatFdcHaut;                         //memorise etat FDC haut
boolean etatFdcBas;                          //memorise etat FDC bas
long compteurTrajetMontant;         //Compteur trajet montant, utilisé pour valider la calibration
long compteurTrajetDescendant;      //Compteur trajet descendant, utilisé pour valider la calibration
unsigned long trajet;                        //Compteur trajet, utilisé pour detecter un probleme de moteur ou de FDC lor de l'utilisation
long difference;                    //Difference entre nombre de pas ouverture/fermeture durant la calibration
unsigned long timerFermeture; 
unsigned long timerEntreOuverte = 0;
boolean fermetureMode = 0;
unsigned long manuPas = 0;                   //compte le nombre de pas lors d'une fermeture manuelle

//********** Declaration des variable pour la mesure de luminosité et traitement de l'info *******************
int seuil;                    //Seuil de declenchement ouverture/fermeture porte.
int lumi;                     //Variable qui stock la valeur de luminosité ambiante
unsigned long delaiTimer;     //Pour valider que le sueil est bien franchi. evite les ouv/ferm intempestives
boolean validation = 0;       //Indique que l'on est en attende de validation que le seuil franchi
int sommeLumi = 0;
int moyenneLumi = 0;

//********** Declaration des variables pour la gestion de la LED *********************************************
unsigned long delaiOn = 0;         //Duree LED allumee
unsigned long delaiOff = 0;        //Duree LED eteinte
unsigned long nbCycles = 0;        //Nombre de cycles a realiser
unsigned long timerOn;             //Timer LED alluméé
unsigned long timerOff;            //Timer LED eteinte
boolean etatLed = 0;               //0 = LED eteinte, 1 = LED allumee
boolean flashMode = 0;             //Indique que la LED doit clignoter
int flashType = 1;                 //Mode de clignotement
int pwm = 0;                       //Utilisé pour alumage et extinction progressive
unsigned long timerProgressif;     //Timer 
unsigned long delaiProgressif;     //Delai pour passer de eteint a allume
unsigned long delaiOffCode;        //Delai entre deux repetition du code
unsigned long timerOffCode;        //Timer delai entre deux repetitions du code
int code;                          //Nombre de flash du code a emettre
unsigned long compteurCode;        //Decompte le nombre de flash emis
boolean pause = 0;                 //Sert à distinguer la pause entre deux emission du code

//********** Declaration des variables pour la gestion des erreurs d'ouverture/fermeture *********************
long compteurOuverture;               //Compte le nombre de pas a l'ouverture 
long compteurFermeture;               //Compte le nombre de pas a la fermeture
int erreurMode = 0;                   //0: pas d'erreur, 1: erreur ouverture, 2: erreur fermeture

void setup() //**********************************************************************************************
{
  
  Serial.begin(9600);                                     //Utilisé pour debug  
  digitalWrite(Enable, HIGH);                             //Desactive le moteur
  pinMode(Step, OUTPUT);
  pinMode(Dir, OUTPUT);
  pinMode(Enable, OUTPUT);
  pinMode(FdcHaut, INPUT);
  pinMode(FdcBas, INPUT);
  etatFdcHaut = digitalRead(FdcHaut);
  digitalWrite(Dir, HIGH);                    //moteur sens monter
  digitalWrite(Enable, LOW);                  //Active le moteur
  while(etatFdcHaut == HIGH)                  //Ouvre la porte
  {
    avance1pas();                             //Fait avancer le moteur de 1 pas
    etatFdcHaut = digitalRead(FdcHaut);       //Lecture etat FDC haut
  }
  digitalWrite(Enable, HIGH);                 //Desactive le moteur
  EEPROM.get(0, trajet);                      //Recupere la valeur de trajet  
  EEPROM.get(5, seuil);                       //Recupere la valeur du seuil
  Serial.print("Trajet: ");
  Serial.println(trajet);
  Serial.print("Seuil: ");
  Serial.println(seuil);
  etatFdcHaut = digitalRead(FdcHaut);
  etatFdcBas = digitalRead(FdcBas);
  if((etatFdcBas == LOW) && (etatFdcHaut == LOW))      //Lance la calibration si le FDC bas est actionné et la porte ouverte
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
  Serial.println("Setup OK");
}

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
    delaiOffCode = (b * 10);        //Defini la duree entre deux repetitions du code
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

void ledOff()  //*******************************************************************************************
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
  etatPorte = 0;                              //Indique que la porte est ouverte
  digitalWrite(Enable, HIGH);                 //Coupe l'alimentation du moteur
  
  //********** Detection erreurs et validadation calibration ******************************************
  
  difference = (compteurTrajetMontant - compteurTrajetDescendant);
  difference = abs(difference);                                                     //Valeur absolue
  if(difference > toleranceCalibration)                                             //Ecart trop important, rejet de la calibration
  {
    ledConfig(1, 100, 200, 35);
  }
  if(difference < toleranceCalibration)                                            //Calibration reussie, enregistre le nb de pas d'un trajet
  {
    trajet = ((compteurTrajetMontant + compteurTrajetDescendant) / 2);             //Calcul le nombre de pas pour ouvrir/fermer la porte
    EEPROM.put(0, trajet);                                                         //Memorise nb de pas necessaire pour ouvrir/fermer la porte  
    analogWrite(LED, 255);                                                         //Allume la LED
    delay(5000);                                                                   //Pase de 5s
    ledOff();                                                                      //Eteint la LED
  }
}

void avance1pas()  //*****************************************************************************************
{
  digitalWrite(Step, HIGH);                 //Fait avancer le moteur de 1 pas
  delay(dureePas);                          //Fait avancer le moteur de 1 pas
  digitalWrite(Step, LOW);                  //Fait avancer le moteur de 1 pas
  delay(dureePas);                          //Fait avancer le moteur de 1 pas
}

void ouverture() //*******************************************************************************************
{
  etatFdcHaut = digitalRead(FdcHaut);                         //lecture etat fin de course haut
  digitalWrite(Dir, HIGH);                                    //moteur sens monter
  digitalWrite(Enable, LOW);                                  //active le moteur
  compteurOuverture = 0;                                      //Remise a 0 du compteur de pas ouverture (detection d'erreur FDC)
  while(etatFdcHaut == HIGH)                                  //Tant que fin de course haut desactivé
  {
    avance1pas();                                             //avance le moteur de 1 pas
    etatFdcHaut = digitalRead(FdcHaut);                       //lecture etat fin de course haut 
    compteurOuverture++;
    if(compteurOuverture > (trajet + toleranceErreur))        //Si probleme de FDC
    {
      erreurMode = 1;
      ledConfig(4, 1, 300, 86400000);                         //Signale l'erreur jusqu'a la fermeture
      digitalWrite(Dir, LOW);                                 //met le moteur en sens descendre
      for(unsigned long i = toleranceErreur ; i > 0 ; i--)    //Met la porte en bonne position
      {
        avance1pas();                                         //avance le moteur de 1 pas
      }
      etatFdcHaut = LOW;                                      //pour sortir du while      
    }
  }
  etatPorte = 0;
  digitalWrite(Enable, HIGH);                                 //desactive le moteur
}  

void fermeture() //*******************************************************************************************
{
  etatFdcBas= digitalRead(FdcBas);                            //lecture fin de course bas
  digitalWrite(Dir, LOW);                                     //moteur sens descente
  digitalWrite(Enable, LOW);                                  //active le moteur
  compteurFermeture = 0;                                      //Remise a 0 du compteur de pas fermeture (detection d'erreur FDC)
  while(etatFdcBas == HIGH)                                   //tant que fin de course bas deasctivé 
  {
    avance1pas();                                             //avance le moteur de 1 pas
    etatFdcBas = digitalRead(FdcBas);
    compteurFermeture++;
    if(compteurFermeture > (trajet + toleranceErreur))        //Si probleme de FDC
    {
      erreurMode = 2;
      ledConfig(4, 2, 300, 86400000);                         //Signale l'erreur jusqu'a l'ouverture
      digitalWrite(Dir, HIGH);                                //met le moteur en sens monter
      for(unsigned long i = toleranceErreur ; i > 0 ; i--)    //Met la porte en bonne position
      {
        avance1pas();                                         //avance le moteur de 1 pas
      }
      etatFdcBas = LOW;                                       //pour sortir du while      
    }     
  }   
  etatPorte = 1;
  digitalWrite(Enable, HIGH);                                 //desactive le moteur
}

void luminosite() //******************************************************************************************
{
  lumi = analogRead(photoResistance);                      //Mesure la luminosité ambiante
  if((etatPorte == 0) && (fermetureMode == 0))            //Si porte ouverte
  {
    if((lumi <= seuil) && (validation == 0))             //Si la luminosité ambiante passe sous le seuil pour la premiere fois
    {
      erreurMode = 0;
      delaiTimer = millis();                             //Demarre le chrono de validation de depassement de seuil
      validation = 1;                                    //Indique que la vailidation de depassement de seuil est active
      ledConfig(3, 1500, 1000, delai);
    }
    if((lumi >= seuil) && (validation == 1))              //Si la luminosité remonte au dessus du seuil avant la fin de la validation
    {
      validation = 0;                                     //Annule la validation et la fermeture de la porte
      ledOff();
    }
    if(((lumi <= seuil) && (validation == 1) && (millis() - delaiTimer) >= delai) && (fermetureMode == 0)) //Si delai de validation ecoulé
    { 
      ledConfig(1, 150, 2850, (delaiFermeture / 3000));
      fermetureMode = 1;                                          //Demarre la temporisatio de fermeture
      timerFermeture = millis();                                  //Ferme la porte
    }
  }
  
  if(etatPorte == 1)                                     //Si la porte est fermee
  {
    if((lumi >= seuil) && (validation == 0))             //Si la luminosité ambiante passe au dessus le seuil pour la premiere fois
    {
      erreurMode = 0;
      delaiTimer = millis();                             //Demarre le chrono de validation de depassement de seuil
      validation = 1;                                    //Indique que la vailidation de depassement de seuil est active
      ledConfig(2, 1500, 1000, delai);
    }
    if((lumi <= seuil) && (validation == 1))             //Si la luminosité descend sous le seuil avant la fin de la validation
    {
      validation = 0;                                    //Annule la validation et l'ouverture de la porte
      ledOff();
    }
    if(((lumi >= seuil) && (validation == 1)) && ((millis() - delaiTimer) > delai))  //Si delai validation ecoulé
    { 
      validation = 0;                                    //Sort du mode validation
      ledOff();
      ouverture();
    }
  }
}

void manuelle() //***********************************************************************************************
{
  etatFdcBas = digitalRead(FdcBas);                                   //Verifie FDC bas
  if((etatPorte == 0) && (etatFdcBas == LOW) && (erreurMode == 0))    //si l'on passe un aimant devant FDC bas quand la porte est ouverte
  {
    if(validation == 1)                                               //Si mode validation fermeture
    {
      validation =0;                                                  //on annule la validation
      ledOff();                                                       //on arrete le clignotement de la LED
    }
    analogWrite(LED, 255);                                            //allume la LED pour signaler la fermeture manuelle
    delay(2000);
    analogWrite(LED, 0);                                              //eteint la LED
    digitalWrite(Dir, LOW);                                           //moteur sens descente
    digitalWrite(Enable, LOW);                                        //active le moteur
    manuPas = 0;                                                      //Met le compteur de pas a 0
    while(manuPas < trajet)                                           //ferme la porte
    {
      avance1pas();
      manuPas++;
      etatFdcBas = digitalRead(FdcBas);                                   //Verifie FDC bas
      if(etatFdcBas == LOW)
      {
        etatPorte = 1;
        digitalWrite(Enable, HIGH);                      //Desactive le moteur
        return;
      }
      if(manuPas > (trajet * 0.1))
      {
        etatFdcHaut = digitalRead(FdcHaut);
        if(etatFdcHaut == LOW)
        {
          etatPorte = 2;
          manuPas = (trajet + 1);
          timerEntreOuverte = millis();           //demarre le timer porte entre ouverte
          analogWrite(LED, 255);
          delay(1500);
          ledOff();
        }
      }
    }  
    digitalWrite(Enable, HIGH);                  //Desactive le moteur
    if(etatPorte == 0)etatPorte = 1;             //etat porte fermée 
  }
 
  etatFdcHaut = digitalRead(FdcHaut);                                 //on verifie FDC haut
  if((etatPorte == 1) && (etatFdcHaut == LOW) && (erreurMode == 0))   //si on passe un aimant devant le FDC haut pendant que la porte est fermee   
  {
    if(validation == 1)                              //si en mode validation ouverture
    {
      validation =0;                                 //on annule la validation
      ledOff();                                      //on arrete le clignotement de la LED
    } 
    analogWrite(LED, 255);                           //on allume la LED pour signaler l'ouverture manuelle
    delay(2000);   
    analogWrite(LED, 0);
    digitalWrite(Dir, HIGH);                         //moteur sens montee
    digitalWrite(Enable, LOW);                       //active le moteur
    manuPas = 0;                                     //Met le compteur de pas a 0
    while(manuPas < trajet)
    {
      avance1pas();
      manuPas++;
      etatFdcHaut = digitalRead(FdcHaut);                                   //Verifie FDC bas
      if(etatFdcHaut == LOW)
      {
        etatPorte = 0;
        digitalWrite(Enable, HIGH);                      //Desactive le moteur
        return;
      }
      if(manuPas > (trajet * 0.1))
      {
        etatFdcBas = digitalRead(FdcBas);
        if(etatFdcBas == LOW)
        {
          etatPorte = 2;
          manuPas = (trajet + 1);
          timerEntreOuverte = millis();              //demarre le timer porte entre ouverte
          analogWrite(LED, 255);
          delay(1500);
          ledOff();
        }
      }
    }
    digitalWrite(Enable, HIGH);                      //Desactive le moteur
    if(etatPorte == 1)etatPorte = 0;                 //etat porte ouverte 
  }
}

void erreurFdc() //*********************************************************************************************
{
  etatFdcHaut = digitalRead(FdcHaut);            //Lecture fin de course
  etatFdcBas = digitalRead(FdcBas);              //Lecture fin de course
  if((erreurMode == 1) && (etatFdcBas == LOW))   //Si erreur ouverture et que l'on passe l'aimant devant le FDC bas
  {
    ledOff();                                    //Arrete le mode clignotement de la LED
    analogWrite(LED, 255);                       //Allume la LED
    erreurMode = 0;                              //Acquitte l'erreur
    delay(3000);                                 //Pour eviter la fermeture manuelle
    analogWrite(LED, 0);                         //Eteint la LED
  }
  if((erreurMode == 2) && (etatFdcHaut == LOW))  //Si erreur fermeture et que l'on passe l'aimant devant le FDC haut
  {
    ledOff();                                    //Arrete le mode clignotement de la LED
    analogWrite(LED, 255);                       //Allume la LED
    erreurMode = 0;                              //Acquitte l'erreur
    delay(3000);                                 //Pour eviter l'ouverture manuelle
    analogWrite(LED, 0);                         //Eteint la LED
  }
}

void porteEntreOuverte()  //**********************************************************************************
{
  
  etatFdcHaut = digitalRead(FdcHaut);
  etatFdcBas = digitalRead(FdcBas);
  if(etatFdcBas == LOW) 
  {
    analogWrite(LED, 255);
    delay(2000);
    ledOff();
    fermeture();
    return;
  }
  if(etatFdcHaut == LOW) 
  {
    analogWrite(LED, 255);
    delay(2000);
    ledOff();
    ouverture();
    return;
  }
  if(delaiEntreOuverte < (millis() - timerEntreOuverte))
  {
    sommeLumi = 0;
    for(int i ; i < 20 ; i++)
    {
      lumi = analogRead(photoResistance);                             //Mesure la luminosité ambiante
      sommeLumi = sommeLumi + lumi;
    }
    moyenneLumi = sommeLumi / 20;
    if(moyenneLumi > seuil) ouverture();
    if(moyenneLumi < seuil) fermeture();   
  }  
}

void loop() //***************************************************************************************************
{
  manuelle();                                             //Gere ouverture manuelle des portes
  if(etatPorte == 2) porteEntreOuverte();
  luminosite();                                           //Surveille la luminosité
  if(flashMode == 1) flash();                             //Gere le clignotement de la LED
  if(fermetureMode == 1)                                  //Gere la fermeture temporisée de la porte
  {
    if((millis() - timerFermeture) > delaiFermeture) 
    {
      fermetureMode = 0;                                  
      validation = 0;                                     //Sort du mode validation
      ledOff();                                           //Eteint la LED
      fermeture();                                        //Ferme la porte
    } 
  }
  if(erreurMode > 0) erreurFdc();                         //si erreur sur un fin de course   
}
