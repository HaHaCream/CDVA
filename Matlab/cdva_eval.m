clear all
%% set input parameters

%if true, more information on data checks is printed
param.setVerbose = false;

%if true, only the 16K operating point is evaluated
param.setSimple = false;

% if true, the retrieval experiment is not evaluated
param.skipRetrieval = false;

% the path for the files output by CDVA
param.csvPath = '../run';

% if true, the C++ implementation of the CDVA evaluation is executed;
% if false, the Matlab implementation of the CDVA evaluation is executed;
param.fastEval = true;

%% execute evaluation
if param.fastEval
    if exist('cdva_eval_mex') ~= 3
       cdva_compile;
    end
    
    CDVAmeasures = cdva_eval_mex(param);
else

    % configure input data
    if param.setSimple == true
        descrLengths = {'16K'}; 
    else    
        descrLengths = {'16K','64K','256K' ,'16K_256K'};
    end

    % at what true negative rates shall true positive rates be computed
    TN = (0.95:0.001:1)';

    % read the CDVA output
    CDVAdata = readCDVA(param.csvPath, descrLengths, param);

    % compute performance
    CDVAmeasures = measureCDVA(CDVAdata, descrLengths, TN, param);

    % print the measurements to the command window
    printCDVAmeasures(CDVAmeasures, CDVAdata, descrLengths, param);

end
