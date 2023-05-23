
#include <Arduino.h>
#include "SDP6x.h"
#include <Wire.h>
#include <TFT_eSPI.h>
#include "TTGO_T_Display.h"

#define BUTTON1PIN 35

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

uint16_t error;
char errorMessage[256];
float difPressure;
float speed;

float difPMin = 500, difPMax = -50, Tmin = 100, Tmax = -100;
uint16_t refreshrate;
boolean extreem = false;

void IRAM_ATTR toggleButton1()
{
    difPMin = 500;
    difPMax = -50;
    Tmin = 100;
    Tmax = -100;
}

void setup()
{

    Serial.begin(115200);
    while (!Serial)
    {
        delay(100);
    }

    Wire.begin();
    Wire.setClock(400000);
    pinMode(BUTTON1PIN, INPUT);

    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);

    if (TFT_BL > 0)
    {                                           // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
        pinMode(TFT_BL, OUTPUT);                // Set backlight pin to output mode
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }
    tft.setSwapBytes(true);
    delay(100);
    tft.setRotation(0);
    int i = 1;
    while (i--)
    {
        tft.fillScreen(TFT_RED);
        delay(100);
        tft.fillScreen(TFT_BLUE);
        delay(100);
        tft.fillScreen(TFT_GREEN);
        delay(100);
    }

    tft.fillScreen(TFT_BLACK);

    tft.setRotation(1);

    tft.setTextDatum(0);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    attachInterrupt(BUTTON1PIN, toggleButton1, FALLING);

    // sdp.startContinuousMeasurementWithDiffPressureTComp();
}

void loop()
{

    difPressure = SDP6x.GetPressureDiff();

    speed = sqrt(abs((2*difPressure) / 1.2));


    Serial.print(difPressure, 5);

    Serial.print("\t");
    Serial.println(speed, 5);

    if (difPMin > difPressure)
    {
        difPMin = difPressure;
        extreem = true;
    }
    if (difPMax < difPressure)
    {
        difPMax = difPressure;
        extreem = true;
    }

    refreshrate++;
    if (refreshrate < 10)
    {
        if (extreem == true)
        {
            tft.setTextColor(TFT_RED, TFT_BLACK);

            tft.drawString("" + String(difPMin, 2) + " Pa    ", 0, 30, 4);

            tft.setTextColor(TFT_BLUE, TFT_BLACK);
            tft.drawString("" + String(difPMax, 2) + " Pa  ", 130, 30, 4);

            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            extreem = false;
        }
        tft.drawString(" Druk: " + String(difPressure, 3) + " Pa      ", 0, 0, 4);

        tft.drawString(" Snel: " + String(speed, 3) + " m/s      ", 0, 60, 4);

        refreshrate = 0;
    }
}