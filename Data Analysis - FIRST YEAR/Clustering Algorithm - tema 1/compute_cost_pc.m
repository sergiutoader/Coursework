% computes a clustering solution total cost
function cost = compute_cost_pc(points, centroids)
	cost = 0; 
  
	% total_points = numarul total de puncte
  total_points = size(points);
  % NC = numarul de centroizi
  NC = size (centroids);
	
  % parcurgere puncte
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
    
    % se adauga distanta dintre punct si cel 
    % mai apropiat centroid la costul total
    cost = cost + shortest_dist;
      
  endfor
  
endfunction