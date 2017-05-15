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
#include <stdint.h>

#define MAX_NUMEROS 50000
#define MAX_VALOR INT_MAX
#define MAX_QUERIES 100000
// 12 es 10 de el num, 1 del espacio 1 de signo
#define TAM_MAX_LINEA (MAX_NUMEROS*12) 
#define CACA_X_MAX_PROFUNDIDAD 19
#define CACA_X_MAX_NODOS_AFECTADOS CACA_X_MAX_PROFUNDIDAD*2
#define CACA_X_MAX_NUMEROS_REDONDEADO (1<<(CACA_X_MAX_PROFUNDIDAD-2))
#define CACA_X_MAX_NODOS (1 << CACA_X_MAX_PROFUNDIDAD)
#define CACA_X_VALOR_INVALIDO -1LL
/** debiera ser 12401660 segun el conteo empirico */
#define CACA_X_MAX_VALORES_INT UINT_MAX
#define CACA_X_MAX_NODOS_LISTA 13000000
#define CACA_X_MAX_BLOQUES 37

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
typedef unsigned long long entero_largo_sin_signo;

typedef enum BOOLEANOS {
	falso = 0, verdadero
} bool;

#define caca_comun_max(x,y) ((x) < (y) ? (y) : (x))
#define caca_comun_min(x,y) ((x) < (y) ? (x) : (y))

#if 0

typedef natural hm_iter;

#define HASH_MAP_VALOR_INVALIDO ((hm_iter)CACA_X_VALOR_INVALIDO)

typedef struct hash_map_entry {
	entero_largo llave;
	entero_largo valor;
}hm_entry;

typedef struct hash_map_cubeta {
	uint64_t hash;
	hm_entry *entry;
}hm_cubeta;

typedef struct hash_map_robin_hood_back_shift {
	hm_cubeta *buckets_;
	uint64_t num_buckets_;
	uint64_t num_buckets_used_;
	uint64_t probing_min_;
	uint64_t probing_max_;
}hm_rr_bs_tabla;

int hash_map_robin_hood_back_shift_init(hm_rr_bs_tabla *ht, int num_cubetas) {
	ht->num_buckets_ = num_cubetas;
	ht->buckets_ = (hm_cubeta *) calloc(ht->num_buckets_, sizeof(hm_cubeta));
	ht->num_buckets_used_ = 0;
	ht->probing_max_ = num_cubetas;
	return 0;
}

int hash_map_robin_hood_back_shift_fini(hm_rr_bs_tabla *ht) {
	if (ht->buckets_ != NULL ) {
		for (uint32_t i = 0; i < ht->num_buckets_; i++) {
			if (ht->buckets_[i].entry != NULL ) {
				free(ht->buckets_[i].entry);
				ht->buckets_[i].entry = NULL;
			}
		}
		free(ht->buckets_);
	}

	return 0;
}

static inline int hash_map_robin_hood_back_shift_llena_distancia_a_indice_inicio(
		hm_rr_bs_tabla *ht, uint64_t index_stored, uint64_t *distance) {
	hm_cubeta cubeta = ht->buckets_[index_stored];

	*distance = 0;

	if (cubeta.entry == NULL )
	return -1;

	uint64_t num_cubetas = ht->num_buckets_;

	uint64_t index_init = cubeta.hash % num_cubetas;
	if (index_init <= index_stored) {
		*distance = index_stored - index_init;
	} else {
		*distance = index_stored + (num_cubetas - index_init);
	}
	return 0;
}

hm_iter hash_map_robin_hood_back_shift_obten(hm_rr_bs_tabla *ht,
		const entero_largo key, entero_largo *value) {
	uint64_t num_cubetas = ht->num_buckets_;
	uint64_t prob_max = ht->probing_max_;

//	uint64_t hash = hash_function_caca(key);
	uint64_t hash = key % num_cubetas;
	uint64_t index_init = hash;
	uint64_t probe_distance = 0;
	uint64_t index_current;
	bool found = falso;
	uint32_t i;
	*value = HASH_MAP_VALOR_INVALIDO;
	for (i = 0; i < num_cubetas; i++) {
		index_current = (index_init + i) % num_cubetas;
		hm_entry *entrada = ht->buckets_[index_current].entry;

		if (entrada == NULL ) {
			break;
		}

		hash_map_robin_hood_back_shift_llena_distancia_a_indice_inicio(ht,
				index_current, &probe_distance);
		if (i > probe_distance) {
			break;
		}

		if (entrada->llave == key) {
			*value = entrada->valor;
			found = verdadero;
			break;
		}
	}

	if (found)
	return index_current;

	return HASH_MAP_VALOR_INVALIDO;
}

hm_iter hash_map_robin_hood_back_shift_pon(hm_rr_bs_tabla *ht, entero_largo key,
		entero_largo value, bool *nuevo_entry) {

	uint64_t num_cubetas = ht->num_buckets_;
	uint64_t prob_max = ht->probing_max_;
	uint64_t prob_min = ht->probing_min_;
	hm_cubeta *cubetas = ht->buckets_;

	*nuevo_entry = verdadero;

	if (ht->num_buckets_used_ == num_cubetas) {
		*nuevo_entry = falso;
		return HASH_MAP_VALOR_INVALIDO;
	}
	ht->num_buckets_used_ += 1;

//	uint64_t hash = hash_function_caca(key);
	uint64_t hash = key % num_cubetas;
	uint64_t index_init = hash;

	hm_entry *entry = (hm_entry *) calloc(1, sizeof(hm_entry));
	entry->llave = key;
	entry->valor = value;

	uint64_t index_current = index_init % num_cubetas;
	uint64_t probe_current = 0;
	uint64_t probe_distance;
	hm_entry *entry_temp;
	uint64_t hash_temp;
	uint64_t i;

	for (i = 0; i < num_cubetas; i++) {
		index_current = (index_init + i) % num_cubetas;
		hm_cubeta *cubeta = cubetas + index_current;

		if (cubeta->entry == NULL ) {
			cubeta->entry = entry;
			cubeta->hash = hash;
			if (index_current > prob_max) {
				ht->probing_max_ = index_current;
			}
			if (index_current < prob_min) {
				ht->probing_min_ = index_current;
			}
			break;
		} else {
			if (cubeta->entry->llave == key) {
				free(entry);
				*nuevo_entry = falso;
				break;
			}
			hash_map_robin_hood_back_shift_llena_distancia_a_indice_inicio(ht,
					index_current, &probe_distance);
			if (probe_current > probe_distance) {
				// Swapping the current bucket with the one to insert
				entry_temp = cubeta->entry;
				hash_temp = cubeta->hash;
				cubeta->entry = entry;
				cubeta->hash = hash;
				entry = entry_temp;
				hash = hash_temp;
				probe_current = probe_distance;
			}
		}
		probe_current++;
	}

	return index_current;
}

int hash_map_robin_hood_back_shift_borra(hm_rr_bs_tabla *ht,
		const tipo_dato key) {
	uint64_t num_cubetas = ht->num_buckets_;
	uint64_t prob_max = ht->probing_max_;
	uint64_t prob_min = ht->probing_max_;

	uint64_t hash = key % num_cubetas;
	uint64_t index_init = hash;
	bool found = falso;
	uint64_t index_current = 0;
	uint64_t probe_distance = 0;
	hm_entry *entrada = NULL;

	for (uint64_t i = 0; i < num_cubetas; i++) {
		index_current = (index_init + i) % num_cubetas;
		entrada = ht->buckets_[index_current].entry;

		hash_map_robin_hood_back_shift_llena_distancia_a_indice_inicio(ht,
				index_current, &probe_distance);
		if (entrada == NULL || i > probe_distance) {
			break;
		}

		if (entrada->llave == key) {
			found = verdadero;
			break;
		}
	}

	if (found) {
		free(entrada);

		uint64_t i = 1;
		uint64_t index_previous = 0, index_swap = 0;

		for (i = 1; i < num_cubetas; i++) {
			index_previous = (index_current + i - 1) % num_cubetas;
			index_swap = (index_current + i) % num_cubetas;

			hm_cubeta *cubeta_swap = ht->buckets_ + index_swap;
			hm_cubeta *cubeta_previous = ht->buckets_ + index_previous;

			if (cubeta_swap->entry == NULL ) {
				cubeta_previous->entry = NULL;
				break;
			}
			uint64_t distance;
			if (hash_map_robin_hood_back_shift_llena_distancia_a_indice_inicio(
							ht, index_swap, &distance) != 0) {
				fprintf(stderr, "Error in FillDistanceToInitIndex()");
			}
			if (!distance) {
				cubeta_previous->entry = NULL;
				break;
			}
			cubeta_previous->entry = cubeta_swap->entry;
			cubeta_previous->hash = cubeta_swap->hash;
		}
		if (i < num_cubetas) {
			if (index_previous == prob_min) {
				prob_min++;
				if (prob_min >= num_cubetas) {
					prob_min = 0;
				} else {
					while (prob_min < num_cubetas
							&& ht->buckets_[prob_min].entry) {
						prob_min++;
					}
					if (prob_min >= num_cubetas) {
						prob_min = num_cubetas;
					}
				}

				ht->probing_min_ = prob_min;
			}

			if (index_previous == prob_max) {
				prob_max--;
				if (prob_max >= num_cubetas) {
					prob_max = num_cubetas;
				} else {
					while (((int64_t) prob_max) >= 0
							&& ht->buckets_[prob_max].entry) {
						prob_max--;
					}
					if (prob_max >= num_cubetas) {
						prob_max = 0;
					}
				}

				ht->probing_max_ = prob_max;
			}

		}

		ht->num_buckets_used_--;
		return 0;
	}

	return 1;
}

static inline int hash_map_robin_hood_back_shift_indice_inicio(
		hm_rr_bs_tabla *ht) {
	return ht->probing_min_;
}

static inline int hash_map_robin_hood_back_shift_indice_final(
		hm_rr_bs_tabla *ht) {
	return ht->probing_max_;
}

