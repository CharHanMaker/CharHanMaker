% モータに12Vを入力した際のステップ応答と
% システムの時定数を求める

clear;

motor0 = tf(764.6, [1 47.76 466.8]);
motor1 = tf(501.5, [1 37.68 312.7]);
t = 0:0.05:1;
u = 12 * ones(size(t));

[y0, time] = lsim(motor0, u, t);
[y1, time] = lsim(motor1, u, t);

[num0, den0] = tfdata(motor0, 'v');
[num1, den1] = tfdata(motor1, 'v');

timeConstant0 = -1/real(den0(2));
timeConstant1 = -1/real(den1(2));

disp(['時定数0: ', num2str(timeConstant0)]);
disp(['時定数1: ', num2str(timeConstant1)]);

figure(1)
plot(time, y0);
hold on
plot(time, y1);
ylim([-2, 22]);
title('12Vを入力したときの応答');
ylabel('角速度 (rad/s)');
legend;
grid on
hold off