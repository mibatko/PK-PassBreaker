g++ -o master master.cpp -I$PVM_ROOT/include -L$PVM_ROOT/lib/LINUX -lpvm3 -lnsl -lcrypt
g++ -o worker worker.cpp -I$PVM_ROOT/include -L$PVM_ROOT/lib/LINUX -lpvm3 -lnsl -lcrypt
mkdir /usr/lib/pvm3/bin/LINUX64
cp worker /usr/lib/pvm3/bin/LINUX64
cp master /usr/lib/pvm3/bin/LINUX64