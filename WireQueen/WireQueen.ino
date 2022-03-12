#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

// The backlight for the ST7789 display (Easier to see)
#define LED 18
#define TFT_CS 10
#define TFT_DC 20
#define TFT_RST 21

#define right Serial2
#define left Serial4
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup()
{
  pinMode(LED, OUTPUT);
  right.begin(1 << 20); // Run at 1Mbps
  left.begin(1 << 20); // Run at 1Mbps
  tft.init(240, 320); // Init ST7789 320x240
  tft.setSPISpeed(60000000);
  tft.fillScreen(ST77XX_BLACK);
  digitalWrite(LED, HIGH);
}

unsigned int last = 0;

void loop()
{
  static int16_t lx1 = 0, ly1 = 0, rx1 = 0, ry1 = 0;
  static uint16_t lw = 1, lh = 1, rw = 1, rh = 1;
  unsigned int now = millis();
  /*
  if (left.available()) {
    unsigned char b = left.read();
    tft.fillRect(lx1, ly1, lw, lh, ST77XX_BLACK);
    uint16_t x = (micros() * now) % 223;
    uint16_t y = (micros() * x) % 157;
    tft.getTextBounds("000", x, y, &lx1, &ly1, &lw, &lh);
    tft.setCursor(x, y);
    tft.setTextColor((micros() ^ millis()) & 0xFFFF);
    tft.print((int) b);
  }
  */
  if (right.available()) {
    unsigned char b = right.read();
    uint8_t sc = b / 3;
    uint8_t mod = b % 3;
    uint16_t color = ST77XX_YELLOW;
    if (sc % 3 != mod) {
      tft.fillRect(185, 290, 55, 30, ST77XX_BLACK);
      tft.setCursor(185, 310);
      tft.setTextColor(ST77XX_RED);
      tft.print((int)b);
      color = ST77XX_WHITE;
    } else {
      uint16_t color = (sc < 36) ? ST77XX_RED : ST77XX_GREEN;
      tft.fillRect((sc % 6) * 5 + 1, 160 + (sc / 6) * 5, 3, 3, color);
    }
    tft.fillRect(rx1, ry1, rw, rh, ST77XX_BLACK);
    uint16_t x = 120;
    uint16_t y = 240;
    tft.getTextBounds("000", x, y, &rx1, &ry1, &rw, &rh);
    tft.setCursor(x, y);
    tft.setTextColor(ST77XX_WHITE);
    tft.print((int) b);
    tft.fillRect(80, 290, 100, 30, ST77XX_BLACK);
    tft.setCursor(80, 310);
    tft.setTextColor(color);
    tft.print(now);
  }
  delay(125);
}
