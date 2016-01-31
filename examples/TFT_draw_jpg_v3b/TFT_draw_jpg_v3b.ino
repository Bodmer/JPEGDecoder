/*
  Descendant of SerialCsvOut.ino
  Sample code of JPEG Decoder for Arduino
  Public domain, Makoto Kurauchi <http://yushakobo.jp>
*/

// Adapted to display images on a 480 x 320 HX8357 or ILI9481
// 16 bit parallel TFT by Bodmer (aka rowboteer)
// Version 0.03b 31/1/16

// Images can be stored on an SD Card (e.g. picture.jpg) or saved as an array is a
// header file (see jpeg1.h etc)

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

// The Mega implementation uses the stock SD library built into the IDE
// A preconfigured copy of the SdFat library to work with the Due is available here:
// https://github.com/Bodmer/SdFat

// A function called createArray() is provided to covert a jpeg file stored on SD Card
// into a C array that can be pasted into any jpegX.h header file. The array is sent
// to the Serial Monitor Window.

// Images on SD Card must be put in the root folder (top level) to be found
// Use the SdFat or SD library examples to verify your SD Card interface works!
// The example images used to test this sketch can be found in the library
// JPEGDecoder/extras folder

// As supplied the function calls that pull images from SD Card are commented out
// change these to suit you image names. As supplied this sketch will draw 4 images to screen
// from the arrays stored in arrays inside each the jpegX.h file

//----------------------------------------------------------------------------------------------------

#include <SPI.h>
#include <arduino.h>

// Next the libraries are selected depending on whether it is an AVR (Mega) or otherwise a Due
// >>>> Note: This works OK with IDE 1.6.7 but might produce errors with other IDE versions. <<<<
// >>>> If you get errors here then edit or comment out the lines not needed.                <<<<

#ifdef __AVR__
// Mega libraries
#include <SD.h>                // Use the SD library for the Mega
#include <TFT_HX8357.h>        // Hardware-specific Mega library
TFT_HX8357 tft = TFT_HX8357(); // Invoke custom Mega library
#else
// Due libraries
#include <SdFat.h>             // Use the SdFat library for the Due
SdFat SD;                      // Permit SD function call for the Due
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

//#include "Free_Fonts.h" // Include the header file attached to this sketch

// Count how many times the image is drawn for test purposes
uint32_t icount = 0;
//----------------------------------------------------------------------------------------------------


//####################################################################################################
// Setup
//####################################################################################################
void setup() {
  Serial.begin(115200);

  tft.begin();
  tft.setRotation(1);  // landscape
  tft.fillScreen(TFT_BLACK);

  Serial.print("Initialising SD card...");

  if (!SD.begin(SD_CS)) {
    Serial.println("initialisation failed!");
    return;
  }

  Serial.println("initialisation done.");
}

//####################################################################################################
// Main loop
//####################################################################################################
void loop() {

  tft.setRotation(0);  // portrait
  tft.fillScreen(random(0xFFFF));
  
  // drawSdJpeg("EagleEye.jpg", 0, 0);     // This draws a jpeg pulled off the SD Card
  // createArray("EagleEye.jpg");  // This pulls a jpeg image off the SD Card and serial dumps an array
  
  // The image is 300 x 300 pixels so we do some sums to position image in the middle of the screen!
  // Doing this by reading the image width and height from the jpeg info is left as an exercise!
  int x = (tft.width()  - 300)/2 - 1;
  int y = (tft.height() - 300)/2 - 1;
  
  drawArrayJpeg(EagleEye, sizeof(EagleEye), x, y); // Draw a jpeg image stored in memory at x,y
  delay(2000);

  tft.setRotation(0);  // portrait
  tft.fillScreen(random(0xFFFF));
  // drawSdJpeg("Baboon40.jpg", 0, 0);     // This draws a jpeg pulled off the SD Card
  // createArray("Baboon40.jpg");  // This pulls a jpeg image off the SD Card and serial dumps an array
  drawArrayJpeg(Baboon40, sizeof(Baboon40), 0, 0); // Draw a jpeg image stored in memory
  delay(2000);

  tft.setRotation(0);  // portrait
  tft.fillScreen(random(0xFFFF));
  // drawSdJpeg("lena20k.jpg", 0, 0);     // This draws a jpeg pulled off the SD Card
  // createArray("lena20k.jpg");  // This pulls a jpeg image off the SD Card and serial dumps an array
  drawArrayJpeg(lena20k, sizeof(lena20k), 0, 0); // Draw a jpeg image stored in memory
  delay(2000);

  tft.setRotation(1);  // landscape
  tft.fillScreen(random(0xFFFF));
  // drawSdJpeg("Mouse480.jpg", 0, 0);     // This draws a jpeg pulled off the SD Card
  // createArray("Mouse480.jpg");  // This pulls a jpeg image off the SD Card and serial dumps an array
  // delay(1000);
  // tft.fillScreen(random(0xFFFF));

  // This image will be deliberately cropped as it is 480 x 320 thes extends off the screen when plotted
  // at coordinate 50,50
  drawArrayJpeg(Mouse480, sizeof(Mouse480), 50, 50); // Draw a jpeg image stored in memory, test cropping
  delay(2000);

  //while(1);
}

