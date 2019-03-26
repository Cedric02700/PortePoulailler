//********** Declaration des constantes **********************************************************************
const int photoResistance = A0;      //Photo resistance connecté sur Pin A0 de l'arduino
const int Step = 2;                  //Pin step du A4988 connecté sur Pin 2 de l'arduino
const int Dir = 3;                   //Pin dir du A4988 connecté sur Pin 3 de l'arduino
const int Enable = 4;                //Pin step du A4988 connecté sur Pin 4 de l'arduino
const int LED = 5;                   //LED connecté sur Pin 5 de l'arduino
const int FdcHaut = A1;              //Fin de course haut connecté sur Pin A1 de l'arduino
const int FdcBas = A2;               //Fin de course bas connecté sur Pin A2 de l'arduino

//********** Declaration des variables pour l'ouverture/fermeture de la porte ********************************
boolean etatPorte;                  //0: ouverte; 1: fermee   
boolean etatFdcHaut;                //memorise etat FDC haut
boolean etatFdcBas;                 //memorise etat FDC bas
long compteurTrajetMontant;         //Compteur trajet montant, utilisé pour valider la calibration
long compteurTrajetDescendant;      //Compteur trajet descendant, utilisé pour valider la calibration
long compteurTrajet;                //Compteur trajet, utilisé pour detecter un probleme de moteur ou de FDC lor de l'utilisation
int extra = 200;                 //Nombre de pas a faire en plus apres ouverture/fermeture

//********** Declaration des variable pour la mesure de luminosité et traitement de l'info *******************
int seuil = 750;              //Seuil de declenchement ouverture/fermeture porte.
int lumi;               //Variable qui stock la valeur de luminosité ambiante
long delaiTimer = 0;    //Pour valider que le sueil est bien franchi. evite les ouv/ferm intempestives
int validation = 0;     //Indique que l'on est en attende de validation que le seuil franchi
boolean hyst = 50;      //Hysteresis
long delai = 2000;      //Delai de validation 

//********** Declaration des variables pour la gestion de la LED *********************************************
int delaiOn = 0;
int delaiOff = 0;
int nbCycles = 0;
long timerOn;
long timerOff;
boolean etatLed = 0;     //0 = LED eteinte, 1 = LED allumee
boolean flashMode = 0;   //Indique que la LED doit clignoter
int flashType = 1;
int pwm = 0;             //utilisé pour alumage et extinction progressive

void setup() 
{
  Serial.begin(9600);          //Utilisé pour debug  
  pinMode(Step, OUTPUT);
  pinMode(Dir, OUTPUT);
  pinMode(Enable, OUTPUT);
  pinMode(FdcHaut, INPUT);
  pinMode(FdcBas, INPUT);
  etatFdcHaut = digitalRead(FdcHaut);
  etatFdcBas = digitalRead(FdcBas);
  digitalWrite(Enable, HIGH);
  flashConfig(500, 500, 5);  //debug
}

