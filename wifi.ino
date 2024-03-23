#include <ESP8266WiFi.h>;
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <time.h>
#include <PubSubClient.h>;
#include <NTPClient.h>;
#include <WiFiUdp.h>;
#include <AESLib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "FS.h"
#include <Base64.h>
#include <string>
#include <cstdint> 


AESLib aesLib;
uint8_t Akey[] = {0x4E, 0x5A, 0x1B, 0xE2, 0x78, 0xDE, 0x08, 0x9A, 0x89, 0x0D, 0x6E, 0x9C, 0x05, 0x85, 0x99, 0x90}; // 128-bit key


#define TIME_ZONE -5
#define THINGNAME "ESP8266"
#define AWS_IOT_PUBLISH_TOPIC   "esp8266/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp8266/sub"

const char* ssid = "GioNet";
const char* password = "shish9999";
const char MQTT_HOST[] = "ad6k0u45kocwb-ats.iot.us-east-1.amazonaws.com";
uint8_t ivAES[16] = {0x8C, 0x0A, 0x15, 0x8E, 0xD8, 0x36, 0xC8, 0x24, 0xB6, 0x06, 0x7A, 0x95, 0x4F, 0xF3, 0x06, 0x4F};


static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";
 
 
// Copy contents from XXXXXXXX-certificate.pem.crt here
static const char client_cert[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUVEZWKYxTPx/x3+PrYRnV/rIJ7P4wDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI0MDIxNDE0MTI1
NloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAM2bw6BRAqr2NJzMpSN4
rDPQOdDTwGsV72lxrx+C85eFec2rXtS6AyEXx5dL4R+dEw+Zn+rjhk+K7oujEuCB
HIV9kd6kb6w300WcTVOS/c9FzeiH+XdbHgMgRKMpmFNRYylUEtfsE16lxAE0ENH/
nLSFGfWremXiqHv9v8PbREeUy/c3Y4Ds1CCQS7xruRMX0iL2LNL6W86aWrznhH50
M8DLGgMXeMJPfvnbUmWT3PKMAxuap1aGGT5m27FQwDpI8SyK2VPtoK0Ek/Xe7z2P
iimTgWjyEom7X/bUgnjx11jiHSUdMmf31Z4gnYsIoCY6BgWwOA4N1xhyLwFd/GhP
r5ECAwEAAaNgMF4wHwYDVR0jBBgwFoAU7ePNj9skzUChZOOWM5JryN0uLF8wHQYD
VR0OBBYEFPljaJZh4dQm/CyOgQ7Kum73O2WSMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBtwoUKoamhVgGEXoS9hrJI0chk
xuGPGtqHN1X65x1RIfeU3RpeBMooo2hbsEvn+W5YjAmcOugVPlvPc3EyTNnEiDvR
oixmZ5og+35l4+5mibD/FhIunoiZIuSXcnIn4SbdNHJtEmL8Xzyc4Bn67zvsHTxX
J5m2qg+5RoHbCRyP/HkZYCS6Zd63t4mvXEVAKsXr+mZX1BubEAoo1eMISdEu3ADE
X0V4/5HrHasyCV1qcI/TWYCJbDX2dzQBofAvf3/v5QpP7dEJ67oDttpS35urYMqT
IpOiKlLAyaTqNTOAnQBCjAfsWqP8xB4e8jHn+FhZ/sHGXWgjzkBMvkEh6NgA
-----END CERTIFICATE-----
 
)KEY";
 
 
// Copy contents from  XXXXXXXX-private.pem.key here
static const char privkey[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAzZvDoFECqvY0nMylI3isM9A50NPAaxXvaXGvH4Lzl4V5zate
1LoDIRfHl0vhH50TD5mf6uOGT4rui6MS4IEchX2R3qRvrDfTRZxNU5L9z0XN6If5
d1seAyBEoymYU1FjKVQS1+wTXqXEATQQ0f+ctIUZ9at6ZeKoe/2/w9tER5TL9zdj
gOzUIJBLvGu5ExfSIvYs0vpbzppavOeEfnQzwMsaAxd4wk9++dtSZZPc8owDG5qn
VoYZPmbbsVDAOkjxLIrZU+2grQST9d7vPY+KKZOBaPISibtf9tSCePHXWOIdJR0y
Z/fVniCdiwigJjoGBbA4Dg3XGHIvAV38aE+vkQIDAQABAoIBAFopmzC7NaNI0iuM
ZkJW0ZTrE99qbC/Jb7JaowP7hdyFMLOrGavJYkPpzKGhKbiWRdyKbqEvYx+QeikL
uuRuWdaECSqMC1iHJoOZFiO3CswQ/UmiDxhzAYc3aEHwpJas0wI1lwnOwXO412n7
rgCYydwznf2oU8VIDsBItwYpKhkE8uIcivGTkghfgdjio7bSiD/x36I/wKtpmSEc
QYf1+EcDTkWFwpMqWwyPhNO7udS9WiCFWJ0G8SSSIBzsR8SUjYAc65bpeKlyG8sF
YZ76nCH4/qJBZtzWcLRfowxzfmManZk08CPdz747GABLFfUGsPiPRdvSkKfZJwSH
jwbPygECgYEA6xIJ4xJ0pEZ240PI2HFbvmM1DFbgoUNCGU4Okk9XYs9A756ZmkuM
vXkVHQkF3AQduZZa0N1A5h0U9LfnsYy9DucWT4tMT9LRnV1eVDVeqMmuIRZ30p76
9tSYFpR2P7q8h/JGQJ3ez6JhZwMx+iteGiToE56PE57aeQLLydbcNCECgYEA3+oy
qu3qxVeofc1Dtefzy2VgDc3aOdyEzGfOkBJzrHCPIjLtu1LPRzxkrIlB/2UkkVqR
bn/CVZk+HvWF6Y03ojNnhIndlKHXfMWne3NnEvxQp+cV1yRPkkDrkiaJsdY9Kg7E
cKM7z8eBH7bjclKl7DAF5WT9OQSeIC199EOXDXECgYEAxdFNTrCmReQigkf1yedD
TGmSJ8soh6zhAAiHDKsGH9nHjJ6MJqMzUfOGSAPi0W8JULdtv1yiE8QfcX0d1nVY
WCUxykiwNHB2A5aiHf03ppTqG2Ab/7vX+EWZo07WX/UWWNUSN6hi65p7m0O+GAfM
zfWG85O4VcuVkMZAlp9geeECgYBkwYu/IoPL/R/SS/U1+4PWeG16iLCz7KjNGcD3
T3ZBl/ywVN5otmCTG3fps3HBLJAB4BEk+dRGlEhBwhVZ/LOWBy+Dkk3kvdarTH2K
rWkqP+NDSlankq1DkHbwMusu3Mp1DGQUkz8sxgcycCd0oAANgaaFjKXtbEiDDjVp
Hqjv4QKBgHLAwzcaieuGYN7Q3bc2PTQCBw1gtOXyBiocpflVUva012TiGL7VEv3j
z8ZLay4am1lfgMaC/wOsk6L53E0qep67w4+EVRJEOgdpe/Dp87Ss81DXPBnZvgMv
c9BtprjxG8ToOIsK3W8Up65Jwzl5VAVbMb76uN+0KotOQsG+oynU
-----END RSA PRIVATE KEY-----
 
)KEY";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
WiFiClientSecure net;
time_t now;
time_t nowish = 1510592825;
unsigned long lastMillis = 0;
 
BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);
 
PubSubClient client(net);

void messageReceived(char *topic, byte *payload, unsigned int length)
{
  //Serial.print("Received [");
  //Serial.print(topic);
  //Serial.print("]: ");
  for (int i = 0; i < length; i++)
  {
    //Serial.print((char)payload[i]);
  }
  //Serial.println();
}

void insertAtBeginning(char arr[], int n, char val) {
    if (n > 0) {
        for (int i = n; i > 0; i--) {
            arr[i] = arr[i - 1];
        }
    }
    arr[0] = val;
}

void publishMessage(const byte *password) {
  size_t length = strlen((char*)password);
  std::string str(reinterpret_cast<const char*>(password), length);
 
  StaticJsonDocument<512> doc;
  doc["password"] = str;
  
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}


void NTPConnect(void)
{
  //Serial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, 0 * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    //Serial.print(".");
    now = time(nullptr);
  }
  //Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  //Serial.print("Current time: ");
  //Serial.print(asctime(&timeinfo));
}

void setup_wifi() {
  delay(10);
  
  net.setBufferSizes(1024, 1024);
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.print("WiFi connected\n");
  //Serial.print("IP address: ");
  //Serial.print(WiFi.localIP());
  //Serial.print("\n");
}

void connectAWS()
{
  delay(3000);
  setup_wifi();
 
  NTPConnect(); //funzione timezone
 
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);

  //Serial.println("Set the anchors");
 
  client.setServer(MQTT_HOST, 8883);
  client.setCallback(messageReceived);
 
  //Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    //Serial.print(".");
    delay(1000);
  }
 
  if (!client.connected()) {
    //Serial.println("AWS IoT Timeout!");
    return;
  }
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  //Serial.println("AWS IoT Connected!");
}

bool isByteArrayEmpty(unsigned char arr[], size_t size) {
  for (size_t i = 0; i < size; i++) {
    if (arr[i] != 0) {
      return false;
    }
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  connectAWS();
}

void loop() {
  byte encrypted_data[16];
  byte decrypted_data[16];
  
  if (Serial.available()) {
    Serial.readBytes(encrypted_data, sizeof(encrypted_data));
    uint16_t dec_bytes = aesLib.decrypt(encrypted_data, 16, decrypted_data, Akey, 128, ivAES);
  }
  

  if (!client.connected()){
    connectAWS();
    //Serial.print("WiFi connected\n");
  } else {
    client.loop();
    if (millis() - lastMillis > 5000)
    {
      lastMillis = millis();
      publishMessage(decrypted_data);
    }
  }
}
