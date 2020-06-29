function imb = gray2binary(T, img)
%GRAY2BINARY grayscale image to binary image
%    convert a grayscale image to a binary image using a 
%    given threshold value
imb = ( img > T);
end

