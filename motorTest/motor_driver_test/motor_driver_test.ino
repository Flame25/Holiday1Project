//note: minimal 100 maks 255
#define RMotBlueCon 18
#define RMotBlackCon 5
#define RMotEn 19

#define LMotBlueCon 4
#define LMotBlackCon 2
#define LMotEn 15

#define Infra 13

int x=0;
bool f=1;
int n=0;

void setup() {
  // put your setup code here, to run once:
  pinMode(RMotBlueCon, OUTPUT);
  pinMode(RMotBlackCon, OUTPUT);
  pinMode(RMotEn, OUTPUT);

  pinMode(LMotBlueCon, OUTPUT);
  pinMode(LMotBlackCon, OUTPUT);
  pinMode(LMotEn, OUTPUT);

  pinMode(Infra, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(f){
    digitalWrite(RMotBlueCon, HIGH);
    digitalWrite(RMotBlackCon,LOW);
  } else{
    digitalWrite(RMotBlueCon, LOW);
    digitalWrite(RMotBlackCon,HIGH);
  }
  analogWrite(RMotEn,x);
  while(Serial.available()<=0);
  n=int(Serial.parseInt());
  if(n==-2){
    f^=1;
  } else{
    x=int(n);
    Serial.println(n);
  }
  Serial.read();
}
