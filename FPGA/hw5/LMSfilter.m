clc;clear;
mu = 0.02

x = zeros(1,1000);

%%for n=3:1000
 %  x(n) = 1.2728*x(n-1)-0.81*x(n-2)+randn; 
%end
x = textread('datain.txt');
w1 = textread('dataout.txt');
w2 = textread('dataout2.txt');
x = x;

w = zeros(1000,2);
%y = zeros(1000);6
for n = 3:1000
    xv = [x(n-1) x(n-2)]';
    y=w(n-1,:)*xv
    e=x(n)-y;
    w(n,:)=w(n-1,:)+mu*e*xv';
end


wr1 = w(:,1);
wr2 = w(:,2);
plot(w(:,1),'r *');hold on;
plot(w(:,2),'b o');hold on;

plot(w1(2:1000),'b o');%hold on;
plot(w2(2:1000),'r *');

%x = x';

%save -ascii datain.txt x
