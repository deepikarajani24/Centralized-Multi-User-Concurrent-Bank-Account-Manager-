Time-Synchronization-in-distributed-systems

COMPILATION:
Command : make clean
Command : make compile
This will compile all the source files.

RUN:
Run with script file
Script takes two parameters,
-	number of processes let’s say N 
-	starting port number let’s say P. 
So, the script creates N processes, first process is assigned Port Number P, second process gets Port Number P+1 and so on.

Command ./script.sh 4 8000 
