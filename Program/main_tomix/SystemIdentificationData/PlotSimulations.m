clear;

file1 = 'Synchro_nishi.slx';
file2 = 'Synchro_tommy.slx';
file3 = 'TDoFCS.slx';

sim(file1);

targetAngleVel = synchro_vel.Data(:,1);
vel0 = synchro_vel.Data(:,2);
vel1 = synchro_vel.Data(:,3);

rad0 = synchro_rad.Data(:,1);
rad1 = synchro_rad.Data(:,2);
angleError = synchro_rad.Data(:,3);

figure(1)
plot(tout, targetAngleVel);
hold on
plot(tout, vel1);
plot(tout, vel0);
ylim([-2, 22]);
xlabel('時間 [s]');
ylabel('角速度 [rad/s]');
legend("目標角速度", "モータAの角速度", "モータBの角速度");
title('目標角速度とモータの角速度');
grid on
hold off

figure(2)
plot(tout, rad1);
hold on
plot(tout, rad0);
plot(tout, angleError);
ylim([-20, 180]);
xlabel('時間 [s]');
ylabel('角 [rad]');
legend("モータAの角", "モータBの角", "2モータの同期偏差");
title('2モータの同期');
grid on
zp = BaseZoom();
zp.plot();
hold off