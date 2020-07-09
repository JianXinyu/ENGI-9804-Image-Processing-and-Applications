function H_out = lp_hp_filters(filter_type, lp_or_hp, P, Q, D0, n)
% The function calculates the LP or HP filters based on the parameters
% given. Filter_type=('ideal' or 'btw' or 'gaussian'), %lp_or_hp=('lp' or 'hp' for low pass or high pass), 
% P, Q are the padded image size, D0 is the cutoff frequency, and n for the
% order of the filter for btw filters
%  Developing frequency domain coordinates
u = 0:(P-1);
v = 0:(Q-1);

idx = find(u > P/2);
u(idx) = u(idx) - P;
idy = find(v > Q/2);
v(idy) = v(idy) - Q;
% Compute the meshgrid coordinates
[V, U] = meshgrid(v, u);
% Compute the istance matrix
D = sqrt(U.^2 + V.^2);

% Begin fiter computations.
switch filter_type
case 'ideal'
   H = double(D <=D0);
     if (strcmp(lp_or_hp,'lp'))        
      H_out=H;
   elseif (strcmp(lp_or_hp,'hp')) 
     H_out=1-H;   
   else
       error('Filter should be Low pass (lp) or High pass (hp).')       
    end
case 'btw'
    if (n==0)
        error('Butterworth Filter should have order (n)>0.')        
    end
      H = 1./(1 + (D./D0).^(2*n));
    if (strcmp(lp_or_hp,'lp'))        
      H_out=H;
   elseif (strcmp(lp_or_hp,'hp')) 
     H_out=1-H;   
   else
       error('Filter should be Low pass (lp) or High pass (hp).')
    end
case 'gaussian'
   H = exp(-(D.^2)./(2*(D0^2)));
   if (strcmp(lp_or_hp,'lp'))        
      H_out=H;
   elseif (strcmp(lp_or_hp,'hp')) 
     H_out=1-H;   
   else
       error('Filter should be Low pass (lp) or High pass (hp).')       
    end
otherwise
   error('Unknown filter type.')
   end