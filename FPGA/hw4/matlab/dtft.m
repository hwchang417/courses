function [ X ] = dtft( x, n, w )
% [X] = dtft(x, n, w)
% X = DTFT values computed at w frequencies
% x = finite duration sequence over n
% n = sample position vector
% w = frequency location vector
%h = [0.0078125 -0.01171875 0.01171875 -0.01953125 -0.0546875 0.28125 0.578125 0.28125 -0.0546875 -0.01953125 0.01171875 -0.01171875 0.0078125];

temp = w' * n;
temp = -1i * temp;
%temp
e = exp(temp);

X = x * e;
end