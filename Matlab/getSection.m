function S = getSection(firstInterval, secondInterval)


if ~iscell(firstInterval),
    firstInterval = sort(firstInterval, 2, 'ascend');
    secondInterval = sort(secondInterval, 2, 'ascend');
    
    
    
    a = firstInterval(:,1); b = firstInterval(:,2);
    c = secondInterval(:,1); d = secondInterval(:,2);
       
%     if c>b || d<a,
%         % DISJOINT
%         S = [];
%     elseif (c<=a && d<=b) || (a<=c && b<=d)
%         % SUBSET
%         S = [max(a,c) min(b,d)];
%     else
%         % PARTIAL OVERLAP
%         S = [max(a,c) min(b,d)];
%     end
    
    S = [max(a,c) min(b,d)];
    % if S(1) > S(2) then the sets are disjoint
    disjointSets = (S(:,1)>S(:,2));
    S(disjointSets,:) = 0;
    
else
    
    firstInterval   = cellfun(@sort, firstInterval, 'UniformOutput', false); % in ascending order
    secondInterval  = cellfun(@sort, secondInterval, 'UniformOutput', false); % in ascending order
    
    a = cellfun(@(x) x(1), firstInterval, 'UniformOutput', false);
    b = cellfun(@(x) x(2), firstInterval, 'UniformOutput', false);
    c = cellfun(@(x) x(1), secondInterval, 'UniformOutput', false);
    d = cellfun(@(x) x(2), secondInterval, 'UniformOutput', false);
    

    lowEdges    = cellfun(@max, a, c, 'UniformOutput', false);
    highEdges   = cellfun(@min, b, d, 'UniformOutput', false);
    
    % if lowEdges{k} > highEdges{k} then the sets in cell k are disjoint
    % in these cases the interval boundaries are set to zero
    areDisjoint = cellfun(@gt, lowEdges, highEdges, 'UniformOutput', false);  
    areDisjoint = cell2mat(areDisjoint);
    indDisjoint = find(areDisjoint)';
    
    for k=indDisjoint,
       lowEdges{k} = 0;
       highEdges{k} = 0;
    end
    
    
    S = cellfun(@(x,y) [x y], lowEdges, highEdges, 'UniformOutput', false);   
   
    
    
    
end

return;


