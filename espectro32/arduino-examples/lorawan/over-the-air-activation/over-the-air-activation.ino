#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

#include "lorawan_id_utils.h"
#include "lorawan_config.h"
#include "utils.h"

#define LORA_HAT_PIN_NSS_B 33
#define LORA_HAT_RST 27
#define LORA_HAT_DIO0 13
#define LORA_HAT_DIO1 14

#define MESSAGE_TYPE_CONFIRMED 1

#define MESSAGE_TYPE_UNCONFIRMED 0

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
void os_getArtEui(u1_t *buf)
{
    memset(buf, 0, 8);
}

// This should also be in little endian format, see above.
void os_getDevEui(u1_t *buf)
{
    lorawanConvertDeviceEUIStr(DEVICE_LRWAN_DEVEUI, buf);
}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
void os_getDevKey(u1_t *buf)
{
    lorawanConvertAppKeyStr(DEVICE_LRWAN_APPKEY, buf);
}

// transmission data buffer and buffer size
static size_t txBufferSize = 0;
static uint8_t txBuffer[11] = {0};

static osjob_t sendjob;
void do_send(osjob_t *j);

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
static const unsigned TX_INTERVAL = 60;

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = LORA_HAT_PIN_NSS_B,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = LORA_HAT_RST,
    .dio = {LORA_HAT_DIO0, LORA_HAT_DIO1, LMIC_UNUSED_PIN},
};

void onEvent(ev_t ev)
{
    Serial.print(os_getTime());
    Serial.print(": ");
    switch (ev)
    {
    case EV_SCAN_TIMEOUT:
        Serial.println(F("EV_SCAN_TIMEOUT"));
        break;
    case EV_BEACON_FOUND:
        Serial.println(F("EV_BEACON_FOUND"));
        break;
    case EV_BEACON_MISSED:
        Serial.println(F("EV_BEACON_MISSED"));
        break;
    case EV_BEACON_TRACKED:
        Serial.println(F("EV_BEACON_TRACKED"));
        break;
    case EV_JOINING:
        Serial.println(F("EV_JOINING"));
        break;
    case EV_JOINED:
        Serial.println(F("EV_JOINED"));
        {
            u4_t netid = 0;
            devaddr_t devaddr = 0;
            u1_t nwkKey[16];
            u1_t artKey[16];
            LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
            Serial.print("netid: ");
            Serial.println(netid, DEC);
            Serial.print("devaddr: ");
            Serial.println(devaddr, HEX);
            Serial.print("AppSKey: ");

            printHexArray(artKey, sizeof(artKey));

            Serial.print("NwkSKey: ");

            printHexArray(nwkKey, sizeof(nwkKey));
        }
        // Disable link check validation (automatically enabled
        // during join, but because slow data rates change max TX
        // size, we don't use it in this example.
        LMIC_setLinkCheckMode(0);
        break;
    /*
    || This event is defined but not used in the code. No
    || point in wasting codespace on it.
    ||
    || case EV_RFU1:
    ||     Serial.println(F("EV_RFU1"));
    ||     break;
    */
    case EV_JOIN_FAILED:
        Serial.println(F("EV_JOIN_FAILED"));
        break;
    case EV_REJOIN_FAILED:
        Serial.println(F("EV_REJOIN_FAILED"));
        break;
    case EV_TXCOMPLETE:
        Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
        if (LMIC.txrxFlags & TXRX_ACK)
            Serial.println(F("Received ack"));
        if (LMIC.dataLen)
        {
            Serial.print(F("Received "));
            Serial.print(LMIC.dataLen);
            Serial.println(F(" bytes of payload"));
        }
        // Schedule next transmission
        os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
        break;
    case EV_LOST_TSYNC:
        Serial.println(F("EV_LOST_TSYNC"));
        break;
    case EV_RESET:
        Serial.println(F("EV_RESET"));
        break;
    case EV_RXCOMPLETE:
        // data received in ping slot
        Serial.println(F("EV_RXCOMPLETE"));
        break;
    case EV_LINK_DEAD:
        Serial.println(F("EV_LINK_DEAD"));
        break;
    case EV_LINK_ALIVE:
        Serial.println(F("EV_LINK_ALIVE"));
        break;
    /*
    || This event is defined but not used in the code. No
    || point in wasting codespace on it.
    ||
    || case EV_SCAN_FOUND:
    ||    Serial.println(F("EV_SCAN_FOUND"));
    ||    break;
    */
    case EV_TXSTART:
        Serial.println(F("EV_TXSTART"));
        break;
    case EV_TXCANCELED:
        Serial.println(F("EV_TXCANCELED"));
        break;
    case EV_RXSTART:
        /* do not print anything -- it wrecks timing */
        break;
    case EV_JOIN_TXCOMPLETE:
        Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
        break;

    default:
        Serial.print(F("Unknown event: "));
        Serial.println((unsigned)ev);
        break;
    }
}

void do_send(osjob_t *j)
{
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND)
    {
        Serial.println(F("OP_TXRXPEND, not sending"));
    }
    else
    {
        // TODO: set transmission data
        // if not set, the data will be the following array:
        txBuffer[txBufferSize++] = 21;
        txBuffer[txBufferSize++] = 12;

        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, txBuffer, txBufferSize, MESSAGE_TYPE_UNCONFIRMED);
        // LMIC_setTxData2(1, txBuffer, txBufferSize, MESSAGE_TYPE_CONFIRMED);
        Serial.println(F("Packet queued"));
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void setup()
{
    Serial.begin(115200);
    Serial.println(F("Starting"));

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    LMIC_setAdrMode(true);

    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
}

void loop()
{
    os_runloop_once();
}
