close all
clear all

%% Visualize Ipopt

Solve_Succeeded=0;
Solved_To_Acceptable_Level = 0;
% Infeasible_Problem_Detected = 5;
Maximum_Iterations_Exceeded = 2;
% Restoration_Failed =3;
%%
 
% addpath('/home/laetus/Desktop/Thesis/Src/data');
% result_RT_IpOpt_SimpleCar;
result_APCSCP_H0_A1_SimpleCar;
% assert(norm(RT_IpOpt_return_status) == 0);

figure(1)
plot(xPosition4, yPosition5,'o-');
title('XY Position')

figure(2)
plot(Orientation3,'o-');
title('Orientation')

figure(3)
plot(Velocity1,'o-');
title('Velocity')

figure(4)
plot(Acceleration0,'o-');
title('Acceleration')

figure(5)
plot(Steering2,'o-');
title('Steering')

figure(6)
plot(xPosition4,'-o');
title('xPosition')



%% Visualize APCSCP
Solve_Succeeded=0;
Solved_To_Acceptable_Level = 0;

% addpath('/home/laetus/Desktop/Thesis/Src/data');
result_APCSCP_H0_A3_SimpleCar;
% assert(norm(APCSCP_return_status) == 0);

figure(1)
hold on
plot(xPosition4, yPosition5,'o-');
title('XY Position')
legend('Ipopt', 'APCSCP')

figure(2)
hold on
plot(Orientation3,'o-');
title('Orientation')
legend('Ipopt', 'APCSCP')

figure(3)
hold on
plot(Velocity1,'o-');
title('Velocity')
legend('Ipopt', 'APCSCP')

figure(4)
hold on
plot(Acceleration0,'o-');
title('Acceleration')
legend('Ipopt', 'APCSCP')

figure(5)
hold on
plot(Steering2,'o-');
title('Steering')
legend('Ipopt', 'APCSCP')

figure(6)
hold on
plot(xPosition4,'-o');
title('xPosition')
legend('Ipopt', 'APCSCP')
