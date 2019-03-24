//********** Declaration des constantes **********************************************************************
const int photoResistance = A0;      //Photo resistance connecté sur Pin A0 de l'arduino
const int setp = 2;                  //Pin step du A4988 connecté sur Pin 2 de l'arduino
const int dir = 3;                   //Pin dir du A4988 connecté sur Pin 3 de l'arduino
const int enable = 4;                //Pin step du A4988 connecté sur Pin 4 de l'arduino
const int LED = 5;                   //LED connecté sur Pin 5 de l'arduino
const int FdcHaut = A1;              //Fin de course haut connecté sur Pin A1 de l'arduino
const int FdcBas = A2;               //Fin de course bas connecté sur Pin A2 de l'arduino

//********** Declaration des variables pour l'ouverture/fermeture de la porte ********************************
boolean etatPorte;      //0: ouverte; 1: fermee   

//********** Declaration des variable pour la mesure de luminosité et traitement de l'info *******************
int seuil;              //Seuil de declenchement ouverture/fermeture porte.
int lumi;               //Variable qui stock la valeur de luminosité ambiante
long delaiTimer = 0;    //Pour valider que le sueil est bien franchi. evite les ouv/ferm intempestives
int validation = 0;     //Indique que l'on est en attende de validation que le seuil franchi
boolean hyst = 50;      //Hysteresis
long delai = 180000;    //Delai de validation 

void setup() 
{
  Serial.begin(9600);          //Utilisé pour debug  
  pinMode(LED, OUTPUT);        //
}

void ouverture() //*******************************************************************************************
{
  
}

void fermeture() //*******************************************************************************************
{
  
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
    if((lumi <= (seuil-hyst)) && (validation == 1))               //Si la luminosité sous le seuil pendant que la validation en cours
    {
      if(((millis() - delaiTimer) >= delai) && (validation == 1)) //Si delai de validation ecoulé
      {
        fermeture();                                              //ferme la porte 
        validation = 0;                                           //Sort du mode validation
      }
    }
  }
  
  if(etatPorte == 1)                                              //Si la porte est fermee
  {
    if((lumi >= (seuil - hyst)) && (validation == 0))             //Si la luminosité ambiante passe au dessus le seuil pour la premiere fois
    {
      delaiTimer = millis();                                      //Demarre le chrono de validation de depassement de seuil
      validation = 1;                                             //Indique que la vailidation de depassement de seuil est active
    }
    if((lumi <= (seuil - hyst)) && (validation == 1))             //Si la luminosité descend sous le seuil avant la fin de la validation
    {
      validation = 0;                                             //Annule la validation et l'ouverture de la porte
    }
    if((lumi >= (seuil-hyst)) && (validation == 1))               //Si la luminosité est au dessus du seuil et que la validation en cours
    {
      if(((millis() - delaiTimer) >= delai) && (validation == 1)) //Si delai de validation ecoulé
      {
        ouverture();                                              //ouvre la porte 
        validation = 0;                                           //Sort du mode validation
      }
    }
  }
}

void loop() 
{

}
