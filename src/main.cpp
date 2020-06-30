#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <AsyncPing.h>
#include <Ticker.h>
#include "wificonfig.h"

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

const char *hosts[3] = {PING_HOSTS};

const int RED_LED = D7; // add an external led

bool last_ping[3] = { true, true, true };
bool net_fail = false;

int win_count = 0;
int lose_count = 0;

Ticker timer;
AsyncPing pings[3];
IPAddress addrs[3];

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
                lose_count = 9;
            } else {
                lose_count++;
            }
        }
    }
    if (allFalse) {
        if ( !net_fail && lose_count >= 9) {
            Serial.printf("\nConnection down! Monitoring...");
            digitalWrite(RED_LED, HIGH);
            net_fail = true;
        }
    } else {
        if ( net_fail && win_count >= 3) {
            Serial.printf("\nConnection up! Monitoring...");
            digitalWrite(RED_LED, LOW);
            net_fail = false;
        }
    }
    for (int i = 0; i < 3; i++) {
        pings[i].begin(hosts[i], 1);
    }
}

void setup() {
    Serial.begin(115200);
    delay(10);

    pinMode(RED_LED, OUTPUT);
    digitalWrite(RED_LED, HIGH);

    Serial.printf("\nConnecting...");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }

    Serial.printf("Connected!\nscreamiboi is %s. ALL SHALL LOVE ME AND DESPAIR!\n",
                  WiFi.localIP().toString().c_str());
    Serial.printf("Pinging hosts...");
    digitalWrite(RED_LED, LOW);

    for (int i = 0; i < 3; i++) {
        if (hosts[i]) {
            if (!WiFi.hostByName(hosts[i], addrs[i]))
                addrs[i].fromString(hosts[i]);
        }
        pings[i].on(true, [i](const AsyncPingResponse& response) {
            if (response.answer) {
                Serial.printf("!");
                last_ping[i] = true;
            } else {
                Serial.printf(".");
                last_ping[i] = false;
            }
            return false;
        });

        pingu();
        timer.attach(PING_DELAY, pingu);
    }
}

void loop() {}
