/*
 ============================================================================
 Name        : caca_x.c
 Author      : ernesto
 Version     :
 Copyright   : a veces siento que
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#define MAX_NUMEROS 50000
#define MAX_VALOR INT_MAX
#define MAX_QUERIES 100000
#define TAM_MAX_LINEA (MAX_NUMEROS*10+MAX_NUMEROS)
#define MAX_NODOS (1 << 16)

#define caca_log_debug printf
/*
 #define caca_log_debug(formato, args...) 0
 */

typedef int tipo_dato;

typedef long long array_bits;

typedef enum BOOLEANOS {
	falso = 0, verdadero
} bool;

typedef struct caca_x_numeros_unicos_en_rango {
	int *numeros;
	int max_numeros;
	int num_numeros;
	int altura;
	int idx;
	int limite_izq;
	int limite_der;
} caca_x_numeros_unicos_en_rango;

typedef struct caca_x_estado_recursion {
	array_bits mapa_numeros_encontrados[MAX_VALOR / 64];
	int profundidad;
	caca_x_numeros_unicos_en_rango *nodo;
} caca_x_estado_recursion;

caca_x_numeros_unicos_en_rango *arbol_numeros_unicos = NULL;

static inline int lee_matrix_long_stdin(tipo_dato *matrix, int *num_filas,
		int *num_columnas, int num_max_filas, int num_max_columnas) {
	int indice_filas = 0;
	int indice_columnas = 0;
	long numero = 0;
	char *siguiente_cadena_numero = NULL;
	char *cadena_numero_actual = NULL;
	char *linea = NULL;

	linea = calloc(TAM_MAX_LINEA, sizeof(char));

	while (indice_filas < num_max_filas && fgets(linea, TAM_MAX_LINEA, stdin)) {
		indice_columnas = 0;
		cadena_numero_actual = linea;
		for (siguiente_cadena_numero = linea;; siguiente_cadena_numero =
				cadena_numero_actual) {
			numero = strtol(siguiente_cadena_numero, &cadena_numero_actual, 10);
			if (cadena_numero_actual == siguiente_cadena_numero) {
				break;
			}
			*(matrix + indice_filas * num_max_columnas + indice_columnas) =
					numero;
			caca_log_debug("en col %d, fil %d, el valor %lu\n", indice_columnas,
					indice_filas, numero);
			indice_columnas++;
			caca_log_debug("las columnas son %d\n", indice_columnas);
		}
		if (num_columnas) {
			num_columnas[indice_filas] = indice_columnas;
		}
		indice_filas++;
		caca_log_debug("las filas son %d, con clos %d\n", indice_filas,
				indice_columnas);
	}

	*num_filas = indice_filas;
	free(linea);
	return 0;
}

static inline char *caca_arreglo_a_cadena(tipo_dato *arreglo, int tam_arreglo,
		char *buffer) {
	int i;
	char *ap_buffer = NULL;
	int characteres_escritos = 0;

	memset(buffer, 0, 100);
	ap_buffer = buffer;

	for (i = 0; i < tam_arreglo; i++) {
		characteres_escritos += sprintf(ap_buffer + characteres_escritos, "%d",
				*(arreglo + i));
		if (i < tam_arreglo - 1) {
			*(ap_buffer + characteres_escritos++) = ',';
		}
	}
	return ap_buffer;
}

static inline void caca_x_inicializar_nodo(caca_x_numeros_unicos_en_rango *nodo,
		int altura, int idx_nodo, int limite_izq, int limite_der) {
	nodo->altura = altura;
	nodo->max_numeros = 1 << nodo->altura;
	nodo->numeros = calloc(nodo->max_numeros, sizeof(int));
	nodo->idx = idx_nodo;
	nodo->limite_izq = limite_izq;
	nodo->limite_der = limite_der;

}

static inline bool caca_comun_checa_bit(array_bits *bits, int posicion) {
	bool res = falso;
	int idx_arreglo = 0;
	int idx_registro = 0;

	idx_arreglo = posicion / 64;
	idx_registro = posicion % 64;

	res = !!(bits[idx_arreglo] & (1 << idx_registro));

	return res;
}

