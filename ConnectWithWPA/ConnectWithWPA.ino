/*

   This example connects to an unencrypted Wifi network.
   Then it prints the  MAC address of the Wifi shield,
   the IP address obtained, and other network details.

Circuit:
 * WiFi shield attached

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */
#include <WiFi.h>
#include "config.h"

char server[] = "data.sparkfun.com";
char ssid[] = kSSID;
char pass[] = kWifiPassword;

String sparkfunStreamID = kSparkfunStreamID;
String sparkfunPrivateKey = kSparkfunPrivateKey;

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 60*1000;  // delay between updates, in milliseconds


int status = WL_IDLE_STATUS;
int currentSensorStatus;// the Wifi radio's status
WiFiClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  pinMode(2, INPUT_PULLUP);
  pinMode(5, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(6, HIGH);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while(true);
  }

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

  digitalWrite(6, LOW);
}

void loop() {
  // check the network connection once every 10 seconds:
  int sensorVal = digitalRead(2);

  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  //print out the value of the pushbutton

  // Keep in mind the pullup means the pushbutton's
  // logic is inverted. It goes HIGH when it's open,
  // and LOW when it's pressed. Turn on pin 13 when the
  // button's pressed, and off when it's not:



  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if (!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    if (currentSensorStatus != sensorVal){ 
      if (sensorVal == HIGH) {
        Serial.println("connecting to server...");
        if (client.connect(server, 80)) {
          httpRequest("Occupied");
          digitalWrite(9, HIGH);
          digitalWrite(8, LOW);
        }
      } else {
        Serial.println("connecting to server...");

        if (client.connect(server, 80)) {
          httpRequest("Vacant");
          digitalWrite(8, HIGH);
          digitalWrite(9, LOW);
        }
      }

      Serial.println(sensorVal);
      currentSensorStatus = sensorVal;
    }
  }

  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}

// @bathroomStatus Vacant or Occupied
void httpRequest(String bathroomStatus) {
  Serial.println("making HTTP request...");
  // make HTTP GET request to twitter:
  client.println("GET /input/" + sparkfunStreamID + "?private_key=" + sparkfunPrivateKey + "&status=" + bathroomStatus);
  client.println("Host:data.sparkfun.com");
  client.println("Connection:close");
  client.println();
}

void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);

}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5],HEX);
  Serial.print(":");
  Serial.print(bssid[4],HEX);
  Serial.print(":");
  Serial.print(bssid[3],HEX);
  Serial.print(":");
  Serial.print(bssid[2],HEX);
  Serial.print(":");
  Serial.print(bssid[1],HEX);
  Serial.print(":");
  Serial.println(bssid[0],HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption,HEX);
  Serial.println();
}
