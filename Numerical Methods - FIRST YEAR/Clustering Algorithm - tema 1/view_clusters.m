% outputs a graphical representation of the clustering solution
function view_clusters(points, centroids)
  
  % numarul total de puncte
  total_points = size(points);
  % numarul de centroizi
  NC = size(centroids);
	
  % parcurgere puncte
  for i = 1 : total_points
    
    shortest_dist = inf;
      
    % parcurgere centroizi
    for j = 1 : NC
      
      % calculez distanta de la centroizi la punct
      dist = norm(centroids(j,:) - points(i,:));
      
      % calculez distanta minima
      if shortest_dist > dist 
        shortest_dist = dist;
      endif
   
      % atribui punctul celui mai apropiat centroid
      if dist == shortest_dist
        group_index(i) = j;      
      endif
        
    endfor
      
  endfor
  % afisez punctele avand ca parametru de culoare indicele grupului din care
  % face parte punctul, obtinand astfel o culoare diferita pentru fiecare grup  
  scatter3(points(:,1), points(:,2), points(:,3), [], group_index(:));
    
endfunction

