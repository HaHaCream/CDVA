function printCDVAmeasures(CDVAmeasures, CDVAdata, descrLengths, param)

% CDVAmeasures =
%
%                   Q: [1x1 struct]
%                   R: [1x1 struct]
%              ROC16K: [51x1 double]
%          TPat1FP16K: 0.0126
%              ROC64K: [51x1 double]
%          TPat1FP64K: 0.0094
%             ROC256K: [51x1 double]
%         TPat1FP256K: 0.0115
%         ROC16K_256K: [51x1 double]
%     TPat1FP16K_256K: 0.0126
%          TempLoc16K: 0.1418
%          TempLoc64K: 0.1415
%         TempLoc256K: 0.1384
%     TempLoc16K_256K: 0.1419
%              mAP16K: 0
%          precAtR16K: 0
%              mAP64K: 0
%          precAtR64K: 0
%             mAP256K: 0
%         precAtR256K: 0
%                  TN: [51x1 double]
%

nLengths = numel(descrLengths);

if ~exist('param','var'),
    param = [];
end

if isfield(param,'skipRetrieval'),
    skipRetrieval = param.skipRetrieval;
else
    skipRetrieval = false;
end



mAP_table = [];
precAtR_table = [];

refAvLengths = [];
queAvLengths = [];

refMaxLengths = [];
queMaxLengths = [];

refN_above = [];
queN_above = [];

TP_at_givenFP = [];
TempLoc         = [];


for lNo = 1:nLengths,
    if ~strcmp(descrLengths{lNo},'16K_256K')  && ~strcmp(descrLengths{lNo},'1K_4K'),
        
        
        if ~skipRetrieval,
            command = ['mAP_table = [mAP_table      CDVAmeasures.mAP' descrLengths{lNo} '];'];
            eval(command);

            command = ['precAtR_table = [precAtR_table      CDVAmeasures.precAtR' descrLengths{lNo} '];'];
            eval(command);
        end
        
        command = ['refAvLengths = [refAvLengths      CDVAmeasures.R.averageLength' descrLengths{lNo} '];'];
        eval(command);
        command = ['queAvLengths = [queAvLengths      CDVAmeasures.Q.averageLength' descrLengths{lNo} '];'];
        eval(command);
        
        command = ['refMaxLengths = [refMaxLengths      CDVAmeasures.R.maxLength' descrLengths{lNo} '];'];
        eval(command);
        command = ['queMaxLengths = [queMaxLengths      CDVAmeasures.Q.maxLength' descrLengths{lNo} '];'];
        eval(command);
        
        
        command = ['refN_above = [refN_above      CDVAmeasures.R.nLongerThan' descrLengths{lNo} '];'];
        eval(command);
        command = ['queN_above = [queN_above      CDVAmeasures.Q.nLongerThan' descrLengths{lNo} '];'];
        eval(command);
        
        command = ['howManyQ = int2str(CDVAmeasures.Q.howmany' descrLengths{lNo} ');'];
        eval(command);
        
        
        command = ['howManyR = int2str(CDVAmeasures.R.howmany' descrLengths{lNo} ');'];
        eval(command);
        
        
        
    end
    
    command = ['TP_at_givenFP = [TP_at_givenFP      CDVAmeasures.TPat1FP' descrLengths{lNo} '];'];
    eval(command);
    
    
    command = ['TempLoc = [TempLoc      CDVAmeasures.TempLoc' descrLengths{lNo} '];'];
    eval(command);
    
end

% output measures required by the Evaluation Framework document

fprintf(1,'%53.53s ', 'Descriptor lengths (Bps):');         for k=1:min(3, nLengths), fprintf(1,'%10.10s ', descrLengths{k}); end;  fprintf(1,'\n');
fprintf(1,'%53.53s ', 'Query average lengths:');            for k=1:min(3, nLengths), fprintf(1,'%10.2f ', queAvLengths(k)); end;   fprintf(1,'\n');
fprintf(1,'%53.53s ', 'Query max lengths:');                for k=1:min(3, nLengths), fprintf(1,'%10.2f ', queMaxLengths(k)); end;  fprintf(1,'\n');
fprintf(1,'%53.53s ', 'Reference average lengths:');        for k=1:min(3, nLengths), fprintf(1,'%10.2f ', refAvLengths(k)); end;   fprintf(1,'\n');
fprintf(1,'%53.53s ', 'Reference max lengths:');            for k=1:min(3, nLengths), fprintf(1,'%10.2f ', refMaxLengths(k)); end;  fprintf(1,'\n');
fprintf(1,'\n\n');

