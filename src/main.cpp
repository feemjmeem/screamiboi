#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <AsyncPing.h>
#include <Ticker.h>
#include "wificonfig.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

const char* remote_host = PING_HOST;

const int RED_LED = D0;

bool last_ping = false;

Ticker timer;
AsyncPing ping;

void pingu() {
    ping.begin(remote_host);
}

void setup() {
    Serial.begin(115200);
    delay(10);

    pinMode(RED_LED, OUTPUT);

    Serial.printf("\nConnecting...");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    
    Serial.printf("Connected!\nscreamiboi is %s. ALL SHALL LOVE ME AND DESPAIR!\n",
                  WiFi.localIP().toString().c_str());
    Serial.printf("Pinging host %s...", remote_host);

    ping.on(true,[](const AsyncPingResponse& response){
        IPAddress addr(response.addr);
        if (response.answer) {
            if (last_ping == false) {
                Serial.printf("\nConnected! Monitoring %s...", remote_host);
            } else {
                Serial.printf(".");
            }
            last_ping = true;
            digitalWrite(RED_LED, HIGH);
        } else {
            if (last_ping == true) {
                Serial.printf("\nConnection lost! Waiting for recovery...");
            } else {
                Serial.printf(".");
            }
            last_ping = false;
            digitalWrite(RED_LED, LOW);
        }
        return false;
    });
    pingu();
    timer.attach(PING_DELAY,pingu);
}

void loop() {
}