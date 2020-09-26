function [min_dist output_img_index] = face_recognition(image_path, ...
  m, A, eigenfaces, pr_img)
  
  % citire imagine din fisier + punere intr-un vector + extragere medie din el
  M = double(rgb2gray(imread(image_path)));
  T = M(:);
  T = T - m;
  
  % calculare proiectia imaginii de test in spatiul fetelor
  PrTestImg = eigenfaces'*T;
  
  % determina imaginea cea mai asemanatoare cu cea de test
  min_dist = inf;
  for i = 1 : size(pr_img)
    dist = norm(PrTestImg - pr_img(:,i));
    if min_dist > dist
      min_dist = dist;
      output_img_index = i;
    endif
      
  endfor
  
end