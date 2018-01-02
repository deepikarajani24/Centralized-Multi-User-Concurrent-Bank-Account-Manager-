#!/bin/bash

count=$1;
port=$2;
> Berkley.txt
for i in $(seq 1 $count);
do {
echo "port is $port"
echo $port >> Berkley.txt
port=$((port+1))
} done

port=$2;
for i in $(seq 1 $((count-1)));
do {
echo $port
gnome-terminal -e "bash -c \"./TimeSynchronization $port $count 127.0.0.1 3; exec bash\""

port=$((port+1))
} done
gnome-terminal -e "bash -c \"./TimeSynchronization $port $count 127.0.0.1 1; exec bash\""
exit 0
