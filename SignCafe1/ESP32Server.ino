//WiFiServer
//サーバー側(SoftAP)
/*
GPIO32にLEDをつないでください（LEDなしでも大丈夫です）
*/

#include<WiFi.h>

const char* ssid     = "YourSSID"; //ご自分のWi-Fi IDに書き換えてください
const char* password = "YourPassword"; //ご自分のパスワードに書き換えてください
const IPAddress Serverip(192,168,10,10);  //サーバーのIPアドレス
const IPAddress Gateway(192, 168, 10, 1);//ゲートウェイ
const IPAddress Subnet(255,255,255,0); //サブネットマスク
WiFiServer server(80);  //サーバーポート80

const int LED1 = 32; //LED

void setup()
{
 pinMode(LED1, OUTPUT); //WHITE LED
 pinMode(LED_BUILTIN, OUTPUT); //内蔵LED

//WiFi接続設定///////////////////////////////////////////////////

 ConnectWiFi();  //サーバー側 SoftAP Wi-Fi接続


//WiFi接続設定///////////////////////////////////////////////////

}


 void ConnectWiFi(){  //softAPでWi-Fi接続     
     Serial.begin(115200);
     WiFi.softAP(ssid,password);  //SSIDとパスの設定
     delay(1000); //接続失敗防止
     WiFi.softAPConfig(Serverip,Gateway,Subnet); //WiFi.softAPIP()でWiFi起動
     server.begin(); //サーバーを起動(htmlを表示させるため) 
     Serial.println();

     printWiFiStatus();
 }

 void printWiFiStatus() {
     Serial.println("Connect Success!"); 
     Serial.print("SSID = ");
     Serial.println(ssid);

     IPAddress myIP = WiFi.softAPIP();
     Serial.print("AP IP address = ");
     Serial.println(myIP);
     Serial.println("Server start!");
     digitalWrite(LED_BUILTIN, HIGH);
 }


 //コマンドを受信してそのコマンド文字列を返す
 String rcvCommand(){
     WiFiClient client = server.available();
     String rstr;
     if (client.connected()) {
       Serial.println("Connected to client");

       //コマンド文字列受信（文字列が来なければタイムアウトする）
       rstr = client.readStringUntil('\r');
       Serial.print("[");
       Serial.print(rstr);
       Serial.println("]");

       //応答送信
       if(rstr == "LED1_ON"){
         client.print("オンで受信しました!サーバーより\r");
         Serial.println("オンで受信しました!サーバーより\r");
       } else {
         client.print("オフで受信しました!サーバーより\r");
         Serial.println("オフで受信しました!サーバーより\r");
       }
       //接続をクローズ
       client.stop();
       Serial.println("Closed");
     }
     return rstr;
 }


 void loop()
 {

   String cmd;
   cmd = rcvCommand();

   //受信コマンド
   if(cmd == "LED1_OFF"){
     Serial.print("cmd = ");
     Serial.println(cmd);
     digitalWrite(LED1, LOW);
   }

   if(cmd == "LED1_ON"){
     Serial.print("cmd = ");
     Serial.println(cmd);
     digitalWrite(LED1, HIGH);
   }

 }
