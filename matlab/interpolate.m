function [ res ] = interpolate( vec, k )
%INTERPOLATE Summary of this function goes here
%   Detailed explanation goes here


n = length(vec);

left = repmat(vec, [k,1]);
left = left(:)';

right = repmat([vec(2:end), vec(end)], [k,1]);
right = right(:)';


fac_left= repmat(k:-1:1, [1,n]);
fac_left = fac_left(:)'/k;


fac_right = repmat(0:k-1 ,[1, n]);
fac_right =fac_right(:)' / k;

res = (left .* fac_left) + (right .* fac_right);

end

