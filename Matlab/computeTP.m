function [roc, TPat1FP, trueNeg, limits] = computeTP( matchingVals, nonmatchingVals, trueNeg )
% [roc, trueNeg, TPat1FP, limits]  = computeTP( matchingVals, nonmatchingVals, trueNeg )
% 

if(nargin <3)
    trueNeg= 0.75:0.01:1;
end



targetFPrate = 0.01;

nonmatchingVals = sort(nonmatchingVals, 'ascend');
num_of_NM_below = round(numel(nonmatchingVals)*trueNeg);

limits =    nonmatchingVals( num_of_NM_below );
roc = zeros(numel(trueNeg),1);

for k = 1:numel(trueNeg),
  roc(k) = sum(  matchingVals > limits(k) )/ numel(matchingVals);  
end


num_of_NM_below = round(numel(nonmatchingVals)*(1-targetFPrate));
TPat1FP =  sum(  matchingVals > nonmatchingVals( num_of_NM_below ) )/ numel(matchingVals);  