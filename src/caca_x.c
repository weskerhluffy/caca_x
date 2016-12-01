/*
 ============================================================================
 Name        : caca_x.c
 Author      : ernesto
 Version     :
 Copyright   : a veces siento que
 Description : Hello World in C, Ansi-style
 ============================================================================
 tamaño 41333
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stddef.h>
#include <unistd.h>
#include <math.h>

#define MAX_NUMEROS 50000
#define MAX_VALOR INT_MAX
#define MAX_QUERIES 100000
// 12 es 10 de el num, 1 del espacio 1 de signo
#define TAM_MAX_LINEA (MAX_NUMEROS*12) 
#define CACA_X_MAX_PROFUNDIDAD 19
#define CACA_X_MAX_NODOS_AFECTADOS CACA_X_MAX_PROFUNDIDAD*2
#define CACA_X_MAX_NUMEROS_REDONDEADO (1<<(CACA_X_MAX_PROFUNDIDAD-2))
#define CACA_X_MAX_NODOS (1 << CACA_X_MAX_PROFUNDIDAD)
#define CACA_X_VALOR_INVALIDO -1
#define CACA_X_MAX_VALORES_INT ((unsigned int)((unsigned int)INT_MAX-(INT_MIN)))

#define CACA_X_BUF_STATICO_DUMP_ARBOL (char[1000] ) { '\0' }

//#define CACA_X_VALIDAR_ARBOLINES

/*
#define caca_log_debug printf
 */
 #define caca_log_debug(formato, args...) 0
#define assert_timeout(condition) assert(condition);
/*
 #define assert_timeout(condition) 0
 #define assert_timeout(condition) if(!(condition)){printf("fuck\n");sleep(10);assert(condition);}
 */

typedef int tipo_dato;
typedef unsigned int natural;
typedef long long entero_largo;
typedef unsigned long long bitch_vector;

typedef enum BOOLEANOS {
	falso = 0, verdadero
} bool;

#define max(x,y) ((x) < (y) ? (y) : (x))
#define min(x,y) ((x) < (y) ? (x) : (y))

#if 1

typedef unsigned int khint32_t;

typedef unsigned long long khint64_t;
typedef khint32_t khint_t;
typedef khint_t khiter_t;
static const double __ac_HASH_UPPER = 0.77;
static inline khint_t __ac_X31_hash_string(const char *s) {
	khint_t h = (khint_t) *s;
	if (h)
		for (++s; *s; ++s)
			h = (h << 5) - h + (khint_t) *s;
	return h;
}
static inline khint_t __ac_Wang_hash(khint_t key) {
	key += ~(key << 15);
	key ^= (key >> 10);
	key += (key << 3);
	key ^= (key >> 6);
	key += ~(key << 11);
	key ^= (key >> 16);
	return key;
}
typedef const char *kh_cstr_t;
typedef struct kh_caca_s {
	khint_t n_buckets, size, n_occupied, upper_bound;
	khint32_t *flags;
	khint32_t *keys;
	int *vals;
} kh_caca_t;

#define kh_key(h, x) ((h)->keys[x])
#define kh_val(h, x) ((h)->vals[x])
#define kh_value(h, x) ((h)->vals[x])
#define kh_begin(h) (khint_t)(0)
#define kh_end(h) ((h)->n_buckets)
#define kh_size(h) ((h)->size)
#define kh_exist(h, x) (!__ac_iseither((h)->flags, (x)))

#define __ac_isempty(flag, i) ((flag[i>>4]>>((i&0xfU)<<1))&2)
#define __ac_isdel(flag, i) ((flag[i>>4]>>((i&0xfU)<<1))&1)
#define __ac_iseither(flag, i) ((flag[i>>4]>>((i&0xfU)<<1))&3)
#define __ac_set_isdel_false(flag, i) (flag[i>>4]&=~(1ul<<((i&0xfU)<<1)))
#define __ac_set_isempty_false(flag, i) (flag[i>>4]&=~(2ul<<((i&0xfU)<<1)))
#define __ac_set_isboth_false(flag, i) (flag[i>>4]&=~(3ul<<((i&0xfU)<<1)))
#define __ac_set_isdel_true(flag, i) (flag[i>>4]|=1ul<<((i&0xfU)<<1))
#define __ac_fsize(m) ((m) < 16? 1 : (m)>>4)

