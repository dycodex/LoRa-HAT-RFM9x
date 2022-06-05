#include <Arduino.h>
#include <SPI.h>

// IMPORTANT!!
// Install this library first: https://github.com/sandeepmistry/arduino-LoRa
#include <LoRa.h>

int counter = 0;

static constexpr auto LORA_FREQUENCY = 921800000; // frequency should match the one in the receiver
static constexpr auto LORA_SS_PIN = 33;
static constexpr auto LORA_RESET_PIN = 27;
static constexpr auto LORA_DIO0_PIN = 13;

static constexpr auto LORA_BANDWIDTH = 125000;   // in Hertz
static constexpr auto LORA_SPREADING_FACTOR = 7; // SF?
static constexpr auto LORA_CODING_RATE = 5;      // 4/?
static constexpr auto LORA_SYNC_WORD = 0x7F;     // sync word should match the one in the LoRa receiver

static constexpr auto APP_SEND_INTERVAL = 30000; // in milliseconds

void setup()
{
    Serial.begin(9600);

    // relax, delay for three seconds
    delay(3000);

    LoRa.setPins(LORA_SS_PIN, LORA_RESET_PIN, LORA_DIO0_PIN);

    if (!LoRa.begin(LORA_FREQUENCY))
    {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }

    LoRa.setSyncWord(LORA_SYNC_WORD);
    LoRa.setSpreadingFactor(LORA_SPREADING_FACTOR);
    LoRa.setSignalBandwidth(LORA_BANDWIDTH);
    LoRa.setCodingRate4(LORA_CODING_RATE);
}

uint32_t lastSendMillis = 0;

void loop()
{
    if (lastSendMillis > 0 && millis() - lastSendMillis < APP_SEND_INTERVAL)
    {
        // not time to send yet
        return;
    }

    lastSendMillis = millis();

    Serial.print("Sending packet: ");
    Serial.println(counter);

    // send packet
    LoRa.beginPacket();
    LoRa.print("hello ");
    LoRa.print(counter);
    LoRa.endPacket();

    counter++;
}