static inline void caca_comun_asigna_bit(array_bits *bits, int posicion) {
	int idx_arreglo = 0;
	int idx_registro = 0;

	idx_arreglo = posicion / 64;
	idx_registro = posicion % 64;

	bits[idx_arreglo] |= (1 << idx_registro);

}

static inline void caca_x_construye_arbol_binario_segmentado(int *numeros,
		int profundidad, int max_profundidad, int idx_ini, int idx_fin,
		int idx_nodo) {
	static caca_x_estado_recursion estado[16] = { 0 };
	int altura = 0;
	caca_x_numeros_unicos_en_rango *nodo = NULL;
	caca_x_estado_recursion *estado_actual = NULL;

	altura = max_profundidad - profundidad;

	caca_log_debug("en altura %d\n", altura);

	nodo = arbol_numeros_unicos + idx_nodo;

	caca_x_inicializar_nodo(nodo, altura, idx_nodo, idx_ini, idx_fin);

	estado_actual = estado + profundidad;

	if (altura) {
		int distancia_media = 0;
		int idx_medio = 0;

		memset(estado_actual, 0, sizeof(caca_x_estado_recursion));
		estado_actual->nodo = nodo;
		estado_actual->profundidad = profundidad;

		distancia_media = (idx_fin - idx_ini) / 2;
		idx_medio = idx_ini + distancia_media;

		caca_log_debug("segmentando izq de %d a %d en alt %d\n", idx_ini,
				idx_medio, altura);
		caca_x_construye_arbol_binario_segmentado(numeros, profundidad + 1,
				max_profundidad, idx_ini, idx_medio, 2 * idx_nodo + 1);

		caca_log_debug("segmentando der de %d a %d en alt %d\n", idx_medio + 1,
				idx_fin, altura);
		caca_x_construye_arbol_binario_segmentado(numeros, profundidad + 1,
				max_profundidad, idx_medio + 1, idx_fin, 2 * idx_nodo + 2);
	} else {
		int profundidad_actual = 0;
		int numero_actual = 0;

		assert(idx_ini == idx_fin);
		numero_actual = numeros[idx_ini];
		*nodo->numeros = numero_actual;
		nodo->num_numeros++;

		caca_log_debug("asignado unico numero %d a indice de arbol %d\n",
				numero_actual, idx_nodo);
		while (profundidad_actual < profundidad) {
			bool num_encontrado = falso;
			array_bits *bits_actuales = NULL;

			estado_actual = estado + profundidad_actual;

			bits_actuales = estado_actual->mapa_numeros_encontrados;

			num_encontrado = caca_comun_checa_bit(bits_actuales, numero_actual);

			if (num_encontrado) {
				caca_log_debug("numero %d encontrado en profundidad %d\n",
						numero_actual, profundidad_actual);
			} else {
				estado_actual->nodo->numeros[estado_actual->nodo->num_numeros++] =
						numero_actual;
				caca_comun_asigna_bit(bits_actuales, numero_actual);
			}

			profundidad_actual++;
		}
	}

}

static inline void caca_x_suma_unicos(int *sumas_arbol_segmentado,
		int num_nodos) {
	caca_x_numeros_unicos_en_rango *nodo = NULL;
	int *numeros_unicos = NULL;
	int num_numeros_unicos = 0;
	char buf[100] = { '\0' };

	for (int i = 0; i < num_nodos; i++) {
		nodo = arbol_numeros_unicos + i;

		if (!nodo->numeros) {
			continue;
		}

		numeros_unicos = nodo->numeros;
		num_numeros_unicos = nodo->num_numeros;

		assert(i == nodo->idx);

		caca_log_debug("los numeros unicos en %d son %s\n", i,
				caca_arreglo_a_cadena(numeros_unicos, num_numeros_unicos, buf));

		sumas_arbol_segmentado[i] = 0;
		for (int j = 0; j < num_numeros_unicos; j++) {
			sumas_arbol_segmentado[i] += numeros_unicos[j];
		}
		caca_log_debug("la suma es %d\n", sumas_arbol_segmentado[i]);
	}
}

