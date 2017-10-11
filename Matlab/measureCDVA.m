function CDVAmeasures = measureCDVA(CDVAdata, descrLengths, TN, param)


if ~exist('param','var'),
    param = [];
end

if isfield(param,'skipRetrieval'),
    skipRetrieval = param.skipRetrieval;
else
    skipRetrieval = false;
end


nLengths = numel(descrLengths);
% get the average descriptor lengths
for lNo = 1:nLengths,
    
    if ~strcmp(descrLengths{lNo},'16K_256K')  && ~strcmp(descrLengths{lNo},'1K_4K'),
        
        switch descrLengths{lNo},
            case '16K'
                bound = 16384;
            case '64K'
                bound = 65536;
            case '256K'
                bound = 262144;
            otherwise
                bound = str2num(descrLengths{lNo}); % for debug purposes
        end
        
        dsColumn = find( strcmp(CDVAdata.headerQuery,'descriptorSize') );
        timeColumn = find( strcmp(CDVAdata.headerQuery,'time') );
        
        command = ['CDVAmeasures.Q.averageLength' descrLengths{lNo} ' = mean(CDVAdata.Q' descrLengths{lNo} '{dsColumn} ./ CDVAdata.Q' descrLengths{lNo} '{timeColumn});'];
        eval(command);
        
        command = ['CDVAmeasures.Q.maxLength' descrLengths{lNo} ' = max(CDVAdata.Q' descrLengths{lNo} '{dsColumn} ./ CDVAdata.Q' descrLengths{lNo} '{timeColumn});'];
        eval(command);
        
        command = ['CDVAmeasures.Q.nLongerThan' descrLengths{lNo} ' = sum( (CDVAdata.Q' descrLengths{lNo} '{dsColumn} ./ CDVAdata.Q' descrLengths{lNo} '{timeColumn}) > bound);'];
        eval(command);
        
        command = ['CDVAmeasures.Q.howmany' descrLengths{lNo} ' = numel(CDVAdata.Q' descrLengths{lNo} '{1});'];
        eval(command);
        
        dsColumn = find( strcmp(CDVAdata.headerReference,'descriptorSize') );
        timeColumn = find( strcmp(CDVAdata.headerReference,'time') );
        
        command = ['CDVAmeasures.R.averageLength' descrLengths{lNo} ' = mean(CDVAdata.R' descrLengths{lNo} '{dsColumn} ./ CDVAdata.R' descrLengths{lNo} '{timeColumn});'];
        eval(command);
        
        command = ['CDVAmeasures.R.maxLength' descrLengths{lNo} ' = max(CDVAdata.R' descrLengths{lNo} '{dsColumn} ./ CDVAdata.R' descrLengths{lNo} '{timeColumn});'];
        eval(command);
        
        command = ['CDVAmeasures.R.nLongerThan' descrLengths{lNo} ' = sum( (CDVAdata.R' descrLengths{lNo} '{dsColumn} ./ CDVAdata.R' descrLengths{lNo} '{timeColumn}) > bound);'];
        eval(command);
        
        command = ['CDVAmeasures.R.howmany' descrLengths{lNo} ' = numel(CDVAdata.R' descrLengths{lNo} '{1});'];
        eval(command);
        
    end
    
end


% Pairwise matching: true positives vs. true negatives

s_m = find(strcmp(CDVAdata.headerMatching,'score'));     % the column that contains the scores
s_nm = find(strcmp(CDVAdata.headerNonMatching,'score')); % the column that contains the scores

% compute the true positive rates as a function of true negative rate
% (ROC curves)
for lNo = 1:nLengths,
    command = ['[CDVAmeasures.ROC' descrLengths{lNo} ', CDVAmeasures.TPat1FP' descrLengths{lNo} '] = computeTP(CDVAdata.M' descrLengths{lNo} '{s_m}, CDVAdata.N' descrLengths{lNo} '{s_nm}, TN);'];
    eval(command);
end


% Pairwise matching: temporal localisation for matching pairs
cellFm = find(strcmp(CDVAdata.headerMatching,'firstMatchTime'));
cellLm = find(strcmp(CDVAdata.headerMatching,'lastMatchTime'));
cellTs = find(strcmp(CDVAdata.headerMatching,'trueStart'));
cellTe = find(strcmp(CDVAdata.headerMatching,'trueEnd'));

for lNo = 1:nLengths,
    
    command = ['detectedIntervals = [ CDVAdata.M' descrLengths{lNo} '{cellFm}  CDVAdata.M' descrLengths{lNo} '{cellLm} ];'];
    eval(command);
    
    command = ['trueIntervals = [ CDVAdata.M' descrLengths{lNo} '{cellTs}  CDVAdata.M' descrLengths{lNo} '{cellTe} ];'];
    eval(command);
    
    TempLoc = temporalLocalisation(detectedIntervals, trueIntervals);
    command = ['CDVAmeasures.TempLoc' descrLengths{lNo} ' = mean(TempLoc);'];
    eval(command);
    
end






% Retrieval: mean average precision, recall@N, success rate for top match
if ~skipRetrieval,
    
    % the matched references: their ids in the DB
    for lNo = 1:nLengths,
        
        % compute retrieval performance (mean average precision, recall@N, success rate for top match)
        if ~strcmp(descrLengths{lNo},'16K_256K') && ~strcmp(descrLengths{lNo},'1K_4K'),
            
            command = ['retrResults = CDVAdata.results' descrLengths{lNo} ';'];
            eval(command);
            
            command = ['[CDVAmeasures.mAP' descrLengths{lNo} ', CDVAmeasures.precAtR' descrLengths{lNo}  '] = retrievalPerformance(CDVAdata.truth, retrResults);'];
            eval(command);
        end
        
    end
    
end

CDVAmeasures.TN = TN;
