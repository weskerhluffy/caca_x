#!/bin/sh 
MAX_VALOR=2147483647
MAX_ELEMS=50000
#MAX_ELEMS=100
saltito=$((MAX_VALOR / MAX_ELEMS))
salto_inicial=$((saltito * 1))
contador_actualizaciones=0


shit=$(seq -f %1.f -s' ' $MAX_VALOR -$saltito $salto_inicial)
echo "$MAX_ELEMS" > /tmp/como_rev.txt
echo "$shit" >> /tmp/como_rev.txt
echo "$MAX_ELEMS" >> /tmp/como_rev.txt
for numerin in $shit
do
        indice_actualizacion=$((MAX_ELEMS - contador_actualizaciones))
        echo "U $indice_actualizacion $numerin" >> /tmp/como_rev.txt
        contador_actualizaciones=$((contador_actualizaciones+ 1))
done
