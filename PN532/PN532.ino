#include <SoftwareSerial.h>
#include <PN532_SWHSU.h>
#include <PN532.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
//********************************************************************************************
//*****                                      def                                        ******
//********************************************************************************************
SoftwareSerial SWSerial( 10, 9 ); // RX, TX
 
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
byte rowPins[ROWS] = {8, 7, 6, 5}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 3, 2}; // connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
char history[30] = "";
#define LED_BUILTIN2 12
//********************************************************************************************
//*****                                    setup                                        ******
//********************************************************************************************
void setup(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_BUILTIN2, OUTPUT);

  Serial.begin(115200);
  lcd.begin(); // initialize the LCD
  lcd.backlight(); // Turn on the backlight and print a message.
  RECOGNITION();
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata)
  {
    Serial.print("Didn't Find PN53x Module");
    while (1); // Halt
  }
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
//********************************************************************************************
//*****                                     loop                                        ******
//********************************************************************************************
void loop()
{
  readNFC();
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
    //delay(1000);  // 1 second halt
    //******************************
    //***        tagId           ***
    //******************************
    if (tagId == "42.170.46.2"){
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Hi Ali");
      LED();
      Enter_pass();
      password_after_tag("5678#");
    }
    //******************************
    //***        tagId           ***
    //******************************
    if (tagId == "131.107.229.39"){
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Hi Hassan");
      LED();
      Enter_pass();
      password_after_tag("1234#");
    }
  }
}
//********************************************************************************************
//*****                                   readNFC                                       ******
//********************************************************************************************
//******************************
//***     RECOGNITION        ***
//******************************
void RECOGNITION(){
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("PENDING");
    lcd.setCursor(3, 1);  //  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
    lcd.print("RECOGNITION");
    history[0] = '\0'; // Reset history
}
//******************************
//***     ENTER PASSWORD     ***
//******************************
void Enter_pass(){
      delay(1000);
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("ENTER PASSWORD");
      lcd.setCursor(1, 1);
      history[0] = '\0'; // Reset history
}
//******************************
//***      turn the LED      ***
//******************************
void LED(){
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(2000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(100);  
}
//******************************
//***      turn the LED2     ***
//******************************
void LED2(){
  digitalWrite(LED_BUILTIN2, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(2000);                      // wait for a second
  digitalWrite(LED_BUILTIN2, LOW);   // turn the LED off by making the voltage LOW
  delay(100);  
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
//******************************
//***        keypad          ***
//******************************
void password_after_tag(const char* pass){
  int tree_time=0;
  int i=0;
  while(i==0){
    char key = keypad.getKey();
    if (key) {
      lcd.print(key);
      // Append the key to history (make sure we don't overflow history buffer)
      if (strlen(history) < sizeof(history) - 1) {
        strncat(history, &key, 1);
      }
      Serial.println(history);
    }
    if (key == '*') {
      Enter_pass();
      if (tree_time == 2){
        i=1;
        RECOGNITION();
      }
      tree_time+=1;
    }
    if (key == '#') {
      lcd.clear();
      lcd.print("CHECKING");
      // Compare history with pass
      if (strcmp(history, pass) == 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("PASSWORD CORRECT");
        lcd.setCursor(5, 1);
        lcd.print("WELCOME");
        LED2();
        delay(1000);
        i=1;
      } else {
        lcd.clear();
        lcd.print("WRONG PASSWORD");
        delay(1000);
        Enter_pass();
        if (tree_time == 2){
          i=1;
          RECOGNITION();
        }
        tree_time+=1;
      }
      history[0] = '\0'; // Reset history after check
    }
  }
}