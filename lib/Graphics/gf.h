#ifndef GF_H
#define GF_H

#include <Adafruit_GFX.h>    // Core graphics library
#include <SD_MMC.h>
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>


#define TFT_DC 19 //A0
#define TFT_CS 22 //CS
#define TFT_MOSI 4 //SDA
#define TFT_CLK 23 //SCK
#define TFT_RST 18  
#define TFT_MISO 0 

void begin();
void beginGraphic();
int getWidth();
int getHeight();
void fillScreen();
void fillRectIP();
void fillRectRL(int r);
void testlines(uint16_t color);
void testdrawtext(char *text, uint16_t color);
void drawtext(int x, int y, char *text, uint16_t color, uint8_t size);
void drawtext(const char *text, uint16_t color, uint8_t size);
void testfastlines(uint16_t color1, uint16_t color2);

void testdrawrects(uint16_t color);

void testfillrects(uint16_t color1, uint16_t color2);

void testfillcircles(uint8_t radius, uint16_t color);

void testdrawcircles(uint8_t radius, uint16_t color);

void testtriangles();

void testroundrects();

void tftPrintTest();

void mediabuttons();
#endif
