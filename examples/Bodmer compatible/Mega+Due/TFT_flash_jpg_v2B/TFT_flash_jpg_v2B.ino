/*
  Descendant of SerialCsvOut.ino
  Sample code of JPEG Decoder for Arduino
  Public domain, Makoto Kurauchi <http://yushakobo.jp>
*/

// Adapted to display images on a 480 x 320 HX8357 or ILI9481
// 16 bit parallel TFT by Bodmer (aka rowboteer)
// Version 0.09b 15/2/16

// Renders images stored in an array in program (FLASH)
// JPEG images are stored in header files (see jpeg1.h etc)

// The sketch does not need the SD or sdFat libraries since it does not access an
// SD Card. You can edit the "User_Config.h" file inside the JPEGDecoder library folder
// to stop the decode loading the SD or sdFat library.
// See comments in "User_Config.h". Editting the User_Config.h file is optional
// and NOT essential but it will save some FLASH space.

// Mega TFT library here:
// https://github.com/Bodmer/TFT_HX8357

// Due TFT library here:
// https://github.com/Bodmer/TFT_HX8357_Due

// This example draws a jpeg compressed image stored in memory onto the screen
// the image is 480 x 320 pixels.

// As an example Baboon40.jpg is compressed from ~460 kBytes (24 bit colour) to a mere
// 24.4 kBytes (~19 times smaller), Mouse480.jpg is 6.45 kBytes (~70 times smaller)

// As well as the HX8357 TFT library you will need the JPEG Decoder library.
// A copy can be downloaded here, it is based on the library by Makoto Kurauchi.
// The following copy has some bug fixes and extra/adapted member functions:
// https://github.com/Bodmer/JPEGDecoder

// As supplied this sketch will draw 4 images to screen in 4 different sizes
// Note the whole JPEG has to be decoded to draw scaled images so drawing scaled
// pictures is not significantly faster than drawing the 1:1 (unscaled) image

//----------------------------------------------------------------------------------------------------

#include <SPI.h>
#include <arduino.h>
#ifdef ESP8266

void setup(){
  
  Serial.begin(115200);
  Serial.println("\r\n\r\nThis one is not compatible with ESP8266 for structural reason. Use the TFT_SPIFF_jpeg_v3 example to use your Flash.");

}

void loop(){yield();}

#else

// Next the libraries are selected depending on whether it is an AVR (Mega) or otherwise a Due
// >>>> Note: This works OK with IDE 1.6.7 but might produce errors with other IDE versions. <<<<
// >>>> If you get errors here then edit or comment out the lines not needed.                <<<<

#ifdef __AVR__
// Mega libraries
#include <TFT_HX8357.h>        // Hardware-specific Mega library
TFT_HX8357 tft = TFT_HX8357(); // Invoke custom Mega library
#else
// Due libraries
#include <TFT_HX8357_Due.h>    // Hardware-specific Due library
TFT_HX8357_Due tft = TFT_HX8357_Due(); // Invoke custom Due library
#endif

// JPEG decoder library
#include <JPEGDecoder.h>

// Chip Select Pin for SD card
#define SD_CS 53

// Include the sketch header file that contains the image stored as an array of bytes
// More than one image array could be stored in each header file.

#include "jpeg1.h"
#include "jpeg2.h"
#include "jpeg3.h"
#include "jpeg4.h"



// Count how many times the image is drawn for test purposes
uint32_t icount = 0;
//----------------------------------------------------------------------------------------------------


//####################################################################################################
// Setup
//####################################################################################################
void setup() {
  Serial.begin(115200);
  tft.begin();
}

//####################################################################################################
// Main loop
//####################################################################################################
void loop() {

  tft.setRotation(2);  // portrait
  tft.fillScreen(random(0xFFFF));

  // The image is 300 x 300 pixels so we do some sums to position image in the middle of the screen!
  // Doing this by reading the image width and height from the jpeg info is left as an exercise!
  int x = (tft.width()  - 300) / 2 - 1;
  int y = (tft.height() - 300) / 2 - 1;

  drawArrayJpeg(EagleEye, sizeof(EagleEye), x, y); // Draw a jpeg image stored in memory at x,y
  delay(2000);


  tft.setRotation(2);  // portrait
  tft.fillScreen(random(0xFFFF));
  drawArrayJpeg(Baboon40, sizeof(Baboon40), 0, 0); // Draw a jpeg image stored in memory
  delay(2000);


  tft.setRotation(2);  // portrait
  tft.fillScreen(random(0xFFFF));
  drawArrayJpeg(lena20k, sizeof(lena20k), 0, 0); // Draw a jpeg image stored in memory
  delay(2000);

  tft.setRotation(1);  // landscape
  tft.fillScreen(random(0xFFFF));

  // This image will be deliberately cropped as it is 480 x 320 thes extends off the screen when plotted
  // at coordinate 100,100
  drawArrayJpeg(Mouse480, sizeof(Mouse480), 100, 100); // Draw a jpeg image stored in memory, test cropping
  //drawArrayJpeg(Mouse480, sizeof(Mouse480), 0, 0); // Draw a jpeg image stored in memory
  delay(2000);
}

