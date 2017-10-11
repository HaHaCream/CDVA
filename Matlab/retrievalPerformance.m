function [mAP, precAtR] = retrievalPerformance(truth, results)
% [mAP, precAtR] = retrievalPerformance(truth, results)
%
% input
%   truth, results          cell arrays with column vectors of integers
%                           the elements are labels of images and each vector is, respectively, 
%                           the ground truth and the outcome of a query
%
%   N                       scalar or vector, numbers at which recall-at-N
%                           will be measured
%  
% output
%   mAP                     mean average precision
%   precAtR                 R-precision


% From:
%   Evaluation Framework for Compact Descriptors for Video Analysis - Search and Retrieval – Version 2.0
%
% 4.2	Performance characteristics
% The information about performance of proposals shall be provided using the following set of measures. 
% 
% Retrieval performance
% •	Mean Average Precision (mAP), used as a primary measure.
% •	Precision at r, where r is the number of reference clips for each query, used as a secondary measure.


% mean average precision
mAP = compute_mAP(truth, results);

precAtR = compute_precision_at_r(truth, results);


return;

