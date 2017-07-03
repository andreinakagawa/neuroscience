%--------------------------------------------------------------------------
% FEDERAL UNIVERSITY OF UBERLANDIA
% Faculty of Electrical Engineering
% Biomedical Engineering Lab
%--------------------------------------------------------------------------
% Authors:  Amanda Medeiros de Freitas, MSc
%           Andrei Nakagawa, MSc
%--------------------------------------------------------------------------
%--------------------------------------------------------------------------
x = load('header.txt');
width = x(1);
height = x(2);
fs = 1000;
s = load('arquivoColeta.txt');
figure(); plot(s(:,2),s(:,3));
xlim([0 width]);
ylim([0 height]);

np = 3;
fc = 10;
wn = fc / (fs/2);
[b,a] = butter(np,wn,'low');
xfilt = filtfilt(b,a,s(:,2));
yfilt = filtfilt(b,a,s(:,3));

figure();
subplot(2,1,1); plot(xfilt);
subplot(2,1,2); plot(diff(xfilt));

figure(); plot(xfilt,yfilt);
xlim([0 width]);
ylim([0 height]);