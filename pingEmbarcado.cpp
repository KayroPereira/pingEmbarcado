#include "pingEmbarcado.h"

#define DELAY_PING_REPEAT	10000

const String pathPing[] = {"www.google.com", "undefinedname"};

Pinger pinger;

uint8_t control = 0;
long timeReference = 0;
bool repeatPing = true;

void onReceivePing(){

  pinger.OnReceive([](const PingerResponse& response){
	if (response.ReceivedResponse){
	  Serial.printf("Reply from %s: bytes=%d time=%lums TTL=%d\n",
		response.DestIPAddress.toString().c_str(),
		response.EchoMessageSize - sizeof(struct icmp_echo_hdr),
		response.ResponseTime,
		response.TimeToLive);
	}
	else{
	  Serial.printf("Request timed out.\n");
	}

	// Return true to continue the ping sequence.
	// If current event returns false, the ping sequence is interrupted.
	return true;
  });
}

void onEndPing(){
	pinger.OnEnd([](const PingerResponse& response){
		// Evaluate lost packet percentage
		float loss = 100;
		if(response.TotalReceivedResponses > 0){
		  loss = (response.TotalSentRequests - response.TotalReceivedResponses) * 100 / response.TotalSentRequests;
		}

		// Print packet trip data
		Serial.printf("Ping statistics for %s:\n",
		  response.DestIPAddress.toString().c_str());
		Serial.printf("    Packets: Sent = %lu, Received = %lu, Lost = %lu (%.2f%% loss),\n",
		  response.TotalSentRequests,
		  response.TotalReceivedResponses,
		  response.TotalSentRequests - response.TotalReceivedResponses,
		  loss);

		// Print time information
		if(response.TotalReceivedResponses > 0){
		  Serial.printf("Approximate round trip times in milli-seconds:\n");
		  Serial.printf("    Minimum = %lums, Maximum = %lums, Average = %.2fms\n",
			response.MinResponseTime,
			response.MaxResponseTime,
			response.AvgResponseTime);
		}

		// Print host data
		Serial.printf("Destination host data:\n");
		Serial.printf("    IP address: %s\n",
		  response.DestIPAddress.toString().c_str());

		if(response.DestMacAddress != nullptr){
		  Serial.printf("    MAC address: " MACSTR "\n",
			MAC2STR(response.DestMacAddress->addr));
		}

		if(response.DestHostname != ""){
		  Serial.printf("    DNS name: %s\n",
			response.DestHostname.c_str());
		}

		repeatPing = true;
		return true;
	});
}

void setup() {

	Serial.begin(57600);

	pinMode(LED_2, OUTPUT);

	WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	Serial.printf("\n\nConnecting to Wi-Fi");
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");
		delay(300);
	}
	Serial.println();
	Serial.print("Connected with IP: ");
	Serial.println(WiFi.localIP());
	Serial.println();
	WiFi.setAutoReconnect(true);

	onReceivePing();
	onEndPing();
}

void updatePin(uint8_t pin, uint8_t val){
	digitalWrite(pin, val);
}

void loop() {

	if(millis() - timeReference > DELAY_PING_REPEAT){

		digitalWrite(LED_2, !digitalRead(LED_2));

		timeReference = millis();
		if(repeatPing){

			repeatPing = false;

			Serial.printf("\n\nControl = %d", control);
			switch(control++){
			case 0:
			case 1:
				// Ping path array
				Serial.printf("\n\n%s\n", pathPing[control-1].c_str());
				if(pinger.Ping(pathPing[control-1].c_str()) == false){
					Serial.println("Error during ping command.");
					repeatPing = true;
				}
				break;

			case 2:
				// Ping default gateway
				Serial.printf("\n\nPinging default gateway with IP %s\n", WiFi.gatewayIP().toString().c_str());
				if(pinger.Ping(WiFi.gatewayIP()) == false){
					Serial.println("Error during last ping command.");
					repeatPing = true;
				}
				break;

			case 3:
				  // Ping invalid ip
				  Serial.printf("\n\nPinging invalid ip 1.2.3.4\n");
				  if(pinger.Ping(IPAddress(1,2,3,4)) == false){
					Serial.println("Error during ping command.");
					repeatPing = true;
				  }
				break;

			default:
				Serial.printf("\n\n%s\n", pathPing[0].c_str());
				if(pinger.Ping((pathPing[0].c_str())) == false){
					Serial.println("Error during ping command.");
					repeatPing = true;
				}

				if(control >= 5){
					control = 0;
				}
			}
		}
	}
}
