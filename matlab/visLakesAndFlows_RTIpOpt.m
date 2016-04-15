%% Setup
try
close 11
catch 
end
try
close 12
catch 
end
try
close 13
catch 
end
try
close 14
catch 
end   
try
close 15
catch
end

Solve_Succeeded = 0;
Infeasible_Problem_Detected = 100;
Solved_To_Acceptable_Level = 1;
addpath('/home/laetus/Desktop/Thesis/Src/data')
result_RT_IpOpt_LakesAndFlows;
%assert(norm(RT_IpOpt_return_status) <= 5);

steps_per_hour = 8;
time = (0:n_f-1)/steps_per_hour;

%% Plot lake heights
figure(11)
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
figure(12)
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
figure(13)
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
figure(14)
plot(time(1:end-1), prodPower(1:(n_f-1))); hold on
plot(time(1:end-1), prodPower(1:(n_f-1)) .* powerPrice(1:n_f-1) ./ steps_per_hour);
legend('produced Power in MWh', 'Profit in EUR','Location', 'northwest');
title('Lakes & Flows: Produced Power & Profit')
xlabel('Time in h')
disp('Money earned:')
profit_rt = prodPower * powerPrice(1:length(prodPower))'

%% Plot power price
figure(15)
plot((0:nTimesteps+n_f-1)/steps_per_hour, powerPrice(1:nTimesteps+n_f),'-o');
legend('powerPrice','Location', 'northwest')
title('Lakes & Flows: Power Price')
xlabel('Time in h')
ylabel('Price in EUR/MWh')