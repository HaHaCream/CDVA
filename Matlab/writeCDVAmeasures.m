function wrote = writeCDVAmeasures(measurePath, CDVAmeasures, CDVAdata, descrLengths)

if ~exist(measurePath, 'dir'),
    mkdir(measurePath);
end

nLengths = numel(descrLengths);




%  pairwise matching 
 fmt =  '%5.3f \t ';  % column for the true negative rates
 for lNo = 1:nLengths,
    fmt = [fmt '%5.3f \t ']; % columns for the true positive rates
 end
 fmt = [fmt '\n'];
 
 outMatrix = CDVAmeasures.TN;
 for lNo = 1:nLengths,    
     command = ['outMatrix = [outMatrix CDVAmeasures.ROC' descrLengths{lNo} '];'];
     eval(command);
 end
   

     
fp = fopen(fullfile(measurePath, 'roc.txt'),'w');
wrote = fprintf(fp, fmt, outMatrix');
fclose(fp);


% descriptor sizes 
fp = fopen(fullfile(measurePath, 'querySizes.txt'),'w');
for lNo = 1:nLengths,  
    
     if ~strcmp(descrLengths{lNo},'16K_256K')  && ~strcmp(descrLengths{lNo},'1K_4K'),
         
    command = ['fprintf(fp, ''Query descriptors at size ' descrLengths{lNo} ' \n '');'];
    eval(command);
    
    command = ['fprintf(fp, '' average length %5.2f \n '' ,  CDVAmeasures.Q.averageLength'  descrLengths{lNo} ');'];
    eval(command);
    
    command = ['fprintf(fp, '' max length %5.2f \n '' ,  CDVAmeasures.Q.maxLength'  descrLengths{lNo} ');'];
    eval(command);
    
    command = ['fprintf(fp, '' n descriptors above %d:  %d \n '' , ' descrLengths{lNo} ' , CDVAmeasures.Q.nLongerThan'  descrLengths{lNo} ');'];
    eval(command);

    fprintf(fp,'\n');

     end
    
end

fclose(fp);
% retrieval
% CDVAmeasures.Q
% CDVAmeasures.R