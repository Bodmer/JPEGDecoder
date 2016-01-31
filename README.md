Arduino JPEGDecoder library
===========

This library supports the rendering of Jpeg files stored on SD card and in arrays within program memory (FLASH) onto a TFT display.

Example images can be found in the "extras" folder. The 480 x 320 Mouse480 image has been to compressed to a mere 6.45 Kbytes!

High Jpeg compression ratios work best on images with smooth colour changes, however the Baboon40.jpg image at only 23.8 Kbytes renders quite nicely. Typically a 480x320 image can be compressed without much degradation to less than 32KBytes, as a 24 bit BMP that image would occupy 461KBytes. When storing the jpeg in a memory array bear in mind the Arduino has a maximum 32767 byte limit fo an array size (32KBytes minus 1 byte).

The decompression of Jpeg images needs more RAM than an UNO provides, thus this library is targetted at the Arduino Mega and Due.

The decompression of Jpegs involves a lot of maths, so it takes a Due about ~1.3s to render a fullscreen (480x320 pixel) image and the Mega will take ~5s to do the same. The time for smaller images will reduce roughly pro-rata with the total pixel count.

This library supports either the SD or SdFat libraries. The SdFat allows a bit-bashed SPI interface to an SD Card which can be convenient for example on pins 50, 51 and 52 of a Due (on Mega these are hardware SPI).

The library has been tested with the 1.6.7 version of the Arduino IDE and may generate error messages at compile time on other versions because "#ifdef __AVR__" is used to distinguish bettwen the Mega and Due and select the correct libraries.

The library has been tested with 3.2" and 3.0" displays based on the HX8357B, HX8357C and ILI9481 driver chips with a 16 bit parallel interface.  Adapting the example sketch for other TFT drivers and their graphics libraries should be quite easy if they support either setWindow() or SetAddrWindow() and pushColor() functions as found in the Adafruit_GFX library.

This library has been based on the excellent picojpeg code and the Arduino library port by Makoto Kurauchi here:
https://github.com/MakotoKurauchi/JPEGDecoder


Makoto's original Readme below:
==============================

JPEG Decoder for Arduino

概要
----
Arduino 用 JPEG デコーダです。デコーダ部には [picojpeg](https://code.google.com/p/picojpeg/) を使用しています。

サンプルコード
----
###SerialCsvOut

SD カード上の JPEG ファイルをブロックごとにデコードし、シリアルから CSV を出力します。

変更履歴
----
V0.01 - 最初のリリース
