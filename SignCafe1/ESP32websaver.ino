//ESP32WEBサーバー
//スマホのブラウザでESP32のLEDをON-OFFするコードです

/*

書き込みボード設定”NODE32S”
GPIO33に白色LED、GPIO32に緑色LEDを接続しておきます
ESP32に書き込み後、シリアルモニタにサーバーのIPが表示されます
シリアルモニタのHost: xxx.xxx.xxx.xxx で表示されたIP アドレスを
スマホのブラウザに入力してサーバーへアクセスしてください
サーバーのWEBページ（LEDのON-OFFボタン）が表示されます

*/


// Load Wi-Fi library
#include <WiFi.h>
#define LED_BUILTIN 2

// Wi-Fi SSID 設定
const char* ssid     = "YourSSID"; //ご自分のWi-Fi IDに書き換えてください
const char* password = "YourPassword"; //ご自分のパスワードに書き換えてください

// サーバーポート設定
WiFiServer server(80);

// HTTPリクエスト
String header;

// ブラウザ上のボタン初期設定をOFF
String LED1State = "OFF"; //WHITE LED
String LED2State = "OFF"; //GREEN LED

// GPIOのピン設定を定数に割り当て
const int LED1 = 32; //WHITE LED 32番ピンに白色LED接続
const int LED2 = 33; //GREEN LED 33番ピンに緑色LED接続

void setup() {
 Serial.begin(115200);
 // GPIOピンの出力設定
 pinMode(LED1, OUTPUT);
 pinMode(LED2, OUTPUT);
 pinMode(LED_BUILTIN, OUTPUT); //内蔵LED

 // 初期設定はLED消灯（点灯はHIGHに設定）
 digitalWrite(LED1, LOW);
 digitalWrite(LED2, LOW);

 // Wi-Fiへ接続
 Serial.print("Connecting to "); //シリアルモニタへ出力（改行なし）
 Serial.println(ssid); //シリアルモニタへ出力（改行あり）
 WiFi.begin(ssid, password);

 int counter = 0;
 while (WiFi.status() != WL_CONNECTED) { //Wi-Fiに接続できなければ"........"の繰り返し
   delay(1000);
   Serial.print("counter = ");
   Serial.println(counter);
   counter++;
   if (counter == 10){
     ESP.restart(); //カウンターが10になるまでに接続できなければESP32を再起動
     counter = 0;
   }
 }

 // 接続が成功したら
 Serial.println("");
 Serial.println("WiFi connected.");
 Serial.print("IP address: ");
 Serial.println(WiFi.localIP());
 server.begin(); //サーバー開始
 digitalWrite(LED_BUILTIN, HIGH); //Wi-Fi接続に成功したらESP32の青色ビルトインLED点灯
}

void loop(){
 WiFiClient client = server.available();   // クライアント接続待ち

 if (client) {                             // 新しいクライアントが接続されたら
   Serial.println("New Client.");          // シリアルポートに"New Client"と出力
   String currentLine = "";                // クライアントからのデータを格納する文字列変数
   while (client.connected()) {            // クライアント接続中の処理
     if (client.available()) {             // クライアントからの読み込みデータがある場合
       char c = client.read();             // １バイト分読み込み
       Serial.write(c);                    // シリアルモニタへ出力

       header += c;
       if (c == '\n') {                    // 改行文字だった場合
         // 現在の行が空だった場合、行に２つの新行文字を含む
         // これがHTTPリクエストの終端で、レスポンスを返す
         if (currentLine.length() == 0) {  // Null「なにも値がない」ではなく長さ0「""」の文字列がある場合
           client.println("HTTP/1.1 200 OK"); // HTTP ヘッダは常に次のようなレスポンスコードを返す (HTTP/1.1 200 OK)
           client.println("Content-type:text/html"); // クライアントにどのようなContent-typeか通知して空行
           client.println("Connection: close");
           client.println();

           // GPIO ON and OFF
           if (header.indexOf("GET /LED1/ON") >= 0) {
             Serial.println("GPIO LED1 WhiteLED ON");
             LED1State = "ON";
             digitalWrite(LED1, HIGH);
           } else if (header.indexOf("GET /LED1/OFF") >= 0) {
             Serial.println("GPIO LED1 WhiteLED OFF");
             LED1State = "OFF";
             digitalWrite(LED1, LOW);
           } else if (header.indexOf("GET /LED2/ON") >= 0) {
             Serial.println("GPIO LED2 GreenLED  ON");
             LED2State = "ON";
             digitalWrite(LED2, HIGH);
           } else if (header.indexOf("GET /LED2/OFF") >= 0) {
             Serial.println("GPIO GreenLED OFF");
             LED2State = "OFF";
             digitalWrite(LED2, LOW);
           }

           // WEBページ表示内容
           client.println("<!DOCTYPE html><html>");
           client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
           client.println("<link rel=\"icon\" href=\"data:,\">");
           // CSSスタイル設定
           client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
           client.println(".button1 { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
           client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
           client.println(".button2 {background-color: #555555;}</style></head>");

           client.println("<body><h1>ESP32 Web Server</h1>"); //タイトルヘッダー

           // 現在のLED1のステータス表示 
           client.println("<p>GPIO WhiteLED33 - State = <font color = 'red'>" + LED1State + "</font></p>");
           // 現在のステータスがOFFならボタンはONで表示、ステータスがONならボタンはOFFで表示     
           if (LED1State=="OFF") {
             client.println("<p><a href=\"/LED1/ON\"><button class=\"button1\">ON</button></a></p>"); // ヘッダに続いてHTTPレスポンス:
           } else {
             client.println("<p><a href=\"/LED1/OFF\"><button class=\"button1 button2\">OFF</button></a></p>");
           }

           // 現在のLED2のステータス表示 
           client.println("<p>GPIO GreenLED32 - State = <font color = 'red'>" + LED2State + "</font></p>");
           // 現在のステータスがOFFならボタンはONで表示、ステータスがONならボタンはOFFで表示   
           if (LED2State=="OFF") {
             client.println("<p><a href=\"/LED2/ON\"><button class=\"button1\">ON</button></a></p>");
           } else {
             client.println("<p><a href=\"/LED2/OFF\"><button class=\"button1 button2\">OFF</button></a></p>");
           }
           client.println("</body></html>");
           client.println(); // HTTP終了
           break; //loopから抜けます

         } else { // 新行の時currentLineをクリア:
           currentLine = "";
         }
       } else if (c != '\r') {  // リターン文字以外だった場合、
         currentLine += c;      // currentLineの末尾に'\r'を追記
       }
     }
   }
   // Clear the header variable
   header = "";
   // Close the connection
   client.stop(); // 接続を切断:
   Serial.println("Client disconnected.");
   Serial.println("");
 }
}
