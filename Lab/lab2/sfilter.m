function output = sfilter(w, f)
%SFILTER spatial filger
%   take an image and the kernel as inputs 
%   and return the filtered image

[M, N] = size(f);
[m, n] = size(w);
a = (m-1)/2;
b = (n-1)/2;
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
               % correlation
               output(i,j) = output(i,j)+ w(k,l)*f_pad(i+k-1, j+l-1);
                % convolution
%                output(i,j) = output(i,j)+ w(k,l)*f_pad(i+m-k, j+n-l);
           end
       end
   end
end

end

