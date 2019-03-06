%random 150 number in range(-128~127)
n = 150;
integer = round(255*rand(n, 1)) - 128;
%integer
fid = fopen('../modelsim/input.txt', 'wt');

 for i = 1:n
    fprintf(fid,'%g',integer(i,1));
    fprintf(fid,'\n');
 end
     
fclose(fid);