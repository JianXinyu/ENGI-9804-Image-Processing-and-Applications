function H_out = band_filter(filter_type, br_or_bp, P, Q, D0, W, n)
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
% Compute the distance matrix
D = sqrt(U.^2 + V.^2);

% Begin fiter computations.
switch filter_type
    
    case 'ideal'
        H = double(D < (D0 - W/2) | D > (D0 + W/2));
        
        if (strcmp(br_or_bp,'br'))
            H_out=H;
        elseif (strcmp(br_or_bp,'bp'))
            H_out=1-H;
        else
            error('Filter should be band reject (br) or band pass (bp).')
        end
        
    case 'btw'
        if (n==0)
            error('Butterworth Filter should have order (n)>0.')
        end
       
        H = 1./(1 + (D*W./(D.^2 - D0^2)).^(2*n));
        
        if (strcmp(br_or_bp,'br'))
            H_out=H;
        elseif (strcmp(br_or_bp,'bp'))
            H_out=1-H;
        else
            error('Filter should be band reject (br) or band pass (bp).')
        end
        
    case 'gaussian'
        H = 1 - exp(-(((D.^2)-(D0^2))./ (D*W)).^2);
        
        if (strcmp(br_or_bp,'br'))
            H_out=H;
        elseif (strcmp(br_or_bp,'bp'))
            H_out=1-H;
        else
            error('Filter should be band reject (br) or band pass (bp).')
        end
    otherwise
        error('Unknown filter type.')
end