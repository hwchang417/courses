cosdata = textread('cosdata.txt');
x = (2*pi*[0:99]' / 100)-pi;
save -ascii phasedata.txt x


plot(cos(x(2:100)));hold;
plot(cosdata(2:100),'or');


