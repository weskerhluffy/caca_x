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
#include <stddef.h>
#include <unistd.h>

#define MAX_NUMEROS 50000
#define MAX_VALOR INT_MAX
#define MAX_QUERIES 100000
#define TAM_MAX_LINEA (MAX_NUMEROS*10+MAX_NUMEROS)
#define MAX_NODOS (1 << 16)

/*
#define caca_log_debug(formato, args...) 0
 */
 #define caca_log_debug printf

/*
 #define assert_timeout(condition) assert(condition);
 */
#define assert_timeout(condition) if(!(condition)){printf("fuck\n");sleep(10);}

#ifndef AVL_TREE_H
#define AVL_TREE_H

typedef struct {
	void* key;
	void* val;
} avltree_node_t;

typedef struct {
	/* size of array */
	int size;
	int count;
	long (*cmp)(const void *e1, const void *e2);
	avltree_node_t *nodes;
} avltree_t;

typedef struct {
	int current_node;
} avltree_iterator_t;

avltree_t* avltree_new(long (*cmp)(const void *e1, const void *e2),
		int nodos_maximos);

void* avltree_remove(avltree_t* me, void* k);

int avltree_count(avltree_t* me);

int avltree_size(avltree_t* me);

int avltree_height(avltree_t* me);

void avltree_empty(avltree_t* me);

void avltree_insert(avltree_t* me, void* k, void* v);

void* avltree_get(avltree_t* me, const void* k);

void* avltree_get_from_idx(avltree_t* me, int idx);

/**
 * Rotate on X:
 * Y = X's parent
 * Step A: Y becomes left child of X
 * Step B: X's left child's becomes Y's right child */
void avltree_rotate_left(avltree_t* me, int idx);

/**
 * Rotate on X:
 * Y = X's left child
 * Step A: X becomes right child of X's left child
 * Step B: X's left child's right child becomes X's left child */
void avltree_rotate_right(avltree_t* me, int idx);

/**
 * Initialise a new hash iterator over this hash
 * It is NOT safe to remove items while iterating.  */
void avltree_iterator(avltree_t * h, avltree_iterator_t * iter);

/**
 * Iterate to the next item on an iterator
 * @return next item key from iterator */
void *avltree_iterator_next(avltree_t * h, avltree_iterator_t * iter);

/**
 * Iterate to the next item on an iterator
 * @return next item value from iterator */
void *avltree_iterator_next_value(avltree_t * h, avltree_iterator_t * iter);

int avltree_iterator_has_next(avltree_t * h, avltree_iterator_t * iter);

void* avltree_iterator_peek_value(avltree_t * h, avltree_iterator_t * iter);

void* avltree_iterator_peek(avltree_t * h, avltree_iterator_t * iter);

#endif /* AVL_TREE_H */

typedef int tipo_dato;

typedef long long array_bits;

typedef enum BOOLEANOS {
	falso = 0, verdadero
} bool;

typedef struct caca_x_numeros_unicos_en_rango {
	avltree_t *arbolazo;
	int max_numeros;
	int num_numeros;
	int altura;
	int idx;
	int limite_izq;
	int limite_der;
} caca_x_numeros_unicos_en_rango;

typedef struct caca_x_estado_recursion {
	int profundidad;
	int idx_ini;
	int idx_fin;
	int idx_nodo;
	caca_x_numeros_unicos_en_rango *nodo;
} caca_x_estado_recursion;

caca_x_numeros_unicos_en_rango *arbol_numeros_unicos = NULL;

caca_x_estado_recursion *estado = NULL;

#if 1

#define max(x,y) ((x) < (y) ? (y) : (x))

static int __child_l(const int idx) {
	return idx * 2 + 1;
}

static int __child_r(const int idx) {
	return idx * 2 + 2;
}

static int __parent(const int idx) {
#if 0
	if (idx == 0) return *(int*)NULL;
#endif
	assert(idx != 0);
	return (idx - 1) / 2;
}

static void __print(avltree_t* me, int idx, int d) {
	int i;

	for (i = 0; i < d; i++)
		printf(" ");
	printf("%c ", idx % 2 == 1 ? 'l' : 'r');

	if (me->size <= idx || !me->nodes[idx].key) {
		printf("\n");
		return;
	}

	printf("%lx\n", (unsigned long int) me->nodes[idx].key);
	__print(me, __child_l(idx), d + 1);
	__print(me, __child_r(idx), d + 1);
}

void avltree_print(avltree_t* me) {
	printf("AVL Tree:\n");
	__print(me, 0, 0);
}

void avltree_print2(avltree_t* me) {
	int i;

	for (i = 0; i < me->size; i++)
		printf("%lx%c", (unsigned long int) me->nodes[i].key,
				i == me->size ? '|' : ' ');
	printf("\n");
}