static inline __attribute__ ((__unused__)) kh_caca_t *kh_init_caca(void) {
	return (kh_caca_t*) calloc(1, sizeof(kh_caca_t));
}
static inline __attribute__ ((__unused__)) void kh_destroy_caca(kh_caca_t *h) {
	if (h) {
		free((void *) h->keys);
		free(h->flags);
		free((void *) h->vals);
		free(h);
	}
}
static inline __attribute__ ((__unused__)) void kh_clear_caca(kh_caca_t *h) {
	if (h && h->flags) {
		__builtin___memset_chk(h->flags, 0xaa,
				((h->n_buckets) < 16 ? 1 : (h->n_buckets) >> 4)
						* sizeof(khint32_t),
				__builtin_object_size(h->flags, 0));
		h->size = h->n_occupied = 0;
	}
}
static inline __attribute__ ((__unused__)) khint_t kh_get_caca(
		const kh_caca_t *h, khint32_t key) {
	if (h->n_buckets) {
		khint_t k, i, last, mask, step = 0;
		mask = h->n_buckets - 1;
		k = (khint32_t) (key);
		i = k & mask;
		last = i;
		while (!((h->flags[i >> 4] >> ((i & 0xfU) << 1)) & 2)
				&& (((h->flags[i >> 4] >> ((i & 0xfU) << 1)) & 1)
						|| !((h->keys[i]) == (key)))) {
			i = (i + (++step)) & mask;
			if (i == last)
				return h->n_buckets;
		}
		return ((h->flags[i >> 4] >> ((i & 0xfU) << 1)) & 3) ? h->n_buckets : i;
	} else
		return 0;
}
static inline __attribute__ ((__unused__)) int kh_resize_caca(kh_caca_t *h,
		khint_t new_n_buckets) {
	khint32_t *new_flags = 0;
	khint_t j = 1;
	{
		(--(new_n_buckets), (new_n_buckets) |= (new_n_buckets) >> 1, (new_n_buckets) |=
				(new_n_buckets) >> 2, (new_n_buckets) |= (new_n_buckets) >> 4, (new_n_buckets) |=
				(new_n_buckets) >> 8, (new_n_buckets) |= (new_n_buckets) >> 16, ++(new_n_buckets));
		if (new_n_buckets < 4)
			new_n_buckets = 4;
		if (h->size >= (khint_t) (new_n_buckets * __ac_HASH_UPPER + 0.5))
			j = 0;
		else {
			new_flags = (khint32_t*) malloc(
					((new_n_buckets) < 16 ? 1 : (new_n_buckets) >> 4)
							* sizeof(khint32_t));
			if (!new_flags)
				return -1;
			__builtin___memset_chk(new_flags, 0xaa,
					((new_n_buckets) < 16 ? 1 : (new_n_buckets) >> 4)
							* sizeof(khint32_t),
					__builtin_object_size(new_flags, 0));
			if (h->n_buckets < new_n_buckets) {
				khint32_t *new_keys = (khint32_t*) realloc((void *) h->keys,
						new_n_buckets * sizeof(khint32_t));
				if (!new_keys) {
					free(new_flags);
					return -1;
				}
				h->keys = new_keys;
				if (1) {
					int *new_vals = (int*) realloc((void *) h->vals,
							new_n_buckets * sizeof(int));
					if (!new_vals) {
						free(new_flags);
						return -1;
					}
					h->vals = new_vals;
				}
			}
		}
	}
	if (j) {
		for (j = 0; j != h->n_buckets; ++j) {
			if (((h->flags[j >> 4] >> ((j & 0xfU) << 1)) & 3) == 0) {
				khint32_t key = h->keys[j];
				int val;
				khint_t new_mask;
				new_mask = new_n_buckets - 1;
				if (1)
					val = h->vals[j];
				(h->flags[j >> 4] |= 1ul << ((j & 0xfU) << 1));
				while (1) {
					khint_t k, i, step = 0;
					k = (khint32_t) (key);
					i = k & new_mask;
					while (!((new_flags[i >> 4] >> ((i & 0xfU) << 1)) & 2))
						i = (i + (++step)) & new_mask;
					(new_flags[i >> 4] &= ~(2ul << ((i & 0xfU) << 1)));
					if (i < h->n_buckets
							&& ((h->flags[i >> 4] >> ((i & 0xfU) << 1)) & 3)
									== 0) {
						{
							khint32_t tmp = h->keys[i];
							h->keys[i] = key;
							key = tmp;
						}
						if (1) {
							int tmp = h->vals[i];
							h->vals[i] = val;
							val = tmp;
						}
						(h->flags[i >> 4] |= 1ul << ((i & 0xfU) << 1));
					} else {
						h->keys[i] = key;
						if (1)
							h->vals[i] = val;
						break;
					}
				}
			}
		}
		if (h->n_buckets > new_n_buckets) {
			h->keys = (khint32_t*) realloc((void *) h->keys,
					new_n_buckets * sizeof(khint32_t));
			if (1)
				h->vals = (int*) realloc((void *) h->vals,
						new_n_buckets * sizeof(int));
		}
		free(h->flags);
		h->flags = new_flags;
		h->n_buckets = new_n_buckets;
		h->n_occupied = h->size;
		h->upper_bound = (khint_t) (h->n_buckets * __ac_HASH_UPPER + 0.5);
	}
	return 0;
}
static inline __attribute__ ((__unused__)) khint_t kh_put_caca(kh_caca_t *h,
		khint32_t key, int *ret) {
	khint_t x;
	if (h->n_occupied >= h->upper_bound) {
		if (h->n_buckets > (h->size << 1)) {
			if (kh_resize_caca(h, h->n_buckets - 1) < 0) {
				*ret = -1;
				return h->n_buckets;
			}
		} else if (kh_resize_caca(h, h->n_buckets + 1) < 0) {
			*ret = -1;
			return h->n_buckets;
		}
	}
	{
		khint_t k, i, site, last, mask = h->n_buckets - 1, step = 0;
		x = site = h->n_buckets;
		k = (khint32_t) (key);
		i = k & mask;
		if (((h->flags[i >> 4] >> ((i & 0xfU) << 1)) & 2))
			x = i;
		else {
			last = i;
			while (!((h->flags[i >> 4] >> ((i & 0xfU) << 1)) & 2)
					&& (((h->flags[i >> 4] >> ((i & 0xfU) << 1)) & 1)
							|| !((h->keys[i]) == (key)))) {
				if (((h->flags[i >> 4] >> ((i & 0xfU) << 1)) & 1))
					site = i;
				i = (i + (++step)) & mask;
				if (i == last) {
					x = site;
					break;
				}
			}
			if (x == h->n_buckets) {
				if (((h->flags[i >> 4] >> ((i & 0xfU) << 1)) & 2)
						&& site != h->n_buckets)
					x = site;
				else
					x = i;
			}
		}
	}
	if (((h->flags[x >> 4] >> ((x & 0xfU) << 1)) & 2)) {
		h->keys[x] = key;
		(h->flags[x >> 4] &= ~(3ul << ((x & 0xfU) << 1)));
		++h->size;
		++h->n_occupied;
		*ret = 1;
	} else if (((h->flags[x >> 4] >> ((x & 0xfU) << 1)) & 1)) {
		h->keys[x] = key;
		(h->flags[x >> 4] &= ~(3ul << ((x & 0xfU) << 1)));
		++h->size;
		*ret = 2;
	} else
		*ret = 0;
	return x;
}
static inline __attribute__ ((__unused__)) void kh_del_caca(kh_caca_t *h,
		khint_t x) {
	if (x != h->n_buckets && !((h->flags[x >> 4] >> ((x & 0xfU) << 1)) & 3)) {
		(h->flags[x >> 4] |= 1ul << ((x & 0xfU) << 1));
		--h->size;
	}
}

