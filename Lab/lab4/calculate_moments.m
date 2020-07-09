function [Mpq,MUpq,NUpq]=calculate_moments(f,p,q)
% Calculate Hu's moment invariants
m=size(f,1); n=size(f,2); % get the size of the image
M00=0;M01=0;M10=0;

for i=1:m,
    for j=1:n,
        M00=M00 +double( f(i,j));     
        M10=M10 +double( i*f(i,j));
        M01=M01 + double(j*f(i,j));
    end
end
x_bar=M10/M00; y_bar=M01/M00; % coordinate of the centroid
% Use them to calculate the central moments
MU00=M00;
MU11=0;MU12=0;MU20=0;MU02=0; MU21=0;MU30=0;MU03=0;
Mpq=0;MUpq=0;NUpq=0;

for i=1:m,
    for j=1:n,
       MUpq=MUpq+double((i-x_bar)^(p)*(j-y_bar)^(q)*(f(i,j)));  
       Mpq=Mpq+double((i)^p*(j)^q*f(i,j));
    end
end
gamma=(p+q)/2 +1;

NUpq=MUpq/double((MU00)^gamma);