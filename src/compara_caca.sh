#!/bin/sh

ulimit -c unlimited
ulimit -c
rm -fv /cores/*
rm -fv core*
#mv -fv ya_echos/* .
rm -rfv ya_echos
mkdir ya_echos
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
	return_codi=$?
	date
	if [ $return_codi -ne 0 ] 
	then
		echo "verga, fallo en $caca"
		exit 1
	fi
	date
#	python cagada/shit.py < $caca > $caca_log_piton
	./shame < $caca > $caca_log_piton
	return_codi=$?
	date
	diferencia_ojala=$(diff $caca_log_c $caca_log_piton)
	echo "la diferencia es"
	#echo "$diferencia_ojala CACA"
	if [[ ! -z "$diferencia_ojala" && $return_codi -eq 0 ]]
	then
		echo "verga, diff en $caca"
		exit 1
	fi
	mv $caca ya_echos/
done
