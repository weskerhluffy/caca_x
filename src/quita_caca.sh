#!/bin/sh -x
cp -fv caca_x.c crap.c
perl  -pi -e "BEGIN{undef \$/;} s/\/\*[\s\S]*?\*\///smg" crap.c
perl  -pi -e "BEGIN{undef \$/;} s/\tcaca_log[\s\S]*?;//smg" crap.c
grep caca_log_debug crap.c|wc -l
sed -i.bak "/^$/d" crap.c
sed -i.bak "s/tipo_dato/td/g" crap.c
sed -i.bak "s/assert_timeout/ass/g" crap.c
sed -i.bak "s/natural/nat/g" crap.c
sed -i.bak "s/llave/ll/g" crap.c
sed -i.bak "s/avl_tree/avt/g" crap.c
sed -i.bak "s/caca_x/cax/g" crap.c
sed -i.bak "s/root/rt/g" crap.c
sed -i.bak "s/nodo/nd/g" crap.c
sed -i.bak "s/ocurrencias/ocu/g" crap.c
sed -i.bak "s/right/rg/g" crap.c
sed -i.bak "s/left/lf/g" crap.c
sed -i.bak "s/arbol/abl/g" crap.c
gcc -g3 crap.c -o crap 
./crap < caca.txt
ls -lisa crap.c
