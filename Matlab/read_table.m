function [table, header] = read_table(tablefile)
% [table, header] = read_table(tablefile)
%
% Read a CDVA .csv file in a cell variable table with a number of element
% equal to the number of column in the .cvs file


if ~exist(tablefile, 'file')
    error(['File ' tablefile ' not found']);
end


text = fileread(tablefile);

if ~isempty(text)
    c = textscan(text, '%s', 'Delimiter', '\n');
    
    list = cell(size(c{1}));
    for i=1:numel(c{1})
        
        texts = textscan(c{1}{i}, '%s', 'Delimiter', ',');
        
        list(i) = texts;
        
    end
    
else
    list = {};
end


header = cell(1, numel(list{end}));
for j=1:numel(list{1})
    header{j} = strtrim( list{1}{j} );
end


% remove the header
list = list(2:end);

table = cell(1, numel(header));

if ~isempty(list)
    
    numeric = false(1, numel(header));
    
    for j=1:numel(header)
        
        if isempty(str2num(list{1}{j})) %#ok<*ST2NM>
            
            table{j} = cell(numel(list),1);
            
        else
            
            numeric(j) = true;
            table{j} = zeros(numel(list),1);
            
        end
        
    end
    
    
    for i=1:numel(list)
        
        for j=1:numel(header)
            
            if numeric(j)
                
                n = str2num(list{i}{j});
                if ~isempty(n)
                    table{j}(i) = n;
                end
                
            else
                
                table{j}{i} = strtrim( list{i}{j} );
                
            end
            
        end
        
    end
    
end


return