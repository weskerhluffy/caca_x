'''
Created on 19/06/2016

@author: ernesto
'''
import fileinput

if __name__ == '__main__':
    num_casos = 0
    num_linea = 0
    lineas = []
   
    lineas = list(fileinput.input())

    num_casos = int(lineas[0])
#    print("el num casos %u" % num_casos)
    
    for num_linea in range(1, num_casos * 2 + 1, 2):
        num_numeros = 0
        suma_actual = 0
        numeros_lista = []
        numeros = set()
        
#        print("ey revolver %u" % (num_linea))
        
        num_numeros = int(lineas[num_linea])
#        print("dont mothers %s"%lineas[num_linea+1])
        numeros_lista = [int(x.strip()) for x in lineas[num_linea + 1].strip().split(" ")]
        assert(num_numeros == len(numeros_lista))
        numeros = set(numeros_lista)
        for numerin in numeros:
            suma_actual += numerin
        
        print("%ld" % (suma_actual))
        
