#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// The TinyGPS++ object
TinyGPSPlus gps;

// Configure software serial port for SIM900
SoftwareSerial SIM900(7, 8);

// Variable to store text message
String textMessage;

// Create a variable to store Lamp state
String lampState = "HIGH";

// Relay pin
const int relay = 12;

void setup() {
  // Automatically turn on the shield
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  delay(5000);

  // Set relay as OUTPUT
  pinMode(relay, OUTPUT);

  // start for relay off
  digitalWrite(relay, HIGH);

  // Initializing serial commmunication
  Serial.begin(9600);
  SIM900.begin(19200);

  // Give time to your GSM shield log on to network
  delay(5000);
  Serial.print("SIM900 ready...");

  // AT command to set SIM900 to SMS mode
  SIM900.print("AT+CMGF=1\r");
  delay(500);
  // Set module to send SMS data to serial out upon receipt
  SIM900.print("AT+CNMI=2,2,0,0,0\r");
  delay(500);
}

void loop() {
  while (Serial.available() > 0)
    if (gps.encode(Serial.read()))
      displayInfo();
      
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
  if (SIM900.available() > 0) {
    textMessage = SIM900.readString();
    Serial.print(textMessage);
    delay(10);
  }
  if (textMessage.indexOf("OFF") >= 0) {
    // menyalakan relay untuk mematikan motor dalam keadaan menyala
    digitalWrite(relay, LOW);
    lampState = "off";
    Serial.println("Relay set to ON");
    textMessage = "";
    delay(1000);
    SMSoff();
    delay(500);
  }
  else if (textMessage.indexOf("ON") >= 0) {
    // mematikan relay agar motor dapat dihidupkan kembali
    digitalWrite(relay, HIGH);
    lampState = "on";
    Serial.println("Relay set to OFF");
    textMessage = "";
    delay(1000);
    SMSon();
    delay(500);
  }
  else if (textMessage.indexOf("POSISI") >= 0) {
    // mengirim link google maps latitude dan longitude untuk mengetahui keberadaan motor
    textMessage = "";
    delay(1000);
    gpstrack();
    delay(500);
  }
}


void SMSoff() {
  // AT command to set SIM900 to SMS mode
  SIM900.print("AT+CMGF=1\r"); 
  delay(100);

  // REPLACE THE X's WITH THE RECIPIENT'S MOBILE NUMBER
  // USE INTERNATIONAL FORMAT CODE FOR MOBILE NUMBERS
  SIM900.println("AT + CMGS = \"+6287778013547\""); 
  delay(100);
  
  // REPLACE WITH YOUR OW   N SMS MESSAGE CONTENT
  SIM900.println(" motor mati."); 
  delay(100);

  // End AT command with a ^Z, ASCII code 26
  SIM900.println((char)26); 
  delay(100);
  SIM900.println();
  // Give module time to send SMS
  delay(5000); 
}

void SMSon() {
  // AT command to set SIM900 to SMS mode
  SIM900.print("AT+CMGF=1\r"); 
  delay(100);

  // REPLACE THE X's WITH THE RECIPIENT'S MOBILE NUMBER
  // USE INTERNATIONAL FORMAT CODE FOR MOBILE NUMBERS
  SIM900.println("AT + CMGS = \"+6287778013547\""); 
  delay(100);
  
  // REPLACE WITH YOUR OW   N SMS MESSAGE CONTENT
  SIM900.println(" motor nyala."); 
  delay(100);

  // End AT command with a ^Z, ASCII code 26
  SIM900.println((char)26); 
  delay(100);
  SIM900.println();
  // Give module time to send SMS
  delay(5000); 
}

void gpstrack() {
  // AT command to set SIM900 to SMS mode
  SIM900.print("AT+CMGF=1\r"); 
  delay(100);

  // REPLACE THE X's WITH THE RECIPIENT'S MOBILE NUMBER
  // USE INTERNATIONAL FORMAT CODE FOR MOBILE NUMBERS
  SIM900.println("AT + CMGS = \"+6287778013547\""); 
  delay(100);
  
  // REPLACE WITH YOUR OW   N SMS MESSAGE CONTENT
  SIM900.print("https://maps.google.com/maps?q=");
  SIM900.print(gps.location.lat(), 6);
  SIM900.print(F(","));
  SIM900.print(gps.location.lng(), 6); 
  delay(100);

  // End AT command with a ^Z, ASCII code 26
  SIM900.println((char)26); 
  delay(100);
  SIM900.println();
  // Give module time to send SMS
  delay(5000); 
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
