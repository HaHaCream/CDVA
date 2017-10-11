#! /bin/bash
#
# Create documentation from source files using Doxygen.
# (to install doxygen on Ubuntu 15.10: sudo apt-get install doxygen doxygen-gui graphviz texlive-full)
# (to install doxygen on Ubuntu 16.04: sudo apt install doxygen doxygen-gui graphviz texlive-full )
#
doxygen doxylinux.cfg > doxygen.log 2> error.log
echo "Done. Check results in doxygen.log and errors in error.log"
