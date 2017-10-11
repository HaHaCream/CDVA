function J = temporalLocalisation(detectedIntervals, trueIntervals)
% J = temporalLocalisation(detectedIntervals, trueIntervals)

intersections = getSection(detectedIntervals, trueIntervals);
unions = getUnion(detectedIntervals, trueIntervals);
J = getJaccard(intersections, unions);