static void __enlarge(avltree_t* me) {
	int ii, end;
	avltree_node_t *array_n;

	/* double capacity */
	me->size *= 2;
	array_n = malloc(me->size * sizeof(avltree_node_t));

	/* copy old data across to new array */
	for (ii = 0, end = avltree_count(me); ii < end; ii++) {
		if (me->nodes[ii].key)
			memcpy(&array_n[ii], &me->nodes[ii], sizeof(avltree_node_t));
		else
			array_n[ii].key = NULL;
	}

	/* swap arrays */
	free(me->nodes);
	me->nodes = array_n;
}

avltree_t* avltree_new(long (*cmp)(const void *e1, const void *e2),
		int nodos_maximos) {
	avltree_t* me;

	assert(cmp);

	me = calloc(1, sizeof(avltree_t));
	me->size = nodos_maximos;
	me->nodes = calloc(me->size, sizeof(avltree_node_t));
	me->cmp = cmp;
	return me;
}

static int __count(avltree_t* me, int idx) {
	if (me->size <= idx || !me->nodes[idx].key)
		return 0;
	return __count(me, __child_l(idx)) + __count(me, __child_r(idx)) + 1;
}

int avltree_count(avltree_t* me) {
//    return __count(me,0);
	return me->count;
}

int avltree_size(avltree_t* me) {
	return me->size;
}

static int __height(avltree_t* me, int idx) {
	if (idx >= me->size || !me->nodes[idx].key)
		return 0;
	return max(
			__height(me,__child_l(idx)) + 1,
			__height(me,__child_r(idx)) + 1);
}

int avltree_height(avltree_t* me) {
	return __height(me, 0);
}

static void __shift_up(avltree_t* me, int idx, int towards) {
	if (!me->nodes[idx].key || idx >= me->size || towards >= me->size) {
		return;
	}

	memcpy(&me->nodes[towards], &me->nodes[idx], sizeof(avltree_node_t));
	me->nodes[idx].key = NULL;
	if (__child_l(idx) <= me->size && __child_l(towards) <= me->size) {
		__shift_up(me, __child_l(idx), __child_l(towards));
	}
	if (__child_r(idx) <= me->size && __child_r(towards) <= me->size) {
		__shift_up(me, __child_r(idx), __child_r(towards));
	}
}

static void __shift_down(avltree_t* me, int idx, int towards) {
	if (!me->nodes[idx].key || idx >= me->size || towards >= me->size) {
		return;
	}

	if (__child_l(idx) < me->size && __child_l(towards) < me->size) {
		__shift_down(me, __child_l(idx), __child_l(towards));
	}
	if (__child_l(idx) < me->size && __child_l(towards) < me->size) {
		__shift_down(me, __child_r(idx), __child_r(towards));
	}
	memcpy(&me->nodes[towards], &me->nodes[idx], sizeof(avltree_node_t));
}

void avltree_rotate_right(avltree_t* me, int idx) {
	/* A Partial
	 * Move X out of the way so that Y can take its spot */
	__shift_down(me, __child_r(idx), __child_r(__child_r(idx)));
	memcpy(&me->nodes[__child_r(idx)], &me->nodes[idx], sizeof(avltree_node_t));

	/* B */
	__shift_down(me, __child_r(__child_l(idx)), __child_l(__child_r(idx)));
	me->nodes[__child_r(__child_l(idx))].key = NULL;

	/* A Final
	 * Move Y into X's old spot */
	__shift_up(me, __child_l(idx), idx);
}

void* avltree_get(avltree_t* me, const void* k) {
	int i;

	for (i = 0; i < me->size;) {
		int r;
		avltree_node_t *n;

		n = &me->nodes[i];

		/* couldn't find it */
		if (!n->key)
			return NULL ;

		r = me->cmp(n->key, k);

		if (r == 0) {
			return n->val;
		} else if (r < 0) {
			i = __child_l(i);
		} else if (r > 0) {
			i = __child_r(i);
		} else {
			assert(0);
		}
	}

	/* couldn't find it */
	return NULL ;
}

void* avltree_get_from_idx(avltree_t* me, int idx) {
	return me->nodes[idx].key;
}

void avltree_rotate_left(avltree_t* me, int idx) {
	int p;

	p = __parent(idx);

	/* A Partial
	 * Move Y out of the way so that X can take its spot */
	__shift_down(me, __child_l(p), __child_l(__child_l(p)));
	memcpy(&me->nodes[__child_l(p)], &me->nodes[p], sizeof(avltree_node_t));

	/* B */
	__shift_down(me, __child_l(idx), __child_r(__child_l(p)));
	me->nodes[__child_l(idx)].key = NULL;

	/* A Final
	 * Move Y into X's old spot */
	__shift_up(me, idx, p);
}

static void __rebalance(avltree_t* me, int idx) {

	while (1) {
		if (2
				<= abs(
						__height(me, __child_l(idx))
								- __height(me, __child_r(idx)))) {
			/*  balance factor left node */
			int bf_r;

			bf_r = __height(me, __child_l(__child_r(idx)))
					- __height(me, __child_r(__child_r(idx)));

			if (bf_r == -1) {
				avltree_rotate_left(me, __child_r(idx));
			} else {
				avltree_rotate_left(me, __child_r(idx));
				avltree_rotate_right(me, __child_r(idx));
			}
		}

		if (0 == idx)
			break;
		idx = __parent(idx);
	}
}

