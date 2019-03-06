
%x_real x_imag y_real y_imag
n = 80
%integer = round(10*rand(n, 1));
integer = round(255*rand(n, 1));
%integer
fid = fopen('input.txt', 'wt');

 for i = 1:n
    fprintf(fid,'%g',integer(i,1));
    fprintf(fid,'\n');
 end
     
fclose(fid)
 
%save -ascii input.txt integer