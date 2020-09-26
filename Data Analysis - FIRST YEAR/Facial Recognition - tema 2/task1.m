function A_k = task1(image, k)
  
  % citire imagine
  A = double(imread(image));
  
  % descompunere valori singulare
  [U S V] = svd(A);
  
  % determinarea matricelor Uk, Sk, Vk
  Uk = U(:,1:k);
  Sk = S(1:k, 1:k);
  Vk = V(:, 1:k);
  
  % determinarea matricei A_k
  A_k = Uk * Sk * Vk';

end