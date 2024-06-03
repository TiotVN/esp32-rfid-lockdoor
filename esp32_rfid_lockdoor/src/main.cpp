#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>

#define SS_PIN 5
#define RST_PIN 0
#define PIN_SG90 27

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
byte nuidPICC[4] = {0x60, 0x31, 0x48, 0x12};
byte nuidPICC_1[4] = {0x60, 0x31, 0x48, 0x12};
Servo sg90;

void printHex(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

void printDec(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], DEC);
    }
}

void motor()
{
    sg90.write(180);
    delay(5000);
    sg90.write(0);
}

void setup()
{
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();
    sg90.setPeriodHertz(50);         
    sg90.attach(PIN_SG90, 500, 2400);

    for (byte i = 0; i < 6; i++)
    {
        key.keyByte[i] = 0xFF;
    }

    Serial.println(F("This code scan the MIFARE Classsic NUID."));
    Serial.print(F("Using the following key:"));
    printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void loop()
{
    if (!rfid.PICC_IsNewCardPresent())
    {
        return;
    }

    if (!rfid.PICC_ReadCardSerial())
    {
        return;
    }

    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    Serial.println(rfid.PICC_GetTypeName(piccType));

    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
        piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
        piccType != MFRC522::PICC_TYPE_MIFARE_4K)
    {
        Serial.println(F("Your tag is not of type MIFARE Classic."));
        return;
    }

    if (rfid.uid.uidByte[0] == nuidPICC[0] &&
        rfid.uid.uidByte[1] == nuidPICC[1] &&
        rfid.uid.uidByte[2] == nuidPICC[2] &&
        rfid.uid.uidByte[3] == nuidPICC[3])
    {
        Serial.println(F("Open the door."));

        Serial.println(F("The NUID tag is:"));
        Serial.print(F("In hex: "));
        printHex(rfid.uid.uidByte, rfid.uid.size);
        Serial.println();
        Serial.print(F("In dec: "));
        printDec(rfid.uid.uidByte, rfid.uid.size);
        Serial.println();

        motor();
    }
    else
        Serial.println(F("Can't open the door"));

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}