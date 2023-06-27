#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// const char *SSID = "MALONDA";
// const char *PWD = "heskymalonda";
const char *SSID = "Redmi Note 10S";
const char *PWD = "transistor";

//motors: note: minimal 100 maks 255
#define LREV 14 //blue /purple black
#define LFWD 12 //black /white black
#define LEN 13 //green 
//power 12V = blue black
#define RREV 19 //blue /purple
#define RFWD 18 //black /white
#define REN 21 //blue /blue

#define updatePeriod0 50 //update every what ms
#define updatePeriodTurn 110 //update every what ms
#define updatePeriodDelay 500

int PWM1= 85; //first step of the step function
int PWM2 = 92; 
int maxPWM1 = 155;
int maxPWM2 = maxPWM1*PWM2/PWM1;

unsigned long now=0;
unsigned long t0=0;
unsigned long tTurn=0;
unsigned long tDelay=0;

int arah1=0; 
int arah2=0;
int arah = 0;
bool isMoving=false;
int lastMove  = 99999;
WebServer server(80);

StaticJsonDocument<250> jsonDocument;
char buffer[250];


// Set your Static IP address
IPAddress local_IP(192, 168, 0, 33);
// Set your Gateway IP address
IPAddress gateway(192, 168, 100, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

void setup_routing() {     
  server.on("/arah", HTTP_POST, handlePost);    
  server.on("/data", handleGet); 
  server.begin();    
}
 
void create_json(char *tag, float value) {  
  jsonDocument.clear();  
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  serializeJson(jsonDocument, buffer);
}
 
void add_json_object(char *tag, float value) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["type"] = tag;
  obj["value"] = value;
}

void handleGet(){  
  jsonDocument.clear(); 
  add_json_object("Current Move : ", arah);
  serializeJson(jsonDocument, buffer); 
  server.send(200, "application/json", buffer); 
}

void handlePost() {
  if (server.hasArg("plain") == false) {
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
  arah = jsonDocument["arah"];
  unsigned long lastNow = millis();
  now = millis();
  while(now - lastNow <= updatePeriodTurn){
      run(arah);
      now = millis(); 
  }
  arah=0;
  if(lastMove != arah){
    Serial.println("Current Move : " + String(arah));
    lastMove=arah;
  }
  server.send(200, "application/json", "");
}

void turn(int direction){
  if(direction==0){
    digitalWrite(LREV, LOW);
    digitalWrite(LFWD, LOW);
    digitalWrite(RREV, LOW);
    digitalWrite(RFWD, LOW);
    isMoving=false;
  } else if(direction==1){
    digitalWrite(LREV, LOW);
    digitalWrite(LFWD, HIGH);
    digitalWrite(RREV, LOW);
    digitalWrite(RFWD, HIGH);
    isMoving=true;
  } else if(direction==2){
    digitalWrite(LREV, HIGH);
    digitalWrite(LFWD, LOW);
    digitalWrite(RREV, HIGH);
    digitalWrite(RFWD, LOW);
    isMoving=true;
  } else if(direction==3){
    digitalWrite(LREV, HIGH);
    digitalWrite(LFWD, LOW);
    digitalWrite(RREV, LOW);
    digitalWrite(RFWD, HIGH);
    isMoving=true;
  } else if(direction==4){
    digitalWrite(LREV, LOW);
    digitalWrite(LFWD, HIGH);
    digitalWrite(RREV, HIGH);
    digitalWrite(RFWD, LOW);
    isMoving=true;
  }
}

void setSpeed(int n, int n1, int n2){
  if(n!=n1){
    digitalWrite(LREV, LOW);
    digitalWrite(LFWD, LOW);
    digitalWrite(RREV, LOW);
    digitalWrite(RFWD, LOW);
  }
  if(n!=n2){
    analogWrite(LEN,maxPWM1);
    analogWrite(REN,maxPWM2);
    tTurn=now;
  } else {
    analogWrite(LEN,PWM1);
    analogWrite(REN,PWM2);
  }
}

void run(int x ) { 
  // now=millis();
  // if(!isMoving && now-tDelay>updatePeriodDelay){
  //   arah =x; 
  //   tDelay=now;
  // }
  // else { 
  //   arah = 0; 
  // }
  if(now-t0>updatePeriod0){
    setSpeed(arah, arah1, arah2);
    turn(arah);
    if(isMoving && now-tTurn>updatePeriodTurn){
      arah=0;
      tTurn=now;
    }

    arah2=arah1;
    arah1=arah;  
    t0=now;
  }
  
}
void setup() {     
  Serial.begin(115200); 
  // if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
  //   Serial.println("STA Failed to configure");
  // } 

  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(SSID, PWD);  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(""); 
 
  Serial.print("Connected! IP Address: ");
  Serial.println(WiFi.localIP());
  setup_routing();
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


}    

void loop() {    
  server.handleClient();    
}