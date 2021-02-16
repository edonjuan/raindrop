// 1. Verificar si esta instalada la libreria PubSubClient
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// 2. Ingresar las credenciales de la red
const char* ssid = "NOMBRE_DE_LA_RED";
const char* password = "CONTRASEÑA_DE_LA_RED";

// 3. Ingresar las credenciales del servidor
const char* mqtt_server = "NOMBRE_DEL_HOST";
const int mqtt_port = 1883;
const char* mqtt_user = "USUARIO_MQTT";
const char* mqtt_pass = "CONTRASEÑA_MQTT";

// 4. Ingrese el tópico (Código de equipo 4 dígitos) con la data1
// Ejemplo: T205/data1

const char* topic = "T308/data1";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[25];

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String incoming = "";
  String topicStr = topic;
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("mensaje ->" + incoming);

  // Switch on the LED if an 1 was received as first character
  if ((incoming == "on")&&(topicStr == "led1")) {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {
      Serial.println("connected");
      digitalWrite(LED_BUILTIN, !HIGH); 
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      // ... and resubscribe
      //client.subscribe("led1");
      //client.subscribe("led2");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  
  // LedTest
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, !LOW); 

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  static int value_x = -1;

  if (!client.connected()) {
    digitalWrite(LED_BUILTIN, !LOW); 
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    //5. EN ESTA PARTE DEL CODIGO LEER EL SENSOR CORRESPONDIENTE
    int value = analogRead(A0); // en el ejemplo se lee un LM35
    if(value != value_x)
    {
      value_x = value;
      float millivolts = (value / 1023.0) * 5000;
      float celsius = millivolts / 10; 

      //celsius.toCharArray(msg,8);
      snprintf (msg, 8, "%f", celsius);
      Serial.print("Publish message: ");
      Serial.println(celsius);
      client.publish(topic, msg);
    }
  }
}
