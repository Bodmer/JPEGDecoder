/*
 JPEGDecoder.h
 
 JPEG Decoder for Arduino
 Public domain, Makoto Kurauchi <http://yushakobo.jp>
*/

#ifndef JPEGDECODER_H
#define JPEGDECODER_H

#include "User_Config.h"

#include "Arduino.h"

#ifdef USE_SD_CARD
  #ifdef __AVR__
    #include <SD.h>
  #else
    #include <SdFat.h>
  #endif
#endif

#include "picojpeg.h"

//#define DEBUG

//------------------------------------------------------------------------------
#ifndef max
#define max(a,b)     (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)     (((a) < (b)) ? (a) : (b))
#endif
//------------------------------------------------------------------------------
typedef unsigned char uint8;
typedef unsigned int uint;
//------------------------------------------------------------------------------

class JPEGDecoder {

private:

#ifdef USE_SD_CARD
    File g_pInFile;
#endif
    pjpeg_scan_type_t scan_type;
    pjpeg_image_info_t image_info;
    
    int is_available;
    int mcu_x;
    int mcu_y;
    uint g_nInFileSize;
    uint g_nInFileOfs;
    uint row_pitch;
    uint decoded_width, decoded_height;
    uint row_blocks_per_mcu, col_blocks_per_mcu;
    uint8 status;
    uint8 array_jpg;
    uint8_t* jpg_data; 
    
    static uint8 pjpeg_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data);
    uint8 pjpeg_need_bytes_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data);
    int decode_mcu(void);
    int decodeCommon(void);
public:

    uint16_t *pImage;
    JPEGDecoder *thisPtr;

    int width;
    int height;
    int comps;
    int MCUSPerRow;
    int MCUSPerCol;
    pjpeg_scan_type_t scanType;
    int MCUWidth;
    int MCUHeight;
    int MCUx;
    int MCUy;
    
    JPEGDecoder();
    ~JPEGDecoder();

    int available(void);
    int read(void);

    // Deprecated, legacy sketch support only
    int decode     (char* pFilename, unsigned char pReduce);
    int decodeFile (char* pFilename, unsigned char pReduce);
    int decodeArray(const uint8_t array[], uint32_t  array_size, unsigned char pReduce);

    int decodeFile (char* pFilename);
    int decodeArray(const uint8_t array[], uint32_t  array_size);
    void abort(void);

};

extern JPEGDecoder JpegDec;

#endif // JPEGDECODER_H