char *kh_shit_dumpear(kh_caca_t *h, char *buf) {
	*buf = '\0';
#ifndef ONLINE_JUDGE
	for (int k = kh_begin(h); k != kh_end(h); ++k) {
		if (kh_exist(h, k)) {
			char *buf_tmp = CACA_X_BUF_STATICO_DUMP_ARBOL;
			sprintf(buf_tmp, "%u -> %u\n", kh_key(h,k), (kh_val(h,k)));
			strcat(buf, buf_tmp);
		}
	}
#endif
	return buf;
}

#endif

#if 1

//http://www.cprogramming.com/snippets/source-code/singly-linked-list-insert-remove-add-count

typedef struct node {
	natural data;
	struct node *next;
} nodo_lista;

typedef struct lista_pendeja
{
        nodo_lista *cabeza;
        nodo_lista *cola;
} lista_pendeja;

void listilla_append(lista_pendeja *lista, natural num) {
	nodo_lista *temp, *right;
	temp = (nodo_lista *) malloc(sizeof(nodo_lista));
	temp->data = num;
	right = (nodo_lista *) lista->cola;
	assert_timeout(!right->next);
	right->next = temp;
	right = temp;
	right->next = NULL;
	lista->cola=right;
}

void listilla_add(lista_pendeja *lista, natural num) {
	nodo_lista *temp;
	nodo_lista *head = lista->cabeza;
	nodo_lista *cola= NULL;
	temp = (nodo_lista *) malloc(sizeof(nodo_lista));
	temp->data = num;
	if (head == NULL ) {
		head = temp;
		head->next = NULL;
		cola=head;
	} else {
		temp->next = head;
		head = temp;
		cola=lista->cola;
	}
	lista->cabeza= head;
	lista->cola=cola;
}

void listilla_insert(lista_pendeja *lista, natural num) {
	natural c = 0;
	nodo_lista *temp;
	nodo_lista *head = lista->cabeza;
	temp = head;
	if (temp == NULL ) {
		listilla_add(lista, num);
	} else {
		listilla_append(lista, num);
	}
	lista->cabeza= head;
}

static inline char *listilla_a_cadena(lista_pendeja *lista, char *buf) {
	nodo_lista *right;
	right = (nodo_lista *) lista->cabeza;
#ifndef ONLINE_JUDGE
	while (right != NULL ) {
		char *buf_tmp = CACA_X_BUF_STATICO_DUMP_ARBOL;
		sprintf(buf_tmp, "%u ", right->data);
		strcat(buf, buf_tmp);
		right = right->next;
	}
	return buf;
}

#endif

#endif

typedef struct caca_x_numeros_unicos_en_rango {
	kh_caca_t *tablon;
	natural max_numeros;
	natural max_num_esperados;
	entero_largo suma;
	natural altura;
	natural idx;
	natural limite_izq;
	natural limite_der;
} caca_x_numeros_unicos_en_rango;

typedef struct caca_preprocesada {
	kh_caca_t *tablon;
	entero_largo suma;
} caca_preprocesada;

