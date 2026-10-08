#include "TCS_Clone.h"

// ── Register map (matches TCS34725 layout) ──────────────────────────────────
#define REG_ENABLE   0x00
#define REG_ATIME    0x01
#define REG_CONTROL  0x0F
#define REG_ID       0x12
#define REG_STATUS   0x13
#define REG_CDATAL   0x14   // Clear low; followed by CDATAH, RDATAL … BDATAH

// ENABLE register bits
#define PON  0x01   // Power ON
#define AEN  0x02   // RGBC ADC Enable

// I2C command byte flags
#define CMD_BIT        0x80
#define CMD_AUTO_INC   0x20

// STATUS register
#define STATUS_AVALID  0x01

// Accepted chip IDs
#define ID_TCS34725    0x44
#define ID_TCS34727    0x10
#define ID_CLONE_19    0x19   // the clone this driver was written for

// ── Constructor ─────────────────────────────────────────────────────────────
TCS_Clone::TCS_Clone(uint8_t address, uint8_t atime, uint8_t gain)
    : _addr(address), _atime(atime), _gain(gain), _chipID(0), _wire(nullptr)
{}

// ── Public API ───────────────────────────────────────────────────────────────
bool TCS_Clone::begin(TwoWire &wire) {
    _wire = &wire;
    _wire->begin();

    _chipID = readReg(REG_ID);
    if (_chipID != ID_TCS34725 &&
        _chipID != ID_TCS34727 &&
        _chipID != ID_CLONE_19) {
        return false;
    }

    // Power on, then enable ADC
    writeReg(REG_ENABLE, PON);
    delay(3);
    writeReg(REG_ENABLE, PON | AEN);
    writeReg(REG_ATIME,  _atime);
    writeReg(REG_CONTROL, _gain);

    // Wait for the first integration cycle to complete
    delay(integrationMs() + 2);
    return true;
}

RGBC TCS_Clone::read() {
    RGBC out = {};

    // Just wait for AVALID — don't restart the integration cycle
    uint32_t deadline = millis() + integrationMs() * 2;
    while (!(readReg(REG_STATUS) & STATUS_AVALID)) {
        if (millis() > deadline) { out.valid = false; return out; }
        delay(2);
    }

    // Single burst read — all 8 RGBC bytes atomically
    _wire->beginTransmission(_addr);
    _wire->write(CMD_BIT | CMD_AUTO_INC | REG_CDATAL);
    _wire->endTransmission(false);
    _wire->requestFrom(_addr, (uint8_t)8);

    out.c = (uint16_t)_wire->read() | ((uint16_t)_wire->read() << 8);
    out.r = (uint16_t)_wire->read() | ((uint16_t)_wire->read() << 8);
    out.g = (uint16_t)_wire->read() | ((uint16_t)_wire->read() << 8);
    out.b = (uint16_t)_wire->read() | ((uint16_t)_wire->read() << 8);
    out.valid = true;

    // Colour temp and lux omitted if sensor is 8-bit only — 
    // McCamy approximation needs more headroom to be meaningful
    return out;
}

void TCS_Clone::setGain(uint8_t gain) {
    _gain = gain;
    writeReg(REG_CONTROL, _gain);
}

void TCS_Clone::setIntegrationTime(uint8_t atime) {
    _atime = atime;
    writeReg(REG_ATIME, _atime);
    delay(integrationMs() + 2);
}

void TCS_Clone::setLED(bool on) {
    // The INT pin doubles as LED control on some breakouts.
    // This toggles it via the interrupt enable bit in ENABLE register.
    uint8_t en = readReg(REG_ENABLE);
    if (on) en &= ~0x10;   // clear AIEN to let INT float high (LED on)
    else    en |=  0x10;   // set AIEN pulls INT low (LED off)
    writeReg(REG_ENABLE, en);
}

void TCS_Clone::calibrateBlack(RGBC r) {
    _blackR = r.r; _blackG = r.g; _blackB = r.b; _blackC = r.c;
}

void TCS_Clone::calibrateWhite(RGBC r) {
    _whiteR = r.r; _whiteG = r.g; _whiteB = r.b; _whiteC = r.c;
}

RGBCNorm TCS_Clone::readNormalised() {
    RGBC raw = read();
    RGBCNorm out;
    out.valid = raw.valid;

    auto norm = [](uint16_t v, uint16_t lo, uint16_t hi) -> float {
        if (hi <= lo) return 0.0f;
        float n = (float)(v - lo) / (float)(hi - lo);
        return constrain(n, 0.0f, 1.0f);
    };

    out.r = norm(raw.r, _blackR, _whiteR);
    out.g = norm(raw.g, _blackG, _whiteG);
    out.b = norm(raw.b, _blackB, _whiteB);
    out.c = norm(raw.c, _blackC, _whiteC);
    return out;
}

// ── Private helpers ──────────────────────────────────────────────────────────
void TCS_Clone::writeReg(uint8_t reg, uint8_t val) {
    _wire->beginTransmission(_addr);
    _wire->write(CMD_BIT | reg);
    _wire->write(val);
    _wire->endTransmission();
}

uint8_t TCS_Clone::readReg(uint8_t reg) {
    _wire->beginTransmission(_addr);
    _wire->write(CMD_BIT | reg);
    _wire->endTransmission(false);
    _wire->requestFrom(_addr, (uint8_t)1);
    return _wire->read();
}

uint16_t TCS_Clone::readWord(uint8_t reg) {
    _wire->beginTransmission(_addr);
    _wire->write(CMD_BIT | CMD_AUTO_INC | reg);
    _wire->endTransmission(false);
    _wire->requestFrom(_addr, (uint8_t)2);
    uint16_t lo = _wire->read();
    uint16_t hi = _wire->read();
    return (hi << 8) | lo;
}

uint16_t TCS_Clone::integrationMs() const {
    return (uint16_t)((256 - _atime) * 2.4f) + 1;
}
