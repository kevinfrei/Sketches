/***************************************************
  This is an example sketch for the Adafruit 1.8" TFT shield with joystick
  ----> http://www.adafruit.com/products/802

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 pins are required to
  interface
  One pin is also needed for the joystick, we use analog 3
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SD.h>
#include <SPI.h>

// TFT display and SD card will share the hardware SPI interface.
// Hardware SPI pins are specific to the Arduino board type and
// cannot be remapped to alternate pins.  For Arduino Uno,
// Duemilanove, etc., pin 11 = MOSI, pin 12 = MISO, pin 13 = SCK.
#define SD_CS    19  // Chip select line for SD card
#define TFT_CS  10  // Chip select line for TFT display
#define TFT_DC   20  // Data/command line for TFT
#define TFT_RST  21  // Reset line for TFT (or connect to +5V)

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#define BUTTON_NONE 0
#define BUTTON_DOWN 1
#define BUTTON_RIGHT 2
#define BUTTON_SELECT 3
#define BUTTON_UP 4
#define BUTTON_LEFT 5

void setup(void) {
  Serial.begin(115200);

  // Initialize 1.8" TFT
  tft.init(240, 320);  // initialize a ST7735S chip, black tab
  tft.setSPISpeed(72000000);
  Serial.println("OK!");
  tft.fillScreen(ST77XX_BLACK);
}


uint8_t readButton(void) {
  static uint32_t last = 0xDEADBEEF;
  uint32_t a = millis();
  if (last != (a & 0x700))  {
    Serial.print("Button read analog = ");
    Serial.println(a);
    last = a & 0x700;
  }
  if ((a & 0x700) == 0) return BUTTON_DOWN;
  if ((a & 0x700) == 0x100) return BUTTON_RIGHT;
  if ((a & 0x700) == 0x700) return BUTTON_SELECT;
  if ((a & 0x700) == 0x200) return BUTTON_UP;
  if ((a & 0x700) == 0x300) return BUTTON_LEFT;
  else return BUTTON_NONE;
}

uint8_t buttonhistory = 0;

void draw(uint16_t color, int x, int y, const char *str) {
  tft.setTextColor(color);
  int16_t cx, cy;
  uint16_t cw, ch;
  tft.getTextBounds(str, x, y, &cx, &cy, &cw, &ch);
  tft.fillRect(cx, cy, cw, ch, ST77XX_BLACK);
  tft.setCursor(x, y);
  tft.print(str);
}

void loop() {
  uint8_t b = readButton();
  tft.setTextSize(3);
  if (b == BUTTON_DOWN) {
    draw(ST77XX_RED, 95, 200, "Down");
    buttonhistory |= 1;
  }
  if (b == BUTTON_LEFT) {
    draw(ST77XX_YELLOW, 0, 100, "Left");
    buttonhistory |= 2;
  }
  if (b == BUTTON_UP) {
    draw(ST77XX_GREEN, 105, 0, "Up");
    buttonhistory |= 4;
  }
  if (b == BUTTON_RIGHT) {
    draw(ST77XX_BLUE, 150, 100, "Right");
    buttonhistory |= 8;
  }
  if ((b == BUTTON_SELECT) && (buttonhistory == 0xF)) {
    draw(ST77XX_MAGENTA, 80, 260, "SELECT");
    buttonhistory |= 8;
    delay(2000);
    Serial.print("Initializing SD card...");
    if (!SD.begin(SD_CS)) {
      Serial.println("failed!");
      return;
    }
    bmpDraw("purple.bmp", 0, 0);
  }
  delay(16);
}

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 2
uint8_t  sdbuffer[3 * BUFFPIXEL]; // pixel buffer (R+G+B per pixel)

void bmpDraw(char *filename, uint8_t x, uint8_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint32_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint16_t *bmp = 0;
  if ((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found");
    return;
  } else {
    Serial.print("SD Clock: ");
    // Serial.println(SD.sd.kHzSdClk());
  }
  

  // Parse BMP header
  if (read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("File size: "); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: "); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: "); Serial.println(bmpDepth);
      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print("Image size: ");
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if ((x + w - 1) >= tft.width())  w = tft.width()  - x;
        if ((y + h - 1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        // tft.startWrite();
        // tft.setAddrWindow(x, y, w, h);
        uint32_t decode = millis();
        bmp = new uint16_t[w * h];
        for (row = 0; row < h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if (false) // flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if (bmpFile.position() != pos) { // Need seek?
            // tft.endWrite();
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
            Serial.println("Seeking");
          }

          for (col = 0; col < w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
              // tft.startWrite();
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            uint16_t color = tft.color565(r,g,b);
            bmp[row * w + col] = color;
            //tft.pushColor(color);
          } // end pixel
        } // end scanline
        // tft.endWrite();
        uint32_t decodeDone = millis();
        tft.drawRGBBitmap(x, y, bmp, w, h);
        uint32_t stop = millis();
        Serial.print("Loaded in ");
        Serial.print(stop - startTime);
        Serial.println(" ms");
        Serial.print("Decoded in ");
        Serial.print(decodeDone - decode);
        Serial.println(" ms");
        Serial.print("Blitted in ");
        Serial.print(stop - decodeDone);
        Serial.println(" ms");
        delete[] bmp;
      } // end goodBmp
    }
  }

  bmpFile.close();
  if (!goodBmp) Serial.println("BMP format not recognized.");
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  f.read(&result, 2);
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  f.read(&result, 4);
  return result;
}
