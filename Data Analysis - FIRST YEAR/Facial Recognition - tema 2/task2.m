function task2(image, k)
  
  % citire imagine
  A = double(imread(image));
  [m n] = size(A);
  
  k_size = length(k);
  A_diff = zeros(m, n);
  
  sum1 = 0;

  graph2_info = zeros(k_size, 1);
  graph3_info = zeros(k_size, 1);
  compression_rate = zeros(k_size, 1);
  
  % descompunere valori singulare
  [U S V] = svd(A);
  
  for k_index = 1 : k_size
    
    % calcularea sumei primelor k valori singulare
    sum1 = 0;
    for i = 1 : k(k_index);
      sum1 = sum1 + (diag(S))(i);
    endfor
    
    % descompunerea redusa a valorilor singulare
    A_k = task1(image, k(k_index));
    
    % matricea A_diff va contine elemente de forma (A(i, j) - A_k(i, j))^2 
    A_diff = A - A_k;
    A_diff = A_diff.^2;  
    
    % calcularea raportului dintre primele k valori si toate valorile singulare
    graph2_info(k_index) = sum1/sum(diag(S));
    
    % se realizeaza graficul sumei elementelor lui A_diff / m*n
    graph3_info(k_index) = sum(A_diff(:))/(m*n);
    
    % rata de compresie calculata dupa formula ceruta
    compression_rate(k_index) = (m*k(k_index) + ...
      n*k(k_index) + k(k_index))/(m*n);
      
  endfor
  
  % valorile singulare se afla pe diagonala matricei S
  figure(1);
  plot(diag(S));
  
  figure(2);
  plot(graph2_info);
  figure(3);
  plot(graph3_info);
  figure(4);
  plot(compression_rate);
  
end