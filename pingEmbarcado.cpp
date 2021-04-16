#include "pingEmbarcado.h"

unsigned long 	sendPingDelay = 0,
				realTime = 0;

bool sendDataFirebase = false;


uint16_t count = 0;


void setup() {

	Serial.begin(57600);

	pinMode(LED_2, OUTPUT);

	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	Serial.print("Connecting to Wi-Fi");
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(300);
	}
	Serial.println();
	Serial.print("Connected with IP: ");
	Serial.println(WiFi.localIP());
	Serial.println();
	WiFi.setAutoReconnect(true);
}

void updatePin(uint8_t pin, uint8_t val){
	digitalWrite(pin, val);
}

void loop() {

	realTime = millis();

	if (realTime - sendPingDelay > 15000) {


		digitalWrite(LED_2, !digitalRead(LED_2));

		sendPingDelay = millis();
	}
}