//####################################################################################################
// Draw a JPEG on the TFT pulled from SD Card
//####################################################################################################
// xpos, ypos is top left corner of plotted image
void drawSdJpeg(char *filename, int xpos, int ypos) {

  JpegDec.decodeFile(filename, 0);
  renderJPEG(xpos, ypos);
}

//####################################################################################################
// Draw a JPEG on the TFT pulled from a program memory array
//####################################################################################################
void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos) {

  JpegDec.decodeArray(arrayname, array_size, 0);
  renderJPEG(xpos, ypos);
}

//####################################################################################################
// Draw a JPEG on the TFT, images will be cropped on the right/bottom sides if they do not fit
//####################################################################################################
// This function assumes xpos,ypos is a valid screen coordinate. For convenience images that do not
// fit totally on the screen are cropped to the nearest MCU size and may leave right/bottom borders.
void renderJPEG(int xpos, int ypos) {

  jpegInfo(); // Print information from the JPEG file (could comment this line out)

  uint8_t  *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;    // Width of MCU
  uint16_t mcu_h = JpegDec.MCUHeight;   // Height of MCU
  uint32_t mcu_pixels = mcu_w * mcu_h;  // Total number of pixels in an MCU

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
        // The pImg [B]lue, [G]reen and [R]ed  8 bit array values are
        // converted to 5+6+5 [B]+[G]+[R] 16 bit format)
        tft.pushColor(pImg[2] >> 3 | (pImg[1] & 0xFC) << 3 | (pImg[0] & 0xF8) << 8);
        // Fetch the next decoded pixel
        pImg += JpegDec.comps ;
      }
    }
  }

  //showTime(millis() - drawTime); // These lines are for sketch testing only
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
// WARNING: for UNO/AVR legacy reasons printing text to the screen will not work for sketch sizes
// greater than ~70KBytes. This is because 16 bit address pointers are used in the HX8357 library.
// >>>> Don't complain, it is on my list to be fixed! :-)  <<<<

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

//####################################################################################################
// Pull a jpeg file off the SD Card and send it as a "C" formatted as an array to the serial port
//####################################################################################################
// The array can be cut and pasted from the Serial Monitor window into jpegX.h attached to this sketch
void createArray(const char *filename) {
  File jpgFile;
  uint8_t  sdbuffer[32];   // SD read pixel buffer (16 bits per pixel)

  // Check file exists and open it
  if ((jpgFile = SD.open(filename)) == NULL) {
    Serial.println(F("JPEG file not found"));
    return;
  }

  uint8_t data;
  byte line_len = 0;

  Serial.print("const uint8_t ");
  // Make the array the same as the file name with the .(file extension) removed
  while (*filename != '.') Serial.print(*filename++);

  Serial.println("[] PROGMEM = {"); // PROGMEM added for AVR processors, it is ignored by Due
  // Pull all data falues from file and print in the array format
  while (jpgFile.available()) {
    data = jpgFile.read();
    Serial.print("0x");                        // Add hexadecimal prefix
    if (abs(data) < 16) Serial.print("0");     // Add a leading zero to create a neater array
    Serial.print(data, HEX); Serial.print(",");// Add value and comma
    line_len++;
    // Add a newline every 32 bytes
    if (line_len >= 32) {
      line_len = 0;
      Serial.println();
    }
  }
  Serial.println("};");
  Serial.println();
  // close the file:
  jpgFile.close();
}
//####################################################################################################
// How to setup SdFat to work with the JPEGDecoder library
//####################################################################################################

// The SD library can be used on the Mega only, it will not work with the Due as the TFT
// display is not connected to the hardware SPI pins. The SD library does NOT need to be
// modified.

// The SdFat library can be used with the Mega or Due.

// To use SdFat with the Due you must edit the SdFatConfig.h file so
// that the Software (bit bashed) SPI signal driving will be employed.
//
// You can get a copy of the SdFat library that has already been modified for the Due here:
// https://github.com/Bodmer/SdFat

//  The changes to be made to the SdFatConfig.h file are indicated by =>> below:

//=>>    #define SD_SPI_CONFIGURATION 2  // Set to 0 for Mega, 2 for Due
//------------------------------------------------------------------------------
/**
   If SD_SPI_CONFIGURATION is defined to be two, these definitions
   will define the pins used for software SPI.

   The default definition allows Uno shields to be used on other boards.
*/
/** Software SPI Master Out Slave In pin */
//=>>             uint8_t const SOFT_SPI_MOSI_PIN = 51;
/** Software SPI Master In Slave Out pin */
//=>>             uint8_t const SOFT_SPI_MISO_PIN = 50;
/** Software SPI Clock pin */
//=>>             uint8_t const SOFT_SPI_SCK_PIN  = 52;
//------------------------------------------------------------------------------

// You MUST also modify the SdFat.h file to make the created SD class members 'global' so
// they can be used in this sketch and by the JPEGDecoder library.
// (The SD library makes the equivalent class global without modifications)
// There may be an easier way to do this but it works OK.
//
// The line to add is 'extern SdFat SD;', this must be addedat around line 283
// after the SdFat class is specified.
// When the change has been made the following line sequence should appear in the
// SdFat.h file (do not add the /* or */ !):
/*

  extern SdFat SD; //<<= Line to add, the next 2 lines are ALREADY in the file, they
                 //    are included here just to help locate the right place!
  //==============================================================================
  #if SD_SPI_CONFIGURATION >= 3 || defined(DOXYGEN)

*/