natural num_numeros = 0;
natural num_numeros_redondeado = 0;
natural idx_numeros_max = 0;
natural idx_numeros_redondeado_max = 0;
natural idx_actualizado = 0;
natural limite_izq = 0;
natural limite_der = 0;
natural num_indices_nodos = 0;
tipo_dato nuevo_valor = 0;
tipo_dato viejo_pendejo = 0;
natural *indices_nodos = (natural[CACA_X_MAX_NODOS_AFECTADOS] ) { 0 };
tipo_dato *numeros = NULL;
caca_x_numeros_unicos_en_rango *arbol_numeros_unicos = NULL;
caca_preprocesada *datos_prepro = NULL;
kh_caca_t *posicion_a_idx_datos_prepro = NULL;
lista_pendeja idx_bloques_by_posiciones[MAX_NUMEROS];

static inline void caca_x_validar_segmentos(
		caca_x_numeros_unicos_en_rango *arbolin_segs, tipo_dato *numeros,
		natural *indices, natural num_numeros, natural num_numeros_redondeado,
		natural num_nodos, natural num_indices);

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

#ifdef ONLINE_JUDGE
	return NULL;
#endif

#ifndef CACA_X_LOG
	return NULL ;
#endif

	memset(buffer, 0, 100);
	ap_buffer = buffer;

	for (i = 0; i < tam_arreglo; i++) {
		characteres_escritos += sprintf(ap_buffer + characteres_escritos, "%d",
				*(arreglo + i));
		if (i < tam_arreglo - 1) {
			*(ap_buffer + characteres_escritos++) = ',';
		}
	}
	*(ap_buffer + characteres_escritos) = '\0';
	return ap_buffer;
}

static inline void caca_x_inicializar_nodo(caca_x_numeros_unicos_en_rango *nodo,
		int altura, int idx_nodo, int limite_izq, int limite_der) {
	nodo->altura = altura;
	nodo->max_numeros = 1 << nodo->altura;
	if (limite_izq <= idx_numeros_max) {
		//TODO: max_numeros
		nodo->tablon = kh_init_caca();
	}
	nodo->idx = idx_nodo;
	nodo->limite_izq = limite_izq;
	nodo->limite_der = limite_der;

}

static inline void caca_x_mergear_arboles(kh_caca_t *tablin_izq,
		kh_caca_t *tablin_der, kh_caca_t **tablin_res,
		entero_largo *suma_unicos) {
	entero_largo suma_unicos_int = 0;

	assert_timeout(tablin_izq || tablin_der);
	if (tablin_izq) {
		int ret = 0;
		khiter_t iter_izq;
		khiter_t iter_res;
		kh_caca_t *tablin_res_int;

		tablin_res_int = kh_init_caca();

		for (iter_izq = kh_begin(tablin_izq) ; iter_izq != kh_end(tablin_izq);
				++iter_izq) {
			if (kh_exist(
					tablin_izq, iter_izq)) {
				iter_res = kh_put_caca(tablin_res_int,
						kh_key(tablin_izq, iter_izq), &ret);
						assert_timeout(
								ret == 1 );
						kh_value(tablin_res_int, iter_res)= kh_val(tablin_izq,iter_izq);
						suma_unicos_int+=kh_key(tablin_res_int, iter_res);
						caca_log_debug("sumando de izq %u de llave %u para sumar %u\n",kh_value(tablin_izq, iter_izq),kh_key(tablin_izq, iter_izq) ,kh_value(tablin_res_int, iter_res));
					}
			}

		assert_timeout(kh_size(tablin_izq)==kh_size(tablin_res_int));

		if (tablin_der) {
			khiter_t iter_der = 0;

			for (iter_der = kh_begin(tablin_der) ; iter_der != kh_end(
					tablin_der); ++iter_der) {
				if (kh_exist(tablin_der, iter_der)) {
					natural llave_der = kh_key(tablin_der, iter_der);
					iter_res = kh_put_caca(tablin_res_int,
							llave_der, &ret);
					assert_timeout(ret != -1);
					if(!ret) {
						kh_value(tablin_res_int,
								iter_res)+= kh_val(tablin_der,iter_der);
						caca_log_debug("sumando de der %u  x q ya staba %u\n",kh_value(tablin_res_int, iter_res),llave_der);
					}
					else {
						kh_value(tablin_res_int, iter_res)= kh_val(tablin_der,iter_der);
						suma_unicos_int+=kh_key(tablin_res_int, iter_res);
						caca_log_debug("anadiendo de der %u x q NO staba %u\n",kh_value(tablin_res_int, iter_res),llave_der);
					}

				}
			}

		}
		*tablin_res = tablin_res_int;
	}
	*suma_unicos = suma_unicos_int;
}

