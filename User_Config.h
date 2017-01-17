// Comment out the next #define if you are not using an SD Card to store the JPEGs
// Commenting out the line is NOT essential but will save some FLASH space if
// SD Card access is not needed.

#define LOAD_SD_LIBRARY

#ifdef ESP8266
    // Comment out the next #define if you do not want the bytes swapped in
	// the returned image blocks. Swapping the bytes does mean pixel blocks can be
	// written to the screen faster using the ESP8266 SPI library writePattern()
	// member function. Comment out for "normal" byte order. Images will look pyscodelic
    // with wrong colours if the byte order is not right for your sketch!
	 #define SWAP_BYTES
#endif
