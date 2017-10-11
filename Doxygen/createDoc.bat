@echo off
echo you need to have doxygen and graphviz in your path for this to work...
doxygen doxygen.cfg > doxygen.log 2> error.log
echo Done. Check results in doxygen.log and error.log
pause
