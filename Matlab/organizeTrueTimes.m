function trueTimesOfMatch = organizeTrueTimes(matchingTable, timeTruth, headerMatching, headerTime)
% trueTimesOfMatch = organizeTrueTimes(matchingTable, timeTruth, headerMatching, headerTime)

nMatches = numel(matchingTable{1});
cellForMatchNames =  strcmp(headerMatching,'queryName');
cellForTimeNames =  strcmp(headerTime,'videoID');

trueTimeIndex = zeros(nMatches,1);



for mno = 1:nMatches,
   queryName = matchingTable{cellForMatchNames}(mno);
   trueTimeIndex(mno) = find(strcmp(queryName, timeTruth{cellForTimeNames}));
end

cellForStart = strcmp(headerTime,'start');
cellForEnd   = strcmp(headerTime,'end');

trueStart = timeTruth{cellForStart}(trueTimeIndex);
trueEnd   = timeTruth{cellForEnd}(trueTimeIndex);

trueTimesOfMatch = [trueStart trueEnd];

return;