//####################################################################################################
// Draw a JPEG on the TFT pulled from a program memory array
//####################################################################################################
void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos) {

  int x = xpos;
  int y = ypos;

  JpegDec.decodeArray(arrayname, array_size);
  renderJPEG(x, y);
  
  Serial.println("#########################");

  JpegDec.decodeArray(arrayname, array_size);
  renderScaledJPEG(x, y, 2);
  JpegDec.decodeArray(arrayname, array_size);
  renderScaledJPEG(x, y, 4);
  JpegDec.decodeArray(arrayname, array_size);
  renderScaledJPEG(x, y, 8);

  Serial.println("@@@@@@@@@@@@@@@@@@@@@@@@@");
}

//####################################################################################################
// Draw a JPEG on the TFT, images will be cropped on the right/bottom sides if they do not fit
//####################################################################################################
// This function assumes xpos,ypos is a valid screen coordinate. For convenience images that do not
// fit totally on the screen are cropped to the nearest MCU size and may leave right/bottom borders.
void renderJPEG(int xpos, int ypos) {

  //jpegInfo(); // Print information from the JPEG file (could comment this line out)

  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;    // Width of MCU
  uint16_t mcu_h = JpegDec.MCUHeight;   // Height of MCU
  uint32_t mcu_pixels = mcu_w * mcu_h;  // Total number of pixels in an MCU

 // Serial.print("comp size = ");Serial.println(comp_size);
  
  uint32_t drawTime = millis(); // For comparison purpose the draw time is measured

  // Fetch data from the file, decode and display
  while (JpegDec.read()) {    // While there is more data in the file
    pImg = JpegDec.pImage ;   // Decode a MCU (Minimum Coding Unit, typically a 8x8 or 16x16 pixel block)

    int mcu_x = JpegDec.MCUx * mcu_w + xpos;  // Calculate coordinates of top left corner of current MCU
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    if ((mcu_x + mcu_w) <= tft.width() && (mcu_y + mcu_h) <= tft.height())
    {
      // Now set a MCU bounding window on the TFT to push pixels into (x, y, x + width - 1, y + height - 1)
      tft.setWindow(mcu_x, mcu_y, mcu_x + mcu_w - 1, mcu_y + mcu_h - 1);

      // Push all MCU pixels to the TFT window
      uint32_t count = mcu_pixels;
      while (count--) {
        // Push each pixel to the TFT MCU area
        tft.pushColor(*pImg++);
      }

      // Push all MCU pixels to the TFT window, ~18% faster to pass an array pointer and length to the library
      // tft.pushColor16(pImg, mcu_pixels); //  To be supported in HX8357 library at a future date

    }
    else if ((mcu_y + mcu_h) >= tft.height()) JpegDec.abort(); // Image has run off bottom of screen so abort decoding
  }

  showTime(millis() - drawTime); // These lines are for sketch testing only
  //Serial.print(" Draw count:");
  //Serial.println(icount++);
}

//####################################################################################################
// Draw a scaled JPEG on the TFT, images will be cropped on the right/bottom sides if they do not fit
//####################################################################################################
// This function assumes xpos,ypos is a valid screen coordinate. For convenience images that do not
// fit totally on the screen are cropped to the nearest MCU size and may leave right/bottom borders.

// Images are scaled 1:N so if scale = 2 the image is drawn half size, 4 quarter size, 8 eighth size

