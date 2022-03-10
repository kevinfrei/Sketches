#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

// The backlight for the ST7789 display (Easier to see)
#define LED 18
#define TFT_CS 10
#define TFT_DC 20
#define TFT_RST 21

//#define right Serial2
#define left Serial4
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup()
{
  pinMode(LED, OUTPUT);
  // right.begin(9600);
  left.begin(115200);
  tft.init(240, 320); // Init ST7789 320x240
  tft.setSPISpeed(60000000);
  tft.fillScreen(ST77XX_BLACK);
  digitalWrite(LED, HIGH);
}

unsigned int last = 0;

void loop()
{
  static int16_t x1 = 0, y1 = 0;
  static uint16_t w = 1, h = 1;
  unsigned int now = millis();
  while (left.available()) {
    unsigned char b = left.read();
    int x = (240 - w) / 2;
    unsigned int y = (now / 200) % 280;
    tft.fillRect(x1, y1, w, h, ST77XX_BLACK);
    tft.getTextBounds("00", x, y, &x1, &y1, &w, &h);
    tft.setCursor(x, y);
    tft.setTextColor(ST77XX_WHITE);
    tft.print((int) b, HEX);
  }
  delay(10);
}
