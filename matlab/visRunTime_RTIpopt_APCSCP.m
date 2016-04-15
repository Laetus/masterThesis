clear all

Solve_Succeeded  = 0 ;
Solved_To_Acceptable_Level = 0;
% Maximum_Iterations_Exceeded = 1;

%  Infeasible_Problem_Detected = 4;
% Maximum_Iterations_Exceeded = 2;
% Restoration_Failed =3;

%close all
% result_APCSCP_LakesAndFlows;
% result_RT_IpOpt_LakesAndFlows;

result_APCSCP_H0_A1_SimpleLake
result_RT_IpOpt_SimpleLake


% result_APCSCP_SimpleCar
% result_RT_IpOpt_SimpleCar

norm(RT_IpOpt_return_status)
norm(APCSCP_H0_A1_return_status)

figure;
plot(APCSCP_H0_A1_t_mainloop.wall) , hold on
plot(RT_IpOpt_t_mainloop.wall)
legend('APCSCP', 'RT\_IpOpt')

figure
plot(RT_IpOpt_t_eval_f.wall); hold on
plot(RT_IpOpt_t_eval_g.wall);
plot(RT_IpOpt_t_eval_grad_f.wall);
plot(RT_IpOpt_t_eval_h.wall);
plot(RT_IpOpt_t_eval_jac_g.wall);

legend('RT\_IpOpt\_evalf', 'RT\_IpOpt\_evalg', 'RT\_IpOpt\_evalgradf', 'RT\_IpOpt\_eval\_h', 'RT\_IpOpt\_eval\_jac\_g');
ylim1 = ylim();

figure
plot(APCSCP_H0_A1_t_eval_f.wall); hold on
plot(APCSCP_H0_A1_t_eval_g.wall);
plot(APCSCP_H0_A1_t_eval_grad_f.wall);
plot(APCSCP_H0_A1_t_eval_h.wall);
plot(APCSCP_H0_A1_t_eval_jac_g.wall);
legend('APCSCP\_evalf', 'APCSCP\_evalg', 'APCSCP\_evalgradf', 'APCSCP\_eval\_h', 'APCSCP\_eval\_jac\_g');
ylim(ylim1);

figure
subplot(2,3,1)
plot(APCSCP_H0_A1_t_eval_f.wall); hold on
plot(RT_IpOpt_t_eval_f.wall); 
% legend('APCSCP', 'RT\_IpOpt')
title('eval\_f')

subplot(2,3,2)
plot(APCSCP_H0_A1_t_eval_g.wall); hold on
plot(RT_IpOpt_t_eval_g.wall);
% legend('APCSCP', 'RT\_IpOpt')
title('eval\_g')

subplot(2,3,3)
plot(APCSCP_H0_A1_t_eval_grad_f.wall); hold on
plot(RT_IpOpt_t_eval_grad_f.wall);
% legend('APCSCP', 'RT\_IpOpt')
title('eval\_grad\_f')

subplot(2,3,4)
plot(APCSCP_H0_A1_t_eval_h.wall); hold on
plot(RT_IpOpt_t_eval_h.wall);
title('eval\_h')

subplot(2,3,5)
plot(APCSCP_H0_A1_t_eval_jac_g.wall); hold on
plot(RT_IpOpt_t_eval_jac_g.wall);
title('eval\_jac\_g')

subplot(2,3,6)
plot(APCSCP_H0_A1_iter_count); hold on
plot(RT_IpOpt_iter_count);
title('iter_count');

legend('APCSCP', 'RT\_IpOpt')



figure
subplot(2,3,1)
plot(APCSCP_H0_A1_n_eval_f); hold on
plot(RT_IpOpt_n_eval_f); 
% legend('APCSCP', 'RT\_IpOpt')
title('#eval\_f')

subplot(2,3,2)
plot(APCSCP_H0_A1_n_eval_g); hold on
plot(RT_IpOpt_n_eval_g);
% legend('APCSCP', 'RT\_IpOpt')
title('#eval\_g')

subplot(2,3,3)
plot(APCSCP_H0_A1_n_eval_grad_f); hold on
plot(RT_IpOpt_n_eval_grad_f);
% legend('APCSCP', 'RT\_IpOpt')
title('#eval\_grad\_f')

subplot(2,3,4)
plot(APCSCP_H0_A1_n_eval_h); hold on
plot(RT_IpOpt_n_eval_h);
title('#eval\_h')

subplot(2,3,5)
plot(APCSCP_H0_A1_n_eval_jac_g); hold on
plot(RT_IpOpt_n_eval_jac_g);
title('#eval\_jac\_g')

subplot(2,3,6)
plot(APCSCP_H0_A1_iter_count); hold on
plot(RT_IpOpt_iter_count);
title('iter_count');

figure
subplot(2,3,1)
plot(APCSCP_H0_A1_t_eval_f.wall ./ APCSCP_H0_A1_n_eval_f); hold on
plot(RT_IpOpt_t_eval_f.wall ./RT_IpOpt_n_eval_f); 
% legend('APCSCP', 'RT\_IpOpt')
title('eval\_f / #eval\_f')

subplot(2,3,2)
plot(APCSCP_H0_A1_t_eval_g.wall ./ APCSCP_H0_A1_n_eval_g); hold on
plot(RT_IpOpt_t_eval_g.wall ./RT_IpOpt_n_eval_g);
% legend('APCSCP', 'RT\_IpOpt')
title('eval\_g / #eval\_g')

subplot(2,3,3)
plot(APCSCP_H0_A1_t_eval_grad_f.wall ./ APCSCP_H0_A1_n_eval_grad_f); hold on
plot(RT_IpOpt_t_eval_grad_f.wall ./RT_IpOpt_n_eval_grad_f);
% legend('APCSCP', 'RT\_IpOpt')
title('eval\_grad\_f / #eval\_grad\_f')

subplot(2,3,4)
plot(APCSCP_H0_A1_t_eval_h.wall ./ APCSCP_H0_A1_n_eval_h); hold on
plot(RT_IpOpt_t_eval_h.wall ./RT_IpOpt_n_eval_h);
title('eval\_h / #eval\_h' )

subplot(2,3,5)
plot(APCSCP_H0_A1_t_eval_jac_g.wall ./ APCSCP_H0_A1_n_eval_jac_g); hold on
plot(RT_IpOpt_t_eval_jac_g.wall ./RT_IpOpt_n_eval_jac_g);
title('eval\_jac\_g / #eval\_jac\_g')

subplot(2,3,6)
plot(APCSCP_H0_A1_iter_count); hold on
plot(RT_IpOpt_iter_count);
title('iter_count');



figure
plot( APCSCP_H0_A1_timing_sec_p_It); hold on
plot( RT_IpOpt_timing_sec_p_It);
legend('apc','ipo')
title('Chrono It:')


figure
plot( APCSCP_H0_A1_timing_sec_p_MS); hold on
plot( RT_IpOpt_timing_sec_p_MS);
legend('apc','ipo')
title('Chrono MS:')

figure
plot( APCSCP_H0_A1_timing_sec_p_Solve); hold on
plot( RT_IpOpt_timing_sec_p_Solve);
legend('apc','ipo')
title('Chrono Solve:')

