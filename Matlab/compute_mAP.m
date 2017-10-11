function mAP = compute_mAP(truth, results)
% mAP = compute_mAP(truth, results)


nQueries = numel(results);
nTruths = cellfun(@numel,truth,'UniformOutput', false); 

% minus one to get real cnt
oneVec = mat2cell(ones(nQueries,1),ones(nQueries,1));
nTruths = cellfun(@minus,nTruths,oneVec,'UniformOutput',false);

% in case nTruths becomes 0 (this operation is necessary to experiment with
% partial datasets)
nTruths = cellfun(@max,nTruths,oneVec,'UniformOutput',false);

nResults = cellfun(@numel,results,'UniformOutput', false);    


relevances = cellfun(@ismember, results, truth, 'UniformOutput',false); % binary relevances
relevances = cellfun(@double, relevances, 'UniformOutput',false);

numberAtCutoff = cellfun(@cumsum, relevances, 'UniformOutput', false);

oneToN = cellfun(@colon, repmat({1}, nQueries,1), nResults, 'UniformOutput',false);
oneToN = cellfun(@transpose, oneToN, 'UniformOutput',false);


for n=1:nQueries,
    if size(oneToN{n},1)==0,
        oneToN{n} = oneToN{n}';
    end
end

precisionAtCutoff = cellfun(@rdivide, numberAtCutoff, oneToN, 'UniformOutput', false);
pTimesRel = cellfun(@dot, precisionAtCutoff, relevances, 'UniformOutput', false);
averagePrecision = cellfun(@rdivide, pTimesRel, nTruths);

mAP = mean(averagePrecision);
