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
        shaftAngleArray[i] = 0;
        shaftAnglePrev[i] = 0;
        shaftAngleZero[i] = 0;
        isCW[i] = true;
    }
}

float MultipleAS5600::readDegree(uint8_t _sensorNumber) { // returns [deg]
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
        angle_f = (float)rawAngle * 0.087890625; // 12bit -> DEGREE(0~360)
        shaftAnglePrev[_sensorNumber] = shaftAngleArray[_sensorNumber]; // [DEG]
        shaftAngleArray[_sensorNumber] = MyMath::gapDegrees(shaftAngleZero[_sensorNumber], angle_f) * (isCW[_sensorNumber] ? 1 : -1);
    } else {
        return 361; // error
    }
    return angle_f;
}

float MultipleAS5600::readRadian(uint8_t _sensorNumber) { // returns [rad]
    return readDegree(_sensorNumber) * DEG_TO_RAD;
}

float MultipleAS5600::getVelocity(uint8_t _sensorNumber) { // returns [rad/s]
    if (_sensorNumber > 7) _sensorNumber = 7;
    if (shaftAngleArray[_sensorNumber] == 361) return 361; // error
    float shaftAngle = shaftAngleArray[_sensorNumber];        // [deg]
    float dt = (float)velTimer[_sensorNumber].read_us() / 1000000;
    velTimer[_sensorNumber].reset();
    float angleDiff = (shaftAnglePrev[_sensorNumber] - shaftAngle) * DEG_TO_RAD;// angleDiff[rad]
    if (angleDiff > PI) {
        angleDiff -= 2 * PI;
    } else if (angleDiff < -PI) {
        angleDiff += 2 * PI;
    }
    float angularVelocity = angleDiff / dt; // [rad/s]
    if (abs(angleDiff) > PI && angularVelocity != 0) {
        angularVelocity += (angularVelocity > 0) ? -2 * PI / dt : 2 * PI / dt;
    }
    angularVelocity = velocityLPF[_sensorNumber].update(angularVelocity); // [rad/s]
    return angularVelocity;
}

void MultipleAS5600::readDegreeAll(uint16_t *_shaftAngleArray) {
    for (size_t i = 0; i < sensorQty; i++) {
        _shaftAngleArray[i] = readDegree(i);
    }
}

// :TODO: @ryoskRFR チェックプリーズ
float MultipleAS5600::getContinuousDegree(uint8_t _sensorNumber) {
    if (_sensorNumber > 7) _sensorNumber = 7;
    if (shaftAngleArray[_sensorNumber] == 361) return 361; // error
    float shaftAngle = shaftAngleArray[_sensorNumber];        // [deg]
    float angleDiff = shaftAnglePrev[_sensorNumber] - shaftAngle; // [deg]

    if (angleDiff > 180) {
        angleDiff = 360 - angleDiff;
    } else if (angleDiff < -180) {
        angleDiff = 360 + angleDiff;
    }
    continuousAngle[_sensorNumber] = continuousAngle[_sensorNumber] + angleDiff; // [deg

    return continuousAngle[_sensorNumber];
}

// :TODO: @ryoskRFR チェックプリーズ
float MultipleAS5600::getContinuousRadian(uint8_t _sensorNumber) {
    return getContinuousDegree(_sensorNumber) * DEG_TO_RAD;
}


// CW　正回転 CCW 逆回転
void MultipleAS5600::setDirection(uint8_t _sensorNumber, bool cw) {
    isCW[_sensorNumber] = cw;
}

void MultipleAS5600::setZero(uint8_t _sensorNumber) {
    if (_sensorNumber > 7) _sensorNumber = 7;
    shaftAngleZero[_sensorNumber] = readDegree(_sensorNumber);
}
