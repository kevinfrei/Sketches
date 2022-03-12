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
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  digitalWrite(LED, HIGH);
}

unsigned int last = 0;
 struct bounds {
  int16_t x, y;
  uint16_t w, h;
} ;

void drawByte(unsigned char b, uint32_t now, uint16_t x_offset, struct bounds *db) {
  uint8_t sc = b / 3;
  uint8_t mod = b % 3;
  uint16_t color = ST77XX_YELLOW;
  if (sc % 3 != mod) {
    tft.fillRect(105 + x_offset, 210, 55, 30, ST77XX_BLACK);
    tft.setCursor(105 + x_offset, 210);
    tft.setTextColor(ST77XX_RED);
    tft.print((int)b);
    color = ST77XX_WHITE;
  } else {
    uint16_t color = (sc < 36) ? ST77XX_RED : ST77XX_GREEN;
    uint8_t val = sc % 36;
    tft.fillRect((5 - (val % 6)) * 25 + 1 + x_offset, (val / 6) * 25, 23, 23, color);
  }
  tft.fillRect(db->x, db->y, db->w, db->h, ST77XX_BLACK);
  uint16_t x = 80 + x_offset;
  uint16_t y = 180;
  tft.getTextBounds("000", x, y, &db->x, &db->y, &db->w, &db->h);
  tft.setCursor(x, y);
  tft.setTextColor(ST77XX_WHITE);
  tft.print((int) b);
  tft.fillRect(80 + x_offset, 210, 100, 30, ST77XX_BLACK);
  tft.setCursor(80 + x_offset, 210);
  tft.setTextColor(color);
  tft.print(now);
}

void loop()
{
  unsigned int now = millis();
  static bounds lb, rb;
  if (left.available()) {
    unsigned char b = left.read();
    if (b)
      drawByte(b - 1, now, 0, &lb);
  }
  if (right.available()) {
    unsigned char b = right.read();
    if (b)
      drawByte(b - 1, now, 160, &rb);
  }
}
