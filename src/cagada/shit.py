'''
Created on 10/01/2016

@author: ernesto
'''
import fileinput
import array
import sys
import math
import logging
from logging import DEBUG
logger_cagada = None

def crea_caca_seg(numeros, inicio_inter, fin_inter, indice_nodo, arbolin_en_array, sumas_unicos):
    mitad_inter = 0
    if(indice_nodo >= len(arbolin_en_array)):
        logger_cagada.debug("el fin de inter %d por lo tanto a la mierda los nodos son %d" % (indice_nodo, len(arbolin_en_array)))
        return set()
    logger_cagada.debug("puta madre el inter actual %d-%d comp %s" % (inicio_inter , fin_inter, inicio_inter == fin_inter))
    if(inicio_inter == fin_inter):
        logger_cagada.debug("tocando fondo en inter %d con num %d" % (inicio_inter, numeros[inicio_inter]))
        arbolin_en_array[indice_nodo].add(numeros[inicio_inter])
    else:
        mitad_inter = inicio_inter + int((fin_inter - inicio_inter) / 2)
        logger_cagada.debug("usando el nodo %d" % indice_nodo)
        logger_cagada.debug("inter izq %d-%d" % (inicio_inter, mitad_inter))
        logger_cagada.debug("inter der %d-%d" % (mitad_inter + 1, fin_inter))
        arbolin_en_array[indice_nodo] |= crea_caca_seg(numeros, inicio_inter, mitad_inter, indice_nodo * 2 + 1, arbolin_en_array, sumas_unicos) | crea_caca_seg(numeros, mitad_inter + 1, fin_inter, indice_nodo * 2 + 2, arbolin_en_array, sumas_unicos)
        
    logger_cagada.debug("en inter %d-%d nodo %d el set es %s" % (inicio_inter, fin_inter, indice_nodo, arbolin_en_array[indice_nodo]))
    sumas_unicos[indice_nodo] = sum(arbolin_en_array[indice_nodo])
    return arbolin_en_array[indice_nodo]

def obtiene_indices_ass(indice_afectado, max_profundidad):
    indice_inicio_nivel = 0
    indice_nodo_actual = 0
    profundidad_actual = 0
    indices = array.array("i")
    
    while profundidad_actual < max_profundidad:
        logger_cagada.debug("profundidad actual %d, indice inicio nivel %d" % (profundidad_actual, indice_inicio_nivel))
        indice_inicio_nivel = indice_inicio_nivel << 1
        indice_inicio_nivel |= 1
        profundidad_actual += 1
        
    logger_cagada.debug("el indice inicio nivel %d" % indice_inicio_nivel)
    
    indice_nodo_actual = indice_inicio_nivel + indice_afectado
    logger_cagada.debug("el indice afectado en el arbolin %d" % indice_nodo_actual)
    
    while indice_nodo_actual >= 0:
        indices.append(int(indice_nodo_actual))
        indice_nodo_actual = (indice_nodo_actual - 1) / 2
        
    indices.reverse()
    return indices

def actualizar_caca(numeros, arbolin_en_array, sumas_unicos, indice_actualizado, numero_nuevo, max_profundidad):
    viejo_pendejo = 0
    indices_afectado = []
    
    viejo_pendejo = numeros[indice_actualizado]
    
    if(viejo_pendejo == numero_nuevo):
        return
    
    indices_afectado = obtiene_indices_ass(indice_actualizado, max_profundidad)
    
    logger_cagada.debug("los indices afectados\t%s" % (indices_afectado))
    logger_cagada.debug("los sets correspondientes\t\t%s" % ([arbolin_en_array[x] for x in indices_afectado]))
    logger_cagada.debug("las sumas\t\t%s" % sumas_unicos)
    
    for indice in indices_afectado:
        set_actual = set()
        set_actual = arbolin_en_array[indice]
        set_actual.remove(viejo_pendejo)
        sumas_unicos[indice] -= viejo_pendejo
        if(numero_nuevo not in set_actual):
            set_actual.add(numero_nuevo)
            sumas_unicos[indice] += numero_nuevo
            
    logger_cagada.debug("los sets correspondientes actual\t%s" % ([arbolin_en_array[x] for x in indices_afectado]))
    logger_cagada.debug("las sumas actual\t%s" % sumas_unicos)
    
    numeros[indice_actualizado] = numero_nuevo
    
