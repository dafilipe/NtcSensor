clear;
clc;
close all;



beta = 3950;      % Beta coefficient [K]
R0 = 10000;       % Nominal resistance [Ohm]
T0 = 25;          % Reference temperature [°C]

T0K = T0 + 273.15;    % Reference temperature [K]



T = 0:0.1:50;         % Temperature [°C]
TK = T + 273.15;      % Temperature [K]

TminK = T(1) + 273.15;
TmaxK = T(end) + 273.15;


% Resistance at minimum temperature
Rmax = R0 * exp(beta * (1/TminK - 1/T0K));

% Resistance at maximum temperature
Rmin = R0 * exp(beta * (1/TmaxK - 1/T0K));

fprintf('--- NTC Resistance ---\n');
fprintf('Rmax = %.2f Ohm = %.2f kOhm\n', Rmax, Rmax/1000);
fprintf('Rmin = %.2f Ohm = %.2f kOhm\n', Rmin, Rmin/1000);


% Resistance over the complete temperature range
R = R0 .* exp(beta .* (1./TK - 1/T0K));

figure;

plot(T, R/1000, 'LineWidth', 2);

grid on;

xlabel('Temperature (°C)');
ylabel('NTC Resistance (k\Omega)');
title('NTC Resistance vs Temperature');

xlim([0 50]);



% Circuit:
%
%       3.3 V
%         |
%        Rfix
%         |
%         +-------- VNTC
%         |
%        NTC
%         |
%        GND

Vcc = 3.3;            
Rfix = 10 * 1000;     


Vntc = Vcc .* R ./ (Rfix + R);

% Maximum and minimum voltage

VntcMAX = Vcc * Rmax / (Rmax + Rfix);
VntcMin = Vcc * Rmin / (Rmin + Rfix);

fprintf('\n--- NTC Voltage ---\n');
fprintf('VntcMAX = %.3f V\n', VntcMAX);
fprintf('VntcMin = %.3f V\n', VntcMin);



figure;

plot(T, Vntc, 'LineWidth', 2);

grid on;

xlabel('Temperature (°C)');
ylabel('V_{NTC} (V)');
title('NTC Voltage vs Temperature');

xlim([0 50]);
ylim([0 Vcc]);


    