#include <WiFi.h>
#include <Firebase_ESP_Client.h>
// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "TECH_8A5F"
#define WIFI_PASSWORD "212152497"

/* 2. Define the RTDB URL */
#define DATABASE_URL "motortest-baf91-default-rtdb.asia-southeast1.firebasedatabase.app" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

//motors: note: minimal 100 maks 255
#define RREV 19 //blue
#define RFWD 18 //black
#define REN 21 //green

#define LREV 4 //blue
#define LFWD 2 //black
#define LEN 15 //blue

#define Acc 250 //in how many changes (in PWM) per second
#define updatePeriod 500 //update every what ms
#define steps 2 //how many steps
#define PWM1 190 //first step of the step function

//variables
int target_L_PWM = 0;
int target_R_PWM = 0;
int L_PWM = 0;
int R_PWM = 0;
unsigned long now;
unsigned long t0=0;
unsigned long tAcc=0;
int n;
int lastn[steps];


/* 3. Define the Firebase Data object */
FirebaseData fbdo;

/* 4, Define the FirebaseAuth data for authentication data */
FirebaseAuth auth;

/* Define the FirebaseConfig data for config data */
FirebaseConfig config;

void setup()
{
    //Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    //Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        //Serial.print(".");
        delay(300);
    }
    //Serial.println();
    //Serial.print("Connected with IP: ");
    //Serial.println(WiFi.localIP());
    //Serial.println();
    //Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    /* Assign the database URL(required) */
    config.database_url = DATABASE_URL;
    config.signer.test_mode = true; //confirm test mode (with full public)
    Firebase.reconnectWiFi(true);

    /* Initialize the library with the Firebase authen and config */
    Firebase.begin(&config, &auth);
    // Or use legacy authenticate method
    // Firebase.begin(DATABASE_URL, DATABASE_SECRET);

    //set motor
    pinMode(RREV, OUTPUT);
    pinMode(RFWD, OUTPUT);
    pinMode(REN, OUTPUT);

    pinMode(LREV, OUTPUT);
    pinMode(LFWD, OUTPUT);
    pinMode(LEN, OUTPUT);

    digitalWrite(LREV, LOW);
    digitalWrite(LFWD, LOW);
    digitalWrite(RREV, LOW);
    digitalWrite(RFWD, LOW);
    analogWrite(LEN,0);
    analogWrite(REN,0);

    //setup all the n
    for(int i=0; i<steps; i++){
      lastn[i]=0;
    }
}

void loop()
{
  now=millis();
  if(now-t0>updatePeriod){
    if(Firebase.RTDB.getInt(&fbdo, F("/motorTest/int"))){
      n=fbdo.to<int>();
      if(n!=lastn[1]){
        analogWrite(LEN,PWM1);
        analogWrite(REN,PWM1);
      } else {
        analogWrite(LEN,255);
        analogWrite(REN,255);
      }
      if(n!=lastn[0] || n==0){
        digitalWrite(LREV, LOW);
        digitalWrite(LFWD, LOW);
        digitalWrite(RREV, LOW);
        digitalWrite(RFWD, LOW);
      } else if(n==1){ //forward
        digitalWrite(LREV, LOW);
        digitalWrite(LFWD, HIGH);
        digitalWrite(RREV, LOW);
        digitalWrite(RFWD, HIGH);
      } else if(n==2){ //backward
        digitalWrite(LREV, HIGH);
        digitalWrite(LFWD, LOW);
        digitalWrite(RREV, HIGH);
        digitalWrite(RFWD, LOW);
      } else if(n==3){ //left
        digitalWrite(LREV, HIGH);
        digitalWrite(LFWD, LOW);
        digitalWrite(RREV, LOW);
        digitalWrite(RFWD, HIGH);
      } else if(n==4){ //right
        digitalWrite(LREV, LOW);
        digitalWrite(LFWD, HIGH);
        digitalWrite(RREV, HIGH);
        digitalWrite(RFWD, LOW);
      }
      lastn[1]=lastn[0];
      lastn[0]=n;
    }
  }
  /*
  if(now-tAcc>1000/Acc){
    //PWM
    if(target_L_PWM>L_PWM){
      L_PWM++;
    } else if(target_L_PWM<L_PWM){
      L_PWM--;
    }
    if(target_R_PWM>R_PWM){
      R_PWM++;
    } else if(target_R_PWM<R_PWM){
      R_PWM--;
    }

    //left
    if(L_PWM>=0){
      digitalWrite(LREV, LOW);
      digitalWrite(LFWD, HIGH);
    } else{
      digitalWrite(LREV, HIGH);
      digitalWrite(LFWD, LOW);
    }
    analogWrite(LEN, abs(L_PWM));

    //right PWM
    if(R_PWM>=0){
      digitalWrite(RREV, LOW);
      digitalWrite(RFWD, HIGH);
    } else{
      digitalWrite(RREV, HIGH);
      digitalWrite(RFWD, LOW);
    }
    analogWrite(REN, abs(R_PWM));

    tAcc=millis();
  }
  */
}