static int __previous_ordered_node(avltree_t* me, int idx) {
	int prev, i;

	for (prev = -1, i = __child_l(idx);
	/* array isn't that big, or key is null -> we don't have this child */
	i < me->size && me->nodes[i].key; prev = i, i = __child_r(i))
		;

	return prev;
}

void* avltree_remove(avltree_t* me, void* k) {
	int i;

	for (i = 0; i < me->size;) {
		long r;
		avltree_node_t *n;

		n = &me->nodes[i];

		/* couldn't find it */
		if (!n->key)
			return NULL ;

		r = me->cmp(n->key, k);

		if (r == 0) {
			/* replacement */
			int rep;

			me->count -= 1;

			k = n->key;

			rep = __previous_ordered_node(me, i);
			if (-1 == rep) {
				/* make sure the node is now blank */
				n->key = NULL;
			} else {
				/* have r's left node become right node of r's parent.
				 * NOTE: r by definition shouldn't have a right child */
				__shift_up(me, __child_l(rep), __child_r(__parent(rep)));

				/* have r replace deleted node */
				__shift_up(me, rep, i);
			}

			if (i != 0)
				__rebalance(me, __parent(i));

			return k;
		} else if (r < 0) {
			i = __child_l(i);
		} else if (r > 0) {
			i = __child_r(i);
		} else {
			assert(0);
		}
	}

	/* couldn't find it */
	return NULL ;
}

void avltree_empty(avltree_t* me) {
	int i;

	for (i = 0; i < me->size; i++) {
		me->nodes[i].key = NULL;
	}
}

void avltree_insert(avltree_t* me, void* k, void* v) {
	int i;
	avltree_node_t* n;

	for (i = 0; i < me->size;) {
		n = &me->nodes[i];

		/* found an empty slot */
		if (!n->key) {
			n->key = k;
			n->val = v;
			me->count += 1;

			if (0 == i)
				return;

			__rebalance(me, __parent(i));
			return;
		}

		long r = me->cmp(n->key, k);

		if (r == 0) {
			/*  we don't need to rebalance because we just overwrite this slot */
			n->val = v;
			return;
		} else if (r < 0) {
			i = __child_l(i);
		} else if (r > 0) {
			i = __child_r(i);
		} else {
			assert(0);
		}
	}

	/* we're outside of the loop because we need to enlarge */
	__enlarge(me);
	n = &me->nodes[i];
	n->key = k;
	n->val = v;
	me->count += 1;
}

void* avltree_iterator_peek(avltree_t * h, avltree_iterator_t * iter) {
	if (iter->current_node < h->size) {
		avltree_node_t *next = NULL;

		next = &h->nodes[iter->current_node];
		if (next->key) {
			return next;
		}
	}

	return NULL ;
}

void* avltree_iterator_peek_value(avltree_t * h, avltree_iterator_t * iter) {
	return avltree_get(h, avltree_iterator_peek(h, iter));
}

int avltree_iterator_has_next(avltree_t * h, avltree_iterator_t * iter) {
	return NULL != avltree_iterator_peek(h, iter);
}

void *avltree_iterator_next_value(avltree_t * h, avltree_iterator_t * iter) {
	void* k;

	k = avltree_iterator_next(h, iter);
	if (!k)
		return NULL ;
	return avltree_get(h, k);
}

void *avltree_iterator_next(avltree_t * h, avltree_iterator_t * iter) {
	avltree_node_t *n;
	avltree_node_t *next;

	assert(iter);

	n = &h->nodes[iter->current_node];

	while (iter->current_node < h->size) {
		next = &h->nodes[iter->current_node];
		iter->current_node++;
		if (next->key)
			break;
	}
#if 0
	while (1)
	{
		next_id = __child_l(iter->current_node)
		next = &h->nodes[next_id];
		if (!next->key)
		{
			next_id = __child_r(iter->current_node)
			next = &h->nodes[next_id];
			if (!next->key)
			{
				int descendant;

				parent = __parent(iter->current_node);
				next_id = __parent(iter->current_node)
				while (__child_r(next_id) == parent)
				{
					parent = __parent(iter->current_node);
					next_id = __parent(iter->current_node)
					next_id = __child_r(next_id)
					next = &h->nodes[next_id];
				}

			}
		}
	}
#endif

	return n;
}

void avltree_iterator(avltree_t * h __attribute__((unused)),
		avltree_iterator_t * iter) {
	iter->current_node = 0;
}

static long __uint_compare(const void *e1, const void *e2) {
	return e2 - e1;
}
#endif

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
	nodo->arbolazo = avltree_new(__uint_compare, nodo->max_numeros);
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

	res = !!(bits[idx_arreglo] & (array_bits) (1 << idx_registro));

	return res;
}

