#!/bin/sh 
MAX_VALOR=2147483647
MAX_ELEMS=50000
#MAX_ELEMS=100
saltito=$((MAX_VALOR / MAX_ELEMS))
salto_inicial=$((saltito * 1))


shit=$(seq -f %1.f -s' ' $MAX_VALOR -$saltito $salto_inicial)
echo "$MAX_ELEMS" > /tmp/como_rev.txt
echo "$shit" >> /tmp/como_rev.txt

