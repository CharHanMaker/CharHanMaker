#include <Robot.hpp>

timer Time;
MultipleAS5600 AbsEncorders(Wire, 0x70, 2);

void setup() {
    Wire.setClock(400000); // I2C の通信速度 : 400k Hz
    Wire.begin();
    Serial.begin(9600);
    AbsEncorders.begin();
}

void loop() {
    int angle1, angle2, time;
    angle1 = AbsEncorders.readDegree(0);     // 0番目のエンコーダの角度を取得
    angle2 = AbsEncorders.readDegree(1);     // 1番目のエンコーダの角度を取得
    float vel = AbsEncorders.getVelocity(1); // 1番目のエンコーダの角速度を取得[rad/s]
    time = Time.read_us(); Time.reset(); //
    Serial.printf("%d, %d, vel:%.2f, time:%dus\n", angle1, angle2, vel, time);
}
