%show output value in 2d graph

input = textread('../modelsim/input.txt');
decimal_output = textread('../modelsim/decimal_output.txt');
output = textread('../modelsim/output.txt');
output_csd = textread('../modelsim/output_csd.txt');

len = length(input);
output(1,:) = [];       output(2,:) = [];
output_csd(1,:) = [];   output_csd(2,:) = [];

index = 1:1:len;
plot(index, input, index, output, index, decimal_output, 'd', index, output_csd, 'o'), ylim([-150, 150]), xlabel('sample index'), ylabel('Amplitude'), legend('input', 'matlab deciaml coeff', '2s complement', 'CSD complement');