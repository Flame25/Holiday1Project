#define Infra 13

int x;

void setup() {
  // put your setup code here, to run once:
  pinMode(Infra, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  x=analogRead(Infra);
  Serial.println(x);
}
