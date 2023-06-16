/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-websocket-server-arduino/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "Redmi Note 10S";
const char* password = "transistor";

//note: minimal 100 maks 255
#define RMotBlueCon 18
#define RMotBlackCon 5
#define RMotEn 19

#define LMotBlueCon 4
#define LMotBlackCon 2
#define LMotEn 15

#define Acc 1 //in how many changes (in PWM) per second

//variables
int target_L_PWM = 0;
int target_R_PWM = 0;
int L_PWM = 0;
int R_PWM = 0;
unsigned long now;
unsigned long tAcc=0;
unsigned long tNot=0;
unsigned long tClean=0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Fire Fighter Controller</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>Fire Fighter Controller</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>Fire Fighter Controller</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>Output - MOVE</h2>
      <p class="state">L_PWM: <span id="l_pwm">%L_PWM%</span>  R_PWM: <span id="r_pwm">%R_PWM%</span></p>
      <p><button id="forward" class="button">Forward</button></p>
      <p><button id="left" class="button">Left</button>  <button id="stop" class="button">Stop</button>  <button id="right" class="button">Right</button></p>
      <p><button id="backward" class="button">Backward</button></p>
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onPWM = onPWM; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onPWM(event) {                             //ubah ini
    var L_PWM;
    var R_PWM;
    L_PWM = event.data;
    R_PWM = "0";
    document.getElementById('l_pwm').innerHTML = L_PWM;
    document.getElementById('r_pwm').innerHTML = R_PWM;
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('forward').addEventListener('click', toggle('forward'));
    document.getElementById('left').addEventListener('click', toggle('left'));
    document.getElementById('stop').addEventListener('click', toggle('stop'));
    document.getElementById('right').addEventListener('click', toggle('right'));
    document.getElementById('backward').addEventListener('click', toggle('backward'));
  }
  function toggle(direction){
    websocket.send(direction);
  }
</script>
</body>
</html>
)rawliteral";

void notifyClients() {
  ws.textAll(String(L_PWM));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "stop") == 0) {
      target_L_PWM=0;
      target_R_PWM=0;
    } else if (strcmp((char*)data, "forward") == 0) {
      target_L_PWM=255;
      target_R_PWM=255;
    } else if (strcmp((char*)data, "left") == 0) {
      target_L_PWM=-255;
      target_R_PWM=255;
    } else if (strcmp((char*)data, "right") == 0) {
      target_L_PWM=255;
      target_R_PWM=-255;
    } else if (strcmp((char*)data, "backward") == 0) {
      target_L_PWM=-255;
      target_R_PWM=-255;
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "L_PWM"){
    return String(L_PWM);
  } else if (var == "R_PWM"){
    return String(R_PWM);
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
    //request->send_P(200, "text/html", index_html, send_R_PWM);
  });

  // Start server
  server.begin();

  //set motor
  pinMode(RMotBlueCon, OUTPUT);
  pinMode(RMotBlackCon, OUTPUT);
  pinMode(RMotEn, OUTPUT);

  pinMode(LMotBlueCon, OUTPUT);
  pinMode(LMotBlackCon, OUTPUT);
  pinMode(LMotEn, OUTPUT);
}

void loop() {
  now=millis();
  if(now-tAcc>1000/Acc){
    //PWM
    if(true/*target_L_PWM>L_PWM*/){
      L_PWM++;
    } else if(target_L_PWM<L_PWM){
      L_PWM--;
    }
    Serial.print(L_PWM);
    if(target_R_PWM>R_PWM){
      R_PWM++;
    } else if(true/*target_R_PWM<R_PWM*/){
      R_PWM--;
    }
    Serial.print("  ");
    Serial.println(R_PWM);

    //left
    if(L_PWM>=0){
      digitalWrite(LMotBlueCon, HIGH);
      digitalWrite(LMotBlackCon, LOW);
    } else{
      digitalWrite(LMotBlueCon, LOW);
      digitalWrite(LMotBlackCon, HIGH);
    }
    analogWrite(LMotEn, abs(L_PWM));

    //right PWM
    if(R_PWM>=0){
      digitalWrite(RMotBlueCon, HIGH);
      digitalWrite(RMotBlackCon, LOW);
    } else{
      digitalWrite(RMotBlueCon, LOW);
      digitalWrite(RMotBlackCon, HIGH);
    }
    analogWrite(RMotEn, abs(R_PWM));

    tAcc=millis();
  }
  
  if(now-tNot>200){//every 0.2 s
    notifyClients(); 
    tNot=millis();
  }

  if(now-tClean>1000){ //every 1 s
    ws.cleanupClients();
    tClean=millis();
  }
}