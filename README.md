JPEGDecoder
===========

Tweaked to use the faster SdFat library instead of the SD library. This allows a bit-bashed SPI interface to an SD Card which can be convenient for example on pins 50, 51 and 52 of a Due (on Mega these are hardware SPI).

Original Readme below:
=====================

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
