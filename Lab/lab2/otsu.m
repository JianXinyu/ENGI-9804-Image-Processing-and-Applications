function T = otsu(img)
%OSTU Otsu’s Method to calculate the threshold
%   find the most suited threshold will maximize 
%   the inter-class variance

% Calculate the histogram
[H, W] = size(img);% height, width of the input image
hist = imhist(img);
% Make hist a double column vector
hist = double( hist(:) );

% % there is a bug in this method
% % img is uint8 type, the range is 0~255. so even if img(i,j)=255,
% % img(i,j)+1 still is 255
% hist = zeros(256,1);
% for i = 1:H
%    for j = 1:W
%       hist(img(i,j)+1) = hist(img(i,j)+1) + 1; 
%    end
% end

% Calculate the probabilities of each intensity level
hist_pdf = hist / (H*W); % 256*1

% Define and initialize class probabilities and class means
omega0 = cumsum(hist_pdf);
omega1 = 1-omega0;
mu0 = cumsum((1:256)' .* hist_pdf)./omega0;
muT = mu0(end);
% Calculate σ_b^2(t)
sigma_b_squared = omega0 .* (mu0 - muT).^2 ./ omega1;

% Find  the maximum value of σ_b^2(t)
% The maximum may extend over several bins, so average together the
% locations. If maxval is NaN, meaning that sigma_b_squared is all NaN,
% then return 0.
% Note: NaN comes from omega0 = 0, so mu0 is NaN
maxval = max(sigma_b_squared);
isfinite_maxval = isfinite(maxval);
if isfinite_maxval
    idx = mean(find(sigma_b_squared == maxval));
    % Normalize the threshold to the range [0, 1].
    T = idx - 1;
else
    T = 0;
end

end

