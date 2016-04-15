load data/4flows.mat


n = 15;
len = length(usual.flow_m3ps);
usual.exp_flow_m3ps = -ones(len,1);
usual2.exp_flow_m3ps = -ones(len,1);
wc_1d.exp_flow_m3ps = -ones(len,1);
wc_flood.exp_flow_m3ps = -ones(length(wc_flood.timestamp), 1);

avg = true;

for k = 1:floor(n/2)
    if(avg)
        usual.exp_flow_m3ps(k) = sum(usual.flow_m3ps(1:n) ) / n;
        usual2.exp_flow_m3ps(k) = sum(usual2.flow_m3ps(1:n) ) / n;
        wc_1d.exp_flow_m3ps(k) = sum(wc_1d.flow_m3ps(1:n) ) / n;
        wc_flood.exp_flow_m3ps(k) = sum(wc_flood.flow_m3ps(1:n-1) ) / n;
        usual.exp_flow_m3ps(end + 1 - k) = sum(usual.flow_m3ps(end-n + 1:end) ) / n;
        usual2.exp_flow_m3ps(end +1 - k) = sum(usual2.flow_m3ps(end-n+ 1:end) ) / n;
        wc_1d.exp_flow_m3ps(end + 1 - k) = sum(wc_1d.flow_m3ps(end-n + 1:end) ) / n;
        wc_flood.exp_flow_m3ps(end + 1 - k) = sum(wc_flood.flow_m3ps(end-n + 1:end) ) / n;
    else
        
        %Median
        usual.exp_flow_m3ps(k) = median(usual.flow_m3ps(1:n) );
        usual2.exp_flow_m3ps(k) = median(usual2.flow_m3ps(1:n) );
        wc_1d.exp_flow_m3ps(k) = median(wc_1d.flow_m3ps(1:n) ) ;
        wc_flood.exp_flow_m3ps(k) = median(wc_flood.flow_m3ps(1:n) ) ;
        usual.exp_flow_m3ps(end + 1 - k) = median(usual.flow_m3ps(end-n + 1:end) );
        usual2.exp_flow_m3ps(end +1 - k) = median(usual2.flow_m3ps(end-n+ 1:end) );
        wc_1d.exp_flow_m3ps(end + 1 - k) = median(wc_1d.flow_m3ps(end-n + 1:end) );
        wc_flood.exp_flow_m3ps(end + 1 - k) = median(wc_flood.flow_m3ps(end-n + 1:end) );
    end
end


for k = (ceil(n/2)):(len-floor(n/2))
    if(avg)
        usual.exp_flow_m3ps(k) = sum(usual.flow_m3ps(k-floor(n/2) : k+ floor(n/2))) / n;
        usual2.exp_flow_m3ps(k) = sum(usual2.flow_m3ps(k-floor(n/2) : k+ floor(n/2))) / n;
        wc_1d.exp_flow_m3ps(k) = sum(wc_1d.flow_m3ps(k-floor(n/2) : k+ floor(n/2))) / n;
    else
        %Median
        usual.exp_flow_m3ps(k) = median(usual.flow_m3ps(k-floor(n/2) : k+ floor(n/2)));
        usual2.exp_flow_m3ps(k) = median(usual2.flow_m3ps(k-floor(n/2) : k+ floor(n/2)));
        wc_1d.exp_flow_m3ps(k) = median(wc_1d.flow_m3ps(k-floor(n/2) : k+ floor(n/2)));
    end
end

for k = (ceil(n/2) : length(wc_flood.timestamp) - floor(n/2))
    if(avg)
        wc_flood.exp_flow_m3ps(k) = sum(wc_flood.flow_m3ps(k-floor(n/2) : k+ floor(n/2))) / n;
    else
        wc_flood.exp_flow_m3ps(k) = median(wc_flood.flow_m3ps(k-floor(n/2) : k+ floor(n/2)));
    end
end

figure(1)
plot(usual.timestamp, usual.flow_m3ps); hold on ; plot(usual.timestamp, usual.exp_flow_m3ps); hold off;
legend('Real flow', 'Forecast') ;
title('July 2th 2015');

figure(2)
plot(usual2.timestamp, usual2.flow_m3ps); hold on ; plot(usual2.timestamp, usual2.exp_flow_m3ps); hold off;
legend('Real flow', 'Forecast') ;
title('June 8th 2015')

figure(3)
plot(wc_1d.timestamp, wc_1d.flow_m3ps); hold on ; plot(wc_1d.timestamp, wc_1d.exp_flow_m3ps); hold off;
legend('Real flow', 'Forecast') ;
title('August 23rd 2005');

figure(4)
plot(wc_flood.timestamp, wc_flood.flow_m3ps); hold on ; plot(wc_flood.timestamp, wc_flood.exp_flow_m3ps); hold off;
legend('Real flow', 'Forecast') ;
title('August/September flood 2005')

save('data/4flows_exp_3avgc.mat', 'wc_1d', 'wc_flood', 'usual', 'usual2');

b = usual.exp_flow_m3ps';
c = usual.flow_m3ps';
csvwrite('data/usual.csv',[b;c]);
%%


csvwrite('data/usual_half.csv',[interpolate(b,2);interpolate(c,2)]);

csvwrite('data/usual_quarter.csv',[interpolate(b,4);interpolate(c,4)]);

csvwrite('data/usual_eighth.csv',[interpolate(b,8);interpolate(c,8)]);