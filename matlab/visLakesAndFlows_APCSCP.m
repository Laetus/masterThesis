%% Setup
try
close 101
catch 
end
try
close 102
catch 
end
try
close 103
catch 
end
try
close 104
catch 
end   
try
close 105
catch
end

Solve_Succeeded = 0;
Solved_To_Acceptable_Level = 1;
Restoration_Failed = 5;
addpath('/home/laetus/Desktop/Thesis/Src/data')
result_APCSCP_LakesAndFlows;
%assert(sum(APCSCP_return_status) < 15);

steps_per_hour = 8;
time = (0:n_f-1)/steps_per_hour;

%% Plot lake heights
figure(101)
plot( (0:(n_f+nTimesteps-1))/steps_per_hour  , powerPrice(1:(n_f+nTimesteps)) / max(powerPrice(1:nTimesteps+n_f)) * max([Lake0,Lake1,Lake2,Lake3]) * 0.9 );hold on;
plot(time, Lake0); 
plot(time, Lake1);
plot(time, Lake2);
plot(time, Lake3);
legend('norm. Power Price', 'Lake0','Lake1','Lake2','Lake3', 'Location', 'northwest');
title('Lakes & Flows: Heights')
xlabel('Time in h')
ylabel('Height in m')
hold off

%% Plot controlled flows
figure(102)
plot((0:nTimesteps+n_f-1)/steps_per_hour, powerPrice(1:nTimesteps+n_f)/ max(powerPrice) * max([Controlled_Flow_4,Controlled_Flow_5,Controlled_Flow_6,Controlled_Flow_7]) * 0.9); hold on;
plot(time(1:end-1), Controlled_Flow_4(1:end-1),'-o');
plot(time(1:end-1), Controlled_Flow_5(1:end-1),'-o');
plot(time(1:end-1), Controlled_Flow_6(1:end-1),'-o');
plot(time(1:end-1), Controlled_Flow_7(1:end-1),'-o');
plot(time(1:end-1), prodPower(1:(n_f-1))/max(abs(prodPower)) * max([Controlled_Flow_4,Controlled_Flow_5,Controlled_Flow_6,Controlled_Flow_7]) * 0.9);
legend('norm. Power Price', 'Pump10', 'Turb02', 'Turb12', 'Turb23','norm. Produced Power','Location','northwest');
title('Lakes & Flows: Controlled Flows')
xlabel('Time in h')
ylabel('Flow in m^3/s')
hold off 

%% Plot external flows
figure(103)
plot(time, External_Flow_10);
hold on
plot(time, External_Flow_9);
plot(time, External_Flow_8);
legend('Outflow3', 'Inflow1', 'Inflow0','Location','west');
title('Lakes & Flows: External Flows')
xlabel('Time in h')
ylabel('Flow in m^3/s')
ylim([0, (ceil(max([External_Flow_10, External_Flow_9, External_Flow_8])*10)+10)/10]);
hold off;

%% Plot produced power and profit
figure(104)
plot(time(1:end-1), prodPower(1:(n_f-1))); hold on
plot(time(1:end-1), prodPower(1:(n_f-1)) .* powerPrice(1:n_f-1) ./ steps_per_hour);
legend('produced Power in MWh', 'Profit in EUR','Location', 'northwest');
title('Lakes & Flows: Produced Power & Profit')
xlabel('Time in h')
disp('Money earned:')
profit_rt = prodPower * powerPrice(1:length(prodPower))'/steps_per_hour

%% Plot power price
figure(105)
plot((0:nTimesteps+n_f-1)/steps_per_hour, powerPrice(1:nTimesteps+n_f),'-o');
legend('powerPrice','Location', 'northwest')
title('Lakes & Flows: Power Price')
xlabel('Time in h')
ylabel('Price in EUR/MWh')
tmp = ylim();
ylim([0, tmp(2)]);
