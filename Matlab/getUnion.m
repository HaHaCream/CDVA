function S = getUnion(firstInterval, secondInterval)
% S = getUnion(firstInterval, secondInterval)
%
% construction of the union of two overlapping intervals.
% if the intervals are disjoint, the results are incorrect but this will not influence the computation of the Jaccard index
% which is (length of intersection)/(length of union)


if ~iscell(firstInterval),
    firstInterval = sort(firstInterval, 2, 'ascend');
    secondInterval = sort(secondInterval, 2, 'ascend');    
    
    a = firstInterval(:,1); b = firstInterval(:,2);
    c = secondInterval(:,1); d = secondInterval(:,2);
    
    S = [min(a,c) max(b,d)];
     
else
    
    firstInterval   = cellfun(@sort, firstInterval, 'UniformOutput', false); % in ascending order
    secondInterval  = cellfun(@sort, secondInterval, 'UniformOutput', false); % in ascending order
    
    a = cellfun(@(x) x(1), firstInterval, 'UniformOutput', false);
    b = cellfun(@(x) x(2), firstInterval, 'UniformOutput', false);
    c = cellfun(@(x) x(1), secondInterval, 'UniformOutput', false);
    d = cellfun(@(x) x(2), secondInterval, 'UniformOutput', false);
    

    lowEdges    = cellfun(@min, a, c, 'UniformOutput', false);
    highEdges   = cellfun(@max, b, d, 'UniformOutput', false);
    
    S = cellfun(@(x,y) [x y], lowEdges, highEdges, 'UniformOutput', false);   
  
end

return;


