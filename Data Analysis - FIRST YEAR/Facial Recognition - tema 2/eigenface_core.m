function [m A eigenfaces pr_img] = eigenface_core(database_path)

  for i = 1 : 10
    image_path = strcat(database_path, "/", int2str(i), ".jpg");
    
    % citire imagine
    M = double(rgb2gray(imread(image_path)));
    
    % transformare matrice in vector coloana
    A(:,i) = M(:);
  endfor
  
  % calculare medie pentru fiecare vector din A
  m = sum(A, 2)/10;
  
  % actualizarea vectorilor
  A = A - m;
  
  % calculare valori si vectori proprii pentru A'*A folosind eigenfaces
 [V S] = eig(A'*A);
 
  % eliminare vectori pentru care valorile proprii sunt mai mici ca 1
  for i = 1 : 10
    if((diag(S))(i) <= 1)
      V(:,i) = [];
    endif
  endfor
  
  % calculare eigenfaces si pr_img
  eigenfaces = A*V;
  pr_img = eigenfaces'*A;

end