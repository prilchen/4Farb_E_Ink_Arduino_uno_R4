#include <Arduino.h>
#include <stdlib.h>
#include "DEV_Config.h"
#include "EPD_1in54g.h"
#include "GUI_Paint.h"

// Wir bleiben bei 200x200, da der Controller diese Datenmenge erwartet
#undef EPD_1IN54G_WIDTH
#define EPD_1IN54G_WIDTH 200
#undef EPD_1IN54G_HEIGHT
#define EPD_1IN54G_HEIGHT 200

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println(">>> 4-FARBEN RETTUNGSMODUS <<<");

    DEV_Module_Init();
    
    // SPI Bremse für den schnellen R4 WiFi
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

    EPD_1IN54G_Init();
    
    // Budget MUSS 10000 Bytes sein für 200x200 @ 2-Bit
    UWORD Imagesize = ((EPD_1IN54G_WIDTH % 4 == 0)? (EPD_1IN54G_WIDTH / 4 ): (EPD_1IN54G_WIDTH / 4 + 1)) * EPD_1IN54G_HEIGHT;
    Serial.print("Berechnetes Budget: ");
    Serial.println(Imagesize);

    UBYTE *ImageBuffer;
    if((ImageBuffer = (UBYTE *)malloc(Imagesize)) == NULL) {
        Serial.println("Fehler: RAM voll!");
        while(1);
    }

    // 1. Image initialisieren
    Paint_NewImage(ImageBuffer, EPD_1IN54G_WIDTH, EPD_1IN54G_HEIGHT, 0, EPD_1IN54G_WHITE);
    
    // 2. WICHTIG: Scale auf 4 setzen (aktiviert den 2-Bit Modus / 4 Farben)
    // Wenn dieser Befehl fehlt, hast du den "Echo-Effekt" aus deinen Fotos!
    Paint_SetScale(4); 
    
    Paint_SelectImage(ImageBuffer);
    Paint_Clear(EPD_1IN54G_WHITE);
    
    // 3. Jetzt zeichnen (Die Farben 0-3 nutzen)
    // 0: Schwarz, 1: Weiß, 2: Gelb, 3: Rot (oder ähnlich, je nach Panel)
    Paint_DrawString_EN(10, 20, "PRILCHEN.DE", &Font16, 1, 0); // Hintergrund 1, Text 0
    
    // Teste die Farben direkt mit Zahlenwerten 0, 2, 3
    Paint_DrawRectangle(10, 50, 60, 100, 3, DOT_PIXEL_1X1, DRAW_FILL_FULL); // Farbe 3 (Rot?)
    Paint_DrawRectangle(70, 50, 120, 100, 2, DOT_PIXEL_1X1, DRAW_FILL_FULL); // Farbe 2 (Gelb?)
    Paint_DrawCircle(150, 75, 25, 0, DOT_PIXEL_1X1, DRAW_FILL_FULL); // Farbe 0 (Schwarz)

    Serial.println("Sende 10000 Bytes im 2-Bit Modus...");
    EPD_1IN54G_Display(ImageBuffer);

    EPD_1IN54G_Sleep();
    free(ImageBuffer);
    Serial.println("Fertig!");
}

void loop() {}