static inline void caca_comun_asigna_bit(array_bits *bits, int posicion) {
	int idx_arreglo = 0;
	int idx_registro = 0;

	idx_arreglo = posicion / 64;
	idx_registro = posicion % 64;

	bits[idx_arreglo] |= (array_bits) (1 << idx_registro);

}

static inline void caca_comun_limpia_bit(array_bits *bits, int posicion) {
	int idx_arreglo = 0;
	int idx_registro = 0;

	idx_arreglo = posicion / 64;
	idx_registro = posicion % 64;

	bits[idx_arreglo] &= ~((array_bits) (1 << idx_registro));

}

static inline void caca_x_construye_arbol_binario_segmentado(int *numeros,
		int profundidad, int max_profundidad, int idx_ini, int idx_fin,
		int idx_nodo) {
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
		caca_x_numeros_unicos_en_rango *nodo_anterior = NULL;

		caca_log_debug("seteando estado nodo %d en prof %d\n", idx_nodo,
				profundidad);

		nodo_anterior = estado_actual->nodo;

		memset(estado_actual, 0, sizeof(caca_x_estado_recursion));

		estado_actual->nodo = nodo;
		estado_actual->profundidad = profundidad;
		estado_actual->idx_nodo = idx_nodo;

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
		avltree_insert(nodo->arbolazo, (void *) (long) numero_actual,
				(void *) 1);
		nodo->num_numeros++;

		caca_log_debug("asignado unico numero %d a indice de arbol %d\n",
				numero_actual, idx_nodo);
		while (profundidad_actual < profundidad) {
			bool numero_encontrado = falso;

			estado_actual = estado + profundidad_actual;

			caca_log_debug("añadiendo %d a idx %d nodo %d\n", numero_actual,
					estado_actual->idx_nodo, estado_actual->nodo->idx);

			numero_encontrado = (bool) avltree_get(
					estado_actual->nodo->arbolazo,
					(void *) (long) numero_actual);

			if (!numero_encontrado) {
				caca_log_debug("numero %d nuevo\n", numero_actual);
				estado_actual->nodo->num_numeros++;
			}

			avltree_insert(estado_actual->nodo->arbolazo,
					(void *) (long) numero_actual, (void*) 1);

			assert_timeout(
					estado_actual->nodo->num_numeros
							== avltree_count(estado_actual->nodo->arbolazo));

			profundidad_actual++;
		}
	}

}

static inline void caca_x_suma_unicos(int *sumas_arbol_segmentado,
		int num_nodos) {
	int *numeros_unicos = NULL;
	char buf[100] = { '\0' };

	numeros_unicos = calloc(MAX_NODOS, sizeof(int));
	assert(numeros_unicos);

	for (int i = 0; i < num_nodos; i++) {
		int num_numeros_unicos = 0;

		caca_x_numeros_unicos_en_rango *nodo = NULL;
		avltree_t *arbolazo_actual = NULL;
		avltree_iterator_t * iterador = &(avltree_iterator_t ) { 0 };

		nodo = arbol_numeros_unicos + i;

		if (!nodo->arbolazo) {
			continue;
		}

		assert_timeout(i == nodo->idx);

		arbolazo_actual = nodo->arbolazo;

		avltree_iterator(arbolazo_actual, iterador);

		sumas_arbol_segmentado[i] = 0;

		while (avltree_iterator_has_next(arbolazo_actual, iterador)) {
			int numero_unico_actual = 0;

			numero_unico_actual =
					(int) ((avltree_node_t*) avltree_iterator_peek(
							arbolazo_actual, iterador))->key;

			sumas_arbol_segmentado[i] += numero_unico_actual;
			numeros_unicos[num_numeros_unicos++] = numero_unico_actual;

			avltree_iterator_next(arbolazo_actual, iterador);
		}

		caca_log_debug("los numeros unicos en %d, altura %d, son %s\n", i,
				nodo->altura,
				caca_arreglo_a_cadena(numeros_unicos, num_numeros_unicos, buf));

		assert_timeout(nodo->num_numeros == avltree_count(arbolazo_actual));

		caca_log_debug("la suma es %d\n", sumas_arbol_segmentado[i]);
	}

	free(numeros_unicos);
}

