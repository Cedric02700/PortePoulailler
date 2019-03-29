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
int validation = 15000;     //Indique que l'on est en attende de validation que le seuil franchi
boolean hyst = 50;      //Hysteresis
long delai = 2000;      //Delai de validation 

//********** Declaration des variables pour la gestion de la LED *********************************************
int delaiOn = 0;         //Duree LED allumee
int delaiOff = 0;        //Duree LED eteinte
int nbCycles = 0;        //Nombre de cycles a realiser
long timerOn;            //Timer LED alluméé
long timerOff;           //Timer LED eteinte
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

void setup() //**********************************************************************************************
{
  Serial.begin(9600);                       //Utilisé pour debug  
  pinMode(Step, OUTPUT);
  pinMode(Dir, OUTPUT);
  pinMode(Enable, OUTPUT);
  pinMode(FdcHaut, INPUT);
  pinMode(FdcBas, INPUT);
  etatFdcHaut = digitalRead(FdcHaut);
  etatFdcBas = digitalRead(FdcBas);
  digitalWrite(Enable, HIGH);
  ledConfig(2, 1800, 1000, 20000);     //debug
}

//**********************************************************************************************************
//*** Utilisation de la fonction ledConfig()                                                             
//*** Mode: - 1 = LED clignotante, 'a': duree allumee, 'b': duree eteinte, 'c': nombre de clignotements
//***       - 2 = LED allumage progressif, 'a': duree pour passer de  eteint a allumee, 'b': duree durant
//***             laquelle le LED est eteinte entre deux cycles, 'c': duree du mode en ms
//***       - 3 = LED a extinction progressive: idem allumage progressif mais a l'envers pour le 'a'.
//***       - 4 = Code: 'a': nb. de flash du code, 'b': duree du flash, 'c':duree d'emission du code en ms.
//***********************************************************************************************************
void ledConfig(int mode, int a, int b, long c)
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

void loop() //***************************************************************************************************
{
//  luminosite();
  if(flashMode == 1) flash();
}
