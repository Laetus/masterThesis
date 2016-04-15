function [ x_out] = findLastEntry( x_in )
%FINDLASTNONZERO Summary of this function goes here
%   Detailed explanation goes here

nRow = size(x_in, 1);
x_out = zeros(1, nRow);

filler = -42;

for i = 1:nRow
    y = x_in(i, :);
    y(x_in(i,:) == filler) = [];
    x_out(i) = y(end);
end

