% reads cluster count and points from input files 
function [NC points] = read_input_data(x, y)
  
  %citire numar centroizi  
	NC = dlmread(x);
  
  %citire matrice de puncte
	points = dlmread(y, " ", 5, 0);
  
endfunction

