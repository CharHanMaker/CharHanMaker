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
        valueArray[i] = 0;
        valuePrev[i] = 0;
        valueZero[i] = 0;
        isCW[i] = true;
    }
}

float MultipleAS5600::readRawValue(uint8_t _sensorNumber){
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
    uint16_t rawValue = 0;
    if (Wire.available()) {
        uint8_t highByte = Wire.read();
        uint8_t lowByte  = Wire.read();
        rawValue  = (0x0F & highByte) << 8 | lowByte;
        // 回転数カウント
        if(rawValue - valuePrev[_sensorNumber] > 2048) count[_sensorNumber]--;
        else if(rawValue - valuePrev[_sensorNumber] < -2048) count[_sensorNumber]++;
        valuePrev[_sensorNumber] = rawValue;
    } else {
        return 4097; // error
    }
    return rawValue;
}

uint16_t MultipleAS5600::read12BitValue(uint8_t _sensorNumber) {
    if (_sensorNumber > 7) _sensorNumber = 7;
    uint16_t value = valueZero[_sensorNumber] - readRawValue(_sensorNumber);
    if (value < 0) value += 4096;
    return value;
}

float MultipleAS5600::readDegree(uint8_t _sensorNumber) { // returns [deg]
    return read12BitValue(_sensorNumber) * BIT_12_TO_DEGREE;
}

float MultipleAS5600::readRadian(uint8_t _sensorNumber) { // returns [rad]
    return read12BitValue(_sensorNumber) * BIT_12_TO_RADIAN;
}

float MultipleAS5600::getVelocity(uint8_t _sensorNumber) { // returns [rad/s]
    if (_sensorNumber > 7) _sensorNumber = 7;
    if (valueArray[_sensorNumber] == 4097) return 4097; // error
    float value = valueArray[_sensorNumber];        // [deg]
    float dt = (float)velTimer[_sensorNumber].read_us() / 1000000;
    velTimer[_sensorNumber].reset();
    float radDiff = (valuePrev[_sensorNumber] - value) * BIT_12_TO_RADIAN;// angleDiff[rad]
    if (radDiff > PI) {
        radDiff -= 2 * PI;
    } else if (radDiff < -PI) {
        radDiff += 2 * PI;
    }
    float angularVelocity = radDiff / dt; // [rad/s]
    if (abs(radDiff) > PI && angularVelocity != 0) {
        angularVelocity += (angularVelocity > 0) ? -2 * PI / dt : 2 * PI / dt;
    }
    angularVelocity = velocityLPF[_sensorNumber].update(angularVelocity); // [rad/s]
    return angularVelocity;
}

// :TODO: @ryoskRFR チェックプリーズ
float MultipleAS5600::getContinuousDegree(uint8_t _sensorNumber) {
 return 0 ;
}

// :TODO: @ryoskRFR チェックプリーズ
float MultipleAS5600::getContinuousRadian(uint8_t _sensorNumber) {
    return getContinuousDegree(_sensorNumber) * DEG_TO_RAD;
}


// CW　正回転 CCW 逆回転
void MultipleAS5600::setDirection(uint8_t _sensorNumber, bool cw) {
    isCW[_sensorNumber] = cw;
}

float MultipleAS5600::setZero(uint8_t _sensorNumber) {
    if (_sensorNumber > 7) _sensorNumber = 7;
    valueZero[_sensorNumber] = readRawValue(_sensorNumber);
    return valueZero[_sensorNumber];
}
