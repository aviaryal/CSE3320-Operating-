1.How to compile?
gcc -o Aryal_7418 Aryal_7418.c

2.How to run?
./Aryal_7418.c 
if you want to print details informations then use
./Aryal_7418.c print
if you want to print only sorted latitude then use
./Aryal_7418.c lat

Instrument
On my virtual box 
using 01 process it took 48 sec
using 02 process it took 19 sec
using 04 process it took 4 sec
using 10 process it took 1 sec

on putty 
using 01 process it took 22 sec
using 02 process it took 11 sec
using 04 process it took 3 sec
using 10 process it took 1 sec


//note since my computer sorted array in 0 sec when used 10 process I have double data in csv file

// Compile threaded porgramme
gcc -o Aryal_7418_threaded Aryal_7418_threaded.c -g -pthread