%--------------------------------------------------------------------------
% FEDERAL UNIVERSITY OF UBERLANDIA
% Faculty of Electrical Engineering
% Biomedical Engineering Lab
% Uberlândia, Brazil
% Developed by: Andrei Nakagawa, MSc
% Contact: andrei.ufu@gmail.com
%--------------------------------------------------------------------------
% Description: Models of sensorimotor adaptation
% Based on the slides from Dr. Robert van Beers, lectured in CoSMo 2015
%--------------------------------------------------------------------------
%Number of trials
N = 400;
%Trials
trials = 1:N;
%Perturbation
i0 = 101;
i1 = 200;
%Degree of perturbation
deg = 30;
%Perturbation
perturbation = zeros(1,N);
perturbation(i0:i1) = deg;
perturbation(301:400) = deg;
%--------------------------------------------------------------------------
%simple model
%x(i+1) = x(i) + Be(i)
%x(1) = 0
%Learning rate
B = 0.1;
x = zeros(1,N);
e = zeros(1,N);
for i=2:N
  e(i) = perturbation(i-1) - x(i-1);
  x(i) = x(i-1) + B*e(i);
end
%--------------------------------------------------------------------------
%single-state model
%x(i+1) = Ax(i) + Be(i)
Ass = 0.99; %retention factor
Bss = 0.1; %learning rate
xss = zeros(1,N);
ess = zeros(1,N);
for i=2:N
  ess(i) = perturbation(i-1) - xss(i-1);
  xss(i) = Ass*xss(i-1) + Bss*ess(i);
end
%--------------------------------------------------------------------------
%two-state space model
%Smith, et al. (2006): Interacting adaptive process with different
%timescales underlie short-term motor learning
%x(i+1) = Ax(i) + Be(i)
%There are two hidden states
%x1 (fast state): Learns quickly but has poor retention
%x2 (slow state): Learns slowly but has good retention
%output is the sum of both states
%Therefore:
%x1(i+1) = Af*x1(i) + Bf*e(i)
%x2(i+1) = As*x2(i) + Bs*e(i)
%x(i) = x1(i) + x2(i)
%Bf < Bs, Af < As
Af = 0.92;
Bf = 0.06;
As = 0.995;
Bs = 0.02;
ets = zeros(1,N);
x1ts = zeros(1,N);
x2ts = zeros(1,N);
xts = zeros(1,N);
for i=2:N
    ets(i) = perturbation(i-1) - xts(i-1); %error
    x1ts(i) = Af*x1ts(i-1) + Bf*ets(i-1); %fast process
    x2ts(i) = As*x2ts(i-1) + Bs*ets(i-1); %slow process
    xts(i) = x1ts(i) + x2ts(i); %net adaptation
end
%--------------------------------------------------------------------------
%Plots
figure();
plot(trials,perturbation,'k');
hold on;
plot(trials,x,'r'); %Simple model
plot(trials,xss,'g'); %Single state
plot(trials,xts,'b'); %Two-state
legend('Perturbation','Simple model','Single-state model','Two-state model');
ylim([-0.5, deg+5]);
xlim([min(trials),max(trials)]);
title('Sensorimotor adaptation');
xlabel('Trials');
ylabel('Direction (deg)');

figure();
plot(trials,xts,'b'); %Two-state model -> net adaptation
hold on;
plot(trials,x1ts,'g'); %Two-state model -> fast process
plot(trials,x2ts,'r'); %Two-state model -> slow process
legend('Two-state model', 'Fast process', 'Slow process');
ylim([-0.5, deg+5]);
xlim([min(trials),max(trials)]);
title('Sensorimotor adaptation');
xlabel('Trials');
ylabel('Direction (deg)');
