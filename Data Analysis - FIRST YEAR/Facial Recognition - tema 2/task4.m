function [A_k S] = task4(image, k)
  
  % citire matrice
  A = double(imread(image));
  [m n] = size(A);

  % calcularea mediei pentru fiecare coloana din A
  miu = sum(A, 2)/n;
  
  % actualizarea vectorilor
  A = A - miu;
  
  % construire matrice de covarianta Z
  Z = A*(A'/(n-1));
  
  % aplicare eig pe Z
  [V S] = eig(Z);
  
  % determinare W (primele k coloane din V)
  W = V(:, 1:k);
  
  % calculare proiectia lui A in spatiul componentelor principale
  Y = W'*A;

  % aproximare matrice initiala
  A_k = W*Y;
  A_k = A_k + miu;

endfunction