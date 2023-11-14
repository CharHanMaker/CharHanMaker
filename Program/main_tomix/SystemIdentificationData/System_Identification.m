% G0_0  56.85%

% G1_0  72.57%
%  -5.638
% ---------
% s + 3.039

% G1_1  75.24%
%   -0.5607 s - 3.955
%   -----------------
%       s + 2.081

% G2_0  72.2 %
%       -409
% ---------------------
% s^2 + 72.38 s + 221.2

% G2_1  74.48%
%     -12.13 s - 74.43
%   ---------------------
%   s^2 + 19.33 s + 39.46

% G3_0  71.35%

% G3_1  78.25%
%            -1.97e07 s - 1.829e07
%   ---------------------------------------
%   s^3 + 219.2 s^2 + 1.343e07 s + 7.726e06

% G3_2  71.36%
%     -17.4 s^2 + 2.96 s - 21.95
%   ------------------------------
%   s^3 + 10 s^2 + 1.216 s + 12.16

clear;
clc;
file = "SIdata_1_average4_2023-11-07_controlable.csv";

FR.u = readmatrix(file, "Range", [2 2 6297 2]); % Volt
FR.y = readmatrix(file, "Range", [2 3 6297 3]); % omega

FRdata = iddata(FR.y, FR.u, 0.005);
FRdata.InputName = '電圧 [V]';
FRdata.OutputName = '角速度 [rad/s]';
% figure(1);
% plot(FRdata);

FRdata_det = detrend(FRdata);%データの平均値を0にする

% G0_0 = tfest(FRdata_det, 0, 0);%極が0、零が0のモデルを同定
G1_0 = tfest(FRdata_det, 1, 0);%極が1、零が0のモデルを同定
G1_1 = tfest(FRdata_det, 1, 1);%極が1、零が1のモデルを同定
% G2_0 = tfest(FRdata_det, 2, 0);%極が2、零が0のモデルを同定
% G2_1 = tfest(FRdata_det, 2, 1);%極が2、零が1のモデルを同定
% G3_0 = tfest(FRdata_det, 3, 0);%極が3、零が0のモデルを同定
G3_1 = tfest(FRdata_det, 3, 1)%極が3、零が0のモデルを同定
G3_2 = tfest(FRdata_det, 3, 2)%極が3、零が0のモデルを同定

figure(2), compare(FRdata,G1_0, G1_1, G3_1, G3_2);%モデルの応答比較の表示

% figure(3), bode(G1_0);
% figure(4), bode(G1_1);
% figure(5), bode(G2_1);