%[text] EMS LAB 1
clear;
clc;
close all;

%[text] 1\. NTC PARAMETERS
R0   = 10000;
beta = 3380;
T0   = 25;

Tmin = 25;
Tmax = 50;

Vdd = 3.3;

%[text] 2\. TEMPERATURE
T0K = T0 + 273.15;

T = Tmin:0.1:Tmax;      % Temperature vector, step = 0.1 °C
TK = T + 273.15;


%[text] 3\. NTC MODEL
Rntc = R0 .* exp(beta .* (1./TK - 1/T0K));


% For an NTC:
% Tmin -> maximum resistance
% Tmax -> minimum resistance
Rmax = Rntc(1);         % First value corresponds to Tmin
Rmin = Rntc(end);       % Last value corresponds to Tmax

%[text] 4\. DIMENSION R1
%R1 = sqrt(Rmax * Rmin);

R1 = 10000;

fprintf('R1 ideal = %.2f kOhm\n', R1/1000);

%[text] 5\. VOLTAGE DIVIDER
%
%       Vdd
%        |
%       NTC
%        |
%        +------ Vin
%        |
%        R1
%        |
%       GND

Vin = Vdd .* R1 ./ (Rntc + R1);


fprintf('\n--- Vin ---\n');

fprintf('Vin min = %.3f V\n', min(Vin));
fprintf('Vin max = %.3f V\n', max(Vin));

%[text] 6\. CURRENT
I = Vdd ./ (R1 + Rntc);


fprintf('\n--- Current ---\n');
fprintf('I max = %.3f mA\n', max(I)*1000);

%[text] 7\. NTC POWER
Pntc = I.^2 .* Rntc;


fprintf('\n--- NTC Power ---\n');

fprintf('P max = %.3f mW\n', max(Pntc)*1000);

%[text] 8\. GRAPHS
figure;

plot(T,Rntc/1000,'LineWidth',2);

grid on;

xlabel('Temperature [°C]');
ylabel('NTC Resistance [k\Omega]');
title('NTC Resistance vs Temperature');


figure;

plot(T,Vin,'LineWidth',2);

grid on;

xlabel('Temperature [°C]');
ylabel('Vin [V]');
title('Voltage Divider Output vs Temperature');


figure;

plot(T,I*1000,'LineWidth',2);

grid on;

xlabel('Temperature [°C]');
ylabel('Current [mA]');
title('NTC Current vs Temperature');

%[appendix]{"version":"1.0"}
%---
%[metadata:view]
%   data: {"layout":"onright","rightPanelPercent":40}
%---

