function view_cost_vs_nc(file_points)
	
  % citire matrice de puncte
  points = dlmread(file_points, " ", 5, 1);
  
  % initializare vector de valori pentru cost
  cost = zeros(10);
  
  total_points = size(points);
  
  % parcurgere valori pentru NC (de la 1 la 10)
  for NC = 1 : 10 
    
    % determinare coordonate centroizi
    centroids = clustering_pc(points, NC);
	  
    %parcurgere puncte
    for i = 1 : total_points
 
      shortest_dist = inf;
      
      % parcurgere centroizi
      for j = 1 : NC
      
        % calculare distanta de la centroizi la punct
        dist = norm(centroids(j,:) - points(i,:));
        % calculare distanta minima
        if shortest_dist > dist 
          shortest_dist = dist;
        endif
        
      endfor
      
      % calculare cost pentru fiecare numar de centroizi
      cost(NC) = cost(NC) + shortest_dist;
      
    endfor
 
  endfor
  
  % graficul costului
  plot(cost);
  
endfunction