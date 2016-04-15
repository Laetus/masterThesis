%% Setup
try
close 31
catch 
end
try
close 32
catch 
end
try
close 33
catch 
end
try
close 34
catch 
end  
try
close 35
catch 
end 

Solve_Suceeded=0;
Solved_To_Acceptable_Level = 0;
Infeasible_Problem_Detected = 1;
Maximum_Iterations_Exceeded = 2;
Restoration_Failed =3;
addpath('/home/laetus/Desktop/Thesis/Src/data')
result_RT_IpOpt_LakesDuctAndFlows;
% assert(norm(RT_IpOpt_return_status) == 0);

steps_per_hour = 8;
time = (0:n_f-1)/steps_per_hour;

%% Plot lake heights
figure(31)
plot( (0:(n_f+nTimesteps-1))/steps_per_hour  , powerPrice(1:(n_f+nTimesteps)) / max(powerPrice(1:nTimesteps+n_f)) * max([Lake0,Lake1,Lake2,Lake3,Lake4]) * 0.9 );hold on;
plot(time, Lake0); 
plot(time, Lake1);
plot(time, Lake2);
plot(time, Lake3);
plot(time, Lake4);
legend('norm. Power Price', 'Lake0','Lake1','Lake2','Lake3','Lake3b', 'Location', 'northwest');
title('Lakes, Ducts & Flows: Heights, RT_IpOpt')
xlabel('Time in h')
ylabel('Height in m')
hold off

%% Plot controlled flows
figure(32)
plot((0:nTimesteps+n_f-1)/steps_per_hour, powerPrice(1:nTimesteps+n_f)/ max(powerPrice) * max([Controlled_Flow_8,Controlled_Flow_9,Controlled_Flow_6,Controlled_Flow_7]) * 0.9); hold on;
plot(time, Lake_Duct_5);
plot(time(1:end-1), Controlled_Flow_6(1:end-1));
plot(time(1:end-1), Controlled_Flow_7(1:end-1));
plot(time(1:end-1), Controlled_Flow_8(1:end-1));
plot(time(1:end-1), Controlled_Flow_9(1:end-1));
legend('norm. Power Price', 'Lake Duct', 'Pump10', 'Turb02', 'Turb12', 'Turb23');
title('Lakes, Ducts & Flows: Controlled Flows, RT_IpOpt')
xlabel('Time in h')
ylabel('Flow in m^3/s')
hold off

%% Plot external flows
figure(33)
plot(time, External_Flow_12);
hold on
plot(time, External_Flow_11);
plot(time, External_Flow_10);
legend('Outflow3', 'Inflow1', 'Inflow0','Location','west');
title('Lakes, Ducts & Flows: External Flows, RT_IpOpt')
xlabel('Time in h')
ylabel('Flow in m^3/s')
tmp = ylim();
ylim([0 , tmp(2)])
hold off 

%% Plot produced power
figure(34)
plot(time(1:end-1), prodPower(1:n_f-1)); hold on
plot(time(1:end-1), prodPower(1:n_f-1) .* powerPrice(1:n_f-1) ./ steps_per_hour);
legend('produced Power in MWh', 'Profit in EUR','Location', 'northwest');
title('Lakes, Ducts & Flows: Produced Power & Profit, RT_IpOpt')
xlabel('Time in h')
disp('Money earned:')
profit_rt = prodPower * powerPrice(1:length(prodPower))'
%% Plot difference between Lake3 and Lake3b
figure(35)
hold on
plot(time(1:end), (Lake3(1:end) - Lake4(1:end))*1e3);
legend('Diff Lake3 - Lake4', 'Location', 'northwest');
title('Lakes, Duct & Flows: Difference between Lake3 and Lake3b, RT_IpOpt')
xlabel('Time in h')
ylabel('Difference in mm')