//**********************************************************************************************************
//*** Utilisation de la fonction ledConfig()                                                             
//*** Mode: - 1 = LED clignotante, on: duree allumee, off: duree eteinte, nb: nombre de clignotements
//***       - 2 = LED allumage progressif, on: duree pour passer de  eteint a allumee, off: duree durant
//***             laquelle le LED est eteinte entre deux cycles.
//***       - 3 = LED a extinction progressive: idem allumage progressif mais a l'envers pour le on.
//***********************************************************************************************************
void ledConfig(int mode, int on, int off, int nb)
{
  delaiOn = on;
  delaiOff = off;
  nbCycles = nb;
  flashType = mode;
  timerOn = millis();
  flashMode = 1;
  if(flashType == 1)
  {
    analogWrite(LED, 255);
    etatLed = 1;
  }
  if((flashType == 2)
  {
    delaiOn = (on / 255);
    pwm = 0;
  }
  if((flashType == 3)
  {
    delaiOn = (on / 255);
    pwm = 255;
  }
}

void flash()  //*********************************************************************************************
{
  if(flashType == 1)
  {
    if(((millis() - timerOn) >= delaiOn) && (etatLed == 1))
    {
      timerOff = millis();
      analogWrite(LED, 0);
      etatLed = 0;
      nbCycles--;
      Serial.println("on");
    }
    if(((millis() - timerOff) >= delaiOff) && (etatLed == 0))
    {
      timerOn = millis();
      analogWrite(LED, 255);
      etatLed = 1;
      Serial.println("off");
    }
    if(nbCycles <= 0)flashMode = 0;
  }
  if(flashType == 2)
  {
    if(((millis() - delaiTimer) >= delaiOn) && (etatLed == 1))
    {
      analogWrite(LED, pwm);
      pwm++;
    }
    if((pwm == 255) && (etatLed == 1))
    {
      analogWrite(LED, 0);
      timerOff = millis();
      pwm = 0;
      etatLed = 0;
    }
    if((etatLed == 0) && ((millis() - timerOff) >= delaiOff)) etatLed = 1;
  }
  if(flashType == 3)
  {
    if(((millis() - delaiTimer) >= delaiOn) && (etatLed == 1))
    {
      analogWrite(LED, pwm);
      pwm--;
    }
    if((pwm == 0) && (etatLed == 1))
    {
      analogWrite(LED, 0);
      timerOff = millis();
      pwm = 255;
      etatLed = 0;
    }
    if((etatLed == 0) && ((millis() - timerOff) >= delaiOff)) etatLed = 1;
  }
}

void calibration()  //*************************************************************************************** 
{
  etatFdcHaut = digitalRead(FdcHaut);         //Lecture etat FDC Haut
  compteurTrajetMontant = 0;                  //Passe le compteur de pas montant a 0
  compteurTrajetDescendant = 0;               //Passe le compteur de pas descendant a 0
  digitalWrite(Enable, LOW);                 //Active l'alimentation du moteur
  if(FdcHaut == LOW)                          //Si le FDC haut activé
  {
    digitalWrite(Dir, LOW);                   //Moteur dans le sens descente
    while(etatFdcHaut == LOW)                  //Tant que le FDC haut est activé
    {
      avance1pas();                           //Fait avancer le moteur de 1 pas
      etatFdcHaut = digitalRead(FdcHaut);     //Lecture etat FDC haut
    } 
    for(int i = 0; i < 1300; i++)             //Fait descendre la porte de 5mm supplementaire
    {
      avance1pas();                           //Fait avancer le moteur de 1 pas
    }
  }
  digitalWrite(Dir, HIGH);                    //Moteur dans le sens monter
  while(etatFdcHaut == HIGH)                  //Tant que fin de course haut desactivé (place la porte en position de debut de calibration en position ouverte)
  {
    avance1pas();                             //Fait avancer le moteur de 1 pas
  }
  digitalWrite(Dir, LOW);                     //Moteur dans le sens descendre
  while(etatFdcBas == HIGH)                   //Tant que FDC bas desactivé (debut de calibration)
  {
    avance1pas();                             //Fait avancer le moteur de 1 pas
    compteurTrajetDescendant++;               //Incremente le compteur de trajet descendant
  }
  digitalWrite(Dir, HIGH);                    //Moteur dans le sens monter
  while(etatFdcHaut == HIGH)                  //Tant que le FDC haut est desactivé
  {
    avance1pas();                             //Avance le moteur de 1 pas
    compteurTrajetMontant++;                  //Increment le compteur de trajet montant
  }
  digitalWrite(Enable, HIGH);                  //Coupe l'alimentation du moteur
  //detection erreur à coder 
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
  digitalWrite(Dir, HIGH);                  //moteur sens montee
  digitalWrite(Enable, LOW);                //active le moteur
  while(etatFdcHaut == HIGH)                //Tant que fin de course haut desactivé
  {
    avance1pas();                           //avance le moteur de 1 pas
    etatFdcHaut = digitalRead(FdcHaut);     //lecture etat fin de course haut   
  } 
  for(int i = extra; i > 0; i--)      //Movement supplementaire pour assurer l'activation du fin de course   
  {
    avance1pas();                         //avance le moteur de 1 pas
  }
  etatPorte = 0;
  digitalWrite(Enable, HIGH);             //desactive le moteur
  Serial.println("Porte Ouverte");
  //detection erreur à coder    
}  

void fermeture() //*******************************************************************************************
{
  etatFdcBas= digitalRead(FdcBas);        //lecture fin de course bas
  digitalWrite(Dir, LOW);                 //moteur sens descente
  digitalWrite(Enable, LOW);              //active le moteur
  while(etatFdcBas == HIGH)               //tant que fin de course bas deasctivé 
  {
    avance1pas();                         //avance le moteur de 1 pas
    etatFdcBas = digitalRead(FdcBas);     
  }   
  for(int i = extra; i>0; i--)        //mouvement supplementaire pour assurer l'activation du fin de course
  {
    avance1pas();                         //avance de 1 pas
  }
  etatPorte = 1;
  digitalWrite(Enable, HIGH);             //desactive le moteur
  Serial.println("Porte Fermee");
  //detection erreur à coder
}

void luminosite() //******************************************************************************************
{
  lumi = analogRead(photoResistance);                             //Mesure la luminosité ambiante
  
  if(etatPorte == 0)                                              //Si porte ouverte
  {
    if((lumi <= (seuil - hyst)) && (validation == 0))             //Si la luminosité ambiante passe sous le seuil pour la premiere fois
    {
      delaiTimer = millis();                                      //Demarre le chrono de validation de depassement de seuil
      validation = 1;                                             //Indique que la vailidation de depassement de seuil est active
    }
    if((lumi >= (seuil - hyst)) && (validation == 1))              //Si la luminosité remonte au dessus du seuil avant la fin de la validation
    {
      validation = 0;                                             //Annule la validation et la fermeture de la porte
    }
    if(((lumi <= (seuil-hyst)) && (validation == 1) && (millis() - delaiTimer) >= delai)) //Si delai de validation ecoulé
    {
      fermeture();                                                //Ferme la porte 
      validation = 0;                                             //Sort du mode validation
    }
  }
  
  if(etatPorte == 1)                                              //Si la porte est fermee
  {
    if((lumi >= (seuil + hyst)) && (validation == 0))             //Si la luminosité ambiante passe au dessus le seuil pour la premiere fois
    {
      delaiTimer = millis();                                      //Demarre le chrono de validation de depassement de seuil
      validation = 1;                                             //Indique que la vailidation de depassement de seuil est active
    }
    if((lumi <= (seuil + hyst)) && (validation == 1))             //Si la luminosité descend sous le seuil avant la fin de la validation
    {
      validation = 0;                                             //Annule la validation et l'ouverture de la porte
    }
    if(((lumi >= (seuil + hyst)) && (validation == 1)) && ((millis() - delaiTimer) > delai))  //Si delai validation ecoulé
    {
      ouverture();                                                //Ouvre la porte 
      validation = 0;                                             //Sort du mode validation
    }
  }
//  Serial.println(lumi);
}

void loop() 
{
//  luminosite();
  if(flashMode == 1) flash();
}
