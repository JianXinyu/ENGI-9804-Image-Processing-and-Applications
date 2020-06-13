function new_img = transform(img, matrix, trans)
    if(nargin <= 2)
        trans = false;
    end
    [y_max, x_max] = size(img); % obtaining the size of the image
    corners = [0,     0,    1;...
               x_max, 0,    1;...
               0,     y_max,1;...
               x_max, y_max,1]; %corner points of the image
    new_corners = corners*matrix;     %new location of corners
    new_width = round(max(new_corners(:,1)) - min(new_corners(:,1)));
    new_height = round(max(new_corners(:,2)) - min(new_corners(:,2)));
    if(trans == true)
        new_width = x_max + matrix(3, 1);
        new_height = y_max + matrix(3, 2);
    end
    min_width = abs(min(new_corners(:,1)));
    min_height = abs(min(new_corners(:,2)));
    
    min_width = round(min_width) + 1;
    min_height = round(min_height) + 1;
    
    new_img = zeros(new_height,new_width);
    
    inv_matrix = inv(matrix);
    for i = 1 : new_height
        for j = 1 : new_width
            temp = [j - min_width, i - min_height, 1] * inv_matrix;
            if(trans == true)
                temp = [j, i, 1] * inv_matrix;
            end
            x_old = round(temp(1,1));
            y_old = round(temp(1,2));
            if(x_old >= 1 && x_old <= x_max && y_old >= 1 && y_old <= y_max)
                new_img(i, j) = img(y_old, x_old);
            end
        end
    end
end

