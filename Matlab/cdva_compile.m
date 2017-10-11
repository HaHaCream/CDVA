function cdvs_compile(target)
%CDVS_COMPILE Compile cdvs MEX files
%
% cdvs_compile(target) compiles only the specified functions.
%
% target can be: all (default), cdva_eval_mex.


if nargin < 1
    target = 'all';
end

pm = fileparts(which('cdva_compile'));

% Compile MEX files

if strcmpi(target, 'all') || strcmpi(target, 'cdva_eval_mex')
    disp('Building cdva_eval_mex');
    arg = {'-outdir'; pm; [pm '/cdva_eval_mex.cpp']; [pm '/CSVReader.cpp']};
    mex(arg{:});
end

return
