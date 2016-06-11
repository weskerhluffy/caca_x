#!/bin/sh -x
for caca in $(seq 20)
do
	echo "iteracion $caca"
	cat /tmp/caca_fiesta_mision_2_random.txt >> caca_fiesta_mision_2_random_big.txt
done
