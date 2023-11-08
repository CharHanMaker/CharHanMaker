syms s
a = 3.039;
b = -5.638;
Kp = 1;
Ki = 0.5;
Kd = 1;

F = (b*Kd*s^2 + b*Kp*s + b*Ki)/((1+b*Kd)*s^2 + (a+b*Kp)*s + b*Ki);
f = ilaplace(F)