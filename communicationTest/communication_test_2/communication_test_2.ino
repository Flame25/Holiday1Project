#include <WiFi.h>
#include <Firebase_ESP_Client.h>
// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
// #define WIFI_SSID "MALONDA"
// #define WIFI_PASSWORD "heskymalonda"
// #define WIFI_SSID "TECH_8A5F"
// #define WIFI_PASSWORD "212152497"
#define WIFI_SSID "Redmi Note 10S"
#define WIFI_PASSWORD "transistor"

/* 2. Define the RTDB URL */
#define DATABASE_URL "motortest-baf91-default-rtdb.asia-southeast1.firebasedatabase.app" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

//motors: note: minimal 100 maks 255
#define LREV 14 //blue /purple black
#define LFWD 12 //black /white black
#define LEN 13 //green 
//power 12V = blue black
#define RREV 19 //blue /purple
#define RFWD 18 //black /white
#define REN 21 //blue /blue

#define Acc 250 //in how many changes (in PWM) per second
#define updatePeriod0 50 //update every what ms
#define updatePeriodTurn 110 //update every what ms
#define updatePeriod1 2500 //update every what ms
#define steps 2 //how many steps
#define maxMadLevel 30 //
#define Tdelay 500
int PWM1= 85; //first step of the step function
int PWM2 = 92; 
int maxPWM1 = 150;
int maxPWM2 = maxPWM1*PWM2/PWM1;
//variables
int target_L_PWM = 0;
int target_R_PWM = 0;
int L_PWM = 0;
int R_PWM = 0;
unsigned long now;
unsigned long t0=0;
unsigned long t1=0;
unsigned long tDelay=0;
unsigned long tTurn=0;
unsigned long tAcc=0;
int n=0;
int failSend=0;
int lastn[steps];
bool isMoving=false;
int madLevel=0;


/* 3. Define the Firebase Data object */
FirebaseData fbdo;
FirebaseData fbdoRUN;
FirebaseData fbdoSTOP;

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
  if(Firebase.ready()){
    now=millis();
    if(now-t0>updatePeriod0){
      // analogWrite(LEN,0);
      // analogWrite(REN,0);
      if(n==0){
        isMoving=false;
      } else{
        isMoving=true;
      }
      if(!isMoving && now-tDelay>Tdelay){
        if(true/*Firebase.ready()*/){
          if(Firebase.RTDB.getInt(&fbdoRUN, F("/motorTest/int"))){
            n=fbdoRUN.to<int>();
          }
        } else{
          madLevel++;
        }
        tDelay=now;
      }
      if(n!=lastn[1]){
        analogWrite(LEN,maxPWM1);
        analogWrite(REN,maxPWM2);
      } else {
        analogWrite(LEN,PWM1);
        analogWrite(REN,PWM2);
      }
      if(n!=lastn[0] || n==0){
        digitalWrite(LREV, LOW);
        digitalWrite(LFWD, LOW);
        digitalWrite(RREV, LOW);
        digitalWrite(RFWD, LOW);
        tTurn=now+updatePeriod0;
      } else if(n==1){ //forward
        digitalWrite(LREV, LOW);
        digitalWrite(LFWD, HIGH);
        digitalWrite(RREV, LOW);
        digitalWrite(RFWD, HIGH);
        if(now-tTurn>updatePeriodTurn && n==lastn[1]){
          if(true/*Firebase.ready()*/){
            if(Firebase.RTDB.setInt(&fbdoSTOP, F("/motorTest/int"), 0)){
              tTurn=now;
              n=0;
            } else{
              failSend++;
            }
          } else{
            madLevel++;
          }
        }
      } else if(n==2){ //backward
        digitalWrite(LREV, HIGH);
        digitalWrite(LFWD, LOW);
        digitalWrite(RREV, HIGH);
        digitalWrite(RFWD, LOW);
        if(now-tTurn>updatePeriodTurn && n==lastn[1]){
          if(true/*Firebase.ready()*/){
            if(Firebase.RTDB.setInt(&fbdoSTOP, F("/motorTest/int"), 0)){
              tTurn=now;
              n=0;
            } else{
              failSend++;
            }
          } else{
            madLevel++;
          }
        }
      } else if(n==3){ //left
        digitalWrite(LREV, HIGH);
        digitalWrite(LFWD, LOW);
        digitalWrite(RREV, LOW);
        digitalWrite(RFWD, HIGH);
        if(now-tTurn>updatePeriodTurn && n==lastn[1]){
          if(true/*Firebase.ready()*/){
            if(Firebase.RTDB.setInt(&fbdoSTOP, F("/motorTest/int"), 0)){
              tTurn=now;
              n=0;
            } else{
              failSend++;
            }
          } else{
            madLevel++;
          }
        }
      } else if(n==4){ //right
        digitalWrite(LREV, LOW);
        digitalWrite(LFWD, HIGH);
        digitalWrite(RREV, HIGH);
        digitalWrite(RFWD, LOW);
        if(now-tTurn>updatePeriodTurn && n==lastn[1]){
          if(true/*Firebase.ready()*/){
            if(Firebase.RTDB.setInt(&fbdoSTOP, F("/motorTest/int"), 0)){
              tTurn=now;
              n=0;
            } else{
              failSend++;
            }
          } else{
            madLevel++;
          }
        }
      }
      lastn[1]=lastn[0];
      lastn[0]=n;
      t0=now;
    }
  } else{
    madLevel++;
  }
    if(madLevel>maxMadLevel){
      Firebase.begin(&config, &auth);
      madLevel=0;
    }
    /* use this if you want to control speed or debug
    if(now-t1>updatePeriod1){
      if(Firebase.RTDB.getInt(&fbdo, F("/motorTest/PWM1"))){
        PWM1=fbdo.to<int>();
      } else{
        failSend++;
      }
      if(Firebase.RTDB.getInt(&fbdo, F("/motorTest/PWM2"))){
        PWM2=fbdo.to<int>();
      } else{
        failSend++;
      }
      Firebase.RTDB.setInt(&fbdo, F("/motorTest/failSend"), failSend);
      failSend=0;
      t1=now;
    } */
  //}
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
