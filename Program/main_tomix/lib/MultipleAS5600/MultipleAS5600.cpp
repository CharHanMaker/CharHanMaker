#include "MultipleAS5600.hpp"

MultipleAS5600::MultipleAS5600(TwoWire &_i2c, uint8_t _muxAddress = MUX_ADDR, uint8_t _sensorQty = 2)
    : muxAddress(MUX_ADDR),
      sensorQty(_sensorQty),
      i2c(&_i2c),
      prevPort(0) {
}

void MultipleAS5600::begin() {
    for (size_t i = 0; i < 8; i++) {
        velTimer[i].reset();
    }
}

float MultipleAS5600::readDegree(uint8_t _sensorNumber) {
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
    float angle_f = 0;
    if (Wire.available()) {
        uint8_t angle_h = Wire.read();
        uint8_t angle_l = Wire.read();
        uint16_t rawAngle = (0x0F & angle_h) << 8 | angle_l;
        angle_f = (float)rawAngle * 0.087890625; // 12bit -> 360.00deg
        angleArray[_sensorNumber] = angle_f;
    } else {
        return 361; // error
    }
    return angle_f;
}

float MultipleAS5600::getVelocity(uint8_t _sensorNumber) {
    if (_sensorNumber > 7) _sensorNumber = 7;
    if (angleArray[_sensorNumber] == 361) return 361;       // error
    float shAngle = DEG_TO_RAD * angleArray[_sensorNumber]; // [rad]
    float dt = (float)velTimer[_sensorNumber].read_us() / 1000000;
    velTimer[_sensorNumber].reset();
    float angleDiff = shAnglePrev[_sensorNumber] - shAngle;
    if (angleDiff > PI) {
        angleDiff -= 2 * PI;
    } else if (angleDiff < -PI) {
        angleDiff += 2 * PI;
    }
    float angularVelocity = angleDiff / dt;
    if (abs(angleDiff) > PI && angularVelocity != 0) {
        angularVelocity += (angularVelocity > 0) ? -2 * PI / dt : 2 * PI / dt;
    }
    angularVelocity = velocityLPF[_sensorNumber].update(angularVelocity);
    shAnglePrev[_sensorNumber] = shAngle;
    return angularVelocity;
}
