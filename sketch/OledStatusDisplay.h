#ifndef OLED_STATUS_DISPLAY_H
#define OLED_STATUS_DISPLAY_H

/**
 * @file OledStatusDisplay.h
 * @brief SSD1306 0.96" 128x64 I2C OLED with the team's layout:
 *        big "RESQ" title, a separator line and 4 small text lines below.
 *
 * The Device writes the lines with setLine() / setTag() and then calls render()
 * to draw them all at once.
 *
 * @author ResQ IoT development team
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/**
 * @class OledStatusDisplay
 * @brief Shows the title, a small corner tag and BODY_LINE_COUNT lines of text.
 */
class OledStatusDisplay {
public:
    static constexpr int SCREEN_WIDTH_PIXELS = 128;
    static constexpr int SCREEN_HEIGHT_PIXELS = 64;
    static constexpr int8_t NO_RESET_PIN = -1;        ///< the module has no reset pin
    static constexpr int CHAR_WIDTH_PIXELS = 6;       ///< text size 1 = 6 x 8 pixels
    static constexpr int LINE_HEIGHT_PIXELS = 8;
    static constexpr uint8_t TITLE_TEXT_SIZE = 2;
    static constexpr uint8_t BODY_TEXT_SIZE = 1;
    static constexpr int TITLE_Y_PIXELS = 5;          ///< same position as the team's code
    static constexpr int SEPARATOR_Y_PIXELS = 28;
    static constexpr int BODY_FIRST_LINE_Y_PIXELS = 31;
    static constexpr int BODY_LINE_COUNT = 4;
    static constexpr int LINE_SIZE = 22;              ///< 21 characters fit in 128 px + '\0'
    static constexpr int TAG_SIZE = 5;                ///< 4 characters + '\0'

    /**
     * @brief Builds the display object (the screen is started later with begin()).
     * @param i2cAddress I2C address of the module (0x3C in the team's module).
     * @param sdaPin I2C data pin.
     * @param sclPin I2C clock pin.
     * @param title Big text on top (for example "RESQ").
     */
    OledStatusDisplay(uint8_t i2cAddress, int sdaPin, int sclPin, const char* title)
        : screen(SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS, &Wire, NO_RESET_PIN),
          i2cAddress(i2cAddress), sdaPin(sdaPin), sclPin(sclPin), title(title),
          isReady(false), tag{}, lines{} {}

    /**
     * @brief Starts the I2C bus and the screen. If the screen fails, the rest keeps working.
     * @return true when the screen answered.
     */
    bool begin() {
        Wire.begin(sdaPin, sclPin);
        isReady = screen.begin(SSD1306_SWITCHCAPVCC, i2cAddress);
        if (isReady) {
            screen.clearDisplay();
            screen.setTextColor(SSD1306_WHITE);
            screen.display();
        }
        return isReady;
    }

    /** @brief Empties every body line. */
    void clearLines() {
        for (int lineIndex = 0; lineIndex < BODY_LINE_COUNT; lineIndex++) {
            lines[lineIndex][0] = '\0';
        }
    }

    /**
     * @brief Copies a text into one body line (it is drawn on the next render()).
     * @param lineIndex Line number, from 0 to BODY_LINE_COUNT - 1.
     * @param text Text to show; it is cut if it is longer than the screen.
     */
    void setLine(int lineIndex, const char* text) {
        if (lineIndex < 0 || lineIndex >= BODY_LINE_COUNT) return;
        snprintf(lines[lineIndex], LINE_SIZE, "%s", text);
    }

    /**
     * @brief Small text in the top-right corner (for example "SIM" or "REAL").
     * @param text Up to 4 characters.
     */
    void setTag(const char* text) { snprintf(tag, TAG_SIZE, "%s", text); }

    /** @brief Draws title, tag, separator and body lines. */
    void render() {
        if (!isReady) return;
        screen.clearDisplay();

        screen.setTextSize(TITLE_TEXT_SIZE);
        int titleWidthPixels = strlen(title) * CHAR_WIDTH_PIXELS * TITLE_TEXT_SIZE;
        screen.setCursor((SCREEN_WIDTH_PIXELS - titleWidthPixels) / 2, TITLE_Y_PIXELS);
        screen.print(title);

        screen.setTextSize(BODY_TEXT_SIZE);
        int tagWidthPixels = strlen(tag) * CHAR_WIDTH_PIXELS;
        screen.setCursor(SCREEN_WIDTH_PIXELS - tagWidthPixels, 0);
        screen.print(tag);

        screen.drawLine(0, SEPARATOR_Y_PIXELS, SCREEN_WIDTH_PIXELS - 1, SEPARATOR_Y_PIXELS, SSD1306_WHITE);

        for (int lineIndex = 0; lineIndex < BODY_LINE_COUNT; lineIndex++) {
            screen.setCursor(0, BODY_FIRST_LINE_Y_PIXELS + lineIndex * LINE_HEIGHT_PIXELS);
            screen.print(lines[lineIndex]);
        }
        screen.display();
    }

private:
    Adafruit_SSD1306 screen;
    uint8_t i2cAddress;
    int sdaPin;
    int sclPin;
    const char* title;
    bool isReady;
    char tag[TAG_SIZE];
    char lines[BODY_LINE_COUNT][LINE_SIZE];
};

#endif // OLED_STATUS_DISPLAY_H
