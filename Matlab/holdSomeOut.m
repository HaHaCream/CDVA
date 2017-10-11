function results = holdSomeOut(results, ignore, fromWhich)
%  results = holdSomeOut(results, ignore, fromWhich)

for k=1:numel(ignore), 
    % results{fromWhich{k}} = setdiff( results{fromWhich{k}}, ignore{k}); 
    
    % remove element both in results{fromWhich{k}} and ignore{k}
    % and keep the element order in results{fromWhich{k}}
    [~, loc] = ismember(ignore{k}, results{fromWhich{k}});
    if loc >= 1 
        results{fromWhich{k}}(loc) = [];
    end
end

return;


