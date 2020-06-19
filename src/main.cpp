#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include "wificonfig.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

const char* remote_host = PING_HOST;

const int RED_LED = D0;
const int BLUE_LED = D4;

bool last_ping = false;

void setup() {
    Serial.begin(115200);
    delay(10);

    pinMode(RED_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);

    Serial.println();
    Serial.println("Connecting");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }

    Serial.println("Connected!");
    Serial.println();
    Serial.print("screamiboi is ");
    Serial.print(WiFi.localIP());
    Serial.println(". ALL SHALL LOVE ME AND DESPAIR!");

    Serial.print("Pinging host ");
    Serial.print(remote_host);
    Serial.println("...");
}

void loop() {
    if(Ping.ping(remote_host)) {
        if(last_ping != true) {
            Serial.println("Connected!");
            Serial.print("Monitoring connection");
        } else {
            Serial.print(".");
        }
        last_ping = true;
        digitalWrite(BLUE_LED, LOW);
        digitalWrite(RED_LED, HIGH);
    } else {
        if(last_ping == true) {
            Serial.println("Connection lost!");
            Serial.print("Waiting for recovery");
        } else {
            Serial.print(".");
        }
        last_ping = false;
        digitalWrite(BLUE_LED, HIGH);
        digitalWrite(RED_LED, LOW);
    }
    delay(500);
}