/*====================================================================================

  This sketch demonstrates loading images which have been stored as files in the
  built-in FLASH memory on a NodeMCU (ESP8266 based) rendering them on a TFT screen
  that uses a ILI9341 driver chip with SPI interface.

  The images are stored in the SPI FLASH Filing System (SPIFFS), which effectively
  functions like a tiny "hard drive". This filing system is built into the ESP8266
  Core that can be loaded from the IDE "Boards manager" menu option. This is at
  version 2.3.0 at the time of sketch creation.

  The size of the SPIFFS partition can be set in the IDE as 1Mbyte or 3Mbytes. Either
  will work with this sketch. Typically most sketches easily fit within 1 Mbyte so a
  3 Mbyte SPIFS partition can be used, in which case it can contain ~18 full screen
  320 x 240 raw images (150 Kbytes each).

  The NodeMCU, TFT and sketch works with TFT_ILI9341_ESP library here:
  https://github.com/Bodmer/TFT_ILI9341_ESP

  Configure the SPI speed to 40MHz maximum in the User_Setup file within the
  TFT_ILI9341_ESP library folder. It *may* work at 80MHz, but typically pixels get lost
  or spurious extra pixels are plotted at very high SPI clock rates.

  The Jpeg library can be found here:
   https://github.com/Bodmer/JPEGDecoder
 
  Images in the Jpeg format can be created using Paint or IrfanView or other picture
  editting software.

  Place the images inside the sketch folder, in a folder called "Data".  Then upload
  all the files in the folder using the Arduino IDE "ESP8266 Sketch Data Upload" option
  in the "Tools" menu.  This takes some time, but the SPIFFS content is not altered
  when a new sketch is uploaded, so there is no need to upload the same files again!
  Note: If open, you must close the "Serial Monitor" window for it to upload!

  The IDE will not copy the "data" folder with the sketch if you save the sketch under
  another name. It is necessary to manually make a copy and place it in the sketch
  folder.

  This sketch includes example images in the Data folder.

  Saving images, uploading and rendering on the TFT screen couldn't be much easier!

  The typical setup for a NodeMCU1.0 (ESP-12 Module) is :

  Display SDO/MISO      to NodeMCU pin D6 <<<<<< This is not used by this sketch
  Display LED           to NodeMCU pin  5V or 3.3V
  Display SCK           to NodeMCU pin D5
  Display SDI/MOSI      to NodeMCU pin D7
  Display DC/RS (or AO) to NodeMCU pin D3
  Display RESET         to NodeMCU pin D4 <<<<<< Or connect to NodeMCU RST pin
  Display CS            to NodeMCU pin D8
  Display GND           to NodeMCU pin GND (0V)
  Display VCC           to NodeMCU pin 5V or 3.3V

  Note: only some versions of the NodeMCU provide the USB 5V on the VIN pin
  If 5V is not available at a pin you can use 3.3V but backlight brightness
  will be lower.

  If the TFT RESET signal is connected to the NodeMCU RST line then define the pin
  in the TFT library User_Config.h file as negative so the library ignores it,
  e.g. TFT_RST -1

  Created by Bodmer 14th Jan 2017 - Tested in Arduino IDE 1.8.0 esp8266 Core 2.3.0
  ==================================================================================*/

//====================================================================================
//                                  Definitions
//====================================================================================


//====================================================================================
//                                  Libraries
//====================================================================================
// Call up the SPIFFS FLASH filing system this is part of the ESP Core
#define FS_NO_GLOBALS
#include <FS.h>

// JPEG decoder library
#include <JPEGDecoder.h>

// Call up the TFT library
#include <TFT_ILI9341_ESP.h> // Hardware-specific library

// Invoke TFT library, pins and settings are defined in "User_Setup.h" in the library folder
// See "User_Setup.h" for default hardware SPI pins (NodeMCU pin naming convention is used)
TFT_ILI9341_ESP tft = TFT_ILI9341_ESP();

//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  Serial.begin(115200); // Used for messages

  tft.begin();
  tft.setRotation(0);  // 0 & 2 Portrait. 1 & 3 landscape
  tft.fillScreen(TFT_BLACK);

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");
  listFiles(); // Lists the files so you can see what is in the SPIFFS

}

//====================================================================================
//                                    Loop
//====================================================================================
void loop()
{
  // Note the / before the SPIFFS file name must be present, this means the file is in
  // the root directory of the SPIFFS, e.g. "/Tiger.rjpg" for a file called "Tiger.jpg"

  tft.setRotation(0);  // portrait
  tft.fillScreen(random(0xFFFF));

  drawFSJpeg("/EagleEye.jpg", 0, 0);
  delay(2000);

  // This is quite a famous picture used for testing image compression algorithms
  drawFSJpeg("/lena20k.jpg", 0, 0);
  delay(2000);

  //tft.fillScreen(random(0xFFFF));
  drawFSJpeg("/Baboon40.jpg", 0, 0);
  delay(2000);

  tft.setRotation(1);  // landscape
  //tft.fillScreen(random(0xFFFF));
  drawFSJpeg("/Mouse480.jpg", 0, 0);
  delay(2000);

  //tft.fillScreen(random(0xFFFF));
  drawFSJpeg("/Baboon20.jpg", 0, 0);
  delay(2000);
}
//====================================================================================

