% G2 : 94.76% motor0
%          -501.5
%   ---------------------
%   s^2 + 37.68 s + 312.7

% G2 : 95.53% motor1
%          -764.6
%   ---------------------
%   s^2 + 47.76 s + 466.8

clear;
clc;
% file = "./Code/CharHanMaker/Program/main_tomix/SystemIdentificationData/SIdata_1_average4_2023-11-07.csv";
file = "SIdata_1_2023-12-23.csv";

FR.u = readmatrix(file, "Range", [2 2 6297 2]); % Volt
FR.y = readmatrix(file, "Range", [2 4 6297 4]); % omega

FR1.u = readmatrix(file, "Range",[2 2 6297 2]);
FR1.y = readmatrix(file, "Range",[2 6 6297 6]);

FRdata = iddata(FR.y, FR.u, 0.005);
FRdata.InputName = '電圧 [V]';
FRdata.OutputName = '角速度 [rad/s]';
% figure(1);
% plot(FRdata);

FRdata1 = iddata(FR1.y, FR1.u, 0.005);
FRdata1.InputName = '電圧 [V]';
FRdata1.OutputName = '角速度 [rad/s]';
figure(1);
plot(FRdata1);

FRdata_det = detrend(FRdata);%データの平均値を0にする

G1 = tfest(FRdata_det, 1, 0)%極が1、零が0のモデルを同定
G2 = tfest(FRdata_det, 2, 0)%極が2、零が0のモデルを同定
G21 = tfest(FRdata_det, 2, 1)%極が2、零が1のモデルを同定
G3 = tfest(FRdata_det, 3, 0)%極が3、零が0のモデルを同定
G31 = tfest(FRdata_det, 3, 1)%極が3、零が1のモデルを同定
G32 = tfest(FRdata_det, 3, 2)%極が3、零が2のモデルを同定

figure(2), compare(FRdata, G1, G2, G21, G3, G31, G32);%モデルの応答比較の表示

% figure(3), bode(G1);
% figure(4), bode(G2);