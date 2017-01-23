/*====================================================================================

   There is limited clipping, images that don't fit within the screen boundaries might
   appear corrupted. Crop the image (in say Paint on Windows OS) before converting to
   Jpeg format, or position image so it does not go off the screen.

  Rendering time in milliseconds .v. SPI frequency (buffer size = 512) and encoding format
  240 x 320 Baboon40 image, 80MHz CPU clock:
   SPI    
  80Mhz   490 ms
  40Mhz   507 ms
  20Mhz   541 ms

  As above, 160MHz CPU clock:
   SPI    
  80Mhz   264 ms
  40Mhz   278 ms
  20Mhz   309 ms

  The ILI9341 displays are designed to run at 20MHz maximum SPI clock, they seem to
  run reliably at 40MHz, but occasional pixel coruption sometimes occurs at 80MHz.

  Created by Bodmer 15th Jan 2017
  ==================================================================================*/

//====================================================================================
//   This function opens the Filing System Jpeg image file and primes the decoder
//====================================================================================
void drawFSJpeg(const char *filename, int xpos, int ypos) {

  Serial.println("=====================================");
  Serial.print("Drawing file: "); Serial.println(filename);
  Serial.println("=====================================");

  // Open the file, the decoder library will close it
  fs::File jpgFile = SPIFFS.open( filename, "r");  // File handle reference for SPIFFS
  //  File jpgFile = SD.open( filename, FILE_READ);  // or, file handle reference for SD library
 
  if ( !jpgFile ) {
    Serial.print("ERROR: File \""); Serial.print(filename); Serial.println ("\" not found!");
    return;
  }

  // To initialise the decoder and provide the file, we can use one of the three following methods:
  //JpegDec.decodeFsFile(jpgFile); // We can pass the SPIFFS file handle to the decoder,
  //JpegDec.decodeSdFile(jpgFile); // or we can pass the SD file handle to the decoder,
  JpegDec.decodeFsFile(filename);  // or we can pass the filename (leading / distinguishes SPIFFS files)
                                   // The filename can be a String or character array

  renderJPEG(xpos, ypos); //Now render to screen, coord datum is the top left corner
}

//====================================================================================
//   Decode and paint onto the TFT screen
//====================================================================================
void renderJPEG(int xpos, int ypos) {

  jpegInfo();
  
  uint16_t  *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t mcu_pixels = mcu_w * mcu_h;
  
  uint32_t drawTime = millis();

  while ( JpegDec.read()) {

    pImg = JpegDec.pImage;
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    if ( ( mcu_x + mcu_w) <= tft.width() && ( mcu_y + mcu_h) <= tft.height()) {

      tft.setAddrWindow(mcu_x, mcu_y, mcu_x + mcu_w - 1, mcu_y + mcu_h - 1);

      // Baboon40 draws in 731 ms
      // Use this method with SWAP_BYTES commented out in User_Config.h (inside JPEGDecoder library)
      uint32_t count = mcu_pixels;
      while (count--) tft.pushColor(*pImg++); // Send to TFT 16 bits at a time
    }

    else if ( ( mcu_y + mcu_h) >= tft.height()) JpegDec.abort();

  }

  drawTime = millis() - drawTime; // Calculate the time it took

  Serial.print  ("Total render time was    : "); Serial.print(drawTime); Serial.println(" ms");
  Serial.println("=====================================");

}

//====================================================================================
//   Send time taken to Serial port
//====================================================================================
void jpegInfo() {

  Serial.println("JPEG image info");
  Serial.println("===============");
  Serial.print("Width      :"); Serial.println(JpegDec.width);
  Serial.print("Height     :"); Serial.println(JpegDec.height);
  Serial.print("Components :"); Serial.println(JpegDec.comps);
  Serial.print("MCU / row  :"); Serial.println(JpegDec.MCUSPerRow);
  Serial.print("MCU / col  :"); Serial.println(JpegDec.MCUSPerCol);
  Serial.print("Scan type  :"); Serial.println(JpegDec.scanType);
  Serial.print("MCU width  :"); Serial.println(JpegDec.MCUWidth);
  Serial.print("MCU height :"); Serial.println(JpegDec.MCUHeight);
  Serial.println("===============");
  Serial.println("");
}

//====================================================================================
//   Read a Jpeg file and dump it to the Serial port as a C array
//====================================================================================
void createArray(const char *filename) {

  fs::File jpgFile;  // File handle reference for SPIFFS
  //  File jpgFile;  // File handle reference For SD library
  
  if ( !( jpgFile = SPIFFS.open( filename, "r"))) {
    Serial.println(F("JPEG file not found"));
    return;
  }

  uint8_t data;
  byte line_len = 0;

  Serial.print("const uint8_t ");
  while (*filename != '.') Serial.print(*filename++);
  Serial.println("[] PROGMEM = {"); // PROGMEM added for AVR processors, it is ignored by Due

  while ( jpgFile.available()) {

    data = jpgFile.read();
    Serial.print("0x"); if (abs(data) < 16) Serial.print("0");
    Serial.print(data, HEX); Serial.print(",");// Add value and comma
    line_len++;
    if ( line_len >= 32) {
      line_len = 0;
      Serial.println();
    }

  }

  Serial.println("};\r\n");
  // jpgFile.seek( 0, SeekEnd);
  jpgFile.close();
}
//====================================================================================