static inline int caca_x_calcular_suma_intersexion(
		caca_x_numeros_unicos_en_rango *nodo_izq,
		caca_x_numeros_unicos_en_rango *nodo_der) {
	int res = 0;
	int num_numeros_mayor = 0;
	int num_numeros_menor = 0;
	array_bits *bits = NULL;
	int *numeros_izq = NULL;
	int *numeros_der = NULL;
	int *numeros_mayor = NULL;
	int *numeros_menor = NULL;

	bits = calloc(MAX_VALOR / 64, sizeof(array_bits));
	assert(bits);

	numeros_izq = nodo_izq->numeros;
	numeros_der = nodo_der->numeros;

	if (nodo_izq->num_numeros > nodo_der->num_numeros) {
		numeros_mayor = nodo_izq->numeros;
		numeros_menor = nodo_der->numeros;
		num_numeros_mayor = nodo_izq->num_numeros;
		num_numeros_menor = nodo_der->num_numeros;
	} else {
		numeros_mayor = nodo_der->numeros;
		numeros_menor = nodo_izq->numeros;
		num_numeros_mayor = nodo_der->num_numeros;
		num_numeros_menor = nodo_izq->num_numeros;
	}

	for (int i = 0; i < num_numeros_mayor; i++) {
		int numero_actual = 0;
		numero_actual = numeros_mayor[i];
		caca_comun_asigna_bit(bits, numero_actual);
	}

	for (int i = 0; i < num_numeros_menor; i++) {
		int numero_actual = 0;
		numero_actual = numeros_menor[i];
		if (caca_comun_checa_bit(bits, numero_actual)) {
			res += numero_actual;
		}
	}

	free(bits);

	return res;
}

static inline void caca_x_generar_sumas_de_intersexiones(
		int matriz_sumas_intersexiones[MAX_NODOS][16],
		caca_x_numeros_unicos_en_rango *arbol_numeros_unicos, int num_nodos) {
	int idx_nodo_derecho = 0;
	int idx_nodo_actual = 0;
	caca_x_numeros_unicos_en_rango *nodo = NULL;
	caca_x_numeros_unicos_en_rango *nodo_derecho = NULL;

	nodo = arbol_numeros_unicos;

	caca_log_debug("generando suma intersex con num_nodos %d\n", num_nodos);

	while (nodo->numeros) {
		int altura_actual = 0;
		int suma_intersexion = 0;

		if ((idx_nodo_actual % 2)) {
			idx_nodo_derecho = idx_nodo_actual + 1;
			nodo_derecho = arbol_numeros_unicos + idx_nodo_derecho;
			altura_actual = nodo_derecho->altura;

			if (!altura_actual) {
				caca_log_debug("%d es oja, no ay suma\n", idx_nodo_actual);
			}

			while (altura_actual) {
				idx_nodo_derecho = 2 * idx_nodo_derecho + 1;
				nodo_derecho = arbol_numeros_unicos + idx_nodo_derecho;
				altura_actual = nodo_derecho->altura;

				suma_intersexion = caca_x_calcular_suma_intersexion(nodo,
						nodo_derecho);

				matriz_sumas_intersexiones[idx_nodo_actual][altura_actual] =
						suma_intersexion;
				caca_log_debug("la sumap de %d,%d(%d) es %d\n", idx_nodo_actual,
						idx_nodo_derecho, altura_actual,
						matriz_sumas_intersexiones[idx_nodo_actual][altura_actual]);
			}

		} else {
			int limite_der_nodo_izq = 0;
			int limite_izq_nodo_der = 0;
			bool primer_ancestro = verdadero;

			idx_nodo_derecho = idx_nodo_actual + 1;
			if (idx_nodo_derecho > num_nodos) {
				caca_log_debug("no ay suma para %d, es el ultimo nodo\n",
						idx_nodo_actual);
				goto fin_ciclo;
			}
			nodo_derecho = arbol_numeros_unicos + idx_nodo_derecho;
			if (nodo_derecho->altura != nodo->altura) {
				caca_log_debug("no ay suma para %d, esta a la orisha\n",
						idx_nodo_actual);
				goto fin_ciclo;
			}

			limite_der_nodo_izq = nodo->limite_der;
			do {
				idx_nodo_derecho = (idx_nodo_derecho - 1) / 2;
				nodo_derecho = arbol_numeros_unicos + idx_nodo_derecho;
				limite_izq_nodo_der = nodo_derecho->limite_izq;
			} while (limite_der_nodo_izq < limite_izq_nodo_der);

			caca_log_debug("ancestro comun %d\n", idx_nodo_derecho);

			do {
				idx_nodo_derecho = 2 * idx_nodo_derecho
						+ (primer_ancestro ? 2 : 1);
				nodo_derecho = arbol_numeros_unicos + idx_nodo_derecho;
				altura_actual = nodo_derecho->altura;

				suma_intersexion = caca_x_calcular_suma_intersexion(nodo,
						nodo_derecho);

				matriz_sumas_intersexiones[idx_nodo_actual][altura_actual] =
						suma_intersexion;
				caca_log_debug("la suma de %d,%d(%d) es %d\n", idx_nodo_actual,
						idx_nodo_derecho, altura_actual,
						matriz_sumas_intersexiones[idx_nodo_actual][altura_actual]);

				primer_ancestro = falso;
			} while (altura_actual);

		}
		fin_ciclo: idx_nodo_actual++;
		nodo = arbol_numeros_unicos + idx_nodo_actual;
	}
}

