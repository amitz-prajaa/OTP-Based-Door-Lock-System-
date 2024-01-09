#include <LiquidCrystal.h>
#include <Keypad.h>
#include <SoftwareSerial.h>

LiquidCrystal lcd(12, 11, 7, 8, 9, 10); // LCD pins  ( rs, en, d4, d5, d6, d7 )
const int ROWS = 4;
const int COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {A5, A4, A3, A2};
byte colPins[COLS] = {A1, A0, 3, 4};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
SoftwareSerial mySerial(13, 6); // SIM800L Tx, Rx pins

String storedPassword = "1234"; // Change to your desired password
String otp ;
#define relay 5

void setup() {
  lcd.begin(16, 2);
  mySerial.begin(9600);
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  digitalWrite(relay , 1);
}


void sendSMS(String number , String message) {
  Serial.println("Initializing...");

  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();

  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();

  mySerial.println("AT+CMGS=\"" + number + "\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();

  mySerial.print(message); //text content
  updateSerial();

  mySerial.write(26);
}



void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (mySerial.available())
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}

String generateOTP() {
  randomSeed(millis());  // Seed the random number generator within the function
  String result = String(random(1111, 9999));
  return result;
}

void loop() {
  // Request user to enter password
  lcd.clear();
  lcd.print("Enter Password:");
  String enteredPassword = getKeypadInput();

  // Verify entered password
  if (enteredPassword.equals(storedPassword)) {

    otp = generateOTP();
    sendSMS("+916266362249" , " |---  Door Lock System ---| Your OTP for opening Lock is " + otp + ". It is valid for one minute only !!"); // Send SMS with OTP
    lcd.clear();

    lcd.print("Enter OTP:");
    Serial.println("Enter OTP:");
    String enteredOTP = getKeypadInput();

    // Verify entered OTP
    Serial.print("<");
    Serial.print(enteredOTP);
    Serial.println(">");
    if (enteredOTP.equals(otp)) {
      lcd.clear();
      lcd.print("Door Opened");
      digitalWrite(relay , 0);
      delay(10000);
      digitalWrite(relay , 1);
      lcd.clear();
      // Add code to trigger the door opening mechanism here
    } else {
      lcd.clear();
      lcd.print("Invalid OTP");
      delay(2000);
    }
  } else {
    lcd.clear();
    lcd.print("Incorrect Password");
    delay(2000);
  }
}

String getKeypadInput() {
  char key;
  String input = "";
  while (input.length() < 4) { // Assuming 4-digit input
    key = keypad.getKey();
    if (key != NO_KEY && isDigit(key)) {
      input += key;
      lcd.setCursor(input.length() - 1, 1);
      lcd.print('*'); // Display * for each digit entered
    }
  }
  delay(500); // Adjust delay as needed
  return input;
}
