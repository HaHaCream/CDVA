function recAtN = compute_recall_at_N(truth, results, N)
% recAtN = compute_recall_at_N(truth, results, N)

% numbers of relevant items
nTruths = cellfun(@numel,truth,'UniformOutput', false);    

logicalRelevant = cellfun(@ismember, results, truth, 'UniformOutput', false);
indexRelevant = cellfun(@find, logicalRelevant, 'UniformOutput', false);

recAtN = zeros(numel(N),1);

for k = 1:numel(N),

  N_asCell = num2cell(N(k)*ones(numel(truth),1));
  nLessThanN = cellfun(@sum, cellfun(@le, indexRelevant, N_asCell, 'UniformOutput', false) , 'UniformOutput', false);
  recAsCell = cellfun(@rdivide, nLessThanN, nTruths); 
  recAtN(k) = mean(recAsCell);
  
end

return;
