#include "Adafruit_TinyUSB.h"
#include <WiiChuck.h>

#define SHOW_INFO 0

// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_GAMEPAD()
};


// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_NONE, 2, false);

// Report payload defined in src/class/hid/hid.h
// - For Gamepad Button Bit Mask see  hid_gamepad_button_bm_t
// - For Gamepad Hat    Bit Mask see  hid_gamepad_hat_t
hid_gamepad_report_t    gp;


Accessory nunchuck1;

bool valid;
uint8_t lastValues[256];
uint32_t lastShown = 0;

void setup() {
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
  TinyUSB_Device_Init(0);
#endif
#if SHOW_INFO
	Serial.begin(115200);
#endif
  usb_hid.begin();
  // wait until device mounted
  while( !TinyUSBDevice.mounted() ) delay(1);
  
#if SHOW_INFO
  Serial.println("Adafruit TinyUSB HID Gamepad example");
#endif

	nunchuck1.begin();
	if (nunchuck1.type == Unknown) {
		/** If the device isn't auto-detected, set the type explicatly
		 * 	NUNCHUCK,
		 WIICLASSIC,
		 GuitarHeroController,
		 GuitarHeroWorldTourDrums,
		 DrumController,
		 DrawsomeTablet,
		 Turntable
		 */
		nunchuck1.type = NUNCHUCK;
	}
  valid = false;
}

// Left -> Right, Down -> Up
#define LSTICK_X 0
#define LSTICK_Y 1
#define RSTICK_X 2
#define RSTICK_Y 3

#define DPAD_LR 6
#define DPAD_UD 7

#define BTN_X 8
#define BTN_Y 9
#define BTN_A 12
#define BTN_B 13

#define BTN_LS 10
#define BTN_LB 11
#define BTN_RB 17
#define BTN_RS 18

// 0: Select(+), 128: open, 255: Start(-)
#define BTN_SEL_START 14
#define BTN_HOME 15

int translateDpad(uint8_t lr, uint8_t ud) {
  switch (lr) {
    case 0: // left
      switch (ud) {
        case 0: // down
          return 6;
        case 128: // neutral
          return 7;
        case 255: // up
          return 8;
        default: 
          return 0;
      }
    case 128: // neutral
      switch (ud) {
        case 0: // down
          return 5;
        case 128: // neutral
          return 0;
        case 255: // up
          return 1;
        default: 
          return 0;
      }
    case 255: // right
      switch (ud) {
        case 0: // down
          return 4;
        case 128: // neutral
          return 3;
        case 255: // up
          return 2;
        default: 
          return 0;
      }
    default:
      return 0;
  }
}

int getButtons(const uint8_t *vals) {
  int res = (vals[BTN_SEL_START]==255) << 9;
  res |= (!!vals[BTN_Y]) << 0;
  res |= (!!vals[BTN_B]) << 1;
  res |= (!!vals[BTN_A]) << 2;
  res |= (!!vals[BTN_X]) << 3;
  res |= (!!vals[BTN_LB]) << 4;
  res |= (!!vals[BTN_RB]) << 5;
  res |= (!!vals[BTN_LS]) << 6;
  res |= (!!vals[BTN_RS]) << 7;
  res |= (!!vals[BTN_HOME]) << 10;
  res |= (!vals[BTN_SEL_START]) << 8;
  return res;
}

void setGamePad(const uint8_t *vals) {
  // Left stick -> X/Y
  gp.x = (int)vals[LSTICK_X] - 128;
  gp.y = (255 - (int)vals[LSTICK_Y]) - 128;
  // Right Stick -> Z/RZ
  gp.z = (int)vals[RSTICK_X] - 128;
  gp.rz = (255 - (int)vals[RSTICK_Y]) - 128;
  // DPad
  gp.hat = translateDpad(vals[DPAD_LR], vals[DPAD_UD]);
  gp.buttons = getButtons(vals);
}

void loop() {
  if ( !usb_hid.ready() ) {
#if SHOW_INFO
    Serial.println("Not ready!");
#endif
    return;
  }
	nunchuck1.readData();    // Read inputs and update maps
  bool different = !valid || (lastShown + 10000) < millis();
  for (int j = 0; !different && j < WII_VALUES_ARRAY_SIZE; j++) {
    different = nunchuck1.values[j] != lastValues[j];
  }
  if (different) {
    /*
    Serial.println("Saw a diff");
	  // nunchuck1.printInputs(); // Print all inputs
  	for (int i = 0; i < WII_VALUES_ARRAY_SIZE; i++) {
      if (lastValues[i] != nunchuck1.values[i]) {
    		Serial.println(
    				"Controller Val " + String(i) + " = "
    						+ String((uint8_t) nunchuck1.values[i]));
        lastValues[i] = nunchuck1.values[i];
      }
  	}
    valid = true;
    lastShown = millis();
    */
    valid = true;
    setGamePad(nunchuck1.values);
    usb_hid.sendReport(0, &gp, sizeof(gp));
  }
}
