%% Setup
try
close 1
catch 
end
try
close 2
catch 
end
try
close 3
catch 
end
try
close 4
catch 
end

Solve_Succeeded = 0;
addpath('/home/laetus/Desktop/Thesis/Src/data')
result_GlobalSolver_LakesAndFlows;
assert(norm(GlobalSolver_return_status) == 0);

steps_per_hour = 4;
time = (0:nTimesteps)/ steps_per_hour;

%% Plot lake heights
figure(1)
% plot(time  , powerPrice * 20);hold on;
plot(time, powerPrice(1:nTimesteps+1)/ max(powerPrice) * max([Lake0,Lake1,Lake2,Lake3]) * 0.9 );hold on;
plot(time, Lake0); 
plot(time, Lake1);
plot(time, Lake2);
plot(time, Lake3);
legend('norm. Power Price', 'Lake0','Lake1','Lake2','Lake3','Location','northwest');
title('Lakes & Flows: Heights')
xlabel('Time in h')
ylabel('Height in m')
hold off

%% Plot controlled flows
figure(2)
plot(time(1:end-1), powerPrice(1:nTimesteps)/ max(powerPrice) * max([Controlled_Flow_4,Controlled_Flow_5,Controlled_Flow_6,Controlled_Flow_7]) * 0.9); hold on;
% plot(time, powerPrice*12); hold on;
plot(time(1:end-1), Controlled_Flow_4);
plot(time(1:end-1), Controlled_Flow_5);
plot(time(1:end-1), Controlled_Flow_6);
plot(time(1:end-1), Controlled_Flow_7);
plot(time(1:end-1), prodPower/max(abs(prodPower)) * max([Controlled_Flow_4,Controlled_Flow_5,Controlled_Flow_6,Controlled_Flow_7]) * 0.9);
legend('norm. Power Price', 'Pump10', 'Turb02', 'Turb12', 'Turb23','norm. Produced Power','Location','northwest');
title('Lakes & Flows: Controlled Flows')
xlabel('Time in h')
ylabel('Flow in m^3/s')
hold off 

%% Plot external flows
figure(3)
plot(time(1:end-1), External_Flow_10);
hold on
plot(time(1:end-1), External_Flow_9);
plot(time(1:end-1), External_Flow_8);
legend('Outflow3', 'Inflow1', 'Inflow0','Location', 'northwest');
title('Lakes & Flows: External Flows')
xlabel('Time in h')
ylabel('Flow in m^3/s')
hold off;

%% Plot produced power and profit
figure(4)
plot(time(1:end-1), prodPower); hold on
plot(time(1:end-1), prodPower .* powerPrice(1:length(prodPower)) ./ steps_per_hour);
legend('produced Power in MWh', 'Profit in EUR','Location', 'northwest');
title('Lakes & Flows: Produced Power & Profit')
xlabel('Time in h')
disp('Money earned:')
profit_glob = prodPower * powerPrice(1:length(prodPower))' / steps_per_hour

%% Plot power Price
figure(5)
plot(time, powerPrice(1:nTimesteps+1),'-o');
ylim([0, ceil(max((powerPrice(1:nTimesteps+1)+10)*10))/10]); 
legend('powerPrice','Location', 'northwest')
title('Lakes & Flows: Power Price')
xlabel('Time in h')
ylabel('Price in EUR/MWh')