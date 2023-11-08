syms s a b Kp Kd Ki
F = (b*Kd*s^2 + b*Kp*s + b*Ki)/((1+b*Kd)*s^2 + (a+b*Kp)*s + b*Ki);
f = ilaplace(F)

%(exp(-(t*(a + Kp*b))/(2*Kd*b + 2))*(cosh((t*((Kp^2*b^2)/4 + (Kp*a*b)/2 + a^2/4 - Kd*Ki*b^2 - Ki*b)^(1/2))/(Kd*b + 1)) - (sinh((t*((Kp^2*b^2)/4 + (Kp*a*b)/2 + a^2/4 - Kd*Ki*b^2 - Ki*b)^(1/2))/(Kd*b + 1))*(Kd*b + 1)*((a + Kp*b)/(2*Kd*b + 2) - (Ki*b)/(Kp*b - Kd*a*b)))/((Kp^2*b^2)/4 + (Kp*a*b)/2 + a^2/4 - Kd*Ki*b^2 - Ki*b)^(1/2))*(Kp*b - Kd*a*b))/(Kd*b + 1)^2 + (Kd*b*dirac(t))/(Kd*b + 1)
 