static inline void caca_x_encuentra_indices_segmento(
		caca_x_numeros_unicos_en_rango *nodos, int idx_nodo, int limite_izq,
		int limite_der, int *indices, int *num_indices) {
	caca_x_numeros_unicos_en_rango *nodo = NULL;

	nodo = nodos + idx_nodo;

	if (limite_izq <= nodo->limite_izq && nodo->limite_der <= limite_der) {
		caca_log_debug("te vas a acordar de mi %d,%d\n", nodo->limite_izq,
				nodo->limite_der);
		indices[(*num_indices)++] = idx_nodo;
	} else {
		if (nodo->limite_der < limite_izq || limite_der < nodo->limite_izq) {
			caca_log_debug("nada que sumar %d:%d\n", nodo->limite_izq,
					nodo->limite_der);
		} else {

			caca_log_debug("pues nadie sera %d,%d\n", nodo->limite_izq,
					nodo->limite_der);
			caca_x_encuentra_indices_segmento(nodos, 2 * idx_nodo + 1,
					limite_izq, limite_der, indices, num_indices);
			caca_x_encuentra_indices_segmento(nodos, 2 * idx_nodo + 2,
					limite_izq, limite_der, indices, num_indices);
		}
	}

}

static inline int caca_x_suma_segmento(int *sumas_arbol_segmentado,
		int matriz_sumas_coincidencias[MAX_NODOS][16], int limite_izq,
		int limite_der) {
	int res = 0;
	int num_indices_nodos = 0;
	int *indices_nodos = (int[16] ) { 0 };
	caca_x_numeros_unicos_en_rango *nodo_izq = NULL;
	caca_x_numeros_unicos_en_rango *nodo_der = NULL;
	char buf[100] = { '\0' };

	caca_x_encuentra_indices_segmento(arbol_numeros_unicos, 0, limite_izq,
			limite_der, indices_nodos, &num_indices_nodos);

	caca_log_debug("indices de segmento %d:%d %s\n", limite_izq, limite_der,
			caca_arreglo_a_cadena(indices_nodos, num_indices_nodos, buf));

	for (int i = 0; i < num_indices_nodos; i++) {
		caca_log_debug("segmento %d aporta %d\n", indices_nodos[i],
				sumas_arbol_segmentado[indices_nodos[i]]);
		res += sumas_arbol_segmentado[indices_nodos[i]];
	}

	for (int i = 0; i < num_indices_nodos - 1; i++) {
		int idx_nodo_izq = 0;
		int idx_nodo_der = 0;

		idx_nodo_izq = indices_nodos[i];
		idx_nodo_der = indices_nodos[i + 1];
		nodo_izq = arbol_numeros_unicos + idx_nodo_izq;
		nodo_der = arbol_numeros_unicos + idx_nodo_der;

		caca_log_debug(
				"encontrando el cruce de %d con %d en la matriz de intersexxxiones\n",
				nodo_izq->idx, nodo_der->altura);

		assert(idx_nodo_izq == nodo_izq->idx);
		assert(idx_nodo_der == nodo_der->idx);
		res -= matriz_sumas_coincidencias[nodo_izq->idx][nodo_der->altura];
	}

	caca_log_debug("La suma es %d\n", res);

	return res;
}