static inline kh_caca_t *caca_x_construye_arbol_binario_segmentado(
		natural idx_nodo, natural idx_num_ini, natural idx_num_fin,
		natural altura) {

	caca_x_numeros_unicos_en_rango *nodo = arbol_numeros_unicos + idx_nodo;
	caca_x_inicializar_nodo(nodo, altura, idx_nodo, idx_num_ini, idx_num_fin);

	if (idx_num_ini <= idx_numeros_max) {
		caca_log_debug("construyendo intervalo %u-%u\n", idx_num_ini,
				idx_num_fin);
		if (idx_num_ini != idx_num_fin) {
			entero_largo suma_unicos = 0;
			natural idx_hijo_izq = idx_nodo << 1 | 1;
			natural altura_sig = altura - 1;
			natural idx_medio = idx_num_ini
					+ ((idx_num_fin - idx_num_ini) >> 1);

			caca_log_debug(
					"esto no es una elegia, digo oja, idx %u, altura %u\n",
					idx_nodo, altura);

			if (idx_numeros_max >= idx_num_fin) {
				nodo->max_num_esperados = nodo->max_numeros;
			} else {
				if (idx_numeros_max >= idx_num_ini) {
					nodo->max_num_esperados =
							(idx_numeros_max - idx_num_ini + 1);
				}
			}

			caca_log_debug(
					"mergeando arboles de %u y %u para %u intervalo %u->%u\n",
					idx_hijo_izq, idx_hijo_izq + 1, idx_nodo, idx_num_ini,
					idx_num_fin);
			caca_x_mergear_arboles(
					caca_x_construye_arbol_binario_segmentado(idx_hijo_izq,
							idx_num_ini, idx_medio, altura_sig),
					caca_x_construye_arbol_binario_segmentado(idx_hijo_izq + 1,
							idx_medio + 1, idx_num_fin, altura_sig),
					&nodo->tablon, &suma_unicos);

			nodo->suma = suma_unicos;
			caca_log_debug(
					"mergeada la suma final en intervalo %u->%u es %lld\n",
					idx_num_ini, idx_num_fin, suma_unicos);

		} else {
			int ret = 0;
			khiter_t iter_tablon;
			kh_caca_t *tablon = nodo->tablon;
			tipo_dato numero_actual = numeros[idx_num_ini];

			assert_timeout(altura == 0);
			nodo->suma = numero_actual;

			assert_timeout(kh_get_caca(tablon, numero_actual)==kh_end(tablon));

			iter_tablon = kh_put_caca(tablon, numero_actual, &ret);

			assert_timeout(ret == 1);
			kh_val(tablon,iter_tablon)=1;
		}
		return nodo->tablon;
	} else {
		return NULL ;
	}
}

static inline void caca_x_encuentra_indices_segmento(natural idx_nodo) {
	caca_x_numeros_unicos_en_rango *nodo = arbol_numeros_unicos + idx_nodo;
	natural nodo_lim_izq = nodo->limite_izq;
	natural nodo_lim_der = nodo->limite_der;

	if (nodo_lim_der < limite_izq || limite_der < nodo_lim_izq) {
		return;
	}

	if (nodo_lim_izq < limite_izq || nodo_lim_der > limite_der) {
		natural idx_nodo_hijo_izq = (idx_nodo << 1) | 1;
		caca_log_debug("pues nadie sera %d,%d\n", nodo->limite_izq,
				nodo->limite_der);
		caca_x_encuentra_indices_segmento(idx_nodo_hijo_izq);
		caca_x_encuentra_indices_segmento(idx_nodo_hijo_izq + 1);
		return;
	}
	assert_timeout(limite_izq <= nodo_lim_izq && nodo_lim_der <= limite_der);
	caca_log_debug("te vas a acordar de mi %d,%d\n", nodo->limite_izq,
			nodo->limite_der);
	indices_nodos[num_indices_nodos++] = idx_nodo;

}

static inline entero_largo caca_x_generar_suma_repetidos(
		kh_caca_t *tablon_unicos) {
	entero_largo suma_repetidos = 0;

	caca_log_debug("sumando repetidos\n");

//	tablon_unicos = arbol_numeros_unicos[indices_nodos[0]].tablon;

	for (int i = 0; i < num_indices_nodos; i++) {
		khiter_t iter_actual;
		kh_caca_t *tablon_actual = arbol_numeros_unicos[indices_nodos[i]].tablon;

		for (iter_actual = kh_begin(tablon_actual) ;
				iter_actual != kh_end(tablon_actual); ++iter_actual) {
			if (kh_exist(tablon_actual, iter_actual)) {
				int ret;
				khiter_t iter_unicos;
				entero_largo numero_actual = kh_key(tablon_actual,iter_actual);
				iter_unicos = kh_get_caca(tablon_unicos, numero_actual);
				if (iter_unicos == kh_end(tablon_unicos)) {
					iter_unicos = kh_put_caca(tablon_unicos, numero_actual,
							&ret);
					assert_timeout(ret == 1 || ret == 2);
					kh_val(tablon_unicos,iter_unicos)=kh_val(tablon_actual,iter_actual);
				} else {
					suma_repetidos += numero_actual;
					kh_val(tablon_unicos,iter_unicos)+=kh_val(tablon_actual,iter_actual);
				}
			}
		}

	}

	caca_log_debug("en total la suma de repetidos es %lld\n", suma_repetidos);
	return suma_repetidos;
}

