function precAtR = compute_precision_at_r(truth, results)
% precAtR = compute_precision_at_r(truth, results)


nTruths = cellfun(@numel,truth,'UniformOutput', false);    
nQueries = numel(results);

% minus one to get real cnt
oneVec = mat2cell(ones(nQueries,1),ones(nQueries,1));
nTruths = cellfun(@minus,nTruths,oneVec,'UniformOutput',false);

% in case nTruths becomes 0 (this operation is necessary to experiment with
% partial datasets)
nTruths = cellfun(@max,nTruths,oneVec,'UniformOutput',false);

logicalRelevant = cellfun(@ismember, results, truth, 'UniformOutput', false);
indexRelevant = cellfun(@find, logicalRelevant, 'UniformOutput', false);

nLessThanR = cellfun(@sum, cellfun(@le, indexRelevant, nTruths, 'UniformOutput', false) , 'UniformOutput', false);
precAtR = cellfun(@rdivide, nLessThanR, nTruths); 

precAtR = mean(precAtR);


return;
