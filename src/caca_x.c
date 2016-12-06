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
	natural tam_inicial;
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

static inline __attribute__ ((__unused__)) int kh_resize_caca(kh_caca_t *h,
		khint_t new_n_buckets);

static inline __attribute__ ((__unused__)) kh_caca_t *kh_init_caca(
		natural tam_inicial) {
	natural tam_inicial_redondeado = 0;
	natural max_profundidad = 0;
	kh_caca_t *mierda = calloc(1, sizeof(kh_caca_t));
	while ((tam_inicial >> max_profundidad)) {
		max_profundidad++;
	}
	tam_inicial_redondeado = (1 << max_profundidad);
	mierda->tam_inicial = tam_inicial_redondeado << 1;
	assert_timeout(kh_resize_caca(mierda, mierda->n_buckets + 1) >= 0);

	return mierda;
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
	khint_t k, i, last, mask, step = 0;
	mask = h->n_buckets - 1;
	k = (khint32_t) (key);
	i = k & mask;
	last = i;
	/*
	 while (!((h->flags[i >> 4] >> ((i & 0xfU) << 1)) & 2)
	 && (((h->flags[i >> 4] >> ((i & 0xfU) << 1)) & 1)
	 */
	khint32_t bandera_caca;
	khint32_t *flags = h->flags;
	khint32_t *keys = h->keys;
	while (!((bandera_caca = (flags[i >> 4] >> ((i & 0xfU) << 1))) & 2)
			&& ((bandera_caca & 1) || !((keys[i]) == (key)))) {
		i = (i + (++step)) & mask;
		if (i == last)
			return h->n_buckets;
	}
	return ((flags[i >> 4] >> ((i & 0xfU) << 1)) & 3) ? h->n_buckets : i;
}
static inline __attribute__ ((__unused__)) int kh_resize_caca(kh_caca_t *h,
		khint_t new_n_buckets) {
	khint32_t *new_flags = 0;
	khint_t j = 1;
	{
		(--(new_n_buckets), (new_n_buckets) |= (new_n_buckets) >> 1, (new_n_buckets) |=
				(new_n_buckets) >> 2, (new_n_buckets) |= (new_n_buckets) >> 4, (new_n_buckets) |=
				(new_n_buckets) >> 8, (new_n_buckets) |= (new_n_buckets) >> 16, ++(new_n_buckets));
		if (new_n_buckets < 4) {
//			new_n_buckets = 512;
			new_n_buckets = h->tam_inicial;
		}
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

/*! @function
 @abstract     Insert a key to the hash table.
 @param  name  Name of the hash table [symbol]
 @param  h     Pointer to the hash table [khash_t(name)*]
 @param  k     Key [type of keys]
 @param  r     Extra return code: -1 if the operation failed;
 0 if the key is present in the hash table;
 1 if the bucket is empty (never used); 2 if the element in
 the bucket has been deleted [int*]
 @return       Iterator to the inserted element [khint_t]
 */
static inline __attribute__ ((__unused__)) khint_t kh_put_caca(kh_caca_t *h,
		khint32_t key, int *ret) {
	khint_t x;

	khint_t k, i, site, last, mask = h->n_buckets - 1, step = 0;
	x = site = h->n_buckets;
	k = (khint32_t) (key);
	i = k & mask;
	khint32_t *flags = h->flags;
	khint32_t *keys = h->keys;
	khint32_t banderilla_loca = (flags[i >> 4] >> ((i & 0xfU) << 1));
	if ((banderilla_loca & 2))
		x = i;
	else {
		last = i;
		while (!((banderilla_loca = flags[i >> 4] >> ((i & 0xfU) << 1)) & 2)
				&& ((banderilla_loca & 1) || !((keys[i]) == (key)))) {
			if ((banderilla_loca & 1))
				site = i;
			i = (i + (++step)) & mask;
			if (i == last) {
				x = site;
				break;
			}
		}
		if (x == h->n_buckets) {
			if ((banderilla_loca & 2) && site != h->n_buckets)
				x = site;
			else
				x = i;
		}
	}

	banderilla_loca = (flags[x >> 4] >> ((x & 0xfU) << 1));
	if ((banderilla_loca & 2)) {
		keys[x] = key;
		(flags[x >> 4] &= ~(3ul << ((x & 0xfU) << 1)));
		++h->size;
		++h->n_occupied;
		*ret = 1;
	} else if ((banderilla_loca & 1)) {
		keys[x] = key;
		(flags[x >> 4] &= ~(3ul << ((x & 0xfU) << 1)));
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

typedef struct lista_pendeja {
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
	lista->cola = right;
}

void listilla_add(lista_pendeja *lista, natural num) {
	nodo_lista *temp;
	nodo_lista *head = lista->cabeza;
	nodo_lista *cola = NULL;
	temp = (nodo_lista *) malloc(sizeof(nodo_lista));
	temp->data = num;
	if (head == NULL ) {
		head = temp;
		head->next = NULL;
		cola = head;
	} else {
		temp->next = head;
		head = temp;
		cola = lista->cola;
	}
	lista->cabeza = head;
	lista->cola = cola;
}

void listilla_insert(lista_pendeja *lista, natural num) {
	nodo_lista *temp;
	nodo_lista *head = lista->cabeza;
	temp = head;
	if (temp == NULL ) {
		listilla_add(lista, num);
	} else {
		listilla_append(lista, num);
	}
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
#endif
	return buf;
}

#endif

typedef struct caca_preprocesada {
	kh_caca_t *tablon;
	entero_largo suma;
} caca_preprocesada;

natural num_numeros = 0;
natural idx_actualizado = 0;
natural limite_izq = 0;
natural limite_der = 0;
tipo_dato nuevo_valor = 0;
tipo_dato viejo_pendejo = 0;
tipo_dato *numeros = NULL;
caca_preprocesada *datos_prepro = NULL;
kh_caca_t *posicion_a_idx_datos_prepro = NULL;
lista_pendeja idx_bloques_by_posiciones[MAX_NUMEROS];
natural tam_bloque = 0;
natural num_bloques = 0;
bitch_vector *mapa_unicos = NULL;

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

static inline void caca_x_crea_datos_preprocesados() {
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
			entero_largo suma_act = 0;
			idx_dato_prepro = i * num_bloques + j;
			entero_largo *suma = &datos_prepro[idx_dato_prepro].suma;
			kh_caca_t *tablon = kh_init_caca(tam_bloque << 5);

			limite_izq = i * tam_bloque;
			limite_der = (j + 1) * tam_bloque - 1;
			if (limite_der > num_numeros - 1) {
				limite_der = num_numeros - 1;
			}

			caca_log_debug(
					"creando caca prepro idx %u(%u,%u) limite izq %u limite der %u\n",
					idx_dato_prepro, i, j, limite_izq, limite_der);
//			*suma = caca_x_suma_segmento(tablon);
			for (int k = limite_izq; k <= limite_der; k++) {
				int ret;
				khiter_t iter;
				lista_pendeja *lista_pos = idx_bloques_by_posiciones + k;
				tipo_dato num_actual = numeros[k];
				iter = kh_put_caca(tablon, num_actual, &ret);
				assert_timeout(ret != -1);
				if (ret) {
					kh_value(tablon,iter)=1;
					suma_act+=num_actual;
				}
				else {
					kh_value(tablon,iter)+=1;
				}

				listilla_insert(lista_pos, idx_dato_prepro);
				caca_log_debug("aora la lista de bloques de pops %u es %s\n", k,
						listilla_a_cadena(lista_pos,
								CACA_X_BUF_STATICO_DUMP_ARBOL));
			}
			*suma = suma_act;
			caca_log_debug("la suma %lld, el mapa %s\n", *suma,
					kh_shit_dumpear(tablon, CACA_X_BUF_STATICO_DUMP_ARBOL));

			datos_prepro[idx_dato_prepro].tablon = tablon;
		}
	}
}

static inline void caca_x_actualizar_datos_preprocesados(
		natural idx_pos_actualizar, tipo_dato nuevo_valor) {
	tipo_dato viejo_pendejo = numeros[idx_pos_actualizar];
	lista_pendeja *lista_idx_bloques = idx_bloques_by_posiciones
			+ idx_pos_actualizar;
	nodo_lista *nodo_act = NULL;

	caca_log_debug("la lista de bloques para idx %u es %s\n",
			idx_pos_actualizar,
			listilla_a_cadena(lista_idx_bloques,
					CACA_X_BUF_STATICO_DUMP_ARBOL));
	nodo_act = lista_idx_bloques->cabeza;

	while (nodo_act) {
		natural idx_bloque_actual = nodo_act->data;
		natural ocurrencias = 0;
		khiter_t iter = 0;
		int ret = 0;

		entero_largo *suma = &datos_prepro[idx_bloque_actual].suma;
		kh_caca_t *tablon = datos_prepro[idx_bloque_actual].tablon;
		caca_log_debug("en nodo %u el tablon %p\n", idx_bloque_actual, tablon);
		if (tablon) {

			caca_log_debug("el bloke %u tiene suma %lld i tablon %s\n",
					idx_bloque_actual, *suma,
					kh_shit_dumpear(tablon, CACA_X_BUF_STATICO_DUMP_ARBOL));

			iter = kh_get_caca(tablon, viejo_pendejo);
			assert_timeout(iter!=kh_end(tablon));
			ocurrencias = kh_val(tablon,iter);
			caca_log_debug("las ocurrencias de %u son %u\n", viejo_pendejo,
					ocurrencias);
			assert_timeout(ocurrencias >= 1);
			ocurrencias--;
			kh_val(tablon,iter)=ocurrencias;
			if (!ocurrencias) {
				caca_log_debug("a suma actual %lld se le resta %u\n", *suma,
						viejo_pendejo);
				(*suma) -= viejo_pendejo;
				kh_del_caca(tablon, iter);
			}

			iter = kh_put_caca(tablon, nuevo_valor, &ret);
			assert_timeout(ret != -1);
			if (ret) {
				kh_val(tablon,iter)=1;
				caca_log_debug("en valor nuevo %u no estaba, sumandolo a %lld\n",nuevo_valor,*suma);
				(*suma)+=nuevo_valor;
			}
			else {
				kh_val(tablon,iter)+=1;
				caca_log_debug("en valor nuevo %u ya estaba, aora ai %u ocurrencias\n",nuevo_valor,kh_val(tablon,iter));
			}
		}
		nodo_act = nodo_act->next;
	}

	numeros[idx_pos_actualizar] = nuevo_valor;
}

static inline void caca_x_main() {
	int *matriz_nums = NULL;
	int num_filas = 0;
	int num_queries = 0;
	int cont_queries = 0;
	char tipo_query = 0;
	int idx_query_ini = 0;
	int idx_query_fin = 0;

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

	printf("armando caca \n");
	caca_x_crea_datos_preprocesados();
	printf("ya caca \n");

	mapa_unicos = calloc(CACA_X_MAX_VALORES_INT / (sizeof(bitch_vector) * 8),
			sizeof(bitch_vector));
	assert_timeout(mapa_unicos);

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
			/*
			 limite_izq = idx_query_ini - 1;
			 limite_der = idx_query_fin - 1;
			 sum = caca_x_suma_segmento();
			 printf("%lld\n", sum);
			 */
			break;
		case 'U':

			idx_actualizado = idx_query_ini - 1;
			nuevo_valor = idx_query_fin;
			caca_x_actualizar_datos_preprocesados(idx_actualizado, nuevo_valor);

			break;
		default:
			abort();
			break;
		}

		cont_queries++;
	}

	/*
	 while (1) {
	 printf("dormdo\n");
	 sleep(10);
	 }
	 */
	free(mapa_unicos);
	free(matriz_nums);
}

int main(void) {
//	puts("he corrido con algo de suerte");
//	sleep(10);
	caca_x_main();
	return EXIT_SUCCESS;
}