int caca_comun_compara_enteros(const void *a, const void *b) {
	int a_int = 0;
	int b_int = 0;
	int resultado = 0;

	a_int = *(int *) a;
	b_int = *(int *) b;

	caca_log_debug("comparando %d con %d\n", a_int, b_int);
	resultado = a_int - b_int;
	return resultado;
}

static inline entero_largo caca_x_suma_segmento(kh_caca_t *tablon_unicos) {
	entero_largo res = 0;
#ifdef CACA_X_LOG
	char buf[100] = {'\0'};
#endif

	num_indices_nodos = 0;
	caca_x_encuentra_indices_segmento(0);

	assert_timeout(num_indices_nodos < CACA_X_MAX_NODOS_AFECTADOS);

	qsort(indices_nodos, num_indices_nodos, sizeof(int),
			caca_comun_compara_enteros);
	caca_log_debug("indices de segmento %d:%d son %u: %s\n", limite_izq,
			limite_der, num_indices_nodos,
			caca_arreglo_a_cadena((tipo_dato *) indices_nodos,
					num_indices_nodos, buf));

#if 1
	for (int i = 0; i < num_indices_nodos; i++) {
		caca_log_debug("segmento %u aporta %lld\n", indices_nodos[i],
				arbol_numeros_unicos[indices_nodos[i]].suma);
		res += arbol_numeros_unicos[indices_nodos[i]].suma;
	}

	res -= caca_x_generar_suma_repetidos(tablon_unicos);
#endif

#if 0
	res = caca_x_generar_suma_unicos();
	caca_log_debug("La suma es %lld\n", res);

#endif
	return res;
}

static inline void caca_x_actualiza_segmentos(natural idx_nodo) {
	int ret = 0;
	khiter_t iter;
	caca_x_numeros_unicos_en_rango *nodo = arbol_numeros_unicos + idx_nodo;
	natural nodo_lim_izq = nodo->limite_izq;
	natural nodo_lim_der = nodo->limite_der;

	caca_log_debug(
			"stand by my side actualizando? indice %u en nodo %u %u->%u\n",
			idx_actualizado, idx_nodo, nodo_lim_izq, nodo_lim_der);

	if (nodo_lim_der < idx_actualizado || idx_actualizado < nodo_lim_izq) {
		return;
	}

	caca_log_debug("pues nadie sera %d,%d\n", nodo->limite_izq,
			nodo->limite_der);

	kh_caca_t *tablon = nodo->tablon;

	if (nodo_lim_izq != nodo_lim_der) {
		natural idx_nodo_hijo_izq = (idx_nodo << 1) | 1;
		natural idx_nodo_hijo_der = idx_nodo_hijo_izq + 1;
		caca_log_debug(
				"antes de actualizar recursivo nodo %u %u->%u suma %lld\n",
				idx_nodo, nodo_lim_izq, nodo_lim_der, nodo->suma);

		caca_x_actualiza_segmentos(idx_nodo_hijo_izq);
		caca_x_actualiza_segmentos(idx_nodo_hijo_der);

		iter = kh_get_caca(tablon, viejo_pendejo);

		assert_timeout(iter!=kh_end(tablon));

		kh_val(tablon,iter)--;

		if (!kh_val(tablon,iter)){
		nodo->suma -= viejo_pendejo;
		kh_del_caca(tablon,iter);
	}
		iter = kh_put_caca(tablon, nuevo_valor, &ret);
		assert_timeout(ret != -1);
		if (ret) {
			assert_timeout(ret == 1 || ret == 2);
			kh_val(tablon,iter)=1;
			nodo->suma += nuevo_valor;
		} else {
			kh_val(tablon,iter)++;
		}
		caca_log_debug(
				"despues de actualizar recursivo nodo %u %u->%u suma %lld\n",
				idx_nodo, nodo_lim_izq, nodo_lim_der, nodo->suma);
	} else {
		iter = kh_get_caca(tablon, viejo_pendejo);

		assert_timeout(iter!=kh_end(tablon));

		assert_timeout(kh_val(tablon,iter)==1);

		kh_del_caca(tablon, iter);

		nodo->suma -= viejo_pendejo;

		assert_timeout(!nodo->suma);

		iter = kh_put_caca(tablon, nuevo_valor, &ret);

		assert_timeout(ret == 1 || ret == 2);

		kh_val(tablon,iter)=1;

		nodo->suma += nuevo_valor;

		assert_timeout(nodo->suma == nuevo_valor);
		caca_log_debug("actualizando de %d a %llu nodo %u %u\n",
				numeros[nodo_lim_izq], nodo->suma, idx_nodo, idx_actualizado);

		numeros[nodo_lim_izq] = nodo->suma;
	}
}

static inline bool caca_comun_checa_bit(bitch_vector *bits,
		unsigned long posicion) {
	bool res = falso;
	int idx_arreglo = 0;
	int idx_registro = 0;

	idx_arreglo = posicion / 64;
	idx_registro = posicion % 64;

	res = !!(bits[idx_arreglo]
			& (bitch_vector) ((bitch_vector) 1 << idx_registro));

	return res;
}

