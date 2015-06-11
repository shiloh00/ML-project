img_mat = load('./COFW_train_color');
%img = img_mat.IsTr(1);
%imwrite(img{1}, './test.jpg');
num_pts = 29

shapeInserter = vision.ShapeInserter('Shape','Circles','BorderColor','Custom','CustomBorderColor',uint8([0 255 0]));
for i = 1:100
    img = img_mat.IsTr(i);
    img = img{1};
    shape = img_mat.phisTr(i,1:num_pts*2);
    for j = 1:num_pts
        circle = int32([shape(j) shape(j+num_pts) 2]);
        img = step(shapeInserter,img,circle);
    end
    %imshow(img);
    imwrite(img, sprintf('./output/%04d.jpg', i));
end
