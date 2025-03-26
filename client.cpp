#include <EEPROM.h>
#include <SoftwareSerial.h>

const byte rxPin = 2;
const byte txPin = 3;
const byte MODE = 4;

SoftwareSerial mySerial(rxPin, txPin);
byte ID = EEPROM.read(3);
byte komu;
char MSG[250];
char INMSG[250];
int MSGcount = 0; //Счётчик символов в сообщении при вводе в память
boolean isMSG = false; //Флаг режима консоли (1 - запись сообщения, 0 - обработка команд)

void setup() {
  pinMode(4,OUTPUT);
  Serial.begin(115200);
  mySerial.begin(9600);
}

void xCon(){
  char M[9];
  M[0]='s'; //Костыль чтобы не уходило в цикл
if (Serial.available()>8) {
  for (int i=0; i<9; i++) {
  M[i]=Serial.read();
  }
}
while ((Serial.available()<9)&(Serial.available()>0)) {
  //Если команда пришла в принципе неправильная
  Serial.read();
}
if ((M[0]=='L')&(M[1]=='S')&(M[2]=='D')){
  //Команда установки своего номера 
  int a = (M[4] - 48)*100 + (M[5] - 48)*10 + (M[6] - 48);
  ID = a;
  Serial.print ("ID = ");
  EEPROM.update(3,ID);
  Serial.println(ID);
}
if ((M[0]=='M')&(M[1]=='S')&(M[2]=='G')){
  //Команда ввода сообщения в память
  Serial.println("Tape your message. // to stop");
  MSGcount = 0;
  isMSG = true;
}
if ((M[0]=='C')&(M[1]=='U')&(M[2]=='R')){
  //Команда вывода сообщения на экран
  Serial.println("------------------------");
          for (int i=0; i<MSGcount; i++) {
            Serial.print(MSG[i]);
          }
          Serial.println();
 Serial.println("------------------------");
}
if ((M[0]=='S')&(M[1]=='N')&(M[2]=='D')){
  //Команда отправки сообщения
  int a = (M[4] - 48)*100 + (M[5] - 48)*10 + (M[6] - 48);
  komu = a;
  Serial.print ("TO ->");
  Serial.println(komu);  
          digitalWrite(MODE, HIGH);
          mySerial.print((char)1);
          if (ID<100) { mySerial.print("0"); }
          if (ID<10) { mySerial.print("0"); }
          mySerial.print(ID);
          mySerial.print(":");
          if (komu<100) { mySerial.print("0"); }
          if (komu<10) { mySerial.print("0"); }
          mySerial.println(komu);
          digitalWrite(MODE, LOW);
} 
}

void loop() {
  int inc = 0;
  INMSG[0] = 0;
  while (mySerial.available()) {
    INMSG[inc] = mySerial.read();
    inc++;
  }
  if (INMSG[0]==2) {
    int a = (INMSG[1] - 48)*100 + (INMSG[2] - 48)*10 + (INMSG[3] - 48);
    if (a==ID) {
      Serial.println("Server OK");
      digitalWrite(MODE, HIGH);
      mySerial.print((char)3);
      if (ID<100) { mySerial.print("0"); }
      if (ID<10) { mySerial.print("0"); }
      mySerial.print(ID);
      mySerial.print(":");
      if (komu<100) { mySerial.print("0"); }
      if (komu<10) { mySerial.print("0"); }
      mySerial.print(komu);
      mySerial.print(":");
      for (int i=0; i<MSGcount; i++) {
            mySerial.print(MSG[i]);
      }
      mySerial.println((char)4);      
      digitalWrite(MODE, LOW);
    }
  }
  if (INMSG[0]==5) {
    int a = (INMSG[1] - 48)*100 + (INMSG[2] - 48)*10 + (INMSG[3] - 48);
    int b = (INMSG[5] - 48)*100 + (INMSG[6] - 48)*10 + (INMSG[7] - 48);
    if (a==ID) {
      Serial.print("MESSAGE FROM ");
      Serial.println(b);
      Serial.println("------------------------");
      int ax = 9;
      while (INMSG[ax]!=4) {
        Serial.print(INMSG[ax]); 
        ax++;
      }
      Serial.println();
      Serial.println("------------------------");
    }
  }
  while (mySerial.available()) {
    Serial.write(mySerial.read()); 
  }  
  if (!isMSG) { 
    xCon(); 
  } else {
    while (Serial.available()) {
      char a = Serial.read();
      if ((a!=10)&(a!=13)) {
        MSG[MSGcount] = a;
        MSGcount++;
        //ПРоверка конца сообщения!
        if ((MSGcount>249)|(((MSG[MSGcount-1]=='/')&(MSG[MSGcount-2]=='/')))) {
          Serial.println("DONE");
          if ((MSG[MSGcount-1]=='/')&(MSG[MSGcount-2]=='/')) { MSGcount-=2; }
          for (int i=0; i<MSGcount; i++) {
            Serial.print(MSG[i]);
          }
          Serial.println();
          isMSG = false;
        }
      }      
    }
  }
delay(100);
}
