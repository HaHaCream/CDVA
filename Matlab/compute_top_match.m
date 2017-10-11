function topMatchSuccessRate = compute_top_match(truth, results)

% get first item in results
firstResults = cellfun(@(x) x(1), results, 'UniformOutput', false);
% is the first item relevant
firstIsRelevant = cellfun(@intersect,firstResults, truth, 'UniformOutput', false);
% how many are irrelevant
nIrrelevant = sum(cell2mat(cellfun(@isempty, firstIsRelevant, 'UniformOutput', false)));
% how many are relevant
nRelevant = numel(truth) - nIrrelevant;
topMatchSuccessRate = nRelevant/numel(truth);

return;
