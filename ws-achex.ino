#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#define espSerial Serial
String bufClient;
//--------------------function websocket event
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
	String login = "";
	String user = "youruser";		
	String passwd = "yourpassword";
	login = "{\"setID\":\"" + user + "\",\"passwd\":\"" + passwd + "\"}";	//create json text
	switch(type) {
		case WStype_DISCONNECTED:
			espSerial.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED: {
			espSerial.printf("[WSc] Connected to url: %s\n", payload);
			// send login json to server when Connected
			webSocket.sendTXT(login);
		}
		break;
		case WStype_TEXT:
			espSerial.printf("[WSc] get text: %s\n", payload);
			break;
		case WStype_BIN:
			espSerial.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);
			break;
	}
}

void setup() {
	espSerial.begin(115200);
	espSerial.setDebugOutput(true);
	espSerial.println();
	espSerial.println();
	espSerial.println();
	for(uint8_t t = 4; t > 0; t--) {
		espSerial.printf("[SETUP] BOOT WAIT %d...\n", t);
		espSerial.flush();
		delay(1000);
	}

	WiFiMulti.addAP("your SSID", "SSID password");
	//WiFi.disconnect();
	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}
	
	webSocket.begin("ws.achex.ca", 4010, "/");	//server, port, url
	webSocket.onEvent(webSocketEvent);		//catch event
	webSocket.setReconnectInterval(5000);	//reconnect every 5 second
}

void loop() {
	webSocket.loop();
	if (Serial.available()) {
		bufClient = "";
		while (Serial.available()) {
			bufClient += Serial.readString();       
		}
	}
	if (bufClient != ""){
		bufClient.trim();
		webSocket.sendTXT(bufClient);
		bufClient = "";
	}
	//------sending random value every half second
	String ranNum = String(random(100,200));
	String ranChart = "{\"to\":\"jiiotclient\",\"value\":\"" + ranNum + "\"}";	//json text
	webSocket.sendTXT(ranChart);
	delay(500);
	//------sending random value every half second
}