static inline void caca_x_main() {
	int *matriz_nums = NULL;
	int num_filas = 0;
	int num_numeros = 0;
	int *numeros = NULL;
	int num_queries = 0;
	int cont_queries = 0;
	char tipo_query = 0;
	int idx_query_ini = 0;
	int idx_query_fin = 0;
	int max_profundidad = 0;
	int num_numeros_redondeado = 0;
	int *sumas_arbol_segmentado = NULL;
	int *matriz_sumas_coincidencias = NULL;

	char buf[100] = { '\0' };

	matriz_nums = calloc(MAX_NUMEROS * 3, sizeof(int));
	assert(matriz_nums);

	arbol_numeros_unicos = calloc(MAX_NODOS,
			sizeof(caca_x_numeros_unicos_en_rango));
	assert(arbol_numeros_unicos);

	sumas_arbol_segmentado = calloc(MAX_NODOS, sizeof(int));
	assert(sumas_arbol_segmentado);

	matriz_sumas_coincidencias = calloc(MAX_NODOS * 16, sizeof(int));
	assert(matriz_sumas_coincidencias);

	num_filas = 3;
	lee_matrix_long_stdin(matriz_nums, &num_filas, NULL, 3, MAX_NUMEROS);

	num_numeros = *matriz_nums;
	numeros = matriz_nums + MAX_NUMEROS;
	num_queries = *(numeros + MAX_NUMEROS);

	caca_log_debug("a vece siento q %d\n", num_numeros);
	caca_log_debug("as corrido con algo de s %s\n",
			caca_arreglo_a_cadena(numeros, num_numeros, buf));
	caca_log_debug("en estas paginas %d\n", num_queries);

	while ((num_numeros >> max_profundidad)) {
		max_profundidad++;
	}
	num_numeros_redondeado = (1 << max_profundidad);

	caca_log_debug("en estas paginas %s\n",
			caca_arreglo_a_cadena(numeros, num_numeros_redondeado, buf));

	caca_log_debug("llamando a func rec con max prof %d\n",
			max_profundidad - 1);

	caca_x_construye_arbol_binario_segmentado(numeros, 0, max_profundidad, 0,
			num_numeros_redondeado - 1, 0);

	caca_x_suma_unicos(sumas_arbol_segmentado, MAX_NODOS);

	caca_x_generar_sumas_de_intersexiones(
			(int (*)[16]) matriz_sumas_coincidencias, arbol_numeros_unicos,
			(2 << (max_profundidad + 0)) - 2);

	while (cont_queries < num_queries) {
		scanf("%c %d %d\n", &tipo_query, &idx_query_ini, &idx_query_fin);
		caca_log_debug("q: %c, ini %d, fin %d\n", tipo_query, idx_query_ini,
				idx_query_fin);

		if (tipo_query == 'Q') {
			caca_x_suma_segmento(sumas_arbol_segmentado,
					(int (*)[16]) matriz_sumas_coincidencias, idx_query_ini - 1,
					idx_query_fin - 1);
		}

		cont_queries++;
	}

	free(matriz_nums);
	free(arbol_numeros_unicos);
	free(sumas_arbol_segmentado);
	free(matriz_sumas_coincidencias);
}

int main(void) {
	puts("he corrido con algo de suerte"); /* prints he corrido con algo de suerte */
	caca_x_main();
	return EXIT_SUCCESS;
}
