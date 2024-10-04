//********************************************************************************************
//***** keypad                      = [2,3,4,5,6,7,8]
//***** PN532             [SDA,SCL] = [A2,A3]
//***** LiquidCrystal_I2C [SDA,SCL] = [A4,A5]
//***** LED                         = [13]
//********************************************************************************************


#include <SoftwareSerial.h>
#include <PN532_SWHSU.h>
#include <PN532.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
//********************************************************************************************
//*****                                      def                                        ******
//********************************************************************************************
SoftwareSerial SWSerial(  10, 9 ); // RX, TX
PN532_SWHSU pn532swhsu( SWSerial );

PN532 nfc( pn532swhsu );
String tagId = "None", dispTag = "None";
byte nuidPICC[4];
 	
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display
const byte ROWS = 4; // four rows
const byte COLS = 3; // three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {8,7, 6, 5}; // connect to the row pinouts of the keypad
byte colPins[COLS] = { 4, 3,2}; // connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
char history[30] = "";

// Function to send messages to Serial
void sendToSerial(const String &message) {
  Serial.println(message);
}

//********************************************************************************************
//*****                                    setup                                        ******
//********************************************************************************************
void setup(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(LED_BUILTIN2, OUTPUT);

  Serial.begin(115200);
  lcd.begin(); // initialize the LCD
  lcd.backlight(); // Turn on the backlight and print a message.
  lcd.setCursor(4, 0);
  lcd.print("STARTING");



  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  int i=0;
  while(i==0){
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (! versiondata)
    {
      Serial.print("Didn't Find PN53x Module");
    }else{
      Serial.print("Find PN53x Module");
      i=1;
    }
  }

  delay(2000);  
  RECOGNITION();

  // Got valid data, print it out!
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); 
  Serial.println((versiondata >> 8) & 0xFF, DEC);
  // Configure board to read RFID tags
  nfc.SAMConfig();
}

//******************************
//***     RECOGNITION        ***
//******************************
void RECOGNITION(){
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("PENDING");
    sendToSerial("PENDING");
    lcd.setCursor(3, 1);  //  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
    lcd.print("RECOGNITION");
    sendToSerial("RECOGNITION");
    history[0] = '\0'; // Reset history
}
//********************************************************************************************
//*****                                     loop                                        ******
//********************************************************************************************
void loop()
{
  readNFC();

  String command = Serial.readStringUntil('\n');  // Read the incoming command from Python
  command.trim();  // Remove any trailing newline or space
  if (command == "tags") {
    int i=0;
    while(i==0){
      if (Serial.available() > 0) {
        String id = Serial.readStringUntil('\n');  // Read the incoming command from Python
        command.trim();  // Remove any trailing newline or space
        i=tags(id);
      }
    }
  }

  


  if (command == "Message_L1") {
    int i=0;
    while(i==0){
      if (Serial.available() > 0) {
        String Message_desired = Serial.readStringUntil('\n');  // Read the incoming command from Python
        command.trim();  // Remove any trailing newline or space
        i=Message_L1(Message_desired);
      }
    }
  }


  if (command == "Message_L2") {
    int i=0;
    while(i==0){
      if (Serial.available() > 0) {
        String Message_desired = Serial.readStringUntil('\n');  // Read the incoming command from Python
        command.trim();  // Remove any trailing newline or space
        i=Message_L2(Message_desired);
      }
    }
  }

  if (command == "RECOGNITION") {
    RECOGNITION();  // Call the RECOGNITION function when this command is received
  }


  if (command == "request") {
    Request1();  // Call the RECOGNITION function when this command is received
  }

  if (command == "read_ID_device") {
    read_ID_device();  // Call the RECOGNITION function when this command is received
  }


  /*if (command == "keypad_star") {
    keypad_star();  // Call the RECOGNITION function when this command is received
  }
  if (command == "Reset") {
    lcd.print("get out");
    return 1;
  }*/
}



//********************************************************************************************
//*****                                   readNFC                                       ******
//********************************************************************************************
void readNFC()
{
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                       // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  if (success)
  {
    LED();
    Serial.print("UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i = 0; i < uidLength; i++)
    {
      nuidPICC[i] = uid[i];
      Serial.print(" "); Serial.print(uid[i], DEC);
    }
    Serial.println();
    tagId = tagToString(nuidPICC);
    dispTag = tagId;
    Serial.print(F("tagId is : "));
    Serial.println(tagId);
    Serial.println("");
    delay(2000);                      // wait for a second
  }
}
//******************************
//***         tagId          ***
//******************************
String tagToString(byte id[4])
{
  String tagId = "";
  for (byte i = 0; i < 4; i++)
  {
    if (i < 3) tagId += String(id[i]) + ".";
    else tagId += String(id[i]);
  }
  return tagId;
}

//********************************************************************************************
//*****                                   readNFC                                       ******
//********************************************************************************************
int tags(String id){
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Valid Tag");
  //lcd.print(id);
  sendToSerial("Valid Tag");
  Enter_pass();
  password_after_tag();
  return 1;
}
//******************************
//***     ENTER PASSWORD     ***
//******************************
void Enter_pass(){
      delay(1000);
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("ENTER PASSWORD");
      sendToSerial("ENTER PASSWORD");
      lcd.setCursor(1, 1);
      history[0] = '\0'; // Reset history
}

void password_after_tag(){
  int i=0;
  while(i==0){
    char key = keypad.getKey();
    if (key) {
      lcd.print("*");
      if (strlen(history) < sizeof(history) - 1) {
        strncat(history, &key, 1);
      }
      sendToSerial(String(history));
    }
    if (key == '*') {
      i=0;
      String command = Serial.readStringUntil('\n');  // Read the incoming command from Python
      command.trim();  // Remove any trailing newline or space
      if (command == "password_clear") {
        password_clear();
      }
    }
    
    if (key == '#') {
      lcd.clear();
      lcd.print("CHECKING");
      delay(800);  
      lcd.clear();
      String command = Serial.readStringUntil('\n');  // Read the incoming command from Python
      command.trim();  // Remove any trailing newline or space
      if (command == "passwords_wrong") {
        passwords_wrong();
        i=1;
      }
      if (command == "passwords_correct") {
        passwords_correct();
        i=1;
      }
    }
  }
}


void passwords_correct() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PASSWORD CORRECT");
  sendToSerial("PASSWORD CORRECT");
  lcd.setCursor(5, 1);
  lcd.print("WELCOME");
  sendToSerial("WELCOME");
  delay(2000);                      // wait for a second
  LED();
  LED();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cheking Server");
  sendToSerial("Cheking Server");
  lcd.setCursor(0, 1);
  lcd.print("Just Moments...");
  sendToSerial("Just Moments...");

}


void passwords_wrong() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PASSWORD WRONG");
  sendToSerial("PASSWORD WRONG");
  delay(2000);                      // wait for a second
  RECOGNITION();
}



void password_clear(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PASSWORD CLEAR");
  Enter_pass();
  //RECOGNITION();
}
//******************************
//***      turn the LED      ***
//******************************
void LED(){
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(100);  
}


int Message_L1(String Message_desired){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(Message_desired);
  sendToSerial(Message_desired);
  return 1;
}




int Message_L2(String Message_desired){
  lcd.setCursor(0, 1);
  lcd.print(Message_desired);
  sendToSerial(Message_desired);
  return 1;
}




void Request1(){
  int i=0;
  while(i==0){
    char key = keypad.getKey();
    if (key) {
      if (key=='1' || key=='2') {
        i=1;
      }
      sendToSerial(String(key));
    }
  }
}



void read_ID_device(){
  sendToSerial("42.170.46.2");
}
