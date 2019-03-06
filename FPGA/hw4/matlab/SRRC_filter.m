%calculate the decimal coeff of SRRC filter with random signal

%try http://www.mathworks.com/help/comm/examples/raised-cosine-filtering.html#zmw57dd0e6287
input = textread('../modelsim/input.txt');
h = [0.0078125,-0.01171875,0.01171875,-0.01953125,-0.0546875,0.28125,0.578125,0.28125,-0.0546875,-0.01953125,0.01171875,-0.01171875,0.0078125];
h2 = [2^-7,-2^-6 + 2^-8,2^-6 - 2^-8,-2^-6 - 2^-8,-2^-4+2^-7,2^-2 + 2^-5, 2^-1 + 2^-4 + 2^-6,2^-2 + 2^-5,-2^-4+2^-7,-2^-6 - 2^-8,2^-6 - 2^-8,-2^-6 + 2^-8,2^-7];
%h2 = single(h2)*256
%intArray = int8(-128);
%h(1) * 100000
n = length(input);
y = zeros(n, 1);

for i = 1:n
    for j = 1:13
%         if(i-j+1<1)
%             break;
%         else
%             y(i) = y(i) + h(j)*input(i-j+1);
%         end
        if(i - j + 1 >= 1)
            y(i) = y(i) + h(j) * input(i - j + 1);
        else
            break;
        end
    end
end

fid = fopen('../modelsim/decimal_output.txt', 'wt');
 for i = 1:n
    fprintf(fid,'%f',y(i));
    fprintf(fid,'\n');
 end
fclose(fid);

%freqz(h)
%Decimal coeff
samples=512;
f=1:1:samples;
H = freqz(h);
mag=mag2db(abs(H));

%2s complement coeff
bit_num=8; %fix-point 位元?
q = quantizer([bit_num 8]) %小數點後第八位
b = num2bin(q,h)
a = bin2num(q,b)
H2 = freqz(a);
mag2=mag2db(abs(H2));
plot(f/512, mag, f/512, mag2),xlabel('Normalized frequency'),ylabel('Magntitude(dB)'),legend('Decimal coeff','2s complement coeff and CSD coeff');


%freqz(a)