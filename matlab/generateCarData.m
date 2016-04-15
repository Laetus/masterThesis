clear all
close all

toPlot = 1;
toWrite= 1;
newError= 0;


path = 'data/Example_NoisyCar';
addpath(strcat('/home/laetus/Desktop/Thesis/Src/', path))
addpath('/home/laetus/Desktop/Thesis/Src/matlab/matlab2tikz-matlab2tikz-3a1ee10/src/')

%% Create constantCar
v = 15/3.6;

pos1 = (4:803) * v;


s2 = v / 2;
pos2 = ( 8:1607)* s2;

if( toPlot)
    figure();
    plot(pos2,'-o');
    title('X Position of a Car driving constantly');
end

s4 = v/4; 
pos4 = ( 16:3215) * s4;

if(toWrite)
    csvwrite('data/constantCar.csv',pos1);
    csvwrite('data/constantCar_half.csv',pos2);
    csvwrite('data/constantCar_quarter.csv',pos4);
end



%% Create 
stepsize = 1;
event0 = 40;      % car breaking, stopping and accelerating
event1 = 175;     % car breaking, stopping and accelerating
errorStart = 100;     % there are measurement errors
event2 = 275;     % another car came into the lane
event3 = 350;     % another car came into the lane

shift3 = 1;
shift4 = 2;
v = 20/3.6;
v_top = 29/3.6;
v_top2 = 22/3.6;
v_min = 1/3.6;

d1 = v/6;
d2 = v_top* 0.15;
d3 = (v_top - v ) * 0.25;
d4 = 1.45;
d5 = 1.75;
d6 = 0.8;
p0 = 30;
noise_factor = 0.1;

vel = ones(1,900) * v;
noise = 2* rand(1,900) -1  ;

vel1 = @(stepsize) [ones(1,event0 /stepsize )* v, v:-d1 * stepsize:v_min,  ones(1,28/ stepsize) * v_min, v_min:d2*stepsize:v_top, ones(1,28/ stepsize)* v_top, v_top:-d3 * stepsize:v];
vel2 = @(stepsize) [v:-d4* stepsize:v_min,  ones(1,25/stepsize) * v_min, v_min:d5*stepsize:v_top2, ones(1,36/stepsize)* v_top2, v_top2:d6*stepsize:v];
noiseSize = @(v) v .* stepsize*0.2;

vel(1:length(vel1(stepsize))) = vel1(stepsize);
vel(event1/stepsize: length(vel2(stepsize))-1 + event1/stepsize) = vel2(stepsize);



n_vel = vel .* (1 + noise_factor .* noise);

pos  = p0  + vel(1)*stepsize;
n_pos  = p0  + n_vel(1)*stepsize;
for i = 2:length(vel)
    pos(i) = pos(i-1) + vel(i)*stepsize;
    n_pos(i) = n_pos(i-1) + n_vel(i)*stepsize;
end

changer3 = (0:899 > event2 * stepsize) * (-shift3*v);
changer4 = (0:899 > event3 * stepsize) * (-shift4*v);
pos = pos + changer3 + changer4;



if(toPlot)
    figure(1)
    plot(n_vel)
    ylabel('Velocity [m/s]')
    xlabel('Time [s]')
    title('Velocity of the predecessor')
    xlim([ 0 750])
    figure(2)
    plot(n_pos)
    ylabel('Distance [m]')
    xlabel(' Time [s]')
    xlim([event3-4 event3 + 6 ]) 
    title('Position of the predecessor (detail)')
end



if(toPlot)
    figure(1)
    matlab2tikz(strcat(path,'/PreceedingCar_noisyVelocity.tikz'), 'width', '.4\textwidth')
    
    
end


if(toWrite)
    csvwrite('data/actionCar.csv',pos);
    csvwrite('data/noisy_actionCar.csv',n_pos);
end

%% 
% stepsize = .5
stepsize = 0.5;
vel = ones(1,1800) * v;
noise = 2 * rand(1,1800) - 1;

vel(1:length(vel1(stepsize))) = vel1(stepsize);
vel(event1/stepsize: length(vel2(stepsize))-1 + event1/stepsize) = vel2(stepsize);

n_vel = vel .* (1 + noise_factor .* noise);

pos2  = p0  + vel(1)*stepsize;
n_pos2  = p0  + n_vel(1)*stepsize;
for i = 2:length(vel)
    pos2(i) = pos2(i-1) + vel(i)*stepsize;
    n_pos2(i) = n_pos2(i-1) + n_vel(i)*stepsize;
end

changer3 = (0:1799 > event2 / stepsize) * (-shift3*v);
changer4 = (0:1799 > event3 / stepsize) * (-shift4*v);
pos2 = pos2 + changer3 + changer4;

if(toPlot)
    figure
    plot(vel)
    figure
    plot(pos2)
    figure
    plot(n_vel)
    title('n_vel')
    figure
    plot(n_pos2)
    title('n_pos')
end

if(toWrite)
    csvwrite('data/actionCar_half.csv',pos2);
    csvwrite('data/noisy_actionCar_half.csv',n_pos2);
end




% stepsize = .25
stepsize = 0.25;
vel = ones(1,3600) * v;
noise = 2 * rand(1,3600) - 1;

vel(1:length(vel1(stepsize))) = vel1(stepsize);
vel(event1/stepsize: length(vel2(stepsize))-1 + event1/stepsize) = vel2(stepsize);

n_vel = vel .* (1 + noise_factor .* noise);

pos4  = p0  + vel(1)*stepsize;
n_pos4  = p0  + n_vel(1)*stepsize;
for i = 2:length(vel)
    pos4(i) = pos4(i-1) + vel(i)*stepsize;
    n_pos4(i) = n_pos4(i-1) + n_vel(i)*stepsize;
end

changer3 = (0:3599 > event2 / stepsize) * (-shift3 * v);
changer4 = (0:3599 > event3 / stepsize) * (-shift4 * v);
pos4 = pos4 + changer3 + changer4;

if(toPlot)
    figure
    plot(vel)
    figure
    plot(pos4)
end

if(toWrite)
    csvwrite('data/actionCar_quarter.csv',pos4);
    csvwrite('data/noisy_actionCar_quarter.csv',n_pos4);
end

%%
if(newError)
    error1 = 2*rand(1,2400) -1;
    error2 = 2*rand(1,2400) -1; 
    error3 = (2*rand(1,2400) -1)*0.25; 

    if(toWrite)
        csvwrite('data/error1.csv', error1);
        csvwrite('data/error2.csv', error2);
        csvwrite('data/error3.csv', error3);
        csvwrite('data/error1_half.csv', 0.5*error1);
        csvwrite('data/error2_half.csv', 0.5*error2);
        csvwrite('data/error3_half.csv', 0.5*error3);
        csvwrite('data/error1_quarter.csv', 0.25*error1);
        csvwrite('data/error2_quarter.csv', 0.25*error2);
        csvwrite('data/error3_quarter.csv', 0.25*error3);
    end
end