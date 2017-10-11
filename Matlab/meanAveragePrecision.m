function map = meanAveragePrecision(truth, results)
% map = meanAveragePrecision(truth, results)
%
% input
%   truth, results       cell arrays with column vectors of integers
%
%   the elements are labels of images and each vector is, respectively, 
%   the ground truth and the outcome of a query
%
%   the elements in truth{n} and results{n} should intersect as much as
%   possible


nTruths = cellfun(@numel,truth); % to be used for denominators    % truth is a cell array


% quali (in results) coincidono tra results e truth
[~,indInResults] = cellfun(@intersect,results, truth,'UniformOutput', false);
% riordina indInResults nell'ordine in cui compaiono in results
indInResSorted = cellfun(@sort, indInResults,'UniformOutput', false);  % come il denominatore in fondo a pagina 7 di Zhu
% un trucco: indInResSorted è già riordinato, avremo vettori (1:n)  in ogni
% cell
[~,numerators] = cellfun(@sort,indInResSorted,'UniformOutput', false);
% i termini della somma per average precision
fractions = cellfun(@rdivide,numerators,indInResSorted,'UniformOutput', false);

% average precision
allSums = cellfun(@sum,fractions);
averagePrecision = allSums./nTruths;

map = mean(averagePrecision);



return;








% double mean_average_precision (QRA **truth, QRA **results, int n)
% {
%   int relevant, queries = 0;
%   double average_precision, MAP = 0.0;
%   int i, j, k;
% 
%   /* scan records: */
%   for (i=0; i<n; i++) {
%     /* sanity check: */
%     if (!strcasecmp(truth[i]->query, results[i]->query)) {  // _strcasecmp ignores case in comparison
%       relevant = 0; 
%       average_precision = 0.;
%       /* scan results	*/
%       for (j=0; j<results[i]->n_matches; j++) {
%         /* scan ground truth */
%         for (k=0; k<truth[i]->n_matches; k++) {
%           if (!strcasecmp(truth[i]->matches[k], results[i]->matches[j])) {
%             /* ground truth match found */
%             relevant ++;
%             average_precision += (double)relevant / (double) (j+1); // j+1 = rank
%             break;
%           }
%         }
%       }
%       /* normalize average precision and add it to MAP:  */
%       if (truth[i]->n_matches) average_precision /= truth[i]->n_matches;
%       MAP += average_precision;
%       queries ++;
%     }
%   }
% 
%   /* normalize and return MAP value: */
%   if (queries) MAP /= (double) queries;
%   return MAP;
% }