static inline bool hash_map_robin_hood_back_shift_indice_existe(
		hm_rr_bs_tabla *ht, hm_iter indice) {
	return !!ht->buckets_[indice].entry;
}

static inline entero_largo hash_map_robin_hood_back_shift_indice_obten_llave(
		hm_rr_bs_tabla *ht, hm_iter indice) {
	assert_timeout(indice <= ht->probing_max_ && indice >= ht->probing_min_);
	hm_entry *entrada = ht->buckets_[indice].entry;
	assert_timeout(entrada);
	return entrada->llave;
}

static inline void hash_map_robin_hood_back_shift_indice_pon_valor(
		hm_rr_bs_tabla *ht, hm_iter indice, entero_largo valor) {
	assert_timeout(indice <= ht->probing_max_ && indice >= ht->probing_min_);
	hm_entry *entrada = ht->buckets_[indice].entry;
	assert_timeout(entrada);
	entrada->valor = valor;
}

int hash_map_robin_hood_back_shift_indice_borra(hm_rr_bs_tabla *ht,
		hm_iter indice) {

	assert_timeout(indice <= ht->probing_max_ && indice >= ht->probing_min_);
	uint64_t num_cubetas = ht->num_buckets_;
	uint64_t prob_max = ht->probing_max_;
	uint64_t prob_min = ht->probing_max_;

	uint64_t index_current = indice;
	hm_entry *entrada = ht->buckets_[index_current].entry;

	assert_timeout(entrada);

	free(entrada);

	uint64_t i = 1;
	uint64_t index_previous = 0, index_swap = 0;

	for (i = 1; i < num_cubetas; i++) {
		index_previous = (index_current + i - 1) % num_cubetas;
		index_swap = (index_current + i) % num_cubetas;

		hm_cubeta *cubeta_swap = ht->buckets_ + index_swap;
		hm_cubeta *cubeta_previous = ht->buckets_ + index_previous;

		if (cubeta_swap->entry == NULL ) {
			cubeta_previous->entry = NULL;
			break;
		}
		uint64_t distance;
		if (hash_map_robin_hood_back_shift_llena_distancia_a_indice_inicio(ht,
						index_swap, &distance) != 0) {
			fprintf(stderr, "Error in FillDistanceToInitIndex()");
		}
		if (!distance) {
			cubeta_previous->entry = NULL;
			break;
		}
		cubeta_previous->entry = cubeta_swap->entry;
		cubeta_previous->hash = cubeta_swap->hash;
	}
	if (i < num_cubetas) {
		if (index_previous == prob_min) {
			prob_min++;
			if (prob_min >= num_cubetas) {
				prob_min = 0;
			} else {
				while (prob_min < num_cubetas && ht->buckets_[prob_min].entry) {
					prob_min++;
				}
				if (prob_min >= num_cubetas) {
					prob_min = num_cubetas;
				}
			}

			ht->probing_min_ = prob_min;
		}

		if (index_previous == prob_max) {
			prob_max--;
			if (prob_max >= num_cubetas) {
				prob_max = num_cubetas;
			} else {
				while (((int64_t) prob_max) >= 0 && ht->buckets_[prob_max].entry) {
					prob_max--;
				}
				if (prob_max >= num_cubetas) {
					prob_max = 0;
				}
			}

			ht->probing_max_ = prob_max;
		}

	}

	ht->num_buckets_used_--;
	return 0;

}

static inline entero_largo hash_map_robin_hood_back_shift_indice_obten_valor(
		hm_rr_bs_tabla *ht, hm_iter indice) {
	assert_timeout(indice <= ht->probing_max_ && indice >= ht->probing_min_);
	hm_entry *entrada = ht->buckets_[indice].entry;
	assert_timeout(entrada);
	return entrada->valor;
}

static inline bool hash_map_robin_hood_back_shift_esta_vacio(hm_rr_bs_tabla *ht) {
	assert_timeout(ht->num_buckets_used_ <= ht->num_buckets_);
	return !ht->num_buckets_used_;
}

#endif

#if 0

//http://www.cprogramming.com/snippets/source-code/singly-linked-list-insert-remove-add-count

typedef struct node {
	natural data;
	struct node *next;
}nodo_lista;

typedef struct lista_pendeja {
	nodo_lista *cabeza;
	nodo_lista *cola;
	nodo_lista cabeza_mem;
}lista_pendeja;

nodo_lista *nodos_mem = NULL;
natural nodos_mem_utilizados = 0;

