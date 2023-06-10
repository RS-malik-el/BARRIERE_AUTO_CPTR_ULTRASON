/**
 * e-mail : openprogramming23@gmail.com
 * gitHub : https://github.com/RS-malik-el
 * Donation : paypal.me/RachelSysteme
 *
 * @AUTEUR : Exaucé KIMBEMBE / @OpenProgramming
 * DATE : 10/06/2023
 * 
 * @Board : Arduino
 * 
 * Barrière automatique
 * 
 * @Outils:
 * 
 * (1) Une carte arduino
 * (2) Deux capteurs ultrason HC-SR04
 * (1) un servomoteur
 * (1) un ecran lcd simple ou I2C
 * (1) un bipeur 
*/

#define LCD_I2C false // Choix du type d'ecran
#define COL     16    // Nombre de colonne de l'ecran
#define ROW     2     // Nombre de colonne de la ligne

#ifdef __AVR__
	
	#include <Servo.h>

	#if (LCD_I2C == true)

		#include <LiquidCrystal_I2C.h>
		
		// Pin connecté au servomoteur
		#define PIN_SERVO   2  
		// Pin capteur ultrason 1
		#define TRIG_PIN_1  3
		#define ECHO_PIN_1  4
		// Pin capteur ultrason 2
		#define TRIG_PIN_2  5
		#define ECHO_PIN_2  6
		//Pin buzzer
		#define PIN_BUZZER  7

		LiquidCrystal_I2C lcd(0x27, COL, ROW);
	
	#else

		#include <LiquidCrystal.h>

		// Connexion de l'ecran LCD
		#define EN 2
		#define RS 3
		#define D4 4
		#define D5 5
		#define D6 6
		#define D7 7
		// Pin connecté au servomoteur
		#define PIN_SERVO   8  
		// Pin capteur ultrason 1
		#define TRIG_PIN_1  9
		#define ECHO_PIN_1  10
		// Pin capteur ultrason 2
		#define TRIG_PIN_2  11
		#define ECHO_PIN_2  12
		//Pin buzzer
		#define PIN_BUZZER  13
		
		LiquidCrystal lcd(RS,EN,D4,D5,D6,D7);
	#endif // fin #if

#else
	#error "Selectionner une carte arduino"
#endif // fin #ifdef

#define ATTENTE     10    // Attente après chaque incrémentation(servomoteur)
#define DISTANCE 	  20    // Distance de detection maximum en cm
#define DETECTION 	true 
#define HAUT 	      180   // Angle servomoteur (porte ouverte) 
#define BAS 	      90    // Angle servomoteur (porte fermer)
#define OPEN 	      false // Constante porte fermer
#define CLOSE     	true  // Constante porte ouverte

// Paramètres buzzer
#define FREQ    200 
#define TIMEOUT 500

Servo servo; // Objet servomoteur

// Prototypes des fonctions
void gestion_porte(const bool etat);   // Permet de positionner le servomoteur
void gestionES(const uint8_t trigPin, const uint8_t echoPin); // Détermine si les 2 capteurs sont franchis
bool detection(const uint8_t trigPin, const uint8_t echoPin); // detecte si l'objet est près du capteur

void setup(){
	// Configuration de l'écran LCD
	#if(LCD_I2C == true)
		lcd.init();
		lcd.backlight();
	#else
		lcd.begin(COL,ROW);
	#endif

	// Configuration du servomoteur
	servo.attach(PIN_SERVO);
	servo.write(BAS);
	// Cobfiguration Buzzer
	pinMode(PIN_BUZZER, OUTPUT);
	//Configuration capteur ultrason
	pinMode(TRIG_PIN_1, OUTPUT);
	pinMode(TRIG_PIN_2, OUTPUT);
	pinMode(ECHO_PIN_1, INPUT);
	pinMode(ECHO_PIN_2, INPUT);
}

void loop(){
	// Affichage du message
	lcd.clear();
	lcd.setCursor(3,0);
	lcd.print("BIENVENUE");
	lcd.setCursor(3,1);
	lcd.print("AU PARKING");

	// Entrée
	if(detection(TRIG_PIN_1, ECHO_PIN_1) == DETECTION && 
		detection(TRIG_PIN_2, ECHO_PIN_2) == not DETECTION)
	{
		tone(PIN_BUZZER,FREQ,TIMEOUT);// bip
		// Affichage du message
		lcd.clear();
		lcd.setCursor(3,0);
		lcd.print("BIENVENUE");
		lcd.setCursor(3,1); // positionnement du curseur 
		lcd.print("BON ARRIVE");
		// Ouverture porte
		gestion_porte(OPEN);
		// Dectection si capteur ultrason 1 franchis
		gestionES(TRIG_PIN_2, ECHO_PIN_2);
		// Fermeture porte
		gestion_porte(CLOSE);
	}// fin if

	// Sortie
	if(detection(TRIG_PIN_1, ECHO_PIN_1) == not DETECTION && 
		detection(TRIG_PIN_2, ECHO_PIN_2) == DETECTION)
	{
		tone(PIN_BUZZER,FREQ,TIMEOUT);// bip
		// Affichage du message
		lcd.clear();
		lcd.setCursor(3,0);
		lcd.print("BIENVENUE");
		lcd.setCursor(2,1); // positionnement du curseur
		lcd.print("BONNE SORTIE");
		// Ouverture porte
		gestion_porte(OPEN);
		// Dectection si capteur ultrason 2 franchis
		gestionES(TRIG_PIN_1, ECHO_PIN_1);
		// Fermeture porte
		gestion_porte(CLOSE);
	}// fin if
  
	delay(500);
}

void gestion_porte(const bool etat){
	if(etat == OPEN){
		for (int i = servo.read(); i <= HAUT; ++i){
			servo.write(i);
			delay(ATTENTE);
		}
	}
	if (etat == CLOSE){
		for (int i = servo.read(); i >= BAS; --i){
			servo.write(i);
			delay(ATTENTE);
		}
	}
}

void gestionES(const uint8_t trigPin, const uint8_t echoPin){
	while(detection(trigPin, echoPin) == not DETECTION){delay(ATTENTE);}
	while(detection(trigPin, echoPin) == DETECTION){delay(ATTENTE);}
	delay(1000);
}


bool detection(const uint8_t trigPin, const uint8_t echoPin){
  unsigned long duration = 0;
  unsigned long distance = 0;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Lit la broche echoPin, renvoie le temps de parcours de l'onde sonore en microsecondes
  duration = pulseIn(echoPin, HIGH); 
  distance = duration * 0.034 / 2;

  if(distance <= DISTANCE)
  	return DISTANCE;
  else
  	return not DISTANCE;
}