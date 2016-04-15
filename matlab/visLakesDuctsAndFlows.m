%% Setup
try
close 21
catch 
end
try
close 22
catch 
end
try
close 23
catch 
end
try
close 24
catch 
end   
try
close 25
catch 
end  

Solve_Succeeded = 0;
Solved_To_Acceptable_Level = 0;
Maximum_Iterations_Exceeded = 1;
addpath('/home/laetus/Desktop/Thesis/Src/data')
result_GlobalSolver_LakesDuctsAndFlows;
powerPrice = [powerPrice, 0];
assert(norm(GlobalSolver_return_status) == 0);

steps_per_hour = 16;
time = (0:nTimesteps)/ steps_per_hour;

%% Plot lake heights
figure(21)
plot(time, powerPrice(1:nTimesteps+1)/ max(powerPrice) * max([Lake0,Lake1,Lake2,Lake3,Lake4]) * 0.9 );hold on;
plot(time, Lake0); 
plot(time, Lake1);
plot(time, Lake2);
plot(time, Lake3);
plot(time, Lake4);
plot(time, Lake5);
legend('norm. Power Price', 'Lake0','Lake1','Lake2','Lake3','Lake3b', 'Lake0b','Location', 'northwest');
xlabel('Time in h')
ylabel('Height in m')
hold off

%% Plot controlled flows
figure(22)
plot(time, powerPrice(1:nTimesteps+1)/ max(powerPrice) * max([Controlled_Flow_8,Controlled_Flow_9,Controlled_Flow_10,Controlled_Flow_11]) * 0.9 );hold on;
plot(time(1:end), Lake_Duct_6);
plot(time(1:end), Lake_Duct_7);
plot(time(1:end-1), Controlled_Flow_8);
plot(time(1:end-1), Controlled_Flow_9);
plot(time(1:end-1), Controlled_Flow_10);
plot(time(1:end-1), Controlled_Flow_11);
legend('norm. Power Price', 'Lake Duct3' , 'Lake Duct0' , 'Pump10', 'Turb02', 'Turb12', 'Turb23', 'Location','northwest');
title('Lakes,Duct & Flows: Controlled Flows')
xlabel('Time in h')
ylabel('Flow in m^3/s')
hold off 

%% Plot external flows
figure(23)
plot(time(1:end-1), External_Flow_14);
hold on
plot(time(1:end-1), External_Flow_13);
plot(time(1:end-1), External_Flow_12);
legend('Outflow3', 'Inflow1', 'Inflow0','Location','northwest');
title('Lakes, Duct & Flows: External Flows')
xlabel('Time in h')
ylabel('Flow in m^3/s')
hold off 

%% Plot produced power and profit
figure(24);
plot(time(1:end-1), prodPower); hold on
plot(time(1:end-1), prodPower .* powerPrice(1:length(prodPower))./ steps_per_hour);
legend('produced Power in MWh', 'Profit in EUR','Location', 'northwest');
title('Lakes, Duct & Flows: Produced Power & Profit')
xlabel('Time in h')
disp('Money earned:')
profit_glob = prodPower * powerPrice(1:length(prodPower))' / steps_per_hour

%% Plot duct difference
figure(25);
hold on
plot(time, (Lake3 - Lake4)*1e3);
plot(time, (Lake0 - Lake5)*1e3);
legend('Diff Lake3 - Lake3b', 'Diff Lake0 - Lake0b', 'Location', 'northwest');
title('Lakes, Duct & Flows: Ducts')
xlabel('Time in h')
ylabel('Difference in mm')

