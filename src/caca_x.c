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
#define CACA_X_AJUSTE_ENTERO 2147483648LL
#define CACA_X_MAX_VALORES_INT UINT_MAX

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

typedef natural hm_iter;

#define HASH_MAP_VALOR_INVALIDO ((hm_iter)CACA_X_VALOR_INVALIDO)

typedef struct hash_map_entry {
	entero_largo llave;
	entero_largo valor;
} hm_entry;

typedef struct hash_map_cubeta {
	uint64_t hash;
	hm_entry *entry;
} hm_cubeta;

typedef struct hash_map_robin_hood_back_shift {
	hm_cubeta *buckets_;
	uint64_t num_buckets_;
	uint64_t num_buckets_used_;
	uint64_t probing_min_;
	uint64_t probing_max_;
} hm_rr_bs_tabla;

int hash_map_robin_hood_back_shift_init(hm_rr_bs_tabla *ht, int num_cubetas) {
	ht->num_buckets_ = num_cubetas;
	ht->buckets_ = (hm_cubeta *) calloc(ht->num_buckets_, sizeof(hm_cubeta));
	ht->num_buckets_used_ = 0;
	ht->probing_max_ = num_cubetas;
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
	*value = 0;
	for (i = 0; i < num_cubetas; i++) {
		index_current = (index_init + i) % num_cubetas;
		hm_entry *entrada = ht->buckets_[index_current].entry;

		hash_map_robin_hood_back_shift_llena_distancia_a_indice_inicio(ht,
				index_current, &probe_distance);
		if (entrada == NULL || i > probe_distance) {
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

	return HASH_MAP_VALOR_INVALIDO ;
}

hm_iter hash_map_robin_hood_back_shift_pon(hm_rr_bs_tabla *ht, entero_largo key,
		entero_largo value, bool *nuevo_entry) {

	uint64_t num_cubetas = ht->num_buckets_;
	uint64_t prob_max = ht->probing_max_;
	uint64_t prob_min = ht->probing_min_;
	hm_cubeta *cubetas = ht->buckets_;

	*nuevo_entry = falso;

	if (ht->num_buckets_used_ == num_cubetas) {
		return HASH_MAP_VALOR_INVALIDO ;
	}
	ht->num_buckets_used_ += 1;

//	uint64_t hash = hash_function_caca(key);
	uint64_t hash = key % num_cubetas;
	uint64_t index_init = hash;

	hm_entry *entry = (hm_entry *) calloc(1, sizeof(hm_entry));
	entry->llave = key;
	entry->valor = value;

	uint64_t index_current = index_init;
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
				*nuevo_entry = verdadero;
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

static inline bool hash_map_robin_hood_back_shift_indice_obten_llave(
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
	return 0;

}

static inline bool hash_map_robin_hood_back_shift_indice_obten_valor(
		hm_rr_bs_tabla *ht, hm_iter indice) {
	assert_timeout(indice <= ht->probing_max_ && indice >= ht->probing_min_);
	hm_entry *entrada = ht->buckets_[indice].entry;
	assert_timeout(entrada);
	return entrada->valor;
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
	nodo_lista cabeza_mem;
} lista_pendeja;

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

typedef struct caca_preprocesada {
	natural idx_inicio;
	natural idx_fin;
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
lista_pendeja idx_bloques_by_posiciones[MAX_NUMEROS] = { 0 };
natural tam_bloque = 0;
natural num_bloques = 0;
bitch_vector *mapa_unicos = NULL;
hm_rr_bs_tabla *mapa_ocurrencias_en_subarreglos = &(hm_rr_bs_tabla ) { 0 };
hm_rr_bs_tabla mapas_num_bloque_mem[MAX_NUMEROS + 200000] = { 0 };
natural mapas_num_bloque_mem_usados = 0;
natural num_nums_anadidos = 0;
tipo_dato *nums_anadidos = NULL;

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

#define caca_comun_checa_bit_mac(bits, posicion, resultado) \
        __asm__ (\
                        "xor %%rdx,%%rdx\n"\
                        "movq %[bitch_posi],%%rax\n"\
                        "movq $8,%%r8\n"\
                        "divq %%r8\n"\
                        "mov $1,%[resul]\n"\
                        "mov %%rdx,%%rcx\n"\
                        "shlq %%cl,%[resul]\n"\
                        "and (%[vectors],%%rax,1),%[resul]\n"\
                        : [resul] "=b" (resultado)\
                        : [bitch_posi] "r" (posicion), [vectors] "r" (bits)\
            :"rax","rdx","rcx","r8")

/*
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

 */
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

static inline void caca_x_inicializa_datos_preprocesados() {
	entero_largo checa_bitch_res;
	natural idx_dato_prepro = 0;

	nums_anadidos = calloc(MAX_NUMEROS, sizeof(tipo_dato));
	assert_timeout(nums_anadidos);

	tam_bloque = ceil(pow(num_numeros, 2.0 / 3.0));
	num_bloques = (num_numeros / tam_bloque);
	if (num_numeros % tam_bloque) {
		num_bloques++;
	}

	caca_log_debug("el tam bloq %u num de bloqs %u\n", tam_bloque, num_bloques);

	datos_prepro = calloc(num_bloques * num_bloques, sizeof(datos_prepro));
	assert_timeout(datos_prepro);

	/** debiera ser 12401660 segun el conteo */
//	nodos_mem = calloc(13000000, sizeof(nodos_mem));
//	assert_timeout(nodos_mem);

	for (int i = 0; i < MAX_NUMEROS; i++) {
		lista_pendeja *lista_pos = idx_bloques_by_posiciones + i;
		lista_pos->cola = lista_pos->cabeza = &lista_pos->cabeza_mem;
		lista_pos->cabeza->data = CACA_X_VALOR_INVALIDO;
	}

	hash_map_robin_hood_back_shift_init(mapa_ocurrencias_en_subarreglos,
			num_numeros << 1);

	for (int i = 0; i < num_bloques; i++) {
		for (int k = 0; k < i; k++) {
			datos_prepro[i * num_bloques + k].suma = CACA_X_VALOR_INVALIDO;
		}
		for (int j = i; j < num_bloques; j++) {
			idx_dato_prepro = i * num_bloques + j;
			caca_preprocesada *bloque_prepro = datos_prepro + idx_dato_prepro;
			num_nums_anadidos = 0;

			bloque_prepro->suma = CACA_X_VALOR_INVALIDO;

			limite_izq = i * tam_bloque;
			limite_der = (j + 1) * tam_bloque - 1;
			if (limite_der > num_numeros - 1) {
				limite_der = num_numeros - 1;
			}

			bloque_prepro->idx_inicio = limite_izq;
			bloque_prepro->idx_fin = limite_der;

			caca_log_debug(
					"creando listas de subarrays por posicon idx %u(%u,%u) limite izq %u limite der %u\n",
					idx_dato_prepro, i, j, limite_izq, limite_der);

			for (int k = limite_izq; k <= limite_der; k++) {
				tipo_dato num_actual = numeros[k];
				lista_pendeja *lista_pos = idx_bloques_by_posiciones + k;
				__asm__("nop\n"
						"nop\n");
				/*

				listilla_insert(lista_pos, idx_dato_prepro);
				caca_log_debug("aora la lista de bloques de pops %u es %s\n", k,
						listilla_a_cadena(lista_pos, CACA_X_BUF_STATICO_DUMP_ARBOL));

				 caca_comun_checa_bit_mac(mapa_unicos,
				 CACA_X_AJUSTE_ENTERO+num_actual, checa_bitch_res);
				 if (!checa_bitch_res) {
				 caca_comun_asigna_bit(mapa_unicos,
				 (unsigned long) (num_actual
				 + (unsigned long) ((unsigned long) INT_MAX
				 + 1)));
				 nums_anadidos[num_nums_anadidos++] = num_actual;
				 }
				 */

			}

		}
	}

	for (int k = 0; k < num_nums_anadidos; k++) {
		tipo_dato num_actual = nums_anadidos[k];
		bool nuevo_entry;

		caca_comun_checa_bit_mac(mapa_unicos,
				(unsigned long) (num_actual + (unsigned long) ((unsigned long) INT_MAX + 1)),
				checa_bitch_res);
		assert_timeout(checa_bitch_res);

		hash_map_robin_hood_back_shift_pon(mapa_ocurrencias_en_subarreglos,
				num_actual,
				(entero_largo) (mapas_num_bloque_mem
						+ (mapas_num_bloque_mem_usados++)), &nuevo_entry);
		assert_timeout(nuevo_entry);
		caca_comun_limpia_bit(mapa_unicos,
				(unsigned long) (num_actual
						+ (unsigned long) ((unsigned long) INT_MAX + 1)));

	}

}

static inline natural caca_x_obten_idx_bloque_prepro(natural idx_pos_ini,
		natural idx_pos_fin) {
	natural idx_bloque_inicio = idx_pos_ini / tam_bloque;
	natural idx_bloque_final = idx_pos_fin / tam_bloque;
	natural idx_bloque = CACA_X_VALOR_INVALIDO;
	if (idx_pos_ini % tam_bloque) {
		idx_bloque_inicio++;
	}
	if ((idx_pos_fin + 1) % tam_bloque) {
		idx_bloque_final--;
	}
	if (idx_bloque_inicio <= idx_bloque_final) {
		idx_bloque = idx_bloque_inicio * num_bloques + idx_bloque_final;
	}
	return idx_bloque;
}

static inline caca_preprocesada *caca_x_genera_bloque_prepro(natural idx_bloque) {
	bool checa_bitch_res;
	caca_preprocesada *bloque_prepro = datos_prepro + idx_bloque;
	natural idx_ini = bloque_prepro->idx_inicio;
	natural idx_fin = bloque_prepro->idx_fin;
	entero_largo suma_act = 0;
	hm_iter iter;

	caca_log_debug("generando suma de bloque %u, %u-%u\n", idx_bloque, idx_ini,
			idx_fin);

	assert_timeout(bloque_prepro->suma==CACA_X_VALOR_INVALIDO);
	assert_timeout(idx_ini <= idx_fin);

	for (natural i = idx_ini; i <= idx_fin; i++) {
		natural conteo_num_en_bloque;
		tipo_dato num_actual = numeros[i];
		hm_rr_bs_tabla *mapa_num_bloque;

		iter = hash_map_robin_hood_back_shift_obten(
				mapa_ocurrencias_en_subarreglos, num_actual,
				(entero_largo *) &mapa_num_bloque);

		assert_timeout(iter!=HASH_MAP_VALOR_INVALIDO && mapa_num_bloque);

		iter = hash_map_robin_hood_back_shift_obten(mapa_num_bloque, idx_bloque,
				(entero_largo *) &conteo_num_en_bloque);

		assert_timeout(iter!=HASH_MAP_VALOR_INVALIDO || !conteo_num_en_bloque);

		if (iter == HASH_MAP_VALOR_INVALIDO ) {
			bool nuevo_entry;
			assert_timeout(!conteo_num_en_bloque);
			iter = hash_map_robin_hood_back_shift_pon(mapa_num_bloque,
					idx_bloque, conteo_num_en_bloque, &nuevo_entry);
			assert_timeout(nuevo_entry);
		}

		conteo_num_en_bloque++;

		hash_map_robin_hood_back_shift_indice_pon_valor(mapa_num_bloque, iter,
				conteo_num_en_bloque);

		caca_comun_checa_bit_mac(mapa_unicos,
				(unsigned long) (num_actual + (unsigned long) ((unsigned long) INT_MAX + 1)),
				checa_bitch_res);
		if (!checa_bitch_res) {
			caca_comun_asigna_bit(mapa_unicos,
					(unsigned long) (num_actual
							+ (unsigned long) ((unsigned long) INT_MAX + 1)));
			nums_anadidos[num_nums_anadidos++] = num_actual;
			suma_act += num_actual;
		}

	}

	for (int k = 0; k < num_nums_anadidos; k++) {
		tipo_dato num_actual = nums_anadidos[k];
		caca_comun_checa_bit_mac(mapa_unicos,
				(unsigned long) (num_actual + (unsigned long) ((unsigned long) INT_MAX + 1)),
				checa_bitch_res);
		assert_timeout(checa_bitch_res);

		caca_comun_limpia_bit(mapa_unicos,
				(unsigned long) (num_actual
						+ (unsigned long) ((unsigned long) INT_MAX + 1)));

	}

	bloque_prepro->suma = suma_act;

	caca_log_debug("la suma de unicos %llu\n", suma_act);

	return datos_prepro;
}

static inline caca_preprocesada *caca_x_obten_bloque_prepro(natural idx_bloque) {
	entero_largo suma_enc = 0;
	caca_preprocesada *dato_prepro = datos_prepro + idx_bloque;
	suma_enc = dato_prepro->suma;

	caca_log_debug("obteniendo el bloke %u, %u-%u\n", idx_bloque,
			dato_prepro->idx_inicio, dato_prepro->idx_fin);

	if (suma_enc == CACA_X_VALOR_INVALIDO) {
		caca_x_genera_bloque_prepro(idx_bloque);
	}

	return dato_prepro;
}

static inline entero_largo caca_x_calcula_suma_unicos(natural idx_ini,
		natural idx_fin) {
	bool checa_bitch_res;
	entero_largo suma = 0;
	natural idx_bloque = 0;

	idx_bloque = caca_x_obten_idx_bloque_prepro(idx_ini, idx_fin);

	caca_log_debug("para el intervalo %u-%u el idx bloque %u\n", idx_ini,
			idx_fin, idx_bloque);

	num_nums_anadidos = 0;

	if (idx_bloque != (natural) CACA_X_VALOR_INVALIDO) {
		caca_preprocesada *dato_prepro = datos_prepro + idx_bloque;
		natural idx_bloque_ini = dato_prepro->idx_inicio;
		natural idx_bloque_fin = dato_prepro->idx_fin;

		caca_log_debug("calculando suma unicos de %u-%u\n", idx_ini,
				idx_bloque_ini);
		for (natural i = idx_ini; i < idx_bloque_ini; i++) {
			tipo_dato num_actual = numeros[i];
			caca_comun_checa_bit_mac(mapa_unicos,
					(unsigned long) (num_actual + (unsigned long) ((unsigned long) INT_MAX + 1)),
					checa_bitch_res);
			if (!checa_bitch_res) {
				natural ocurrencias;
				hm_iter iter;
				hm_rr_bs_tabla *mapa_ocurrencias_por_bloque;

				caca_log_debug("el num %d no esta en el mapita\n", num_actual);

				caca_comun_asigna_bit(mapa_unicos,
						(unsigned long) (num_actual
								+ (unsigned long) ((unsigned long) INT_MAX + 1)));
				nums_anadidos[num_nums_anadidos++] = num_actual;

				iter = hash_map_robin_hood_back_shift_obten(
						mapa_ocurrencias_en_subarreglos, num_actual,
						(entero_largo *) &mapa_ocurrencias_por_bloque);

				assert_timeout(
						iter!=HASH_MAP_VALOR_INVALIDO && mapa_ocurrencias_por_bloque);

				iter = hash_map_robin_hood_back_shift_obten(
						mapa_ocurrencias_por_bloque, idx_bloque,
						(entero_largo *) &ocurrencias);

				caca_log_debug(
						"el num %d tiene %u ocurrencias en bloque %u (%u-%u)\n",
						num_actual, idx_bloque, idx_bloque_ini, idx_bloque_fin);

				if (iter == HASH_MAP_VALOR_INVALIDO ) {
					assert_timeout(!ocurrencias);
					suma += num_actual;
				}
			}

		}

		caca_log_debug("calculando suma unicos de %u-%u\n", idx_bloque_fin+1,
				idx_fin);
		for (natural i = idx_bloque_fin + 1; i <= idx_fin; i++) {
			tipo_dato num_actual = numeros[i];

			caca_comun_checa_bit_mac(mapa_unicos,
					(unsigned long) (num_actual + (unsigned long) ((unsigned long) INT_MAX + 1)),
					checa_bitch_res);

			if (!checa_bitch_res) {
				natural ocurrencias;
				hm_iter iter;
				hm_rr_bs_tabla *mapa_ocurrencias_por_bloque;

				caca_log_debug("el num %d no esta en el mapita\n", num_actual);

				caca_comun_asigna_bit(mapa_unicos,
						(unsigned long) (num_actual
								+ (unsigned long) ((unsigned long) INT_MAX + 1)));
				nums_anadidos[num_nums_anadidos++] = num_actual;

				iter = hash_map_robin_hood_back_shift_obten(
						mapa_ocurrencias_en_subarreglos, num_actual,
						(entero_largo *) &mapa_ocurrencias_por_bloque);

				assert_timeout(
						iter!=HASH_MAP_VALOR_INVALIDO && mapa_ocurrencias_por_bloque);

				iter = hash_map_robin_hood_back_shift_obten(
						mapa_ocurrencias_por_bloque, idx_bloque,
						(entero_largo *) &ocurrencias);

				caca_log_debug(
						"el num %d tiene %u ocurrencias en bloque %u (%u-%u)\n",
						num_actual, idx_bloque, idx_bloque_ini, idx_bloque_fin);

				if (iter == HASH_MAP_VALOR_INVALIDO ) {
					assert_timeout(!ocurrencias);
					suma += num_actual;
				}
			}

		}

		caca_log_debug("la suma del bloque prepro %lld, la calculeada %lld\n",
				dato_prepro->suma, suma);
		suma += dato_prepro->suma;

	} else {
		caca_log_debug("no ay bloque prepro, %u-%u\n", idx_ini, idx_fin);
		for (natural i = idx_ini; i <= idx_fin; i++) {
			tipo_dato num_actual = numeros[i];

			caca_comun_checa_bit_mac(mapa_unicos,
					(unsigned long) (num_actual + (unsigned long) ((unsigned long) INT_MAX + 1)),
					checa_bitch_res);
			if (!checa_bitch_res) {

				caca_log_debug("el num %d no esta en el mapita\n", num_actual);

				caca_comun_asigna_bit(mapa_unicos,
						(unsigned long) (num_actual
								+ (unsigned long) ((unsigned long) INT_MAX + 1)));
				nums_anadidos[num_nums_anadidos++] = num_actual;

				suma += num_actual;
			}
		}
	}

	for (int k = 0; k < num_nums_anadidos; k++) {
		tipo_dato num_actual = nums_anadidos[k];
		caca_comun_checa_bit_mac(mapa_unicos,
				(unsigned long) (num_actual + (unsigned long) ((unsigned long) INT_MAX + 1)),
				checa_bitch_res);
		assert_timeout(checa_bitch_res);

		caca_comun_limpia_bit(mapa_unicos,
				(unsigned long) (num_actual
						+ (unsigned long) ((unsigned long) INT_MAX + 1)));

	}

	return suma;
}

static inline void caca_x_actualizar_datos_preprocesados(
		natural idx_pos_actualizar, tipo_dato nuevo_valor) {
	tipo_dato viejo_pendejo = numeros[idx_pos_actualizar];
	lista_pendeja *lista_idx_bloques = idx_bloques_by_posiciones
			+ idx_pos_actualizar;
	nodo_lista *nodo_act = NULL;

	caca_log_debug("la lista de bloques para idx %u es %s\n",
			idx_pos_actualizar,
			listilla_a_cadena(lista_idx_bloques, CACA_X_BUF_STATICO_DUMP_ARBOL));
	//TODO: Esta lista, por que iterar todos cuando sabemos q no todos estan inicializados?
	nodo_act = lista_idx_bloques->cabeza;

	while (nodo_act) {
		natural idx_bloque_actual = nodo_act->data;
		natural ocurrencias = 0;
		hm_iter iter = 0;
		int ret = 0;
		bool nuevo_entry;
		hm_rr_bs_tabla *tablon = NULL;

		entero_largo *suma = &datos_prepro[idx_bloque_actual].suma;
		caca_log_debug("en nodo %u el tablon %p\n", idx_bloque_actual, tablon);

		caca_log_debug("el bloke %u tiene suma %lld i tablon %s\n",
				idx_bloque_actual, *suma,
				kh_shit_dumpear(tablon, CACA_X_BUF_STATICO_DUMP_ARBOL));

		iter = hash_map_robin_hood_back_shift_obten(
				mapa_ocurrencias_en_subarreglos, viejo_pendejo,
				(entero_largo *) &tablon);
		assert_timeout(iter != HASH_MAP_VALOR_INVALIDO && tablon);

		iter = hash_map_robin_hood_back_shift_obten(tablon, idx_bloque_actual,
				(entero_largo *) &ocurrencias);

		if (iter != HASH_MAP_VALOR_INVALIDO ) {

			assert_timeout(ocurrencias >= 1);

			caca_log_debug("las ocurrencias de %u son %u\n", viejo_pendejo,
					ocurrencias);

			ocurrencias--;
			if (!ocurrencias) {
				caca_log_debug("a suma actual %lld se le resta %u\n", *suma,
						viejo_pendejo);
				(*suma) -= viejo_pendejo;
				hash_map_robin_hood_back_shift_indice_borra(tablon, iter);
			} else {
				hash_map_robin_hood_back_shift_indice_pon_valor(tablon, iter,
						ocurrencias);
			}

			iter = hash_map_robin_hood_back_shift_pon(tablon, idx_bloque_actual,
					1, &nuevo_entry);

			assert_timeout(iter!=HASH_MAP_VALOR_INVALIDO);
			if (nuevo_entry) {
				caca_log_debug(
						"en valor nuevo %u no estaba, sumandolo a %lld\n",
						nuevo_valor, *suma);
				(*suma) += nuevo_valor;
			} else {
				ocurrencias = hash_map_robin_hood_back_shift_indice_obten_valor(
						tablon, iter);
				ocurrencias++;
				hash_map_robin_hood_back_shift_indice_pon_valor(tablon, iter,
						ocurrencias);
				caca_log_debug(
						"en valor nuevo %u ya estaba, aora ai %u ocurrencias\n",
						nuevo_valor, kh_val(tablon,iter));
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

	mapa_unicos = calloc(CACA_X_MAX_VALORES_INT / (sizeof(bitch_vector) * 8),
			sizeof(bitch_vector));
	assert_timeout(mapa_unicos);

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
	caca_x_inicializa_datos_preprocesados();
	printf("ya caca \n");

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
//			caca_x_actualizar_datos_preprocesados(idx_actualizado, nuevo_valor);

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
