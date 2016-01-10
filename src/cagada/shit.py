'''
Created on 10/01/2016

@author: ernesto
'''
import fileinput
import array

def crea_caca_seg(numeros, inicio_inter, fin_inter, indice_nodo, arbolin_en_array, sumas_unicos):
    mitad_inter = 0
    if(indice_nodo >= len(arbolin_en_array)):
        print("el fin de inter %d por lo tanto a la mierda los nodos son %d" % (indice_nodo, len(arbolin_en_array)))
        return set()
    print("puta madre el inter actual %d-%d comp %s" % (inicio_inter , fin_inter, inicio_inter == fin_inter))
    if(inicio_inter == fin_inter):
        print("tocando fondo en inter %d con num %d" % (inicio_inter, numeros[inicio_inter]))
        arbolin_en_array[indice_nodo].add(numeros[inicio_inter])
    else:
        mitad_inter = inicio_inter + int((fin_inter - inicio_inter) / 2)
        print("usando el nodo %d" % indice_nodo)
        print("inter izq %d-%d" % (inicio_inter, mitad_inter))
        print("inter der %d-%d" % (mitad_inter + 1, fin_inter))
        arbolin_en_array[indice_nodo] |= crea_caca_seg(numeros, inicio_inter, mitad_inter, indice_nodo * 2 + 1, arbolin_en_array, sumas_unicos) | crea_caca_seg(numeros, mitad_inter + 1, fin_inter, indice_nodo * 2 + 2, arbolin_en_array, sumas_unicos)
        
    print("en inter %d-%d nodo %d el set es %s" % (inicio_inter, fin_inter, indice_nodo, arbolin_en_array[indice_nodo]))
    sumas_unicos[indice_nodo] = sum(arbolin_en_array[indice_nodo])
    return arbolin_en_array[indice_nodo]
        

num_numeros = 0
num_queries = 0
num_nodos = 0
max_profundidad = 0
numeros = array.array("d")
arbolin_en_array = []
sumas_unicos = []
lineas = None

lineas = list(fileinput.input())

num_numeros = int(lineas[0])
numeros = [int(x.strip()) for x in lineas[1].split(" ")]
num_queries = int(lineas[2])

while num_numeros >> max_profundidad:
    max_profundidad += 1


num_nodos = (2 << max_profundidad)

arbolin_en_array = [set() for _ in range(num_nodos)]
sumas_unicos = [0 for _ in range(num_nodos)]

print("num numeros es %d" % num_numeros)
print("los numeros en si %s" % (numeros))
print("num queries es %d" % num_queries)
print("num nodos es %d" % num_nodos)
print("max prof es %d" % max_profundidad)

crea_caca_seg(numeros, 0, num_numeros - 1, 0, arbolin_en_array, sumas_unicos)

print("las sumas de unicos %s" % sumas_unicos)

