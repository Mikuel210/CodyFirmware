#pragma once

#include <Arduino.h>
#include <Wire.h>

// Integration time: actual_ms = (256 - value) * 2.4
// Common presets:
#define TCS_ATIME_2MS    0xFF  //   2.4 ms
#define TCS_ATIME_24MS   0xF6  //  24.0 ms
#define TCS_ATIME_103MS  0xD5  // 103.2 ms  (default)
#define TCS_ATIME_154MS  0xC0  // 153.6 ms
#define TCS_ATIME_700MS  0x00  // 614.4 ms

// Gain options:
#define TCS_GAIN_1X   0x00  // (default)
#define TCS_GAIN_4X   0x01
#define TCS_GAIN_16X  0x02
#define TCS_GAIN_60X  0x03

struct RGBC {
    uint16_t r;
    uint16_t g;
    uint16_t b;
    uint16_t c;
    uint16_t colorTemp;
    uint16_t lux;
    bool     valid;
};

struct RGBCNorm {
    double r;
    double g;
    double b;
    double c;
    bool   valid;
};

class TCS_Clone {
public:
    // address: 0x29 is the standard TCS34725 address
    TCS_Clone(uint8_t address = 0x29,
            uint8_t atime   = TCS_ATIME_103MS,
            uint8_t gain    = TCS_GAIN_1X);

    // Pass a custom TwoWire instance if not using the default Wire
    bool begin(TwoWire &wire = Wire);

    RGBC read();

    void setGain(uint8_t gain);
    void setIntegrationTime(uint8_t atime);  // use TCS_ATIME_* constants

    // Enable/disable the onboard LED if your breakout has one wired to INT
    void setLED(bool on);

    uint8_t chipID() const { return _chipID; }

    void calibrateBlack(RGBC reading);
    void calibrateWhite(RGBC reading);
    RGBCNorm readNormalised();

private:
    uint8_t  _addr;
    uint8_t  _atime;
    uint8_t  _gain;
    uint8_t  _chipID;
    TwoWire *_wire;

    uint16_t _blackR, _blackG, _blackB, _blackC;
    uint16_t _whiteR, _whiteG, _whiteB, _whiteC;

    void     writeReg(uint8_t reg, uint8_t val);
    uint8_t  readReg(uint8_t reg);
    uint16_t readWord(uint8_t reg);
    uint16_t integrationMs() const;
};
