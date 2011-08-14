#!/bin/bash

make -C test/support/MockAVR all; rc1=$?
make -C test test; rc2=$?
make -C main all; rc3=$?

rc=0
if [ $rc1 -ne 0 ] || [ $rc2 -ne 0 ] || [ $rc3 -ne 0 ]; then
    growlnotify -a Xcode -n simple-ci -d bravo5.simpleci -t CI -m "BUILD FAILED" 
    rc=1
# else
#     growlnotify -a Xcode -n simple-ci -d bravo5.simpleci -m "Success" 
fi

exit $rc
