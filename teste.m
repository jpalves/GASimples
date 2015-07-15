clear
close all
tic
%xb      = ga3(@fitness,100,50,40,4,10,10,0.2,0.09,0.05,20,0.9,0.5,0.1,0.1,1);
xb      = ga3(@fitness,50,50,32,40,10,50,0.9,0.09,0.05,30,0.9,0.5,0.1,0.01,0.5,50,0.05,1);
toc
varInt  = sum(xb.*repmat(2.^((size(xb,2)-1):-1:0),size(xb,1),1),2);
maxInt  = repmat(2.^size(xb,2)-1,size(xb,1),1);
zeroaum = (maxInt - varInt)./maxInt;
x       = zeroaum*2*pi;
out     = [x cellfun(@fitness,num2cell(xb,2))];
disp(out(end,:))
x = 0:0.01:2*pi;
y = x+10*sin(5*x)+7*cos(4*x);
figure()
plot(x,y)
hold on
plot(out(end,1),out(end,2),'or')
hold off
grid on

figure()
plot(1:size(out,1),out(:,2))
grid on
