#!/bin/bash
rm -rf *.log
for number in {1..100}
do
./shm_client "c$number" > log_client_$number.log&
sleep 1
done
exit 0