static inline int caca_x_calcular_suma_intersexion(
		caca_x_numeros_unicos_en_rango *nodo_izq,
		caca_x_numeros_unicos_en_rango *nodo_der) {
	int res = 0;
	int num_arbolazo_mayor = 0;
	int num_arbolazo_menor = 0;

	avltree_iterator_t *iterador = &(avltree_iterator_t ) { 0 };
	avltree_t *arbolazo_izq = NULL;
	avltree_t *arbolazo_der = NULL;
	avltree_t *arbolazo_mayor = NULL;
	avltree_t *arbolazo_menor = NULL;

	arbolazo_izq = nodo_izq->arbolazo;
	arbolazo_der = nodo_der->arbolazo;

	if (nodo_izq->num_numeros > nodo_der->num_numeros) {
		caca_log_debug("esCogido para iterar el der %d\n", nodo_der->idx);
		arbolazo_mayor = nodo_izq->arbolazo;
		arbolazo_menor = nodo_der->arbolazo;
		num_arbolazo_mayor = nodo_izq->num_numeros;
		num_arbolazo_menor = nodo_der->num_numeros;
	} else {
		caca_log_debug("esCogido para iterar el izq %d\n", nodo_izq->idx);
		arbolazo_mayor = nodo_der->arbolazo;
		arbolazo_menor = nodo_izq->arbolazo;
		num_arbolazo_mayor = nodo_der->num_numeros;
		num_arbolazo_menor = nodo_izq->num_numeros;
	}

	avltree_iterator(arbolazo_menor, iterador);

	while (avltree_iterator_has_next(arbolazo_menor, iterador)) {
		int numero_actual = 0;
		int numero_encontrado = 0;

		numero_actual = (int) ((avltree_node_t *) avltree_iterator_peek(
				arbolazo_menor, iterador))->key;

		caca_log_debug("el numero %d se va a buscar \n", numero_actual);

		numero_encontrado = (int) avltree_get(arbolazo_mayor,
				(const void *) (long) numero_actual);
		if (numero_encontrado) {
			caca_log_debug("el numero %d encontrado en intersexion\n",
					numero_actual);
			res += numero_actual;
		}

		avltree_iterator_next(arbolazo_menor, iterador);
	}

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

	while (nodo->arbolazo) {
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

static inline void caca_x_encuentra_indices_a_actualizar_laterales(
		caca_x_numeros_unicos_en_rango *arbol_numeros_unicos, int idx_nodo,
		int *indices_izq, int *indices_der, int *num_indices_izq,
		int *num_indices_der, int num_nodos) {
	bool es_nodo_izq = falso;
	bool calculando_nodo_izq = verdadero;
	int num_iteraciones = 0;
	caca_x_numeros_unicos_en_rango *nodo = NULL;

	es_nodo_izq = idx_nodo % 2;

	caca_log_debug("nodo %d es %s\n", idx_nodo, es_nodo_izq ? "izq" : "der");

	nodo = arbol_numeros_unicos + idx_nodo;

	do {
		int *indices_actuales = NULL;
		int *num_indices_actuales = NULL;

		if (num_iteraciones) {
			calculando_nodo_izq = falso;
		}

		indices_actuales = calculando_nodo_izq ? indices_izq : indices_der;
		num_indices_actuales =
				calculando_nodo_izq ? num_indices_izq : num_indices_der;

		if ((calculando_nodo_izq && idx_nodo)
				|| (!calculando_nodo_izq && (idx_nodo < num_nodos))) {
			int idx_nodo_lateral = 0;
			int idx_nodo_ancestro = 0;
			int idx_nodo_a_actualizar = 0;
			int altura = 0;
			int limite_intervalo = 0;
			caca_x_numeros_unicos_en_rango *nodo_lateral_1 = NULL;
			caca_x_numeros_unicos_en_rango *nodo_ancestro = NULL;
			caca_x_numeros_unicos_en_rango *nodo_a_actualizar = NULL;

			idx_nodo_lateral =
					calculando_nodo_izq ? idx_nodo - 1 : idx_nodo + 1;
			caca_log_debug("calculando nodo %s (%d) de %d\n",
					calculando_nodo_izq ? "izq" : "der", idx_nodo_lateral,
					idx_nodo);
			nodo_lateral_1 = arbol_numeros_unicos + idx_nodo_lateral;
			altura = nodo->altura;
			if (altura == nodo_lateral_1->altura) {
				if ((es_nodo_izq && calculando_nodo_izq)
						|| (!es_nodo_izq && !calculando_nodo_izq)) {
					limite_intervalo =
							calculando_nodo_izq ?
									nodo->limite_izq : nodo->limite_der;

					caca_log_debug(
							"el limite intervalo q c buscara en sibling %d\n",
							limite_intervalo);

					idx_nodo_ancestro = idx_nodo_lateral;
					do {
						nodo_ancestro = arbol_numeros_unicos
								+ idx_nodo_ancestro;

						if (nodo_ancestro->limite_izq <= limite_intervalo
								&& limite_intervalo
										<= nodo_ancestro->limite_der) {
							break;
						}

						idx_nodo_ancestro = (idx_nodo_ancestro - 1) / 2;
					} while (idx_nodo_ancestro);

					if (!idx_nodo_ancestro) {
						nodo_ancestro = arbol_numeros_unicos
								+ idx_nodo_ancestro;
					}

					caca_log_debug("ancestro comund de %d y %d es %d\n",
							idx_nodo, idx_nodo_lateral, idx_nodo_ancestro);

					assert_timeout(
							nodo_ancestro->limite_izq <= limite_intervalo
									&& limite_intervalo
											<= nodo_ancestro->limite_der);

					idx_nodo_a_actualizar = idx_nodo_ancestro;

					idx_nodo_a_actualizar = 2 * idx_nodo_a_actualizar
							+ (calculando_nodo_izq ? 1 : 2);

					do {
						nodo_a_actualizar = arbol_numeros_unicos
								+ idx_nodo_a_actualizar;
						indices_actuales[(*num_indices_actuales)++] =
								idx_nodo_a_actualizar;
						idx_nodo_a_actualizar = 2 * idx_nodo_a_actualizar
								+ (calculando_nodo_izq ? 2 : 1);
					} while (nodo_a_actualizar->altura);
				} else {
					if (nodo_lateral_1->altura) {
						idx_nodo_a_actualizar = 2 * idx_nodo_lateral
								+ (es_nodo_izq ? 1 : 2);
						nodo_a_actualizar = arbol_numeros_unicos
								+ idx_nodo_a_actualizar;

						indices_actuales[(*num_indices_actuales)++] =
								idx_nodo_a_actualizar;

						while (nodo_a_actualizar->altura) {
							idx_nodo_a_actualizar = 2 * idx_nodo_a_actualizar
									+ (es_nodo_izq ? 1 : 2);
							nodo_a_actualizar = arbol_numeros_unicos
									+ idx_nodo_a_actualizar;

							indices_actuales[(*num_indices_actuales)++] =
									idx_nodo_a_actualizar;
						}
					} else {
						caca_log_debug(
								"el nodo %d es hoja, no hay nada q acer en su relacion con %d\n",
								idx_nodo_lateral, idx_nodo);
					}
				}
			} else {
				caca_log_debug(
						"nodo %d es %s de su fila, nada q acer para el lado %s\n",
						idx_nodo, es_nodo_izq ? "primero" : "ultimo",
						calculando_nodo_izq ? "izq" : "der");
			}
		} else {
			caca_log_debug("%s nodo %d, nada q acer para el lado %s\n",
					!idx_nodo ? "1er" : "ultimo", idx_nodo,
					calculando_nodo_izq ? "izq" : "der");
		}

		num_iteraciones++;
	} while (calculando_nodo_izq);
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

		assert_timeout(idx_nodo_izq == nodo_izq->idx);
		assert_timeout(idx_nodo_der == nodo_der->idx);

		caca_log_debug(
				"cruce de %d con %d en la matriz de intersexxxiones resta %d\n",
				nodo_izq->idx, nodo_der->altura,
				matriz_sumas_coincidencias[nodo_izq->idx][nodo_der->altura]);

		res -= matriz_sumas_coincidencias[nodo_izq->idx][nodo_der->altura];
	}

	caca_log_debug("La suma es %d\n", res);

	return res;
}

static inline void caca_x_encuentra_indices_afectados_por_actualizacion(
		caca_x_numeros_unicos_en_rango *nodos, int idx_nodo,
		int idx_actualizado, int *indices, int *num_indices) {
	caca_x_numeros_unicos_en_rango *nodo = NULL;

	nodo = nodos + idx_nodo;

	if (nodo->limite_izq == idx_actualizado
			&& idx_actualizado == nodo->limite_der) {
		caca_log_debug("cuando la luna %d,%d\n", nodo->limite_izq,
				nodo->limite_der);
		indices[(*num_indices)++] = idx_nodo;

	} else {
		if (nodo->limite_der < idx_actualizado
				|| idx_actualizado < nodo->limite_izq) {
			caca_log_debug("ilumine tus ojos %d:%d\n", nodo->limite_izq,
					nodo->limite_der);
		} else {

			caca_log_debug("en la oscuridad %d,%d\n", nodo->limite_izq,
					nodo->limite_der);
			indices[(*num_indices)++] = idx_nodo;
			caca_x_encuentra_indices_afectados_por_actualizacion(nodos,
					2 * idx_nodo + 1, idx_actualizado, indices, num_indices);
			caca_x_encuentra_indices_afectados_por_actualizacion(nodos,
					2 * idx_nodo + 2, idx_actualizado, indices, num_indices);
		}
	}

}

static inline bool caca_comun_arreglo_contiene(tipo_dato *arreglo,
		int tam_arreglo, tipo_dato valor_buscado) {
	int i;
	for (i = 0; i < tam_arreglo; i++) {
//		caca_log_debug("comprarando %ld con %ld", *(arreglo + i), valor_buscado);
		if (*(arreglo + i) == valor_buscado) {
			return verdadero;
		}
	}
	return falso;
}

static inline void caca_x_actualiza_intersexxxiones_lateral(
		caca_x_numeros_unicos_en_rango *arbol_numeros_unicos,
		int matriz_sumas_coincidencias[MAX_NODOS][16], int *indices_afectados,
		int num_indices_afectados, int nuevo_valor, int viejo_valor,
		int idx_actualizado, bool calculando_lado_izq) {

	for (int i = 0; i < num_indices_afectados; i++) {
		bool numero_encontrado = falso;
		int idx_a_actualizar = 0;
		int idx_matrix_izq = 0;
		int idx_matrix_der = 0;
		int altura_nodo_actualizado = 0;
		caca_x_numeros_unicos_en_rango *nodo_lateral = NULL;
		caca_x_numeros_unicos_en_rango *nodo_actualizado = NULL;
		avltree_t *arbolazo_nodo_lateral = NULL;

		idx_a_actualizar = indices_afectados[i];

		nodo_lateral = arbol_numeros_unicos + idx_a_actualizar;

		arbolazo_nodo_lateral = nodo_lateral->arbolazo;

		nodo_actualizado = arbol_numeros_unicos + idx_actualizado;
		altura_nodo_actualizado = nodo_actualizado->altura;

		if (calculando_lado_izq) {
			idx_matrix_izq = idx_a_actualizar;
			idx_matrix_der = altura_nodo_actualizado;
		} else {
			idx_matrix_izq = idx_actualizado;
			idx_matrix_der = nodo_lateral->altura;
		}

		numero_encontrado = (bool) avltree_get(arbolazo_nodo_lateral,
				(const void *) (long) viejo_valor);

		if (numero_encontrado) {
			matriz_sumas_coincidencias[idx_matrix_izq][idx_matrix_der] -=
					viejo_valor;
		}

		numero_encontrado = (bool) avltree_get(arbolazo_nodo_lateral,
				(const void*) (long) nuevo_valor);

		if (numero_encontrado) {
			matriz_sumas_coincidencias[idx_matrix_izq][idx_matrix_der] +=
					nuevo_valor;
		}

	}

}

static inline void caca_x_actualiza_sumas_intersexxxiones(
		caca_x_numeros_unicos_en_rango *arbol_numeros_unicos,
		int matriz_sumas_coincidencias[MAX_NODOS][16], int *indices_afectados,
		int num_indices_afectados, int nuevo_valor, int viejo_valor,
		int idx_actualizado, int num_nodos) {

	for (int i = 0; i < num_indices_afectados; i++) {
		int idx_a_actualizar = 0;
		int num_indices_izq = 0;
		int num_indices_der = 0;
		int *indices_izq = (int[16] ) { 0 };
		int *indices_der = (int[16] ) { 0 };
		char buf[100];

		idx_a_actualizar = indices_afectados[i];

		caca_x_encuentra_indices_a_actualizar_laterales(arbol_numeros_unicos,
				idx_a_actualizar, indices_izq, indices_der, &num_indices_izq,
				&num_indices_der, num_nodos);

		caca_log_debug("los indices afectados de lado izq %s, der %s para %d\n",
				caca_arreglo_a_cadena(indices_izq, num_indices_izq, buf),
				caca_arreglo_a_cadena(indices_der, num_indices_der, buf),
				idx_a_actualizar);

		caca_x_actualiza_intersexxxiones_lateral(arbol_numeros_unicos,
				matriz_sumas_coincidencias, indices_izq, num_indices_izq,
				nuevo_valor, viejo_valor, idx_a_actualizar, verdadero);
		caca_x_actualiza_intersexxxiones_lateral(arbol_numeros_unicos,
				matriz_sumas_coincidencias, indices_der, num_indices_der,
				nuevo_valor, viejo_valor, idx_a_actualizar, falso);

	}
}

static inline void caca_x_actualiza_arbol_numeros_unicos(
		caca_x_numeros_unicos_en_rango *arbol_numeros_unicos,
		int *indices_a_actualizar, int num_indices_a_actualizar,
		int viejo_pendejo, int nuevo_valor) {

	for (int i = 0; i < num_indices_a_actualizar; i++) {
		int idx_a_actualizar = 0;
		caca_x_numeros_unicos_en_rango *nodo_a_actualizar = NULL;
		avltree_t *arbolazo = NULL;

		idx_a_actualizar = indices_a_actualizar[i];
		nodo_a_actualizar = arbol_numeros_unicos + idx_a_actualizar;
		arbolazo = nodo_a_actualizar->arbolazo;

		avltree_remove(arbolazo, (void *) (long) viejo_pendejo);
		avltree_insert(arbolazo, (void *) (long) nuevo_valor, (void *) 1);
	}
}

static inline void caca_x_actualiza_sumas_arbol_segmentado(
		int *sumas_arbol_segmentado, int *indices_a_actualizar,
		int num_indices_a_actualizar, int nuevo_valor, int viejo_pendejo) {

	for (int i = 0; i < num_indices_a_actualizar; i++) {
		int idx_a_actualizar = 0;

		idx_a_actualizar = indices_a_actualizar[i];

		sumas_arbol_segmentado[idx_a_actualizar] -= viejo_pendejo;
		sumas_arbol_segmentado[idx_a_actualizar] += nuevo_valor;
	}
}

static inline void caca_x_actualiza_estado(int *numeros,
		caca_x_numeros_unicos_en_rango *arbol_numeros_unicos,
		int *sumas_arbol_segmentado,
		int matriz_sumas_coincidencias[MAX_NODOS][16], int idx_actualizado,
		int nuevo_valor, int num_nodos) {
	int num_indices_afectados_actualizacion = 0;
	int viejo_pendejo = 0;
	int *indices_afectados_actualizacion = (int[16] ) { 0 };
	char buf[100];

	viejo_pendejo = numeros[idx_actualizado];

	if (viejo_pendejo == nuevo_valor) {
		caca_log_debug("nuevo y viejo valor son %d, nada q acer\n",
				viejo_pendejo);
		return;
	}

	caca_x_encuentra_indices_afectados_por_actualizacion(arbol_numeros_unicos,
			0, idx_actualizado, indices_afectados_actualizacion,
			&num_indices_afectados_actualizacion);

	caca_log_debug("los idx afectados %s\n",
			caca_arreglo_a_cadena(indices_afectados_actualizacion, num_indices_afectados_actualizacion, buf));

	caca_log_debug("el viejo %d y el nuevo %d\n", viejo_pendejo, nuevo_valor);

	caca_x_actualiza_sumas_intersexxxiones(arbol_numeros_unicos,
			matriz_sumas_coincidencias, indices_afectados_actualizacion,
			num_indices_afectados_actualizacion, nuevo_valor, viejo_pendejo,
			idx_actualizado, num_nodos);

	caca_x_actualiza_arbol_numeros_unicos(arbol_numeros_unicos,
			indices_afectados_actualizacion,
			num_indices_afectados_actualizacion, viejo_pendejo, nuevo_valor);

	caca_x_actualiza_sumas_arbol_segmentado(sumas_arbol_segmentado,
			indices_afectados_actualizacion,
			num_indices_afectados_actualizacion, nuevo_valor, viejo_pendejo);

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
	int num_nodos = 0;
	int *sumas_arbol_segmentado = NULL;
	int *matriz_sumas_coincidencias = NULL;

	char buf[100] = { '\0' };

	matriz_nums = calloc(MAX_NUMEROS * 3, sizeof(int));
	assert_timeout(matriz_nums);

	matriz_sumas_coincidencias = calloc(MAX_NODOS * 16, sizeof(int));
	assert_timeout(matriz_sumas_coincidencias);

#ifdef USA_MALLOC
	estado = malloc(16 * sizeof(caca_x_estado_recursion));
	assert_timeout(estado);
	memset(estado, 0, 16 * sizeof(caca_x_estado_recursion));
#else
	estado = calloc(16, sizeof(caca_x_estado_recursion));
	assert_timeout(estado);
#endif

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

	num_nodos = (2 << (max_profundidad + 0));

	caca_log_debug("el numero de nodos %d\n",num_nodos);

	arbol_numeros_unicos = calloc(num_nodos,
			sizeof(caca_x_numeros_unicos_en_rango));
	assert_timeout(arbol_numeros_unicos);

	sumas_arbol_segmentado = calloc(num_nodos, sizeof(int));
	assert_timeout(sumas_arbol_segmentado);

	caca_log_debug("llamando a func rec con max prof %d\n",
			max_profundidad - 1);

	caca_x_construye_arbol_binario_segmentado(numeros, 0, max_profundidad, 0,
			num_numeros_redondeado - 1, 0);

	caca_x_suma_unicos(sumas_arbol_segmentado, num_nodos);

	caca_x_generar_sumas_de_intersexiones(
			(int (*)[16]) matriz_sumas_coincidencias, arbol_numeros_unicos,
			num_nodos - 2);

	while (cont_queries < num_queries) {
		int idx_actualizado = 0;
		int nuevo_valor = 0;
		int sum = 0;
		scanf("%c %d %d\n", &tipo_query, &idx_query_ini, &idx_query_fin);
		caca_log_debug("q: %c, ini %d, fin %d\n", tipo_query, idx_query_ini,
				idx_query_fin);

		switch (tipo_query) {
		case 'Q':
			sum = caca_x_suma_segmento(sumas_arbol_segmentado,
					(int (*)[16]) matriz_sumas_coincidencias, idx_query_ini - 1,
					idx_query_fin - 1);
			printf("%d\n", sum);
			break;
		case 'U':

			idx_actualizado = idx_query_ini - 1;
			nuevo_valor = idx_query_fin;
			caca_x_actualiza_estado(numeros, arbol_numeros_unicos,
					sumas_arbol_segmentado,
					(int (*)[16]) matriz_sumas_coincidencias, idx_actualizado,
					nuevo_valor, (2 << (max_profundidad + 0)) - 2);
			break;
		default:
			abort();
			break;
		}

		cont_queries++;
	}

	free(matriz_nums);
	free(arbol_numeros_unicos);
	free(sumas_arbol_segmentado);
	free(matriz_sumas_coincidencias);
	free(estado);
}

int main(void) {
//	puts("he corrido con algo de suerte"); /* prints he corrido con algo de suerte */
//	sleep(10);
	caca_x_main();
	return EXIT_SUCCESS;
}
