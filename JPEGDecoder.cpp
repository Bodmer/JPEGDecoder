/*
 JPEGDecoder.cpp
 
 JPEG Decoder for Arduino
 Public domain, Makoto Kurauchi <http://yushakobo.jp>

 Adapted by Bodmer:
 https://github.com/Bodmer/JPEGDecoder

*/

#include "JPEGDecoder.h"
#include "picojpeg.h"


JPEGDecoder JpegDec;

JPEGDecoder::JPEGDecoder(){
    mcu_x = 0 ;
    mcu_y = 0 ;
    is_available = 0;
    thisPtr = this;
}


JPEGDecoder::~JPEGDecoder(){
    delete pImage;
}


unsigned char JPEGDecoder::pjpeg_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data)
{
    JPEGDecoder *thisPtr = JpegDec.thisPtr ;
    thisPtr->pjpeg_need_bytes_callback(pBuf, buf_size, pBytes_actually_read, pCallback_data);
    return 0;
}


unsigned char JPEGDecoder::pjpeg_need_bytes_callback(unsigned char* pBuf, unsigned char buf_size, unsigned char *pBytes_actually_read, void *pCallback_data)
{
    uint n;

    pCallback_data;
    
    n = min(g_nInFileSize - g_nInFileOfs, buf_size);

    if (array_jpg) for (int i = 0; i < n; i++) {
      #ifdef __AVR__
        pBuf[i] = pgm_read_byte(jpg_data++);
      #else
        pBuf[i] = *jpg_data++;
      #endif
      //Serial.println(pBuf[i],HEX);
    }
#ifdef USE_SD_CARD
    else g_pInFile.read(pBuf,n);
#endif
    *pBytes_actually_read = (unsigned char)(n);
    g_nInFileOfs += n;
    return 0;
}

int JPEGDecoder::decode_mcu(void){

    status = pjpeg_decode_mcu();
    
    if (status)
    {
        is_available = 0 ;
        mcu_y = 0;       // <<<<<< Added to correct 2nd image bug
        delete pImage;   // <<<<<< Added to correct memory leak bug
#ifdef USE_SD_CARD
        g_pInFile.close();
#endif
        if (status != PJPG_NO_MORE_BLOCKS)
        {
            #ifdef DEBUG
            Serial.print("pjpeg_decode_mcu() failed with status ");
            Serial.println(status);
            #endif
            
            delete pImage;
            return -1;
        }
    }
    return 1;
}


int JPEGDecoder::read(void)
{
    int y, x;
    uint16_t *pDst_row;
    
    if(is_available == 0) return 0;

    if (mcu_y >= image_info.m_MCUSPerCol)
    {
        delete pImage;
#ifdef USE_SD_CARD
        g_pInFile.close();
#endif
        return 0;
    }

    // Copy MCU's pixel blocks into the destination bitmap.
    pDst_row = pImage;
    for (y = 0; y < image_info.m_MCUHeight; y += 8)
    {
    const int by_limit = min(8, image_info.m_height - (mcu_y * image_info.m_MCUHeight + y));

        for (x = 0; x < image_info.m_MCUWidth; x += 8)
        {
            uint16_t *pDst_block = pDst_row + x;

            // Compute source byte offset of the block in the decoder's MCU buffer.
            uint src_ofs = (x * 8U) + (y * 16U);
            const uint8 *pSrcR = image_info.m_pMCUBufR + src_ofs;
            const uint8 *pSrcG = image_info.m_pMCUBufG + src_ofs;
            const uint8 *pSrcB = image_info.m_pMCUBufB + src_ofs;

            const int bx_limit = min(8, image_info.m_width - (mcu_x * image_info.m_MCUWidth + x));

            if (image_info.m_scanType == PJPG_GRAYSCALE)
            {
                int bx, by;
                for (by = 0; by < by_limit; by++)
                {
                    uint16_t *pDst = pDst_block;

                    for (bx = 0; bx < bx_limit; bx++)
                        *pDst++ = *pSrcR >> 3 | (*pSrcR & 0xFC) <<3 | (*pSrcR & 0xF6) << 8;

                    pSrcR += (8 - bx_limit);

                    pDst_block += row_pitch;
                }
            }
            else
            {
                int bx, by;
                for (by = 0; by < by_limit; by++)
                {
                    uint16_t *pDst = pDst_block;

                    for (bx = 0; bx < bx_limit; bx++)
                    {
                        *pDst++ = *pSrcB >> 3 | (*pSrcG & 0xFC) <<3 | (*pSrcR & 0xF8) << 8;
                        pSrcR++; pSrcG++; pSrcB++;
                    }

                    pSrcR += (8 - bx_limit);
                    pSrcG += (8 - bx_limit);
                    pSrcB += (8 - bx_limit);

                    pDst_block += row_pitch;
                }
            }
        }
        pDst_row += (row_pitch * 8);
    }

    MCUx = mcu_x;
    MCUy = mcu_y;
    
    mcu_x++;
    if (mcu_x == image_info.m_MCUSPerRow)
    {
        mcu_x = 0;
        mcu_y++;
    }

    if(decode_mcu()==-1) is_available = 0 ;

    return 1;
}

