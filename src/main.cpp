#include <M5StickC.h>
#include <WiFiClientSecure.h>
#include <ssl_client.h>

// #define SSID "E426867F5D3B-2G"
// #define PASSWORD "gapf8fcpgfd2e8"
#define TOKEN "xedWGy5ZY6UJBB1UrM4TgvTh6YaqDKpXBv2vlOQgvKY"
#define SSID "The_Fire_Choir"
#define PASSWORD "smile108"

int lcdValue_g = 9;

void lcd(const char *str, int x, int y, int size)
{
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextColor(ORANGE);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextFont(4);
  M5.Lcd.setTextSize(size);
  M5.Lcd.setCursor(x, y);
  M5.Lcd.print(str);
}

#define RETRY_MAX 100
void wifi_connect(const char *ssid, const char *password)
{
  Serial.printf("wifi connecting...");
  while (true)
  {
    int retry = 0;
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(100);
      Serial.printf("%d ", retry);
      retry++;
      if (retry > RETRY_MAX) // max 100 wifi-reset
      {
        retry = 0;
        break;
      }
    }
    break;
  }
  Serial.println("\nwifi connect ok");
}

#define RECIEVE_MAX 48
void line_send(const char *token, String message)
{
  const char *host = "notify-api.line.me";
  WiFiClientSecure client;
  client.setInsecure();
  Serial.println("Try");
  //LineのAPIサーバに接続
  if (!client.connect(host, 443))
  {
    Serial.println("Connection failed");
    return;
  }
  Serial.println("Connected");
  //リクエストを送信
  String query = String("message=") + message;
  String request = String("") +
                   "POST /api/notify HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Authorization: Bearer " + token + "\r\n" +
                   "Content-Length: " + String(query.length()) + "\r\n" +
                   "Content-Type: application/x-www-form-urlencoded\r\n\r\n" +
                   query + "\r\n";
  client.print(request);

  //受信終了まで待つ
  for (int i = 0; client.connected() && i < RECIEVE_MAX; i++) //max 48line
  {
    String line = client.readStringUntil('\n');
    Serial.println(">" + line);
    if (line == "")
    {
      break;
    }
  }
  Serial.println("recieve end.\n");
}

void setup()
{
  M5.begin();
  M5.Axp.ScreenBreath(lcdValue_g);
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextSize(2);

  Serial.begin(9600); //シリアルモニタ　ボーレートの設定

  lcd("PUSH", 10, 20, 2);
}

void loop()
{
  M5.update();
  if (M5.BtnA.wasReleased())
  {
    lcd("wifi connect...", 0, 30, 1);
    wifi_connect(SSID, PASSWORD);
    lcd("send...", 30, 30, 1);
    line_send(TOKEN, "連絡ください");
    lcd("success!", 30, 30, 1);
    WiFi.disconnect(true);
    delay(3000);
    lcd("PUSH", 10, 20, 2);
  }
  else if (M5.BtnB.wasReleased())
  {
    lcd("wifi connect...", 0, 30, 1);
    wifi_connect(SSID, PASSWORD);
    lcd("send...", 30, 30, 1);
    line_send(TOKEN, "てすと");
    lcd("success!", 30, 30, 1);
    WiFi.disconnect(true);
    delay(3000);
    lcd("PUSH", 10, 20, 2);
  }
  else if (M5.Axp.GetBtnPress() == 2)
  {
    lcdValue_g++;
    if (lcdValue_g > 12) {
      lcdValue_g = 7;
    }
    Serial.printf("%d ", lcdValue_g);
    M5.Axp.ScreenBreath(lcdValue_g);
    lcd("LINE BUTTON\n\n      by asamizu", 0, 0, 1);
    delay(3000);
    lcd("PUSH", 10, 20, 2);
  }
}