#!/bin/bash

#The amount of used CPUs is limited to avoid using more cores than the ones that are actually available. 
# Uncomment for each benchmark

while true
do
    printf '2\nn\n' | taskset -c 2,3,6,7 phoronix-test-suite benchmark x265 &
    #printf 'n\n' | taskset -c 2,3,6,7 phoronix-test-suite benchmark mnn &
    #printf '1\nn\n' | taskset -c 2,3,6,7 phoronix-test-suite benchmark opencv &
    #printf '3\nn\n' | taskset -c 2,3,6,7 phoronix-test-suite benchmark cassandra &
    #printf 'n\n' | taskset -c 2,3,6,7 phoronix-test-suite benchmark build-gcc &
    #printf 'n\n' | taskset -c 2,3,6,7 phoronix-test-suite benchmark cloudsuite-ws &
    #printf '4\nn\n' | taskset -c 2,3,6,7 phoronix-test-suite benchmark sqlite &
    #printf '3\n3\n3\nn\n' | taskset -c 2,3,6,7 phoronix-test-suite benchmark pgbench &
done