def obtiene_indices_a_mergear(indice_inicio, indice_final, indice_buscado_inicial, indice_buscado_final, indice_nodo):
    indice_medio = 0
    
    if("indices" not in obtiene_indices_a_mergear.__dict__):
        obtiene_indices_a_mergear.indices = array.array("i")
        
    logger_cagada.debug("buscando intervalo %d-%d en segmento %d-%d" % (indice_buscado_inicial, indice_buscado_final, indice_inicio, indice_final))
        
    if(indice_buscado_inicial <= indice_inicio and indice_final <= indice_buscado_final):
        logger_cagada.debug("thunderstruck %d" % indice_nodo)
        obtiene_indices_a_mergear.indices.append(indice_nodo)
    else:
        if(indice_final < indice_buscado_inicial or indice_buscado_final < indice_inicio):
            logger_cagada.debug("nada q acer")
        else:
            indice_medio = indice_inicio + math.floor((indice_final - indice_inicio) / 2)
            obtiene_indices_a_mergear(indice_inicio, indice_medio, indice_buscado_inicial, indice_buscado_final, indice_nodo * 2 + 1)
            obtiene_indices_a_mergear(indice_medio + 1, indice_final, indice_buscado_inicial, indice_buscado_final, indice_nodo * 2 + 2)
    
    return obtiene_indices_a_mergear.indices

def sumar_cagada(arbolin_en_array, sumas_unicos, indice_inicio, indice_final, num_numeros):
    suma = 0
    sumas_segmentos = []
    sets_a_mergear = []
    indices = array.array("i")
    set_unicos = set()
    
    obtiene_indices_a_mergear.indices = array.array("i")
    indices = sorted(obtiene_indices_a_mergear(0, num_numeros, indice_inicio, indice_final, 0))
    
    logger_cagada.debug("los indices a mergear %s" % indices)
    
    sumas_segmentos = [sumas_unicos[idx_suma_seg] for idx_suma_seg in indices]
    logger_cagada.debug("las sumas de los segmentos %s" % sumas_segmentos)
    suma = sum(sumas_segmentos)
    
    logger_cagada.debug("la suma raw %d" % suma)
    
    if(len(indices) > 1):
        sets_a_mergear = [arbolin_en_array[indice_set] for indice_set in indices]
        logger_cagada.debug("los sets a mergar %s" % sets_a_mergear)
        
        set_unicos = sets_a_mergear[0]
        
        for set_actual in sets_a_mergear[1:]:
            suma_intersexion = 0
            set_intersexion = set()
            set_intersexion = set_unicos & set_actual
            logger_cagada.debug("la intersexion de %s y %s es %s" % (set_unicos, set_actual, set_intersexion))
            suma_intersexion = sum(set_intersexion)
            logger_cagada.debug("la suma de intersex %d" % suma_intersexion)
            suma -= suma_intersexion
            set_unicos |= set_actual
    
    return suma

num_numeros = 0
num_queries = 0
num_nodos = 0
max_profundidad = 0
num_numeros_redondeado = 0
numeros = array.array("d")
arbolin_en_array = []
sumas_unicos = []
lineas = None

logging.basicConfig(level=logging.DEBUG)
logger_cagada=logging.getLogger("asa")
logger_cagada.setLevel(DEBUG)

lineas = list(fileinput.input())

num_numeros = int(lineas[0])
numeros = [int(x.strip()) for x in lineas[1].split(" ")]
num_queries = int(lineas[2])

while num_numeros >> max_profundidad:
    max_profundidad += 1


num_nodos = (2 << max_profundidad)

arbolin_en_array = [set() for _ in range(num_nodos)]
sumas_unicos = [0 for _ in range(num_nodos)]

num_numeros_redondeado = (1 << max_profundidad);

numeros += [0 for _ in range(len(numeros), num_numeros_redondeado)]

logger_cagada.debug("num numeros es %d" % num_numeros)
logger_cagada.debug("num numeros redondeados es %d" % num_numeros_redondeado)
logger_cagada.debug("los numeros en si %s" % (numeros))
logger_cagada.debug("num queries es %d" % num_queries)
logger_cagada.debug("num nodos es %d" % num_nodos)
logger_cagada.debug("max prof es %d" % max_profundidad)

crea_caca_seg(numeros, 0, num_numeros_redondeado - 1, 0, arbolin_en_array, sumas_unicos)

logger_cagada.debug("las sumas de unicos %s" % sumas_unicos)

for linea in lineas[3:]:
    param1 = 0
    param2 = 0
    suma = 0
    tipo_query = ""
    param1_str = ""
    param2_str = ""
    (tipo_query, param1_str, param2_str) = linea.split(" ")
    
    param1 = int(param1_str)
    param2 = int(param2_str)
    
    logger_cagada.debug("tipo query %s, param1 %d param2 %d" % (tipo_query, param1, param2))
    
    if tipo_query == "Q":
        suma = sumar_cagada(arbolin_en_array, sumas_unicos, param1 - 1, param2 - 1, num_numeros_redondeado - 1)
        print(suma)
    else:
        if tipo_query == "U":
            actualizar_caca(numeros, arbolin_en_array, sumas_unicos, param1 - 1, param2, max_profundidad)
        else:
            sys.exit("mierda")

# actualizar_caca(numeros, arbolin_en_array, sumas_unicos, 2, 2, max_profundidad)
# actualizar_caca(numeros, arbolin_en_array, sumas_unicos, 2, 1000, max_profundidad)



