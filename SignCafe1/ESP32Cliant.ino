//WiFiClient
//クライアント側(固定IP)
/*
GPIO32にLEDをつないでください（LEDなしでも大丈夫です）
*/

#include <WiFi.h>

const char* ssid     = "YourSSID"; //ご自分のWi-Fi IDに書き換えてください
const char* password = "YourPassword"; //ご自分のパスワードに書き換えてください
const IPAddress Serverip(192,168,10,10); //サーバーのIPアドレス
const IPAddress Gateway(192, 168, 10, 1);//ゲートウェイ
const IPAddress Subnet(255,255,255,0); //サブネットマスク
const IPAddress Localip(192,168,10,20);  //クライアントのIPアドレス
const int ServerPort = 80;

const int LED1 = 32; //LED


static WiFiClient client; //WiFiClient型でclientと宣言

void setup()
{
//WiFi接続設定///////////////////////////////////////////////////

 pinMode(LED_BUILTIN, OUTPUT);
 pinMode(LED1, OUTPUT);
 ConnectWiFi();

//WiFi接続設定/////////////////////////////////////////////////// 
}

 void ConnectWiFi(){ //クライアント側Wi-Fi接続
     Serial.begin(115200);

     WiFi.mode(WIFI_STA); //ステーション : WIFI_AP アクセスポイント : WIFI_AP_STA アクセスポイント＋ステーション 
     WiFi.begin(ssid, password);
     WiFi.config(Localip,Gateway,Subnet);
     delay(500);

     int retrycnt = 0;
     while(WiFi.status() != WL_CONNECTED){ //Wi-Fiに接続できなければカウント
       Serial.print(".");
       delay(1000);
       retrycnt++;
       if(retrycnt > 10){
         ESP.restart(); //カウンターが10(10秒)になるまでに接続できなければESP32を再起動
       }
     }
     Serial.println("Connect Success!");
     Serial.print("SSID = ");
     Serial.println(ssid);
     IPAddress myIP = WiFi.localIP(); //IP Address 取得
     Serial.print("myIP = ");
     Serial.println(myIP);
     digitalWrite(LED_BUILTIN, HIGH);
 }

//WiFi接続設定/////////////////////////////////////////////////// 



void loop()
{
 if (WiFi.status() == WL_CONNECTED) {

   myEvent();//myEventの実行


 }else{
   //WiFi接続が切れた場合
   Serial.println("WiFi ERR. Restart.");
   ESP.restart();
 }
 client.stop();
}


//イベント1
int Counter = 0;
void myEvent(){ //３秒毎にONとOFFを交互に送る
 if(Counter == 0){ //はじめてここを通過（Counterが0なら）するなら
   delay(3000); //３秒待ってサーバーへ送信
   sendSocket("LED1_ON\r");
   Counter = 1; //次回はCounterが1になるのでここは通らない
 } else {
   delay(3000); //３秒待ってサーバーへ送信
   sendSocket("LED1_OFF\r");
   Counter = 0; //Counterを0に戻す
 }
}


void sendSocket(String str)
{
 String line;
 client.connect(Serverip, ServerPort);
 if (client.connected() == true) {
   Serial.println("サーバーへ送信 = " + str);
   //送信
   client.print(str); 
   //応答受信
   client.setTimeout(1000);
   do{
     line = client.readStringUntil('\r');
     //Serial.print(line);
   } while (client.available() != 0);  //残りがあるときはさらに受信のためループ

   if(line == "オフで受信しました!サーバーより"){
     Serial.print("Server Message = ");
     Serial.println(line);
     Serial.println();
     digitalWrite(LED1, LOW);
   }

   if(line == "オンで受信しました!サーバーより"){
     Serial.print("Server Message = ");
     Serial.println(line);
     Serial.println();
     digitalWrite(LED1, HIGH);
   }
 }
 else
 {
   Serial.println("サーバーが接続されていません");
   delay(3000);
   //digitalWrite(LED1, LOW);
   ESP.restart();   
 }
}
