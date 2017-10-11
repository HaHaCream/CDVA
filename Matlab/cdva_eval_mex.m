% cdva_eval_mex  C++ implementation of the CDVA evaluation
%
%   M = cdva_eval_mex(Params) computes the CDVA evaluation framework
%   output measures
%
%   Params is a structure containing input parameters:
%       setVerbose - if true, more information on data checks is printed
%       setSimple - if true, only the 16K operating point is evaluated
%       skipRetrieval - if true, the retrieval experiment is not evaluated
%       csvPath - contains the path of the CSV files produced by CDVA
%
%   M is a structure containing evaluation measures:
%       QueryAverageLen - the average length of query descriptors (byte/second)
%       QueryMaxLen - the maximum length of query descriptors (byte/second)
%       ReferenceAverageLen - the average length of reference descriptors (byte/second)
%       ReferenceMaxLen - the maximum length of reference descriptors (byte/second)
%       MeanAveragePrecision - the retrieval mean average precision
%       R_Precision - the retrieval precision at r
%       TruePositiveRate - the pairwise matching true positive rate at 1% FPR
%       MeanJaccardIndex - the temporal localization mean Jaccard index 
