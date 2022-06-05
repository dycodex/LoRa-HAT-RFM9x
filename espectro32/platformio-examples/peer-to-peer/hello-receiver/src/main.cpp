#include <Arduino.h>
#include <SPI.h>
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

void loop()
{
    auto packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        // received a packet
        Serial.print("Received packet '");

        // read packet
        while (LoRa.available())
        {
            Serial.print((char)LoRa.read());
        }

        // print RSSI of packet
        Serial.print("' with RSSI ");
        Serial.println(LoRa.packetRssi());
    }
}