function task5(image, k)
  
  % citire imagine
  A = double(imread(image));
  [m n] = size(A);
  
  A_diff = zeros(m,n);  
  k_size = length(k);
  
  graph2_info = zeros(min(m,n), 1);
  graph3_info = zeros(min(m,n), 1);
  compression_rate = zeros(min(m,n), 1);
  
  for k_index = 1 : k_size
    
    % determinare A_k si S folosind task3
    [A_k, S] = task3(image, k(k_index));
     
    sum1 = 0;
    % calcularea sumei primelor k valori singulare
    for i = 1 : k(k_index);
      sum1 = sum1 + (diag(S))(i);
    endfor
    
    % matricea A_diff va contine elemente de forma (A(i, j) - A_k(i, j))^2
    A_diff = A - A_k;
    A_diff = A_diff.^2;
    
    % calcularea raportului dintre primele k valori si toate valorile singulare
    graph2_info(k_index) = sum1/sum(diag(S));
    
    % se realizeaza graficul sumei elementelor lui A_diff / m*n
    graph3_info(k_index) = sum(A_diff(:))/(m*n);
    
    % rata de compresie calculata dupa formula ceruta
    compression_rate(k_index) = (2*k(k_index) + 1)/n;
    
  endfor
  
  figure(1);
  plot(diag(S));
  figure(2);
  plot(graph2_info);
  figure(3);
  plot(graph3_info);
  figure(4);
  plot(compression_rate);
  
end