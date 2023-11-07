% G1 72.57%
%  -14.1
% ---------
% s + 7.598

% G2 72.2 %
%       -2556
% ------------------
% s^2 + 181 s + 1382

% G3 71.35 %
%              -6994
% -------------------------------
% s^3 + 8.02 s^2 + 502.9 s + 3808

clear;
clc;
% file = "./Code/CharHanMaker/Program/main_tomix/SystemIdentificationData/SIdata_1_average4_2023-11-07.csv";
file = "SIdata_1_average4_2023-11-07_controlable.csv";

FR.u = readmatrix(file, "Range", [2 2 6297 2]); % Volt
FR.y = readmatrix(file, "Range", [2 3 6297 3]); % omega

FRdata = iddata(FR.y, FR.u, 0.002);
FRdata.InputName = '電圧 [V]';
FRdata.OutputName = '角速度 [rad/s]';
figure(1);
plot(FRdata);

FRdata_det = detrend(FRdata);%データの平均値を0にする

G1 = tfest(FRdata_det, 1, 0)%極が1、零が0のモデルを同定
G2 = tfest(FRdata_det, 2, 0)%極が2、零が0のモデルを同定
G3 = tfest(FRdata_det, 3, 0)%極が3、零が0のモデルを同定

figure(2), compare(FRdata,G1, G2, G3);%モデルの応答比較の表示