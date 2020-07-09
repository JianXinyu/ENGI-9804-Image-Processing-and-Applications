%function Moment_invariants
function Moment_invariants(I)

f=im2double(I);
 % pass the image to a function to calculate the moments
[M20,MU20,NU20]=calculate_moments(f,2,0);
[M02,MU02,NU02]=calculate_moments(f,0,2);
[M11,MU11,NU11]=calculate_moments(f,1,1);
[M21,MU21,NU21]=calculate_moments(f,2,1);
[M12,MU12,NU12]=calculate_moments(f,1,2);
[M03,MU03,NU03]=calculate_moments(f,0,3);
[M30,MU30,NU30]=calculate_moments(f,3,0);

phi1= NU20+NU02;
phi2=(NU20-NU02)^2+(2*NU11)^2;
phi3=(NU30-3*NU12)^2+(3*NU21-NU03)^2;
phi4=(NU30+NU12)^2+(NU21+NU03)^2;
phi5=(NU30-3*NU12)*(NU30+NU12)*((NU30+NU12)^2 -3*(NU21+NU03)^2)+(3*NU21-NU03)*(NU21+NU03)*(3*(NU30+NU12)^2-(NU21+NU03)^2);
phi6=(NU20-NU02)*((NU30+NU12)^2-(NU21+NU03)^2) + 4*NU11*(NU30+NU12)*(NU21+NU03);
phi7= (3*NU21-NU03)*(NU30+NU12)*( (NU30+NU12)^2-3*(NU21+NU03)^2) + (3*NU12-NU30)*(NU21+NU03)*(3*(NU30+NU12)^2-(NU21+NU03)^2);

Moment_Invariants=[sign(phi1)*log10(abs(phi1)) sign(phi2)*log10(abs(phi2)) sign(phi3)*log10(abs(phi3)) sign(phi4)*log10(abs(phi4)) sign(phi5)*log10(abs(phi5)) sign(phi6)*log10(abs(phi6)) sign(phi7)*log10(abs(phi7))]