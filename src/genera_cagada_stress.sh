#!/bin/sh 
MAX_VALOR=2147483647
MAX_ELEMS=50000
#MAX_ELEMS=100
saltito=$((MAX_VALOR / MAX_ELEMS))
salto_inicial=$((saltito * 1))


shit=$(seq -f %1.f -s' ' $salto_inicial $saltito $MAX_VALOR)
echo "$MAX_ELEMS" > /tmp/como.txt
echo "$shit" >> /tmp/como.txt

