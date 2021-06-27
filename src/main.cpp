#include <AceButton.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <AsyncPing.h>
#include <Ticker.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "wificonfig.h"
using namespace ace_button;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// uncomment to debug
#define SCREAM_DEBUG

// init I2C SSD1306 4-wire display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   
byte displayCount = 0;

// button configs
ButtonConfig rebootConfig;

// buttons
AceButton rebootButton(&rebootConfig);

// time me
Ticker timer;

// ping me
AsyncPing pings[3];
IPAddress addrs[3];

// wifi
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

// whomst shall i pingst
const char *hosts[3] = {PING_HOSTS};

// pins
const int RESET_PIN = D6;
const int buzzer = D5;

// globals

bool last_ping[3] = { true, true, true };
bool net_fail = false;

int win_count = 0;
int lose_count = 0;

// prototypes
void displayClear();
void displaySad();
void handleRebootEvent(AceButton*, uint8_t, uint8_t);
void pingu();

void displayClear() {
    display.clearBuffer();
    display.sendBuffer();
}

void displaySad() {
    display.clearBuffer();
    display.setFont(u8g2_font_open_iconic_www_4x_t);
    display.drawGlyph(21, 50, 0x51);
    display.drawGlyph(67, 50, 0x4a);
    display.sendBuffer();
}

void handleRebootEvent(AceButton*, uint8_t eventType, uint8_t) {
    switch (eventType) {
        case AceButton::kEventLongPressed:
            tone(buzzer, 500, 100);
            delay(150);
            tone(buzzer, 500, 100);
            delay(150);
            tone(buzzer, 500, 300);
            delay(300);
            break;
    }
}

void pingu() {
    bool allFalse = false;
    for (int i = 0; i < 3; i++) {
        if (last_ping[i]) {
            allFalse = false;
            lose_count = 0;
            if (win_count > 32766) {
                win_count = 3;
            } else {
                win_count++;
            }
            break;
        } else {
            allFalse = true;
            win_count = 0;
            if (lose_count > 32766) {
                lose_count = 18;
            } else {
                lose_count++;
            }
        }
    }

    if ( net_fail ) {
        if ( win_count >= 3 ) {
            net_fail = false;
            displayClear();
        }
    } else {
        if ( lose_count ) {
            if (allFalse) {
                if ( lose_count >= 18) {
                    net_fail = true;
                    displaySad();
                } else {
                }
            }
        } else {
        }
    }
    for (int i = 0; i < 3; i++) {
        pings[i].begin(hosts[i], 1);
    }
}

void setup() {
    // give me your face
    display.begin();
    displayClear();
    
    // PUSH ME
    rebootConfig.setEventHandler(handleRebootEvent);
    rebootConfig.setFeature(ButtonConfig::kFeatureLongPress);
    pinMode(RESET_PIN, INPUT);
    rebootButton.init(RESET_PIN, LOW);

    // buzz buzz motherfucker
    pinMode(buzzer, OUTPUT);

    // let's get going
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    for (int i = 0; i < 3; i++) {
        if (hosts[i]) {
            if (!WiFi.hostByName(hosts[i], addrs[i]))
                addrs[i].fromString(hosts[i]);
        }
        pings[i].on(true, [i](const AsyncPingResponse& response) {
            if (response.answer) {
                last_ping[i] = true;
            } else {
                last_ping[i] = false;
            }
            return false;
        });

        pingu();
        timer.attach(PING_DELAY, pingu);
    }
}

void loop() {
    if ( net_fail ) {
       rebootButton.check();
    }
}