void renderScaledJPEG(int xpos, int ypos, uint8_t scale) {

  jpegInfo(); // Print information from the JPEG file (could comment this line out)

  if ((scale !=1) && (scale !=2) && (scale !=4) && (scale !=8)) return; // Image size reduction factors of 1, 2, 4 and 8 supported

  uint16_t  *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;    // Width of MCU
  uint16_t mcu_h = JpegDec.MCUHeight;   // Height of MCU

  uint32_t drawTime = millis(); // For comparison purpose the draw time is measured

  // Fetch data from the file, decode and display
  while (JpegDec.read()) {    // While there is more data in the file
    pImg = JpegDec.pImage;    // Decode a MCU (Minimum Coding Unit, typically a 8x8 or 16x16 pixel block)

    int mcu_x = JpegDec.MCUx * mcu_w / scale + xpos; // Calculate coordinates of top left corner of current MCU
    int mcu_y = JpegDec.MCUy * mcu_h / scale + ypos;

    if ((mcu_x + mcu_w) <= tft.width() && (mcu_y + mcu_h) <= tft.height())
    {
      // Now set a MCU bounding window on the TFT to push pixels into (x, y, x + width - 1, y + height - 1)
      tft.setWindow(mcu_x, mcu_y, mcu_x + mcu_w / scale - 1, mcu_y + mcu_h / scale - 1);
      // Push all MCU pixels to the TFT window
      for (uint8_t yp = 0; yp < mcu_h; yp += scale) {
        for (uint8_t xp = 0; xp < mcu_w; xp += scale) {
          // Push each pixel to the TFT MCU area
          // The pImg [B]lue, [G]reen and [R]ed  8 bit array values are
          // converted to 5+6+5 [B]+[G]+[R] 16 bit format)

          uint16_t red   = 0;
          uint16_t green = 0;
          uint16_t blue  = 0;

          for (uint8_t ya = 0; ya < scale; ya++) {
            for (uint8_t xa = 0; xa < scale; xa ++ ) {
              red  += *pImg >> 11;
              green += (*pImg & 0x7E0) >> 5;
              blue   += *pImg & 0x1F;
            }
          }

          if (scale == 1) tft.pushColor(blue >> 0 | ((green << 5) & 0x7E0) | ((red << 11) & 0xF800));
          if (scale == 2) tft.pushColor(blue >> 2 | ((green << 3) & 0x7E0) | ((red << 9) & 0xF800));
          if (scale == 4) tft.pushColor(blue >> 4 | ((green << 1) & 0x7E0) | ((red << 7) & 0xF800));
          if (scale == 8) tft.pushColor(blue >> 6 | ((green >> 1) & 0x7E0) | ((red << 5) & 0xF800));
          
          // Goto the next decoded pixel block
          pImg += scale;
        }
        pImg += (scale-1) * mcu_w;
      }
    }
    else if ((mcu_y + mcu_h) >= tft.height()) JpegDec.abort(); // Image has run off bottom of screen so abort decoding
  }
  showTime(millis() - drawTime); // These lines are for sketch testing only
  //Serial.print(" Draw count:");
  //Serial.println(icount++);
}

//####################################################################################################
// Print image information to the serial port (optional)
//####################################################################################################
// JpegDec.decodeFile(...) or JpegDec.decodeArray(...) must be called before this info is available!
void jpegInfo() {

  // Print information extracted from the JPEG file
  Serial.println("JPEG image info");
  Serial.println("===============");
  Serial.print("Width      :");
  Serial.println(JpegDec.width);
  Serial.print("Height     :");
  Serial.println(JpegDec.height);
  Serial.print("Components :");
  Serial.println(JpegDec.comps);
  Serial.print("MCU / row  :");
  Serial.println(JpegDec.MCUSPerRow);
  Serial.print("MCU / col  :");
  Serial.println(JpegDec.MCUSPerCol);
  Serial.print("Scan type  :");
  Serial.println(JpegDec.scanType);
  Serial.print("MCU width  :");
  Serial.println(JpegDec.MCUWidth);
  Serial.print("MCU height :");
  Serial.println(JpegDec.MCUHeight);
  Serial.println("===============");
  Serial.println("");
}

//####################################################################################################
// Show the execution time (optional)
//####################################################################################################
// WARNING: for UNO/AVR legacy reasons printing text to the screen with the Mega might not work for
// sketch sizes greater than ~70KBytes because 16 bit address pointers are used in some libraries.

// The Due will work fine with the HX8357_Due library.

void showTime(uint32_t msTime) {
  //tft.setCursor(0, 0);
  //tft.setTextFont(1);
  //tft.setTextSize(2);
  //tft.setTextColor(TFT_WHITE, TFT_BLACK);
  //tft.print(F(" JPEG drawn in "));
  //tft.print(msTime);
  //tft.println(F(" ms "));
  Serial.print(F(" JPEG drawn in "));
  Serial.print(msTime);
  Serial.println(F(" ms "));
}

#endif

