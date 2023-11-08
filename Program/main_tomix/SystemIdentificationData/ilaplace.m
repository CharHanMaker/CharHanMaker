syms s a b Kp Kd Ki
F = (b*Kd*s^2 + b*Kp*s + b*Ki)/((1+b*Kd)*s^2 + (a+b*Kp)*s + b*Ki);
f = ilaplace(F)