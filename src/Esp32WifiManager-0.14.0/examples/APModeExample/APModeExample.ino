#include <Preferences.h>
#include <WiFi.h>
#include <Esp32WifiManager.h>

//Create a wifi manager
WifiManager manager;
void setup() {
	manager.setupAP();// Always go straight into AP mode without doing a re-scan first
}

void loop() {
	// read the serial port for new passwords and maintain connections
	manager.loop();
	if (manager.getState() == Connected) {
		// use the Wifi Stack now connected and a device is connected to the AP
	}
}
