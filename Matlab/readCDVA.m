function CDVAdata = readCDVA(csvPath, descrLengths, param)

nLengths = numel(descrLengths);



if ~exist('param','var'),
    param = [];
end

if isfield(param,'skipRetrieval'),
    skipRetrieval = param.skipRetrieval;
else
    skipRetrieval = false;
end

if isfield(param,'truthPath'),
    truthPath = param.truthPath;
else
    truthPath = ''; % read Retrieval.GT.csv, Timing.GT.csv, Ignore.GT.csv from the present working directory
end




nSingleLengths = 0;
for lNo=1:nLengths,
    if ~strcmp(descrLengths{lNo},'16K_256K') && ~strcmp(descrLengths{lNo},'1K_4K'),
        nSingleLengths = nSingleLengths+1;
    end
end

nSteps = nSingleLengths + nLengths + 1 + nLengths + 2 + 5 + nSingleLengths;
stepNo = 0;

% read the files produced by CDVA
% Extract
h = waitbar(0,'reading queries and references...');
for lNo = 1:nLengths,
    if ~strcmp(descrLengths{lNo},'16K_256K') && ~strcmp(descrLengths{lNo},'1K_4K'),
        command = ['[CDVAdata.Q' descrLengths{lNo} ', CDVAdata.headerQuery] = read_table(''' fullfile(csvPath,'Queries') '.' descrLengths{lNo} '.csv''); '];
        eval(command);
        
        command = ['[CDVAdata.R' descrLengths{lNo} ', CDVAdata.headerReference] = read_table(''' fullfile(csvPath,'References') '.' descrLengths{lNo} '.csv''); '];
        eval(command);
        stepNo = stepNo+1; waitbar( stepNo/nSteps , h, 'reading queries and references...');
    end
end


% Match
stepNo = stepNo+1; waitbar( stepNo/nSteps , h, 'reading annotations for pairwise matching...');

for lNo = 1:nLengths,
    % read the results for the matching pairs
    command = ['[CDVAdata.M' descrLengths{lNo} ', CDVAdata.headerMatching] = read_table(''' fullfile(csvPath,'MatchingPairs') '.' descrLengths{lNo} '.csv''); '];
    eval(command);
    
    % read the results for the nonmatching pairs
    command = ['[CDVAdata.N' descrLengths{lNo} ', CDVAdata.headerNonMatching] = read_table(''' fullfile(csvPath,'NonMatchingPairs') '.' descrLengths{lNo} '.csv''); '];
    eval(command);
    
    stepNo = stepNo+1; waitbar( stepNo/nSteps , h, 'reading annotations for pairwise matching...');
end


% Time annotation
stepNo = stepNo+1; waitbar( stepNo/nSteps , h,'reading the annotation of times for pairwise matching...');
command = ['[CDVAdata.TGT, CDVAdata.headerTGT] = read_table(''' fullfile(truthPath,'Timing.GT.csv') ''');'];
eval(command);

for lNo = 1:nLengths,
    command = ['matchingTable = CDVAdata.M' descrLengths{lNo} ';'];
    eval(command);
    
    trueTimesOfMatch = organizeTrueTimes(matchingTable, CDVAdata.TGT, CDVAdata.headerMatching, CDVAdata.headerTGT);
    command = ['CDVAdata.M' descrLengths{lNo} ' = [ CDVAdata.M' descrLengths{lNo} ' trueTimesOfMatch(:,1) trueTimesOfMatch(:,2) ];'];
    eval(command);
end

CDVAdata.headerMatching = [CDVAdata.headerMatching 'trueStart' 'trueEnd'];



% Retrieval

if ~skipRetrieval,
    % read the file names in the DB
    stepNo = stepNo+1; waitbar( stepNo/nSteps , h,'reading names of the references and distractors...');
    command = ['[DB, headerDB] = read_table(''' fullfile(csvPath,'Database.DB.csv') ''');'];
    eval(command);
    
    
    
    % read ground truth
    stepNo = stepNo+1; waitbar( stepNo/nSteps , h,'reading the annotation for retrieval...');
    command = ['[GT, headerGT] = read_table(''' fullfile(truthPath, 'Retrieval.GT.csv') ''');'];
    eval(command);
    
    
    % the names of files used as queries
    namesInDB = DB{ strcmp(headerDB,'pathname') };
    
    % the ground truth references: what are their ids in the DB
    stepNo = stepNo+1; waitbar( stepNo/nSteps , h,'encoding file names...');
    [CDVAdata.truth, aa] = getNumericalReferences(GT, namesInDB, headerGT);
    
    
    % read names of videos to ignore
    stepNo = stepNo+1; waitbar( stepNo/nSteps , h,'reading the hold-one-out annotation for retrieval...');
    command = ['[ToIgnore, headerIgnore] = read_table(''' fullfile(truthPath, 'Ignore.GT.csv') ''');'];   %%%% poi deve stare nella stessa cartella!!!!!!!!
    eval(command);
    
    % the hold-one-out videos: what are their ids in the DB
    stepNo = stepNo+1; waitbar( stepNo/nSteps , h,'encoding file names...');
    [CDVAdata.ignore, fromWhich]  = getNumericalReferences(ToIgnore, namesInDB, headerIgnore);
    
    
    
    
    % get the retrieval results
    stepNo = stepNo+1; waitbar( stepNo/nSteps , h,'reading the retrieval results...');
    for lNo = 1:nLengths,
        if ~strcmp(descrLengths{lNo},'16K_256K') && ~strcmp(descrLengths{lNo},'1K_4K'),
            command = ['[RET_' descrLengths{lNo} ' , headerRetrieval] = read_table(''' fullfile(csvPath,['Retrieval.'  descrLengths{lNo}  '.csv']) ''');'];
            eval(command);
            
            
            % the retrieval outcome
            command = ['CDVAdata.results' descrLengths{lNo} ' = getNumericalReferences(RET_' descrLengths{lNo} ', namesInDB, headerRetrieval);'];
            eval(command);
            
            
            % for each query, hold out those detected references that contain replicas
            % of the query video
            command = ['CDVAdata.results' descrLengths{lNo} ' = holdSomeOut(CDVAdata.results' descrLengths{lNo} ', CDVAdata.ignore, fromWhich);'];
            eval(command);
            
            
            stepNo = stepNo+1; waitbar( stepNo/nSteps , h, 'reading the retrieval results...');
            
        end
    end

end
delete(h);

