function output = mfilter(m,n,f)
%MFILTER median filter
%   replace the pixel intensity with the median intensity 
%   of the pixels overlapped with the kernel.
[M, N] = size(f);
a = (m-1)/2;
b = (n-1)/2;
mid = (m*n-1)/2;
kernel = zeros(m,n);
output = zeros(M,N);
% size of zero-padding image
Sv = M+m-1;
Sh = N+n-1;
f_pad = zeros(Sv, Sh);
for i = 1:M
   for j = 1:N
      f_pad(i+a,j+b) = f(i,j);  
   end
end


for i = 1:M
   for j = 1:N
       for k = 1:m
           for l = 1:n
               kernel(k, l) = f_pad(i+k-1, j+l-1);
           end
       end
       array = kernel(:); % transform the matrix into an array
       array = sort(array); % sort the array
       output(i,j) = array(mid);
   end
end
end