void listilla_insert(lista_pendeja *lista, natural num) {
	nodo_lista *temp;
	temp = nodos_mem + nodos_mem_utilizados++;
	temp->data = num;
	temp->next = NULL;
	/*
	 if (lista->cabeza == NULL ) {
	 lista->cola = lista->cabeza = temp;
	 } else
	 */
	assert_timeout(!lista->cola->next);
	lista->cola->next = temp;
	lista->cola = temp;
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

#define CACA_COMUN_MAPA_BITCH_AJUSTE_ENTERO 2147483648LL
#define caca_comun_mapa_bitch_ajusta_numero(num) (CACA_COMUN_MAPA_BITCH_AJUSTE_ENTERO+num)

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

void caca_x_anade_caca(tipo_dato numero);
void caca_x_quita_caca(tipo_dato numero);

#if 1

typedef unsigned long long bitch_vector;

#define BITCH_MAX_NUMEROS_AGREGADOS MAX_NUMEROS
#define BITCH_TAM_MAPA ( (CACA_X_MAX_VALORES_INT / (sizeof(bitch_vector) * 8)) + 1)

entero_largo_sin_signo bitch_numeros_agregados[BITCH_MAX_NUMEROS_AGREGADOS] = {
		0 };
natural bitch_numeros_agregados_tam = 0;
bitch_vector *bitch_mapa = NULL;

static inline void bitch_init() {
	bitch_mapa = calloc(BITCH_TAM_MAPA, sizeof(bitch_vector));
	assert_timeout(bitch_mapa);
}

#define bitch_checa(bits, posicion, resultado) \
        __asm__ (\
                        "xor %%rdx,%%rdx\n"\
                        "movq %[bitch_posi],%%rax\n"\
                        "movq $64,%%r8\n"\
                        "divq %%r8\n"\
                        "mov $1,%[resul]\n"\
                        "mov %%rdx,%%rcx\n"\
                        "shll %%cl,%[resul]\n"\
                        "and (%[vectors],%%rax,8),%[resul]\n"\
                        : [resul] "=b" (resultado)\
                        : [bitch_posi] "r" (posicion), [vectors] "r" (bits)\
            :"rax","rdx","rcx","r8")

static inline void bitch_asigna(bitch_vector *bits,
		entero_largo_sin_signo posicion) {
	natural idx_arreglo = 0;
	natural idx_registro = 0;

	idx_arreglo = posicion / 64;
	idx_registro = posicion % 64;

	bits[idx_arreglo] |= (bitch_vector) ((bitch_vector) 1 << idx_registro);

	bitch_numeros_agregados[bitch_numeros_agregados_tam++] = posicion;
}

static inline void bitch_limpia(bitch_vector *bits,
		entero_largo_sin_signo posicion) {
	int idx_arreglo = 0;
	int idx_registro = 0;

	idx_arreglo = posicion / 64;
	idx_registro = posicion % 64;

	bits[idx_arreglo] &= (bitch_vector) ~((bitch_vector) 1 << idx_registro);
}

static inline void bitch_limpia_todo() {
	for (natural i = 0; i < bitch_numeros_agregados_tam; i++) {
		bool encendido = falso;
		entero_largo_sin_signo num_actual = bitch_numeros_agregados[i];

		bitch_checa(bitch_mapa, num_actual, encendido);
		assert_timeout(encendido);
		bitch_limpia(bitch_mapa, num_actual);
	}

	bitch_numeros_agregados_tam = 0;
}

#endif

#if 1
typedef enum mo_mada_tipo_query {
	mo_mada_actualizacion = 'U', mo_mada_consulta = 'Q'
} mo_mada_tipo_query;

typedef struct mo_mada {
	mo_mada_tipo_query tipo;
	natural idx_query;
	natural intervalo_idx_ini;
	natural intervalo_idx_fin;
	natural orden;
	entero_largo resulcaca;
} mo_mada;

natural mo_mada_tam_bloque = 0;
entero_largo mo_mada_resultado = 0;

int mo_mada_ord_bloque(const void *pa, const void *pb) {
	int res = 0;
	mo_mada *a = (mo_mada *) pa;
	mo_mada *b = (mo_mada *) pb;
	natural idx_bloque_a = a->intervalo_idx_ini / mo_mada_tam_bloque;
	natural idx_bloque_b = b->intervalo_idx_ini / mo_mada_tam_bloque;
	natural idx_fin_a = a->intervalo_idx_fin;
	natural idx_fin_b = b->intervalo_idx_fin;
	natural idx_ini_a = a->intervalo_idx_ini;
	natural idx_ini_b = b->intervalo_idx_ini;

	if (idx_bloque_a != idx_bloque_b) {
		res = idx_bloque_a - idx_bloque_b;
	} else {
		if (idx_fin_a != idx_fin_b) {
			res = idx_fin_a - idx_fin_b;
		} else {
			if (idx_ini_a != idx_ini_b) {
				res = idx_ini_a - idx_ini_b;
			} else {
				res = a->orden - b->orden;
			}
		}
	}

	return res;
}

int mo_mada_ord_idx_query(const void *pa, const void *pb) {
	int res = 0;
	mo_mada *a = (mo_mada *) pa;
	mo_mada *b = (mo_mada *) pb;

	res = a->idx_query - b->idx_query;
	return res;
}

#define mo_mada_fn_ord_mo mo_mada_ord_bloque
#define mo_mada_fn_ord_idx mo_mada_ord_idx_query
#define mo_mada_fn_anade_caca caca_x_anade_caca
#define mo_mada_fn_quita_caca caca_x_quita_caca

static inline mo_mada *mo_mada_core(mo_mada *consultas, tipo_dato *numeros,
		natural num_consultas, natural num_numeros) {
	natural idx_izq_act = 0;
	natural idx_der_act = 0;
	mo_mada_tam_bloque = ceil(sqrt(num_numeros));
	caca_log_debug("total de nums %u, tam bloq %u\n", num_numeros,
			mo_mada_tam_bloque);

	qsort(consultas, num_consultas, sizeof(mo_mada), mo_mada_fn_ord_mo);

	idx_izq_act = idx_der_act = (consultas)->intervalo_idx_ini;

	caca_log_debug("anadiendo inicialmente %u\n", numeros[idx_izq_act]);
	mo_mada_fn_anade_caca((numeros[idx_izq_act]));

	caca_log_debug("puta mierda %lld\n", mo_mada_resultado);

	for (int i = 0; i < num_consultas; i++) {
		natural consul_idx_izq = (consultas + i)->intervalo_idx_ini;
		natural consul_idx_der = (consultas + i)->intervalo_idx_fin;

		if ((consultas + i)->tipo == mo_mada_actualizacion) {
			continue;
		}

#if 0
		assert_timeout(
				ceil(abs((int )idx_izq_act - (int )consul_idx_izq))
				<= mo_mada_tam_bloque * 2);
#endif

		caca_log_debug("vamos a bailar %u-%u\n", consul_idx_izq,
				consul_idx_der);

		caca_log_debug("disminu izq act %u a izq consul %u\n", idx_izq_act,
				consul_idx_izq);
		while (idx_izq_act > consul_idx_izq) {
			idx_izq_act--;
			mo_mada_fn_anade_caca(numeros[idx_izq_act]);
			caca_log_debug("puta mierda %lld\n", mo_mada_resultado);
		}

		caca_log_debug("aumen der act %u a der consul %u\n", idx_der_act,
				consul_idx_der);
		while (idx_der_act < consul_idx_der) {
			idx_der_act++;
			mo_mada_fn_anade_caca(numeros[idx_der_act]);
			caca_log_debug("puta mierda %lld\n", mo_mada_resultado);
		}

		caca_log_debug("aumen izq act %u a izq consul %u\n", idx_izq_act,
				consul_idx_izq);
		while (idx_izq_act < consul_idx_izq) {
			mo_mada_fn_quita_caca(numeros[idx_izq_act]);
			idx_izq_act++;
		}

		caca_log_debug("disminu der act %u a der consul %u\n", idx_der_act,
				consul_idx_der);
		while (idx_der_act > consul_idx_der) {
			mo_mada_fn_quita_caca(numeros[idx_der_act]);
			idx_der_act--;
		}

		caca_log_debug("el conteo uniq de la consul %u es %lld\n", i,
				mo_mada_resultado);
		(consultas + i)->resulcaca = mo_mada_resultado;
	}

	qsort(consultas, num_consultas, sizeof(mo_mada), mo_mada_fn_ord_idx);
	return consultas;
}

#endif

#if 1

#define TROZO_TREE_MAX_NUMEROS (MAX_NUMEROS+2)
#define TROZO_TREE_MAX_NODOS (((TROZO_TREE_MAX_NUMEROS<<1)+2)*100)
#define TROZO_TREE_MAX_VALOR INT_MAX

typedef struct trozo_tree {
	struct trozo_tree *hijo_izq;
	struct trozo_tree *hijo_der;
	tipo_dato valor;
} trozo_tree;

trozo_tree nodos_trozo[TROZO_TREE_MAX_NODOS] = { 0 };
natural nodos_trozo_usados = 0;
tipo_dato *trozo_tree_numeros = NULL;
//tipo_dato trozo_tree_numeros[TROZO_TREE_MAX_NUMEROS] = { 0 };
natural trozo_tree_numeros_tam = 0;
natural trozo_tree_idx_ini_buscado = 0;
natural trozo_tree_idx_fin_buscado = 0;
natural trozo_tree_idx_a_actualizar = 0;

static inline trozo_tree *trozo_tree_nuevo_nodo() {
	trozo_tree *nodo_nuevo = (nodos_trozo + (nodos_trozo_usados++));
	return nodo_nuevo;
}

static inline void trozo_tree_contruye(trozo_tree **nodo_actual,
		natural idx_ini, natural idx_fin) {
	trozo_tree *nodo_actual_int = NULL;
	if (!*nodo_actual) {
		*nodo_actual = trozo_tree_nuevo_nodo();
	}
	nodo_actual_int = *nodo_actual;
	if (idx_ini != idx_fin) {
		natural idx_mid = idx_ini + ((idx_fin - idx_ini) >> 1);

		trozo_tree_contruye(&nodo_actual_int->hijo_izq, idx_ini, idx_mid);
		trozo_tree_contruye(&nodo_actual_int->hijo_der, idx_mid + 1, idx_fin);

		nodo_actual_int->valor = caca_comun_min(
				nodo_actual_int->hijo_izq->valor,
				nodo_actual_int->hijo_der->valor);
	} else {
		nodo_actual_int->valor = trozo_tree_numeros[idx_ini];
	}
}

static inline void trozo_tree_actualiza(trozo_tree **nodo_actual,
		natural idx_ini, natural idx_fin, tipo_dato nuevo_valor) {
	natural idx_mid = idx_ini + ((idx_fin - idx_ini) >> 1);
	trozo_tree *nodo_actual_int = NULL;
	if (!*nodo_actual) {
		*nodo_actual = trozo_tree_nuevo_nodo();
	}
	nodo_actual_int = *nodo_actual;
	if (idx_ini == idx_fin) {
		assert_timeout(idx_ini == idx_fin);
		assert_timeout(trozo_tree_idx_a_actualizar == idx_fin);
		nodo_actual_int->valor += nuevo_valor;
		return;
	}
	if (trozo_tree_idx_a_actualizar <= idx_mid) {
		trozo_tree_actualiza(&nodo_actual_int->hijo_izq, idx_ini, idx_mid,
				nuevo_valor);
	} else {
		trozo_tree_actualiza(&nodo_actual_int->hijo_der, idx_mid + 1, idx_fin,
				nuevo_valor);
	}

	nodo_actual_int->valor = 0;
	if (nodo_actual_int->hijo_izq) {
		nodo_actual_int->valor += nodo_actual_int->hijo_izq->valor;
	} else {
		nodo_actual_int->valor += nodo_actual_int->hijo_der->valor;
	}
}

static inline entero_largo trozo_tree_consulta(trozo_tree *nodo_actual,
		natural idx_ini, natural idx_fin) {
	tipo_dato resul = 0;

	if (trozo_tree_idx_ini_buscado > idx_fin) {
		resul = 0;
	} else {
		if (trozo_tree_idx_fin_buscado < idx_ini) {
			resul = 0;
		} else {
			if (!nodo_actual) {
				resul = 0;
			} else {
				if (idx_ini >= trozo_tree_idx_ini_buscado
						&& idx_fin <= trozo_tree_idx_fin_buscado) {
					resul = nodo_actual->valor;
				} else {
					natural idx_mid = idx_ini + ((idx_fin - idx_ini) >> 1);
					entero_largo resul_izq = 0;
					entero_largo resul_der = 0;

					resul_izq = trozo_tree_consulta(nodo_actual->hijo_izq,
							idx_ini, idx_mid);
					resul_der = trozo_tree_consulta(nodo_actual->hijo_der,
							idx_mid + 1, idx_fin);

					resul = resul_izq + resul_der;
				}
			}
		}
	}
	return resul;
}

#endif

#if 1
#define BIT_CH_VALOR_INVALIDO ULLONG_MAX
#define BIT_CH_MAX_NODOS MAX_NUMEROS

typedef struct bit_ch {
	natural num_nodos_bit_ch;
//	tipo_dato nodos_bit_ch[BIT_CH_MAX_NODOS + 2];
	trozo_tree *nodos_bit_ch[BIT_CH_MAX_NODOS + 2];
} bit_ch;

static inline void bit_ch_init(bit_ch *bit, tipo_dato valor_inicial,
		natural num_cacas, tipo_dato *numeros) {

	bit->num_nodos_bit_ch = num_cacas;
	trozo_tree_numeros = numeros;
}

static inline void bit_ch_actualiza_trozos(bit_ch *bit, natural idx_bit_ch,
		natural idx_a_actualizar_en_trozo, tipo_dato num_nuevo) {
	trozo_tree **trozos = (trozo_tree **) bit->nodos_bit_ch;
	trozo_tree_idx_a_actualizar = idx_a_actualizar_en_trozo;
	for (natural i = idx_bit_ch; i <= bit->num_nodos_bit_ch; i += (i & (-i))) {
		caca_log_debug("actualizando bit idx %u asta pos %u con %d\n", i,
				idx_a_actualizar_en_trozo, num_nuevo);
		trozo_tree_actualiza(trozos + i, 1, trozo_tree_numeros_tam, num_nuevo);
	}
}

static inline void bit_ch_actualiza(bit_ch *bit, natural ocurrencia_anterior,
		natural ocurrencia_a_actualizar, natural ocurrencia_siguiente,
		tipo_dato num_nuevo) {

	bit_ch_actualiza_trozos(bit, ocurrencia_anterior + 1,
			ocurrencia_a_actualizar, num_nuevo);
	bit_ch_actualiza_trozos(bit, ocurrencia_anterior + 1, ocurrencia_siguiente,
			-num_nuevo);

	bit_ch_actualiza_trozos(bit, ocurrencia_a_actualizar + 1,
			ocurrencia_a_actualizar, -num_nuevo);
	bit_ch_actualiza_trozos(bit, ocurrencia_a_actualizar + 1,
			ocurrencia_siguiente, num_nuevo);
}

static inline entero_largo bit_ch_consulta_trozos(bit_ch *bit,
		natural idx_bit_ch, natural idx_trozo) {
	entero_largo suma = 0;
	trozo_tree **trozos = bit->nodos_bit_ch;
	trozo_tree_idx_ini_buscado = 1;
	trozo_tree_idx_fin_buscado = idx_trozo;
	for (int i = idx_bit_ch; i > 0; i -= (i & (-i))) {
		caca_log_debug("consultando trozo %u\n", i);
		suma += trozo_tree_consulta(trozos[i], 1, trozo_tree_numeros_tam);
	}
	return suma;
}

#if 0
static inline void bit_ch_aumenta(bit_ch *bit, tipo_dato nuevo_valor,
		natural idx) {
	tipo_dato *nodos = bit->nodos_bit_ch;
	caca_log_debug("q verga actualizando %u con %lld", idx, nuevo_valor);
	for (natural i = idx; i <= bit->num_nodos_bit_ch; i += (i & (-i))) {
		caca_log_debug("actualizando caca %u con %lld antes %lld", i,
				nuevo_valor, nodos[i]);
		nodos[i] += nuevo_valor;
		caca_log_debug("actualizado caca %u aora %lld", i, nodos[i]);
	}
}

static inline tipo_dato bit_ch_consulta(bit_ch *bit, natural idx) {
	tipo_dato *nodos = bit->nodos_bit_ch;
	tipo_dato res = 0;
	for (int i = idx; i > 0; i -= (i & (-i))) {
		assert_timeout(i<BIT_CH_MAX_NODOS + 2);
		caca_log_debug("consultando caca %u tiene %lld", i, nodos[i]);
		res += nodos[i];
	}
	caca_log_debug("regresando %lld", res);
	return res;
}

static inline void bit_ch_aumenta_rango(bit_ch *bit, tipo_dato nuevo_valor,
		natural idx_ini, natural idx_fin) {
	caca_log_debug("aumentando de %u a %u con valor %lld", idx_ini, idx_fin,
			nuevo_valor);

	bit_ch_aumenta(bit, nuevo_valor, idx_ini);
	bit_ch_aumenta(bit, -nuevo_valor, idx_fin + 1);
}

static inline void bit_ch_aumenta_rango_consulta_rango(bit_ch *bit_puto,
		bit_ch *bit_aux, natural idx_ini, natural idx_fin,
		tipo_dato nuevo_valor) {

	caca_log_debug("aumentando de %u a %u con valor %lld para consulta rango",
			idx_ini, idx_fin, nuevo_valor);

	bit_ch_aumenta_rango(bit_puto, nuevo_valor, idx_ini, idx_fin);
	caca_log_debug(
			"actualizando para consulta rango inicial %d con %lld (%lld * %u)",
			idx_ini, nuevo_valor * (idx_ini - 1), nuevo_valor, idx_ini - 1);
	bit_ch_aumenta(bit_aux, nuevo_valor * (idx_ini - 1), idx_ini);
	caca_log_debug(
			"actualizando para consulta rango final %d con %lld(%lld,%u)",
			idx_fin + 1, -nuevo_valor * idx_fin, -nuevo_valor, idx_fin);
	bit_ch_aumenta(bit_aux, -nuevo_valor * idx_fin, idx_fin + 1);
}

static inline tipo_dato bit_ch_consulta_rango_actualizado_rango(
		bit_ch *bit_puto, bit_ch *bit_aux, natural idx) {
	tipo_dato resul = 0;
	tipo_dato valor_puto = 0;
	tipo_dato valor_aux = 0;

	valor_puto = bit_ch_consulta(bit_puto, idx);

	caca_log_debug("el valor putual en %u es %lld, aportara %lld (%lld * %u)",
			idx, valor_puto, valor_puto * idx, valor_puto, idx);

	valor_aux = bit_ch_consulta(bit_aux, idx);

	caca_log_debug("el valor aux en %u es %lld", idx, valor_puto);

	resul = valor_puto * idx - valor_aux;

	caca_log_debug("la suma acumulacaca hasta %u es %lld", idx, resul);

	return resul;
}

static inline tipo_dato bit_ch_consulta_rango(bit_ch *bit_puto, bit_ch *bit_aux,
		natural idx_ini, natural idx_fin) {
	tipo_dato resul = 0;
	tipo_dato resul_idx_ini = 0;
	tipo_dato resul_idx_fin = 0;

	resul_idx_ini = bit_ch_consulta_rango_actualizado_rango(bit_puto, bit_aux,
			idx_ini - 1);
	caca_log_debug("la suma acumulacaca ini hasta %u es %lld", idx_ini,
			resul_idx_ini);

	resul_idx_fin = bit_ch_consulta_rango_actualizado_rango(bit_puto, bit_aux,
			idx_fin);
	caca_log_debug("la suma acumulacaca fin hasta %u es %lld", idx_fin,
			resul_idx_fin);

	resul = resul_idx_fin - resul_idx_ini;

	caca_log_debug("la suma acumulacaca del rango %u-%u es %lld", idx_ini,
			idx_fin, resul);

	return resul;
}
#endif

#endif

#if 1

#define AVL_TREE_VALOR_INVALIDO CACA_X_VALOR_INVALIDO
#define AVL_TREE_MAX_RANGO_A_MAPEAR (MAX_VALOR<<1)

struct avl_tree_node_s {
	tipo_dato llave;
	natural altura;
	natural num_decendientes;
	natural indice_en_arreglo;
	natural ocurrencias;
	tipo_dato pasajero_oscuro;
	struct avl_tree_node_s *left;
	struct avl_tree_node_s *right;
	struct avl_tree_node_s *padre;
};

typedef struct avl_tree_node_s avl_tree_node_t;

struct avl_tree_s {
	natural max_nodos;
	natural nodos_realmente_en_arbol;
	natural nodos_usados;
	struct avl_tree_node_s *root;
	avl_tree_node_t *nodos_mem;
	natural *nodos_libres_idx;
	unsigned long siguiente_idx_para_usar;
	unsigned long ultimo_idx_anadido;
//	bitch_vector mapa_recorridos[AVL_TREE_MAX_RANGO_A_MAPEAR / (sizeof(bitch_vector) * 8)];
//	natural numeros_marcados[AVL_TREE_MAX_RANGO_A_MAPEAR];
};

typedef struct avl_tree_s avl_tree_t;

typedef struct avl_tree_iterator_t {
	avl_tree_t *arbolin;
//	char *contador_visitas;
	avl_tree_node_t *nodo_actual;
} avl_tree_iterator_t;

/* Create a new AVL tree. */
avl_tree_t *avl_tree_create(avl_tree_t **arbolin, int max_nodos) {
	avl_tree_t *tree = NULL;

	assert_timeout(arbolin);

	tree = calloc(1, sizeof(avl_tree_t));

	assert_timeout(tree);

	tree->max_nodos = max_nodos;

	tree->nodos_mem = calloc(max_nodos * 2, sizeof(avl_tree_node_t));

	assert_timeout(tree->nodos_mem);

	tree->root = NULL;

	*arbolin = tree;

	tree->nodos_libres_idx = calloc(max_nodos, sizeof(natural));
	memset(tree->nodos_libres_idx, 0xffff, sizeof(natural) * max_nodos);

	assert_timeout(tree->nodos_libres_idx);

	return tree;
}

static inline void avl_tree_destroy(avl_tree_t *arbolin) {
	free(arbolin->nodos_mem);
	free(arbolin->nodos_libres_idx);
	free(arbolin);

}

/* Initialize a new node. */
avl_tree_node_t *avl_tree_create_node(avl_tree_t *arbolin) {
	avl_tree_node_t *node = NULL;

	assert_timeout(
			arbolin->siguiente_idx_para_usar < arbolin->ultimo_idx_anadido
					|| ((arbolin->siguiente_idx_para_usar
							== arbolin->ultimo_idx_anadido)
							&& arbolin->nodos_usados < arbolin->max_nodos));

	if (arbolin->siguiente_idx_para_usar < arbolin->ultimo_idx_anadido) {
		node = arbolin->nodos_mem
				+ arbolin->nodos_libres_idx[arbolin->siguiente_idx_para_usar
						% arbolin->max_nodos];
		node->indice_en_arreglo =
				arbolin->nodos_libres_idx[arbolin->siguiente_idx_para_usar
						% arbolin->max_nodos];

		arbolin->nodos_libres_idx[arbolin->siguiente_idx_para_usar
				% arbolin->max_nodos] = 0xffffffff;
		arbolin->siguiente_idx_para_usar++;
	} else {
		node = arbolin->nodos_mem + arbolin->nodos_usados++;
		node->indice_en_arreglo = arbolin->nodos_usados - 1;
	}
	arbolin->nodos_realmente_en_arbol++;
	caca_log_debug("aumentando nodos realmente en arbol a %u\n",
			arbolin->nodos_realmente_en_arbol);
	return node;
}

/* Find the height of an AVL no repulsivamente */
int avl_tree_node_height(avl_tree_node_t *node) {
	return node ? node->altura : 0;
}

static inline void avl_tree_node_actualizar_altura(avl_tree_node_t *node) {
	int height_left = 0;
	int height_right = 0;

	if (node->left) {
		height_left = node->left->altura;
	}
	if (node->right) {
		height_right = node->right->altura;
	}
	if (node->left || node->right) {
		node->altura = (height_right > height_left ? height_right : height_left)
				+ 1;
	} else {
		node->altura = 0;
	}
}

static inline void avl_tree_node_actualizar_num_decendientes(
		avl_tree_node_t *node) {
	int conteo_left = 0;
	int conteo_right = 0;

	if (node->left) {
		conteo_left = node->left->num_decendientes;
	}
	if (node->right) {
		conteo_right = node->right->num_decendientes;
	}
	if (node->left || node->right) {
		node->num_decendientes = conteo_left + conteo_right
				+ (node->left ? 1 : 0) + (node->right ? 1 : 0);
	} else {
		node->num_decendientes = 0;
	}
}

/* Left Left Rotate */
avl_tree_node_t *avl_tree_rotate_leftleft(avl_tree_node_t *node) {
	avl_tree_node_t *a = node;
	avl_tree_node_t *b = a->left;
	avl_tree_node_t *padre = NULL;

	padre = node->padre;

	a->left = b->right;
	b->right = a;

	avl_tree_node_actualizar_altura(a);
	avl_tree_node_actualizar_altura(b);
	avl_tree_node_actualizar_num_decendientes(a);
	avl_tree_node_actualizar_num_decendientes(b);

	a->padre = b;
	b->padre = padre;
	if (a->left) {
		a->left->padre = a;
	}

	return (b);
}

/* Left Right Rotate */
avl_tree_node_t *avl_tree_rotate_leftright(avl_tree_node_t *node) {
	avl_tree_node_t *a = node;
	avl_tree_node_t *b = a->left;
	avl_tree_node_t *c = b->right;
	avl_tree_node_t *padre = NULL;

	padre = node->padre;

	a->left = c->right;
	b->right = c->left;
	c->left = b;
	c->right = a;

	avl_tree_node_actualizar_altura(a);
	avl_tree_node_actualizar_altura(b);
	avl_tree_node_actualizar_altura(c);

	avl_tree_node_actualizar_num_decendientes(a);
	avl_tree_node_actualizar_num_decendientes(b);
	avl_tree_node_actualizar_num_decendientes(c);

	a->padre = c;
	b->padre = c;
	c->padre = padre;
	if (a->left) {
		a->left->padre = a;
	}
	if (b->right) {
		b->right->padre = b;
	}

	return (c);
}

/* Right Left Rotate */
avl_tree_node_t *avl_tree_rotate_rightleft(avl_tree_node_t *node) {
	avl_tree_node_t *a = node;
	avl_tree_node_t *b = a->right;
	avl_tree_node_t *c = b->left;
	avl_tree_node_t *padre = NULL;

	padre = node->padre;

	a->right = c->left;
	b->left = c->right;
	c->right = b;
	c->left = a;

	avl_tree_node_actualizar_altura(a);
	avl_tree_node_actualizar_altura(b);
	avl_tree_node_actualizar_altura(c);

	avl_tree_node_actualizar_num_decendientes(a);
	avl_tree_node_actualizar_num_decendientes(b);
	avl_tree_node_actualizar_num_decendientes(c);

	a->padre = c;
	b->padre = c;
	c->padre = padre;
	if (a->right) {
		a->right->padre = a;
	}
	if (b->left) {
		b->left->padre = b;
	}

	return (c);
}

/* Right Right Rotate */
avl_tree_node_t *avl_tree_rotate_rightright(avl_tree_node_t *node) {
	avl_tree_node_t *a = node;
	avl_tree_node_t *b = a->right;
	avl_tree_node_t *padre = NULL;

	padre = node->padre;

	a->right = b->left;
	b->left = a;

	avl_tree_node_actualizar_altura(a);
	avl_tree_node_actualizar_altura(b);

	avl_tree_node_actualizar_num_decendientes(a);
	avl_tree_node_actualizar_num_decendientes(b);

	a->padre = b;
	b->padre = padre;
	if (a->right) {
		a->right->padre = a;
	}

	return (b);
}

/* Find the balance of an AVL node */
int avl_tree_balance_factor(avl_tree_node_t *node) {
	int bf = 0;

	if (node->left)
		bf += avl_tree_node_height(node->left);
	if (node->right)
		bf -= avl_tree_node_height(node->right);

	return bf;
}

static inline avl_tree_node_t *avl_tree_balance_node_insertar(
		const avl_tree_node_t *node, const tipo_dato llave_nueva,
		const tipo_dato pasajero_oscuro) {
	avl_tree_node_t *newroot = NULL;
	avl_tree_node_t *nodo_actual = NULL;

	newroot = (avl_tree_node_t *) node;
	nodo_actual = node->padre;
	while (nodo_actual) {

		int bf = 0;
		avl_tree_node_t *padre = NULL;
		avl_tree_node_t **rama_padre = NULL;

		bf = avl_tree_balance_factor(nodo_actual);

		if (bf >= 2) {
			/* Left Heavy */
			if (llave_nueva > nodo_actual->left->llave) {
				newroot = avl_tree_rotate_leftright(nodo_actual);
			} else {
				if (llave_nueva < nodo_actual->left->llave) {
					newroot = avl_tree_rotate_leftleft(nodo_actual);
				} else {
					if (pasajero_oscuro > nodo_actual->left->pasajero_oscuro) {
						newroot = avl_tree_rotate_leftright(nodo_actual);
					} else {
						newroot = avl_tree_rotate_leftleft(nodo_actual);
					}
				}
			}

		} else if (bf <= -2) {
			/* Right Heavy */
			if (llave_nueva < nodo_actual->right->llave) {
				newroot = avl_tree_rotate_rightleft(nodo_actual);
			} else {
				if (llave_nueva > nodo_actual->right->llave) {
					newroot = avl_tree_rotate_rightright(nodo_actual);
				} else {
					if (pasajero_oscuro < nodo_actual->right->pasajero_oscuro) {
						newroot = avl_tree_rotate_rightleft(nodo_actual);
					} else {
						newroot = avl_tree_rotate_rightright(nodo_actual);
					}
				}
			}

		} else {
			/* This node is balanced -- no change. */
			newroot = nodo_actual;
			avl_tree_node_actualizar_altura(nodo_actual);
		}

		if (newroot->padre) {
			padre = newroot->padre;
			if (llave_nueva < padre->llave) {
				rama_padre = &padre->left;
			} else {
				if (llave_nueva > padre->llave) {
					rama_padre = &padre->right;
				} else {
					if (pasajero_oscuro < padre->pasajero_oscuro) {
						rama_padre = &padre->left;
					} else {
						if (pasajero_oscuro > padre->pasajero_oscuro) {
							rama_padre = &padre->right;
						} else {
							assert_timeout(0);
						}
					}
				}
			}
			*rama_padre = newroot;
		}

		nodo_actual = nodo_actual->padre;
	}

	return (newroot);
}

/* Balance a given tree */
void avl_tree_balance_insertar(avl_tree_t *tree, avl_tree_node_t *nodo,
		tipo_dato llave_nueva, tipo_dato pasajero_oscuro) {

	avl_tree_node_t *newroot = NULL;

	newroot = avl_tree_balance_node_insertar(nodo, llave_nueva,
			pasajero_oscuro);

	if (newroot != tree->root) {
		tree->root = newroot;
	}
}

/* Insert a new node. */
void avl_tree_insert(avl_tree_t *tree, tipo_dato value,
		tipo_dato pasajero_oscuro) {
	avl_tree_node_t *node = NULL;
	avl_tree_node_t *next = NULL;
	avl_tree_node_t *last = NULL;

	/* Well, there must be a first case */
	if (tree->root == NULL ) {
		node = avl_tree_create_node(tree);
		node->llave = value;
		node->pasajero_oscuro = pasajero_oscuro;

		tree->root = node;

		/* Okay.  We have a root already.  Where do we put this? */
	} else {
		next = tree->root;

		while (next != NULL ) {
			last = next;

			next->num_decendientes++;

			if (value < next->llave) {
				next = next->left;
			} else {
				if (value > next->llave) {
					next = next->right;
				} else {
					if (value == next->llave) {

						if (pasajero_oscuro < next->pasajero_oscuro) {
							next = next->left;
						} else {
							if (pasajero_oscuro > next->pasajero_oscuro) {
								next = next->right;
							} else {
								avl_tree_node_t *ancestro_actal = NULL;
								/* Have we already inserted this node? */
								next->ocurrencias++;
								caca_log_debug(
										"llave ya existe, aumentando contador a carajo %u\n",
										next->ocurrencias);

								ancestro_actal = next;
								while (ancestro_actal) {
									caca_log_debug("bajando decendientes de %u",
											ancestro_actal->llave);
									ancestro_actal->num_decendientes--;
									ancestro_actal = ancestro_actal->padre;
								}
								return;
							}
						}
					} else {
						caca_log_debug("verga, no es maior menor ni igual\n");
						assert_timeout(0);
					}
				}
			}
		}

		node = avl_tree_create_node(tree);
		node->llave = value;
		node->pasajero_oscuro = pasajero_oscuro;

		if (value < last->llave) {
			last->left = node;
		} else {
			if (value > last->llave) {
				last->right = node;
			} else {
				if (pasajero_oscuro < last->pasajero_oscuro) {
					last->left = node;
				} else {
					last->right = node;
				}
			}
		}

		node->padre = last;

	}
	node->ocurrencias = 1;

	avl_tree_balance_insertar(tree, node, value, pasajero_oscuro);
}

static inline bool avl_tree_es_hijo_perra(avl_tree_node_t *nodo) {
	bool es_hijo_perra = falso;

	caca_log_debug("tonig padre %p nodo left %p nodo act %p\n", nodo->padre,
			nodo->padre ? nodo->padre->left : NULL, nodo);
	if (nodo->padre) {
		caca_log_debug("tiene padre\n");
		if ((tipo_dato) nodo->padre->left == (tipo_dato) nodo) {
			caca_log_debug("es ijo izq");
			es_hijo_perra = verdadero;
		}
	}

	return es_hijo_perra;
}
/* Find the node containing a given value */
avl_tree_node_t *avl_tree_find(avl_tree_t *tree, tipo_dato value,
		tipo_dato pasajero_oscuro, bool buscar_ant) {
	avl_tree_node_t *current = tree->root;
	avl_tree_node_t *last_of_us = NULL;

	while (current) {
		if (value > current->llave) {
//			last_of_us = current;
			current = current->right;
		} else {
			if (value < current->llave) {
				current = current->left;
			} else {
				if (pasajero_oscuro != AVL_TREE_VALOR_INVALIDO) {
					if (pasajero_oscuro > current->pasajero_oscuro) {
						if (buscar_ant) {
							last_of_us = current;
						}
						current = current->right;
					} else {
						if (pasajero_oscuro < current->pasajero_oscuro) {
							if (!buscar_ant) {
								last_of_us = current;
							}
							current = current->left;
						} else {
							break;
						}
					}
				} else {
					break;
				}
			}
		}
	}
	if (!current) {
		current = last_of_us;
	}

	return current;
}

/* Do a depth first traverse of a node. */
void avl_tree_traverse_node_dfs(avl_tree_node_t *node, int depth) {
	int i = 0;

	if (node->left)
		avl_tree_traverse_node_dfs(node->left, depth + 2);

	for (i = 0; i < depth; i++)
		putchar(' ');
	printf("%d: %d\n", node->llave, avl_tree_balance_factor(node));

	if (node->right)
		avl_tree_traverse_node_dfs(node->right, depth + 2);
}

/* Do a depth first traverse of a tree. */
void avl_tree_traverse_dfs(avl_tree_t *tree) {
	avl_tree_traverse_node_dfs(tree->root, 0);
}

static inline void avl_tree_iterador_ini(avl_tree_t *arbolin,
		avl_tree_iterator_t *iter) {
//	iter->contador_visitas = calloc(arbolin->max_nodos, sizeof(char));
//	assert_timeout(iter->contador_visitas);
	iter->arbolin = arbolin;
}

static inline void avl_tree_iterador_fini(avl_tree_iterator_t *iter) {
//	free(iter->contador_visitas);
}

static inline avl_tree_node_t* avl_tree_iterador_siguiente(
		avl_tree_iterator_t *iter) {
	bool res_bitch;
	avl_tree_node_t *nodo = NULL;
	avl_tree_node_t *nodo_actual = NULL;

	nodo_actual = iter->nodo_actual;
	if (!nodo_actual) {
		iter->nodo_actual = iter->arbolin->root;
		bitch_checa(bitch_mapa, iter->nodo_actual->indice_en_arreglo,
				res_bitch);
		if (res_bitch) {
			return NULL ;
		}
		bitch_asigna(bitch_mapa, iter->nodo_actual->indice_en_arreglo);
		return iter->nodo_actual;
	}

	if (nodo->right) {
		bitch_checa(bitch_mapa, nodo_actual->right->indice_en_arreglo,
				res_bitch);
	}
	if (nodo_actual->right && !res_bitch) {
		nodo_actual = nodo_actual->right;
		while (nodo_actual->left) {
			nodo_actual = nodo_actual->left;
		}
		nodo = nodo_actual;
		bitch_asigna(bitch_mapa, nodo->indice_en_arreglo);
	} else {
		nodo_actual = nodo_actual->padre;
		if (nodo_actual) {
			bitch_checa(bitch_mapa, nodo_actual->indice_en_arreglo, res_bitch);
		}
		while (nodo_actual && res_bitch) {
			nodo_actual = nodo_actual->padre;
			if (nodo_actual) {
				bitch_checa(bitch_mapa, nodo_actual->indice_en_arreglo,
						res_bitch);
			}
		}
		if (!nodo_actual) {
			nodo = NULL;
		} else {
			nodo = nodo_actual;
			bitch_asigna(bitch_mapa, nodo->indice_en_arreglo);
		}
	}

	iter->nodo_actual = nodo;

	return nodo;
}

static inline avl_tree_node_t* avl_tree_iterador_anterior(
		avl_tree_iterator_t *iter) {
	bool res_bitch;
	avl_tree_node_t *nodo = NULL;
	avl_tree_node_t *nodo_actual = NULL;

	nodo_actual = iter->nodo_actual;
	if (!nodo_actual) {
		iter->nodo_actual = iter->arbolin->root;
		bitch_checa(bitch_mapa, iter->nodo_actual->indice_en_arreglo,
				res_bitch);
		if (res_bitch) {
			return NULL ;
		}
		bitch_asigna(bitch_mapa, iter->nodo_actual->indice_en_arreglo);
		return iter->nodo_actual;
	}

	if (nodo_actual->left) {
		bitch_checa(bitch_mapa, nodo_actual->left->indice_en_arreglo,
				res_bitch);
	}
	if (nodo_actual->left && !res_bitch) {
		nodo_actual = nodo_actual->left;
		while (nodo_actual->right) {
			nodo_actual = nodo_actual->right;
		}
		nodo = nodo_actual;
		bitch_asigna(bitch_mapa, nodo->indice_en_arreglo);
	} else {
		nodo_actual = nodo_actual->padre;
		if (nodo_actual) {
			bitch_checa(bitch_mapa, nodo_actual->indice_en_arreglo, res_bitch);
		}
		while (nodo_actual && res_bitch) {
			nodo_actual = nodo_actual->padre;
			if (nodo_actual) {
				bitch_checa(bitch_mapa, nodo_actual->indice_en_arreglo,
						res_bitch);
			}
		}
		if (!nodo_actual) {
			nodo = NULL;
		} else {
			nodo = nodo_actual;
			bitch_asigna(bitch_mapa, nodo->indice_en_arreglo);
		}
	}

	iter->nodo_actual = nodo;

	return nodo;
}

static inline avl_tree_node_t* avl_tree_iterador_obtener_actual(
		avl_tree_iterator_t *iter) {
	avl_tree_node_t *nodo = NULL;

	nodo = iter->nodo_actual;

	return nodo;

}

static inline char *avl_tree_inoder_node_travesti(avl_tree_node_t *nodo,
		char *buf, int profundidad_maxima) {
	char num_buf[100] = { '\0' };
	int profundidad = 0;
	int i = 0;

	assert_timeout(profundidad_maxima == -1 || profundidad != -1);

	if (nodo != NULL ) {
		profundidad = profundidad_maxima - nodo->altura;

		assert_timeout(!nodo->right || nodo->right->padre == nodo);
		avl_tree_inoder_node_travesti(nodo->right, buf, profundidad_maxima);

		if (profundidad_maxima != -1) {
			for (i = 0; i < profundidad; i++) {
				strcat(buf, " ");
			}
		}
		sprintf(num_buf, "%d", nodo->llave);
		strcat(buf, num_buf);
		if (profundidad_maxima != -1) {
			strcat(buf, "\n");
		}

		assert_timeout(!nodo->left || nodo->left->padre == nodo);
		avl_tree_inoder_node_travesti(nodo->left, buf, profundidad_maxima);

		/*
		 if (profundidad_maxima != -1) {
		 strcat(buf, "\n");
		 for (int i = 0; i <= profundidad; i++) {
		 strcat(buf, " ");
		 }
		 }
		 */
	}
	return buf;
}

static inline char* avl_tree_sprint(avl_tree_t *arbolini, char *buf) {
	avl_tree_inoder_node_travesti(arbolini->root, buf, -1);
	return buf;
}

static inline char *avl_tree_inoder_node_travesti_conteo(avl_tree_node_t *nodo,
		char *buf, int profundidad_maxima) {
	char num_buf[100] = { '\0' };
	int profundidad = 0;
	int i = 0;

	assert_timeout(profundidad_maxima == -1 || profundidad != -1);

	if (nodo != NULL ) {
		profundidad = profundidad_maxima - nodo->altura;

		assert_timeout(!nodo->right || nodo->right->padre == nodo);
		avl_tree_inoder_node_travesti_conteo(nodo->right, buf,
				profundidad_maxima);

		if (profundidad_maxima != -1) {
			for (i = 0; i < profundidad; i++) {
				strcat(buf, " ");
			}
		}
		sprintf(num_buf, "%d [%u,%u] (%u) ocu %u", nodo->llave,
				(natural ) (nodo->llave), (natural ) nodo->llave,
				nodo->num_decendientes, nodo->ocurrencias);
		strcat(buf, num_buf);
		if (profundidad_maxima != -1) {
			strcat(buf, "\n");
		}

		assert_timeout(!nodo->left || nodo->left->padre == nodo);
		avl_tree_inoder_node_travesti_conteo(nodo->left, buf,
				profundidad_maxima);

		/*
		 if (profundidad_maxima != -1) {
		 strcat(buf, "\n");
		 for (int i = 0; i <= profundidad; i++) {
		 strcat(buf, " ");
		 }
		 }
		 */
	}
	return buf;
}

static inline char* avl_tree_sprint_identado(avl_tree_t *arbolini, char *buf) {
	int profundidad_maxima = 0;

	*buf = '\0';

	if (!arbolini->root) {
		return buf;
	}

	profundidad_maxima = arbolini->root->altura;
	avl_tree_inoder_node_travesti_conteo(arbolini->root, buf,
			profundidad_maxima);
	return buf;
}

static inline avl_tree_node_t* avl_tree_iterador_asignar_actual(
		avl_tree_iterator_t *iter, avl_tree_node_t *nodo) {
	bool res_bitch;

	bitch_checa(bitch_mapa, nodo->indice_en_arreglo, res_bitch);
	assert_timeout(!res_bitch);
	iter->nodo_actual = nodo;

	bitch_asigna(bitch_mapa, iter->nodo_actual->indice_en_arreglo);
	return iter->nodo_actual;
}

static inline avl_tree_node_t* avl_tree_max_min(avl_tree_t *arbolin, bool max) {
	avl_tree_node_t *nodo_actual = NULL;
	avl_tree_node_t *last_of_us = NULL;

	nodo_actual = last_of_us = arbolin->root;

	while (nodo_actual) {
		last_of_us = nodo_actual;
		if (max) {
			nodo_actual = nodo_actual->right;
		} else {
			nodo_actual = nodo_actual->left;
		}
	}

	return last_of_us;
}

static inline void avl_tree_validar_arbolin_indices(avl_tree_t *arbolin,
		avl_tree_node_t *nodo) {
	if (nodo != NULL ) {
		assert_timeout(&arbolin->nodos_mem[nodo->indice_en_arreglo] == nodo);
		assert_timeout(!nodo->left || nodo->left->padre == nodo);
		avl_tree_validar_arbolin_indices(arbolin, nodo->left);
		assert_timeout(!nodo->right || nodo->right->padre == nodo);
		avl_tree_validar_arbolin_indices(arbolin, nodo->right);
	}
}

static inline void avl_tree_validar_orden(avl_tree_t *arbolin) {
	avl_tree_iterator_t *iter = &(avl_tree_iterator_t ) { 0 };
	avl_tree_node_t *nodo_inicial = NULL;
	avl_tree_node_t *nodo_actual = NULL;
	tipo_dato llave_ant = 0;
	tipo_dato pasajero_oscuro_ant = 0;
	if (!arbolin->root) {
		return;
	}

	nodo_inicial = avl_tree_max_min(arbolin, falso);

	avl_tree_iterador_ini(arbolin, iter);
	avl_tree_iterador_asignar_actual(iter, nodo_inicial);

	llave_ant = nodo_inicial->llave;
	pasajero_oscuro_ant = nodo_inicial->pasajero_oscuro;

	while ((nodo_actual = avl_tree_iterador_siguiente(iter))) {
		assert_timeout(llave_ant <= nodo_actual->llave);
		assert_timeout(
				llave_ant < nodo_actual->llave
						|| pasajero_oscuro_ant <= nodo_actual->pasajero_oscuro);
		llave_ant = nodo_actual->llave;
		pasajero_oscuro_ant = nodo_actual->pasajero_oscuro;
	}

	avl_tree_iterador_fini(iter);
}

static inline void avl_tree_validar_alv(avl_tree_t *arbolin) {
	avl_tree_validar_arbolin_indices(arbolin, arbolin->root);
	avl_tree_validar_orden(arbolin);
}

/* Balance a given node */
/* Given a non-empty binary search tree, return the node with minimum
 key value found in that tree. Note that the entire tree does not
 need to be searched. */
static inline avl_tree_node_t* avl_tree_siguiente_nodo_inorder(
		avl_tree_node_t *node) {
	avl_tree_node_t *current = node;

	/* loop down to find the leftmost leaf */
	while (current->left != NULL ) {
		current = current->left;
	}

	return current;
}

static inline avl_tree_node_t *avl_tree_nodo_borrar(avl_tree_t *arbolini,
		avl_tree_node_t *root, tipo_dato key, bool ignora_conteo,
		tipo_dato pasajero_oscuro) {

	if (root == NULL ) {
		return root;
	}

	if (key < root->llave) {
		root->left = avl_tree_nodo_borrar(arbolini, root->left, key,
				ignora_conteo, pasajero_oscuro);
		assert_timeout(!root->left || root->left->padre == root);
	} else {
		if (key > root->llave) {
			root->right = avl_tree_nodo_borrar(arbolini, root->right, key,
					ignora_conteo, pasajero_oscuro);
			assert_timeout(!root->right || root->right->padre == root);
		} else {
			if (pasajero_oscuro == AVL_TREE_VALOR_INVALIDO) {
				if ((root->ocurrencias - 1) == 0 || ignora_conteo) {
					if (root->left == NULL || root->right == NULL ) {
						caca_log_debug("el nodo q si c va ALV %d %u",
								root->llave, root->pasajero_oscuro);
						avl_tree_node_t *temp =
								root->left ? root->left : root->right;

						if (temp == NULL ) {
							temp = root;
							root = NULL;
						} else {
							natural idx_en_arreglo = 0;
							avl_tree_node_t *padre = NULL;

							padre = root->padre;
							idx_en_arreglo = root->indice_en_arreglo;
							*root = *temp;
							root->padre = padre;
							root->indice_en_arreglo = idx_en_arreglo;
							if (root->left) {
								root->left->padre = root;
							}
							if (root->right) {
								root->right->padre = root;
							}
						}

						assert_timeout(
								arbolini->ultimo_idx_anadido
										- arbolini->siguiente_idx_para_usar
										< arbolini->max_nodos);
						arbolini->nodos_libres_idx[arbolini->ultimo_idx_anadido++
								% arbolini->max_nodos] =
								temp->indice_en_arreglo;
						memset(temp, 0, sizeof(avl_tree_node_t));
						temp->llave = AVL_TREE_VALOR_INVALIDO;
						temp->pasajero_oscuro = AVL_TREE_VALOR_INVALIDO;
						arbolini->nodos_realmente_en_arbol--;
						caca_log_debug(
								"disminuiendo nodos realmente en arbol a %u",
								arbolini->nodos_realmente_en_arbol);

					} else {
						caca_log_debug("el nodo q c va ALV cpm des,adre %d %u",
								root->llave, root->pasajero_oscuro);
						avl_tree_node_t *temp = avl_tree_siguiente_nodo_inorder(
								root->right);

						root->llave = temp->llave;
						root->pasajero_oscuro = temp->pasajero_oscuro;
						root->ocurrencias = temp->ocurrencias;

						root->right = avl_tree_nodo_borrar(arbolini,
								root->right, temp->llave, verdadero,
								temp->pasajero_oscuro);
					}
				} else {
					root->ocurrencias--;
					return root;
				}
			} else {
				if (pasajero_oscuro < root->pasajero_oscuro) {
					root->left = avl_tree_nodo_borrar(arbolini, root->left, key,
							ignora_conteo, pasajero_oscuro);
					assert_timeout(!root->left || root->left->padre == root);
				} else {
					if (pasajero_oscuro > root->pasajero_oscuro) {
						root->right = avl_tree_nodo_borrar(arbolini,
								root->right, key, ignora_conteo,
								pasajero_oscuro);
						assert_timeout(
								!root->right || root->right->padre == root);
					} else {

						if ((root->ocurrencias - 1) == 0 || ignora_conteo) {
							if (root->left == NULL || root->right == NULL ) {
								caca_log_debug("el nodo q si c va ALV %d %u",
										root->llave, root->pasajero_oscuro);
								avl_tree_node_t *temp =
										root->left ? root->left : root->right;

								if (temp == NULL ) {
									temp = root;
									root = NULL;
								} else {
									natural idx_en_arreglo = 0;
									avl_tree_node_t *padre = NULL;

									padre = root->padre;
									idx_en_arreglo = root->indice_en_arreglo;
									*root = *temp;
									root->padre = padre;
									root->indice_en_arreglo = idx_en_arreglo;
									if (root->left) {
										root->left->padre = root;
									}
									if (root->right) {
										root->right->padre = root;
									}
								}

								assert_timeout(
										arbolini->ultimo_idx_anadido
												- arbolini->siguiente_idx_para_usar
												< arbolini->max_nodos);
								arbolini->nodos_libres_idx[arbolini->ultimo_idx_anadido++
										% arbolini->max_nodos] =
										temp->indice_en_arreglo;
								memset(temp, 0, sizeof(avl_tree_node_t));
								temp->llave = AVL_TREE_VALOR_INVALIDO;
								arbolini->nodos_realmente_en_arbol--;
								caca_log_debug(
										"disminuiendo nodos realmente en arbol a %u",
										arbolini->nodos_realmente_en_arbol);

							} else {
								caca_log_debug(
										"el nodo q c va ALV con desmadre %d %u",
										root->llave, root->pasajero_oscuro);
								avl_tree_node_t *temp =
										avl_tree_siguiente_nodo_inorder(
												root->right);

								root->llave = temp->llave;
								root->pasajero_oscuro = temp->pasajero_oscuro;
								root->ocurrencias = temp->ocurrencias;

								root->right = avl_tree_nodo_borrar(arbolini,
										root->right, temp->llave, verdadero,
										temp->pasajero_oscuro);
							}

						} else {
							root->ocurrencias--;
							return root;
						}
					}
				}
			}
		}
	}

	if (root == NULL ) {
		return root;
	}

	avl_tree_node_actualizar_altura(root);
	avl_tree_node_actualizar_num_decendientes(root);

	int balance = avl_tree_balance_factor(root);

	if (balance > 1 && avl_tree_balance_factor(root->left) >= 0) {
		return avl_tree_rotate_leftleft(root);
	}

	if (balance > 1 && avl_tree_balance_factor(root->left) < 0) {
		return avl_tree_rotate_leftright(root);
	}

	if (balance < -1 && avl_tree_balance_factor(root->right) <= 0) {
		return avl_tree_rotate_rightright(root);
	}

	if (balance < -1 && avl_tree_balance_factor(root->right) > 0) {
		return avl_tree_rotate_rightleft(root);
	}

	return root;
}

void avl_tree_borrar(avl_tree_t *tree, tipo_dato value,
		tipo_dato pasajero_oscuro) {

	avl_tree_node_t *newroot = NULL;
	caca_log_debug("borrando valor %d", value);

	if (!tree->root) {
		return;
	}
	newroot = avl_tree_nodo_borrar(tree, tree->root, value, falso,
			pasajero_oscuro);

	if (newroot != tree->root) {
		tree->root = newroot;
	}
}

static inline avl_tree_node_t* avl_tree_nodo_posicion_anterior(
		avl_tree_t *arbolin, avl_tree_node_t *nodo_actual) {
	avl_tree_node_t *nodo_sig;

	nodo_sig = nodo_actual;

	if (nodo_sig->left) {
		nodo_sig = nodo_sig->left;
		while (nodo_sig->right) {
			nodo_sig = nodo_sig->right;
		}
	} else {
		avl_tree_node_t *last_of_us = nodo_sig;
		nodo_sig = nodo_sig->padre;
		while (nodo_sig) {
			if (!avl_tree_es_hijo_perra(last_of_us)) {
				break;
			}
			last_of_us = nodo_sig;
			nodo_sig = nodo_sig->padre;
		}
	}

	return nodo_sig;
}

static inline avl_tree_node_t* avl_tree_nodo_posicion_siguiente(
		avl_tree_t *arbolin, avl_tree_node_t *nodo_actual) {
	avl_tree_node_t *nodo_sig = NULL;

	nodo_sig = nodo_actual;

	if (nodo_sig->right) {
		nodo_sig = nodo_sig->right;
		while (nodo_sig->left) {
			nodo_sig = nodo_sig->left;
		}
	} else {
		avl_tree_node_t *last_of_us = nodo_sig;
		nodo_sig = nodo_sig->padre;
		while (nodo_sig) {
			if (avl_tree_es_hijo_perra(last_of_us)) {
				break;
			}
			last_of_us = nodo_sig;
			nodo_sig = nodo_sig->padre;
		}
	}

	return nodo_sig;
}

#endif

int numeros[MAX_NUMEROS + 2] = { 0 };
natural numeros_tam = 0;
mo_mada consultas[MAX_QUERIES] = { 0 };
natural consultas_tam = 0;
bit_ch *biatch = &(bit_ch ) { 0 };

void caca_x_anade_caca(tipo_dato numero) {
	bool res_bitch = falso;
	bitch_checa(bitch_mapa, ((entero_largo_sin_signo)numero), res_bitch);
	if (!res_bitch) {
		mo_mada_resultado += numero;
		bitch_asigna(bitch_mapa, numero);
	}
}

void caca_x_quita_caca(tipo_dato numero) {
	bool res_bitch = falso;
	bitch_checa(bitch_mapa, ((entero_largo_sin_signo)numero), res_bitch);
	if (res_bitch) {
		mo_mada_resultado -= numero;
		bitch_limpia(bitch_mapa, numero);
	}
}

static inline void caca_x_main() {
	natural num_queries = 0;
	natural cont_queries = 0;
	char tipo_query = 0;
	natural idx_query_ini = 0;
	natural idx_query_fin = 0;
	avl_tree_t *arbolin = NULL;

	char buf[100] = { '\0' };

	bitch_init();

	scanf("%u\n", &numeros_tam);

	lee_matrix_long_stdin(numeros + 1, &(int ) { 1 }, NULL, 1, numeros_tam + 1);

	scanf("%u\n", &num_queries);
	consultas_tam = num_queries;

	caca_log_debug("el num nums %u el num qs %u\n", numeros_tam, consultas_tam);

	caca_log_debug("a vece siento q %d\n", numeros_tam);
	caca_log_debug("as corrido con algo de s %s\n",
			caca_arreglo_a_cadena(numeros, numeros_tam + 1, buf));
	caca_log_debug("en estas paginas %d\n", num_queries);

	while (cont_queries < consultas_tam) {
		mo_mada *consul_act = consultas + cont_queries;
		scanf("%c %d %d\n", &tipo_query, &idx_query_ini, &idx_query_fin);
		if (tipo_query == 'Q' && idx_query_ini > idx_query_fin) {
			tipo_dato tmp = 0;
			tmp = idx_query_fin;
			idx_query_fin = idx_query_ini;
			idx_query_ini = tmp;
		}
		caca_log_debug("q: %c, ini %d, fin %d\n", tipo_query, idx_query_ini,
				idx_query_fin);

		consul_act->idx_query = cont_queries;
		consul_act->intervalo_idx_ini = idx_query_ini;
		consul_act->intervalo_idx_fin = idx_query_fin;
		consul_act->orden = cont_queries;
		consul_act->tipo = tipo_query;

		cont_queries++;
	}

	mo_mada_core(consultas, numeros, consultas_tam, numeros_tam);
#ifdef CACA_X_LOG
	for (int i = 0; i < consultas_tam; i++) {
		caca_log_debug("dancing cat %u-%u da %lld\n",
				consultas[i].intervalo_idx_ini, consultas[i].intervalo_idx_fin,
				consultas[i].resulcaca);
	}
#endif

	avl_tree_create(&arbolin, MAX_NUMEROS);

	for (int i = 1; i <= numeros_tam; i++) {
		tipo_dato numero_act = numeros[i];
		avl_tree_insert(arbolin, numero_act, i);
	}
#ifdef CACA_X_LOG
	for (int i = 1; i <= numeros_tam; i++) {
		tipo_dato numero_act = numeros[i];
		avl_tree_node_t *nodo_caca = NULL;

		caca_log_debug("el inmediato anterior de %u(%u)\n", numero_act, i - 1);
		nodo_caca = avl_tree_find(arbolin, numero_act, i - 1,verdadero);
		if (nodo_caca) {
			caca_log_debug("es %u(%u)\n", nodo_caca->llave,
					nodo_caca->pasajero_oscuro);
		} else {
			caca_log_debug("nada antes\n");
		}

		caca_log_debug("el inmediato posterior de %u(%u)\n", numero_act, i + 1);
		nodo_caca = avl_tree_find(arbolin, numero_act, i + 1,falso);
		if (nodo_caca) {
			caca_log_debug("es %u(%u)\n", nodo_caca->llave,
					nodo_caca->pasajero_oscuro);
		} else {
			caca_log_debug("nada despues\n");
		}
	}
#endif

	bit_ch_init(biatch, 0, numeros_tam + 2, numeros);
	trozo_tree_numeros_tam = numeros_tam + 2;
	for (int i = 0; i < consultas_tam; i++) {
		mo_mada *consul = consultas + i;

		assert_timeout(
				consul->tipo == mo_mada_consulta
						|| consul->tipo == mo_mada_actualizacion);

		if (consul->tipo == mo_mada_actualizacion) {
			natural idx_actualizar = consul->intervalo_idx_ini;
			natural idx_viejo_ant = 0;
			natural idx_viejo_pos = numeros_tam + 1;
			natural idx_nuevo_ant = 0;
			natural idx_nuevo_pos = numeros_tam + 1;
			tipo_dato valor_nuevo = consul->intervalo_idx_fin;
			tipo_dato viejo_pendejo = numeros[idx_actualizar];
			avl_tree_node_t *ocurrencia_viejo_ant = NULL;
			avl_tree_node_t *ocurrencia_viejo_act = NULL;
			avl_tree_node_t *ocurrencia_viejo_pos = NULL;
			avl_tree_node_t *ocurrencia_nuevo_ant = NULL;
			avl_tree_node_t *ocurrencia_nuevo_pos = NULL;

			ocurrencia_viejo_act = avl_tree_find(arbolin, viejo_pendejo,
					idx_actualizar, verdadero);
			assert_timeout(
					ocurrencia_viejo_act
							&& ocurrencia_viejo_act->llave == viejo_pendejo
							&& ocurrencia_viejo_act->pasajero_oscuro
									== idx_actualizar);

			ocurrencia_viejo_ant = avl_tree_nodo_posicion_anterior(arbolin,
					ocurrencia_viejo_act);

			ocurrencia_viejo_pos = avl_tree_nodo_posicion_siguiente(arbolin,
					ocurrencia_viejo_act);

			caca_log_debug(
					"de nodo %u(%u) el anterior es %u(%u) el sig %u(%u)\n",
					viejo_pendejo, idx_actualizar,
					ocurrencia_viejo_ant ? ocurrencia_viejo_ant->llave : -1,
					ocurrencia_viejo_ant ? ocurrencia_viejo_ant->pasajero_oscuro : -1,
					ocurrencia_viejo_pos ? ocurrencia_viejo_pos->llave : -1,
					ocurrencia_viejo_pos ? ocurrencia_viejo_pos->pasajero_oscuro : -1);

			if (ocurrencia_viejo_ant
					&& ocurrencia_viejo_ant->llave == viejo_pendejo) {
				idx_viejo_ant = ocurrencia_viejo_ant->pasajero_oscuro;
			}
			if (ocurrencia_viejo_pos
					&& ocurrencia_viejo_pos->llave == viejo_pendejo) {
				idx_viejo_pos = ocurrencia_viejo_pos->pasajero_oscuro;
			}

			caca_log_debug("de viejo pendejo %d el ant %u el pos %u\n",
					viejo_pendejo, idx_viejo_ant, idx_viejo_pos);

			ocurrencia_nuevo_ant = avl_tree_find(arbolin, valor_nuevo,
					idx_actualizar, verdadero);
			assert_timeout(
					!ocurrencia_nuevo_ant
							|| ocurrencia_nuevo_ant->llave != valor_nuevo
							|| ocurrencia_nuevo_ant->pasajero_oscuro
									!= idx_actualizar
							|| valor_nuevo == numeros[idx_actualizar]);
			if (ocurrencia_nuevo_ant) {
				ocurrencia_viejo_pos = avl_tree_nodo_posicion_siguiente(arbolin,
						ocurrencia_nuevo_ant);
			} else {
				ocurrencia_nuevo_ant = avl_tree_find(arbolin, valor_nuevo,
						idx_actualizar, falso);
			}

			assert_timeout(
					!ocurrencia_nuevo_pos
							|| ocurrencia_nuevo_pos->llave != valor_nuevo
							|| ocurrencia_nuevo_pos->pasajero_oscuro
									!= idx_actualizar);

			if (ocurrencia_nuevo_ant
					&& ocurrencia_nuevo_ant->llave == valor_nuevo) {
				idx_nuevo_ant = ocurrencia_nuevo_ant->pasajero_oscuro;
			}
			if (ocurrencia_nuevo_pos
					&& ocurrencia_nuevo_pos->llave == valor_nuevo) {
				idx_nuevo_pos = ocurrencia_nuevo_pos->pasajero_oscuro;

			}

			caca_log_debug("de nueva mierda %d el ant %u el pos %u\n",
					valor_nuevo, idx_nuevo_ant, idx_nuevo_pos);

			bit_ch_actualiza(biatch, idx_viejo_ant, idx_actualizar,
					idx_viejo_pos, -viejo_pendejo);
			bit_ch_actualiza(biatch, idx_nuevo_ant, idx_actualizar,
					idx_nuevo_pos, valor_nuevo);

			avl_tree_borrar(arbolin, viejo_pendejo, idx_actualizar);
			avl_tree_insert(arbolin, valor_nuevo, idx_actualizar);

			numeros[idx_actualizar] = valor_nuevo;
		} else {
			entero_largo delta = 0;
			entero_largo resu = 0;
			natural idx_ini = consul->intervalo_idx_ini;
			natural idx_fin = consul->intervalo_idx_fin;
			delta = bit_ch_consulta_trozos(biatch, idx_ini, idx_fin);

			caca_log_debug("le delta %lld\n", delta);

			resu = consul->resulcaca + delta;

			caca_log_debug("el resul nuevo %lld viene de %lld\n", resu,
					consul->resulcaca);
			printf("%lld\n", resu);
		}
	}

}

int main(void) {
//	puts("he corrido con algo de suerte");
//	sleep(10);
	caca_x_main();
	return EXIT_SUCCESS;
}
