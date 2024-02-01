#include <SPI.h>
#include <MFRC522.h>

#include <PubSubClient.h>
#include <WiFi.h>

// #define MQTT_SERVER "9d857b132785452498f2e907a2adcfe2.s1.eu.hivemq.cloud"
#define MQTT_SERVER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_USER "hivemq.webclient.1702680632350"
#define MQTT_PASSWORD "3QnKD!?Za&i182.SkGbj"

//SSID of network
// char ssid[] = "Phong";
// char ssid[] = "P304/109";
// char ssid[] = "Ngoc Quan";
// char ssid[] = "QuanDN";
char ssid[] = "Coffee later? inside";


//password of WPA Network
// char pass[] = "hoiitthoi";
// char pass[] = "12345678@";
// char pass[] = "Hoiitthoi";
// char pass[] = "daothithuy";
char pass[] = "khonggianyentinh";


#define RFID_SS 5    // ESP32 pin GPIO5
#define RFID_RST 15  // ESP32 pin GPIO15
#define RFID_D0 19   // ESP32 pin GPIO19

#define red 12
#define green 13
#define buzzer 14

MFRC522 rfid(RFID_SS, RFID_RST);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(buzzer, OUTPUT);
  noTone(buzzer);

  SPI.begin();      // init SPI bus
  rfid.PCD_Init();  // init MFRC522

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());

  client.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop() {
  digitalWrite(red, HIGH);
  digitalWrite(green, LOW);

  client.loop();
  if (rfid.PICC_IsNewCardPresent()) {  // new tag is available
    if (rfid.PICC_ReadCardSerial()) {  // NUID has been readed
      digitalWrite(red, LOW);
      digitalWrite(green, HIGH);
      int i = 2;
      while (i--) {
        // tone(buzzer, 262, 30);
        tone(buzzer, 262, 200);
        delay(50);
        noTone(buzzer);
        delay(50);
      }
      delay(100);
      digitalWrite(green, LOW);
      digitalWrite(red, HIGH);

      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.print("RFID/NFC Tag Type: ");
      Serial.println(rfid.PICC_GetTypeName(piccType));

      // print UID in Serial Monitor in the hex format
      String content = "";
      Serial.print("UID: ");
      for (int i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
        Serial.print(rfid.uid.uidByte[i], HEX);
        content.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : ""));
        content.concat(String(rfid.uid.uidByte[i], HEX));
      }
      content.toUpperCase();
      // client.publish("rfid/uid", content.c_str());

      Serial.println();

      rfid.PICC_HaltA();       // halt PICC
      rfid.PCD_StopCrypto1();  // stop encryption on PCD

      // if (!client.connected()) {
      //   // Loop until we’re reconnected
      //   while (!client.connected()) {
      //     Serial.print("Attempting MQTT connection…");
      //     String clientId = content;
      //     // Attempt to connect
      //     // Insert your password
      //     if (client.connect(clientId.c_str() ,MQTT_USER, MQTT_PASSWORD)) {
      //       Serial.println("connected");
      //       // Once connected, publish an announcement…
      //       client.publish("rfid/uid", content.c_str());
      //       // … and resubscribe
      //       // client->subscribe("testTopic");
      //     } else {
      //       Serial.print("failed, rc = ");
      //       Serial.print(client.state());
      //       Serial.println(" try again in 1 seconds");
      //       // Wait 1 seconds before retrying
      //       delay(1000);
      //     }
      //   }
      // }
      // client.loop();
      Serial.print("Attempting MQTT connection…");
      String clientId = content;
      // Attempt to connect
      // Insert your password
      if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
        Serial.println("connected");
        // Once connected, publish an announcement…
        client.publish("rfid/uid", content.c_str());
        // … and resubscribe
        // client->subscribe("testTopic");
      } else {
        Serial.print("failed, rc = ");
        Serial.print(client.state());
        Serial.println(" try again in 1 seconds");
        // Wait 1 seconds before retrying
        delay(1000);
      }
    }
  }
  digitalWrite(green, LOW);
  digitalWrite(red, HIGH);
}
