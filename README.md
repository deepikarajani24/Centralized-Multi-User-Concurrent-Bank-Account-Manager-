Time-Synchronization-in-distributed-systems

COMPILATION:
make clean
make compile
This will compile all the source files.

RUN: Run with script file, script takes two parameters
-	number of processes let’s say N 
-	starting port number let’s say P. 
The script creates N processes, first process is assigned Port Number P, second process gets Port Number P+1 and so on.

Command ./script.sh 4 8000 
