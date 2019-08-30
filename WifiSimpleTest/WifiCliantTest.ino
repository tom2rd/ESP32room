/*クライアント側*/
#include<WiFi.h>
const char *ssid="ESP32-WiFi"; //サーバーのSSID
const char *pass="esp32wifi"; //サーバーのパスワード
static WiFiClient client; //WiFiClient型でclientと宣言

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid,pass);  //サーバーに接続
  Serial.printf("\n");
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.printf("\n");
  Serial.println("WiFi Connected");
  IPAddress ip(192,168,0,9);  //サーバーのIPアドレス
  client.connect(ip,80);  //IPアドレスとポート番号を指定して接続
}
//単純に一文字送っています
const char *value="L";
void loop()
{
  delay(1000);
  if(client.connected()==true)
  {
    client.write(value,1);

    //Serial.println(value);


  }
}