static inline void caca_comun_asigna_bit(bitch_vector *bits,
		unsigned long posicion) {
	int idx_arreglo = 0;
	int idx_registro = 0;

	idx_arreglo = posicion / 64;
	idx_registro = posicion % 64;

	bits[idx_arreglo] |= (bitch_vector) ((bitch_vector) 1 << idx_registro);

}

static inline void caca_comun_limpia_bit(bitch_vector *bits,
		unsigned long posicion) {
	int idx_arreglo = 0;
	int idx_registro = 0;

	idx_arreglo = posicion / 64;
	idx_registro = posicion % 64;

	bits[idx_arreglo] &= (bitch_vector) ~((bitch_vector) 1 << idx_registro);

}

static inline void caca_x_valida_segmentos_sumas(
		caca_x_numeros_unicos_en_rango *arbolini, tipo_dato *numeros,
		natural num_nodos, natural num_numeros, natural *indices,
		natural num_indices) {
	natural limite_iteracion = 0;
	bitch_vector *mapa_unicos = NULL;

	mapa_unicos = calloc(CACA_X_MAX_VALORES_INT / (sizeof(bitch_vector) * 8),
			sizeof(bitch_vector));

	assert_timeout(!!num_numeros != !!(indices && num_indices));

	limite_iteracion = num_numeros ? num_numeros : num_indices;

	for (int i = 0; i < limite_iteracion; i++) {
		natural num_idx_ini = 0;
		natural num_idx_fin = 0;
		natural indice_nodo = 0;
		caca_x_numeros_unicos_en_rango *nodo = NULL;

		indice_nodo = num_numeros ? i : indices[i];

		nodo = arbolini + indice_nodo;

		num_idx_ini = nodo->limite_izq;
		num_idx_fin = nodo->limite_der;

		if (nodo->max_num_esperados) {
			entero_largo suma_presumado = 0;
			entero_largo suma_mapa_unicos = 0;
			entero_largo suma_arboles = 0;
			natural encontrados_en_mapa = 0;
			natural encontrados_en_arbol = 0;

			khiter_t iter;

			suma_presumado = nodo->suma;

			for (natural j = num_idx_ini;
					j
							<= (num_idx_fin > idx_numeros_max ?
									idx_numeros_max : num_idx_fin); j++) {
				if (!caca_comun_checa_bit(mapa_unicos,
						(unsigned long) (numeros[j]
								+ (unsigned long) ((unsigned long) INT_MAX + 1)))) {
					khiter_t iter;
					suma_mapa_unicos += numeros[j];
					caca_comun_asigna_bit(mapa_unicos,
							(unsigned long) (numeros[j]
									+ (unsigned long) ((unsigned long) INT_MAX
											+ 1)));

					iter = kh_get_caca(nodo->tablon, numeros[j]);
					assert_timeout(iter!=kh_end(nodo->tablon));
					encontrados_en_mapa += 1;
				}
			}

			for (iter = kh_begin(nodo->tablon) ; iter != kh_end(nodo->tablon);
					++iter) {
				if (kh_exist(nodo->tablon, iter)) {
					natural num_act = kh_key(nodo->tablon,iter);
					suma_arboles += num_act;
					assert_timeout(
							caca_comun_checa_bit(mapa_unicos, (unsigned long) (num_act+ (unsigned long) ((unsigned long) INT_MAX + 1))));
				encontrados_en_arbol += 1;
				caca_comun_limpia_bit(mapa_unicos,
						(unsigned long) (num_act
								+ (unsigned long) ((unsigned long) INT_MAX + 1)));
				}
				}

			assert_timeout(encontrados_en_mapa == kh_size(nodo->tablon));

			assert_timeout(encontrados_en_arbol == encontrados_en_mapa);

			for (natural j = num_idx_ini; j <= num_idx_fin; j++) {
				assert_timeout(
						!caca_comun_checa_bit(mapa_unicos, (unsigned long) (numeros[j]+ (unsigned long) ((unsigned long) INT_MAX + 1))));
				caca_comun_limpia_bit(mapa_unicos,
						(unsigned long) (numeros[j]
								+ (unsigned long) ((unsigned long) INT_MAX + 1)));
			}

			assert_timeout(suma_presumado == suma_mapa_unicos);
			assert_timeout(suma_presumado == suma_arboles);
		}

	}

	free(mapa_unicos);
}

static inline void caca_x_validar_segmentos(
		caca_x_numeros_unicos_en_rango *arbolin_segs, tipo_dato *numeros,
		natural *indices, natural num_numeros, natural num_numeros_redondeado,
		natural num_nodos, natural num_indices) {
	caca_x_valida_segmentos_sumas(arbolin_segs, numeros, num_nodos, num_numeros,
			indices, num_indices);

}

