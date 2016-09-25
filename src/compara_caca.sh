#!/bin/sh

ulimit -c unlimited
ulimit -c
rm -fv /cores/*
rm -fv core*
for caca in $(ls *txt)
do
	caca_truncada=""
	return_codi=0

	caca_truncada=${caca%.txt}
	echo "la caca actual $caca"
	echo "la caca truncada $caca_truncada"
	caca_log_c=$caca_truncada.c.log
	caca_log_piton=$caca_truncada.piton.log
	echo "la caca en c $caca_log_c"
	echo "la caca en piton $caca_log_piton"
	date
	./caca_x < $caca > $caca_log_c
	date
	return_codi=$?
	if [ $return_codi -ne 0 ] 
	then
		echo "verga, fallo en $caca"
		exit 1
	fi
	date
	python cagada/shit.py < $caca > $caca_log_piton
	date
	diferencia_ojala=$(diff $caca_log_c $caca_log_piton)
	echo "la diferencia es"
	echo "$diferencia_ojala CACA"
done
