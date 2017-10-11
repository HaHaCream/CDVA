function J = getJaccard(theIntersections, theUnions)

if iscell(theIntersections),
    
    lowEdges = cellfun(@(x) x(1), theIntersections, 'UniformOutput', false);
    highEdges = cellfun(@(x) x(2), theIntersections, 'UniformOutput', false);
    isLengths = cellfun(@(x,y) y-x,  lowEdges, highEdges, 'UniformOutput', false);
    
    lowEdges = cellfun(@(x) x(1), theUnions, 'UniformOutput', false);
    highEdges = cellfun(@(x) x(2), theUnions, 'UniformOutput', false);
    uLengths = cellfun(@(x,y) y-x,  lowEdges, highEdges, 'UniformOutput', false);
    
    smallTerms = repmat({1e-10}, numel(uLengths),1); % so that no denominator is zero
    uLengths = cellfun(@plus, uLengths, smallTerms, 'UniformOutput', false);
    
    % the Jaccard index
    J = cellfun(@rdivide, isLengths, uLengths, 'UniformOutput', false);
    
else
    
    lowEdges = theIntersections(:,1);
    highEdges = theIntersections(:,2);
    isLengths = highEdges-lowEdges;
    
    lowEdges = theUnions(:,1);
    highEdges = theUnions(:,2);
    uLengths = highEdges-lowEdges + 1e-10;
    
    J = isLengths./uLengths;
    
end

return;