static inline void caca_x_crea_datos_preprocesados() {
	natural tam_bloque = 0;
	natural num_bloques = 0;
	natural idx_dato_prepro = 0;

	tam_bloque = ceil(pow(num_numeros, 2.0 / 3.0));
	num_bloques = (num_numeros / tam_bloque);
	if (num_numeros % tam_bloque) {
		num_bloques++;
	}

	caca_log_debug("el tam bloq %u num de bloqs %u\n", tam_bloque, num_bloques);

	datos_prepro = calloc(num_bloques * num_bloques, sizeof(caca_preprocesada));

	for (int i = 0; i < num_bloques; i++) {
		for (int j = i; j < num_bloques; j++) {
			idx_dato_prepro = i * num_bloques + j;
			entero_largo *suma = &datos_prepro[idx_dato_prepro].suma;
			kh_caca_t *tablon = datos_prepro[idx_dato_prepro].tablon;

			tablon = kh_init_caca();

			limite_izq = i * tam_bloque;
			limite_der = (j + 1) * tam_bloque - 1;
			if (limite_der > num_numeros - 1) {
				limite_der = num_numeros - 1;
			}

			caca_log_debug(
					"creando caca prepro idx %u(%u,%u) limite izq %u limite der %u\n",
					idx_dato_prepro, i, j, limite_izq, limite_der);
			*suma = caca_x_suma_segmento(tablon);
			caca_log_debug("la suma %lld, el mapa %s\n", *suma,
					kh_shit_dumpear(tablon, CACA_X_BUF_STATICO_DUMP_ARBOL));
			for (int k = limite_izq; k <= limite_der; k++) {
				lista_pendeja *lista_pos= idx_bloques_by_posiciones+k;
				listilla_insert(lista_pos, idx_dato_prepro);
				caca_log_debug("aora la lista de bloques de pops %u es %s\n", k,
						listilla_a_cadena(lista_pos,
								CACA_X_BUF_STATICO_DUMP_ARBOL));
			}
		}
	}
}

static inline void caca_x_main() {
	int *matriz_nums = NULL;
	int num_filas = 0;
	int num_queries = 0;
	int cont_queries = 0;
	char tipo_query = 0;
	int idx_query_ini = 0;
	int idx_query_fin = 0;
	int max_profundidad = 0;
	int num_nodos = 0;

	char buf[100] = { '\0' };

	matriz_nums = calloc(CACA_X_MAX_NUMEROS_REDONDEADO * 3, sizeof(int));
	assert_timeout(matriz_nums);

	num_filas = 3;
	lee_matrix_long_stdin(matriz_nums, &num_filas, NULL, 3,
			CACA_X_MAX_NUMEROS_REDONDEADO);

	num_numeros = *matriz_nums;
	numeros = matriz_nums + CACA_X_MAX_NUMEROS_REDONDEADO;
	num_queries = *(numeros + CACA_X_MAX_NUMEROS_REDONDEADO);

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

	num_nodos = (2 << (max_profundidad + 0)) - 1;

	caca_log_debug("el numero de nodos %d\n", num_nodos);

	arbol_numeros_unicos = calloc(num_nodos,
			sizeof(caca_x_numeros_unicos_en_rango));
	assert_timeout(arbol_numeros_unicos);

	caca_log_debug("llamando a func rec con max prof %d\n",
			max_profundidad + 2);

	idx_numeros_max = num_numeros - 1;
	idx_numeros_redondeado_max = num_numeros_redondeado - 1;
	caca_x_construye_arbol_binario_segmentado(0, 0, idx_numeros_redondeado_max,
			max_profundidad);

#ifdef CACA_X_VALIDAR_ARBOLINES
	caca_x_validar_segmentos(arbol_numeros_unicos, numeros, NULL, num_numeros,
			num_numeros_redondeado, num_nodos, 0);
#endif

	caca_x_crea_datos_preprocesados();
	/*
	 while (cont_queries < num_queries) {
	 entero_largo sum = 0;
	 scanf("%c %d %d\n", &tipo_query, &idx_query_ini, &idx_query_fin);
	 if (tipo_query == 'Q' && idx_query_ini > idx_query_fin) {
	 tipo_dato tmp = 0;
	 tmp = idx_query_fin;
	 idx_query_fin = idx_query_ini;
	 idx_query_ini = tmp;
	 }
	 caca_log_debug("q: %c, ini %d, fin %d\n", tipo_query, idx_query_ini,
	 idx_query_fin);

	 switch (tipo_query) {
	 case 'Q':
	 limite_izq = idx_query_ini - 1;
	 limite_der = idx_query_fin - 1;
	 sum = caca_x_suma_segmento();
	 printf("%lld\n", sum);
	 break;
	 case 'U':

	 idx_actualizado = idx_query_ini - 1;
	 nuevo_valor = idx_query_fin;
	 viejo_pendejo = numeros[idx_actualizado];
	 caca_x_actualiza_segmentos(0);

	 #ifdef CACA_X_VALIDAR_ARBOLINES
	 caca_x_validar_segmentos(arbol_numeros_unicos, numeros, NULL,
	 num_numeros, num_numeros_redondeado, num_nodos, 0);
	 #endif

	 break;
	 default:
	 abort();
	 break;
	 }

	 cont_queries++;
	 }
	 */

	free(matriz_nums);
	free(arbol_numeros_unicos);
}

int main(void) {
//	puts("he corrido con algo de suerte");
//	sleep(10);
	caca_x_main();
	return EXIT_SUCCESS;
}
