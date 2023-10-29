#include "MultipleAS5600.hpp"


MultipleAS5600::MultipleAS5600(TwoWire &_i2c, uint8_t _muxAddress = MUX_ADDR, uint8_t _sensorQty = 2)
    : muxAddress(MUX_ADDR),
      sensorQty(_sensorQty),
      i2c(&_i2c),
      prevPort(0) {
}

void MultipleAS5600::begin() {
    // なんか後で書くか
}

uint16_t MultipleAS5600::readDegree(uint8_t _sensorNumber) {
    // program Mux to read from correct port(0-7)
    if (_sensorNumber > 7) _sensorNumber = 7;

    // 前回と同じポートだったらMUXにアドレス更新をしない
    if (prevPort != _sensorNumber) {
        i2c->beginTransmission(muxAddress);
        i2c->write(1 << _sensorNumber); // それぞれのビットに通信したポート情報を書きこむ
        i2c->endTransmission();
    }
    prevPort = _sensorNumber;

    // read from AS5600
    Wire.beginTransmission(AS5600_ADDR);
    Wire.write(0x0C);
    Wire.endTransmission(false);
    Wire.requestFrom(0x36, 2);
    //     delayMicroseconds(10);
    uint16_t rawAngle = 0;
    if (Wire.available()) {
        uint8_t angle_h = Wire.read();
        uint8_t angle_l = Wire.read();
        rawAngle = (0x0F & angle_h) << 8 | angle_l;
        rawAngle = rawAngle * 360 / 4096; // 12bit -> 360deg
    } else {
        return 9999; // error
    }
    return rawAngle;
}