function [numericReference, refID] = getNumericalReferences(retrievalTable, namesInDB, header)
% numref = getNumericalReferences(retrievalTable, namesInDB, header)
%
% for each query, a vector of id numbers pointing to videos in the DB is
% produced
%
% The output is a cell array with one cell per query. Each cell contains a
% column vector of numbers
%

% get the names of reference videos
refNames = retrievalTable{strcmp(header,'matchingReferenceID')};
% transform into ID numbers (numbers wrt. to the DB)
referenceID = zeros(numel(refNames),1);
for k=1:numel(referenceID),
    foundID = find(strcmp(refNames{k}, namesInDB), 1 );
    if ~isempty(foundID),
        referenceID(k) = foundID;
    else
        referenceID(k) = -1;
    end
end

% get the ID numbers of the queries (numbers wrt. to the table of queries)
queryID = retrievalTable{strcmp(header,'id')};  

% uniqueQueryIDs = unique(queryID,'stable');
% for compatibility with Octave:
[~,I] = unique(queryID);
uniqueQueryIDs = queryID(sort(I));

% get the ID numbers of the references
numericReference = cell(numel(uniqueQueryIDs),1);
refID  = cell(numel(uniqueQueryIDs),1);
for k=1:numel(uniqueQueryIDs),
    refID{k}  = uniqueQueryIDs(k); 
    numericReference{k} = referenceID(queryID == refID{k}); 
end

return;