% fprintf(1,'%53.53s %10.10s %10.10s %10.10s \n', 'Descriptor lengths (Bps):', descrLengths{1:3});
% fprintf(1,'%53.53s %10.2f %10.2f %10.2f \n', 'Query average lengths:', queAvLengths);
% fprintf(1,'%53.53s %10.2f %10.2f %10.2f \n', 'Query max lengths:', queMaxLengths);
% fprintf(1,'%53.53s %10.2f %10.2f %10.2f \n', 'Reference average lengths:', refAvLengths);
% fprintf(1,'%53.53s %10.2f %10.2f %10.2f \n', 'Reference max lengths:', refMaxLengths);
% fprintf(1,'\n\n');

% Retrieval
if ~skipRetrieval,
    
    fprintf(1,'%53.53s ', 'Retrieval performance at:');             for k=1:min(3, nLengths), fprintf(1,'%10.10s ', descrLengths{k}); end;  fprintf(1,'\n');
    fprintf(1,'%53.53s ', 'Mean average precision:');               for k=1:min(3, nLengths), fprintf(1,'%10.3f ', mAP_table(k)); end;   fprintf(1,'\n');
    fprintf(1,'%53.53s ', 'r-Precision:');                          for k=1:min(3, nLengths), fprintf(1,'%10.3f ', precAtR_table(k)); end;   fprintf(1,'\n');
    
%     fprintf(1,'%53.53s %10.10s %10.10s %10.10s \n', 'Retrieval performance at:', descrLengths{1:3});
%     fprintf(1,'%53.53s %10.3f %10.3f %10.3f \n', 'Mean average precision:', mAP_table);
%     fprintf(1,'%53.53s %10.3f %10.3f %10.3f \n', 'r-Precision:', precAtR_table);
    
    % for n = 1:numel(CDVAmeasures.N_for_rec),
    %     fprintf(1,'%20.20s %5.0f : %10.3f %10.3f %10.3f \n', 'Recall at', recAtN_table(n,:));
    % end
    fprintf(1,'\n');
end

% Pairwise matching
fprintf(1,'%53.53s ', 'Pairwise matching performance at:');             for k=1:min(4, nLengths), fprintf(1,'%10.10s', descrLengths{k}); end; fprintf(1,'\n');
fprintf(1,'%53.53s ', 'True positive rate at 1% false positive rate:'); for k=1:min(4, nLengths), fprintf(1,'%10.3f', TP_at_givenFP(k)); end; fprintf(1,'\n');
fprintf(1,'%53.53s ', 'Mean Jaccard index for temporal localisation:'); for k=1:min(4, nLengths), fprintf(1,'%10.3f', TempLoc(k)); end; fprintf(1,'\n');



% fprintf(1,'%53.53s %10.10s %10.10s %10.10s %10.10s\n', 'Pairwise matching performance at:', descrLengths{1:4});
% fprintf(1,'%53.53s %10.3f %10.3f %10.3f %10.3f\n', 'True positive rate at 1% false positive rate:', TP_at_givenFP(1:4));
% fprintf(1,'%53.53s %10.3f %10.3f %10.3f %10.3f\n', 'Mean Jaccard index for temporal localisation:', TempLoc(1:4));

allCDVAmeasures = [];


for lNo = 1:nLengths,
    
    command = ['allCDVAmeasures = [allCDVAmeasures CDVAmeasures.ROC' descrLengths{lNo} '];'];
    eval(command);
    
end


% Show pairwise matching ROC


plot(CDVAmeasures.TN, allCDVAmeasures,'LineWidth',2);
axis([min(CDVAmeasures.TN) max(CDVAmeasures.TN) 0 1])
title('CDVA Pairwise matching','FontSize',18);
xlabel('True Negative','FontSize',15);
ylabel('True Positive','FontSize',15);


figLabels = descrLengths;
for lNo=1:nLengths,
    underscorePos = strfind(figLabels{lNo},'_');
    if ~isempty(underscorePos),
        figLabels{lNo} = [figLabels{lNo}(1:underscorePos-1) '\' figLabels{lNo}(underscorePos:end)];
    end
end

lobj = legend(figLabels ,'Location','southwest');
set(lobj,'FontSize',15);