//##########################################################################################################

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// >>>>>>>> Deprecated, use decodeFile() or decodeArray() <<<<<<<<
// >>>>>>>> pReduce is no longer supported                <<<<<<<<
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

int JPEGDecoder::decode(char* pFilename, unsigned char pReduce){
    decodeFile(pFilename);
}

int JPEGDecoder::decodeFile(char* pFilename, unsigned char pReduce){
    decodeFile(pFilename);
}

int JPEGDecoder::decodeArray(const uint8_t array[], uint32_t  array_size, unsigned char pReduce){
    decodeArray(array, array_size);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


int JPEGDecoder::decodeFile(char* pFilename){
#ifdef USE_SD_CARD
    array_jpg = 0;

    g_pInFile = SD.open(pFilename, FILE_READ);
    if (!g_pInFile)
        return -1;

    g_nInFileOfs = 0;

    g_nInFileSize = g_pInFile.size();

    return decodeCommon();
#else
    return 0;
#endif
}


int JPEGDecoder::decodeArray(const uint8_t array[], uint32_t  array_size){
    
    array_jpg = 1;

    g_nInFileOfs = 0;

    jpg_data = (uint8_t *)array;

    g_nInFileSize = array_size;
    
    return decodeCommon();
}


int JPEGDecoder::decodeCommon(void) {

    status = pjpeg_decode_init(&image_info, pjpeg_callback, NULL, 0);
            
    if (status)
    {
        #ifdef DEBUG
        Serial.print("pjpeg_decode_init() failed with status ");
        Serial.println(status);
        
        if (status == PJPG_UNSUPPORTED_MODE)
        {
            Serial.println("Progressive JPEG files are not supported.");
        }
        #endif

        return -1;
    }
    
    decoded_width =  image_info.m_width;
    decoded_height =  image_info.m_height;

    row_pitch = image_info.m_MCUWidth;
    pImage = new uint16_t[image_info.m_MCUWidth * image_info.m_MCUHeight];
    if (!pImage)
    {
        #ifdef DEBUG
        Serial.println("Memory Allocation Failure");
        #endif
        
        return -1;
    }
    memset(pImage , 0 , sizeof(pImage));

    row_blocks_per_mcu = image_info.m_MCUWidth >> 3;
    col_blocks_per_mcu = image_info.m_MCUHeight >> 3;
    
    is_available = 1 ;

    width = decoded_width;
    height = decoded_height;
    comps = 1;
    MCUSPerRow = image_info.m_MCUSPerRow;
    MCUSPerCol = image_info.m_MCUSPerCol;
    scanType = image_info.m_scanType;
    MCUWidth = image_info.m_MCUWidth;
    MCUHeight = image_info.m_MCUHeight;

    return decode_mcu();
}

void JPEGDecoder::abort(void){
    
    mcu_x = 0 ;
    mcu_y = 0 ;
    is_available = 0;
    delete pImage;
#ifdef USE_SD_CARD
    if (g_pInFile) g_pInFile.close();
#endif
}


