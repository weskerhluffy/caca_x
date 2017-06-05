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
#define TAM_MAX_LINEA (MAX_NUMEROS*12) 
#define CX_MAX_PROFUNDIDAD 19
#define CX_MAX_NODOS_AFECTADOS CX_MAX_PROFUNDIDAD*2
#define CX_MAX_NUMEROS_REDONDEADO (1<<(CX_MAX_PROFUNDIDAD-2))
#define CX_MAX_NODOS (1 << CX_MAX_PROFUNDIDAD)
#define CX_VALOR_INVALIDO -1LL
#define CX_MAX_VALORES_INT UINT_MAX
#define CX_MAX_NODOS_LISTA 13000000
#define CX_MAX_BLOQUES 37
#define CX_BUF_STATICO_DUMP_ARBOL (char[1000] ) { '\0' }
#define caca_log_debug(formato, args...) 0
#define ass(condition) assert(condition);
typedef int td;
typedef unsigned int nat;
typedef long long enl;
typedef unsigned long long enl_sin_signo;
typedef enum BOOLEANOS {
	falso = 0, verdadero
} bool;
#define caca_comun_max(x,y) ((x) < (y) ? (y) : (x))
#define caca_comun_min(x,y) ((x) < (y) ? (x) : (y))
static inline int lee_matrix_long_stdin(td *matrix, int *num_filas,
		int *num_columnas, int num_max_filas, int num_max_columnas) {
	int indice_filas = 0;
	int indice_columnas = 0;
	long numero = 0;
	char *sig_cadena_numero = NULL;
	char *cadena_numero_act = NULL;
	char *linea = NULL;
	linea = calloc(TAM_MAX_LINEA, sizeof(char));
	while (indice_filas < num_max_filas && fgets(linea, TAM_MAX_LINEA, stdin)) {
		indice_columnas = 0;
		cadena_numero_act = linea;
		for (sig_cadena_numero = linea;; sig_cadena_numero =
				cadena_numero_act) {
			numero = strtol(sig_cadena_numero, &cadena_numero_act, 10);
			if (cadena_numero_act == sig_cadena_numero) {
				break;
			}
			*(matrix + indice_filas * num_max_columnas + indice_columnas) =
					numero;

			indice_columnas++;

			setbuf(stdout, NULL );
		}
		if (num_columnas) {
			num_columnas[indice_filas] = indice_columnas;
		}
		indice_filas++;

	}
	*num_filas = indice_filas;
	free(linea);
	return 0;
}
int caca_comun_compara_enteros(const void *a, const void *b) {
	int a_int = 0;
	int b_int = 0;
	int resultado = 0;
	a_int = *(int *) a;
	b_int = *(int *) b;
	resultado = a_int - b_int;
	return resultado;
}
void cax_anade_caca(td numero);
void cax_quita_caca(td numero);
#if 1
typedef nat hm_iter;
#define HM_VALOR_INVALIDO ((hm_iter)CX_VALOR_INVALIDO)
typedef struct hash_map_entry {
	enl ll;
	enl valor;
} hm_entry;
typedef struct hash_map_cubeta {
	uint64_t hash;
	hm_entry *entry;
} hm_cubeta;
typedef struct hmrh {
	hm_cubeta *buckets_;
	uint64_t num_buckets_;
	uint64_t num_buckets_used_;
	uint64_t probing_min_;
	uint64_t probing_max_;
} hm_rr_bs_tabla;
int hmrh_init(hm_rr_bs_tabla *ht, int num_cubetas) {
	ht->num_buckets_ = num_cubetas;
	ht->buckets_ = (hm_cubeta *) calloc(ht->num_buckets_, sizeof(hm_cubeta));
	ht->num_buckets_used_ = 0;
	ht->probing_max_ = num_cubetas;
	return 0;
}
int hmrh_fini(hm_rr_bs_tabla *ht) {
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
static inline int hmrh_llena_distancia_a_indice_inicio(
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
hm_iter hmrh_obten(hm_rr_bs_tabla *ht, const enl key,
		enl *value) {
	uint64_t num_cubetas = ht->num_buckets_;
	uint64_t prob_max = ht->probing_max_;
//	uint64_t hash = hash_function_caca(key);
	uint64_t hash = key % num_cubetas;
	uint64_t index_init = hash;
	uint64_t probe_distance = 0;
	uint64_t index_current;
	bool found = falso;
	uint32_t i;
	*value = HM_VALOR_INVALIDO;
	for (i = 0; i < num_cubetas; i++) {
		index_current = (index_init + i) % num_cubetas;
		hm_entry *entrada = ht->buckets_[index_current].entry;
		if (entrada == NULL ) {
			break;
		}
		hmrh_llena_distancia_a_indice_inicio(ht,
				index_current, &probe_distance);
		if (i > probe_distance) {
			break;
		}
		if (entrada->ll == key) {
			*value = entrada->valor;
			found = verdadero;
			break;
		}
	}
	if (found)
		return index_current;
	return HM_VALOR_INVALIDO ;
}
hm_iter hmrh_pon(hm_rr_bs_tabla *ht, enl key,
		enl value, bool *nuevo_entry) {
	uint64_t num_cubetas = ht->num_buckets_;
	uint64_t prob_max = ht->probing_max_;
	uint64_t prob_min = ht->probing_min_;
	hm_cubeta *cubetas = ht->buckets_;
	*nuevo_entry = verdadero;
	if (ht->num_buckets_used_ == num_cubetas) {
		*nuevo_entry = falso;
		return HM_VALOR_INVALIDO ;
	}
	ht->num_buckets_used_ += 1;
	uint64_t hash = key % num_cubetas;
	uint64_t index_init = hash;
	hm_entry *entry = (hm_entry *) calloc(1, sizeof(hm_entry));
	entry->ll = key;
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
			if (cubeta->entry->ll == key) {
				free(entry);
				*nuevo_entry = falso;
				break;
			}
			hmrh_llena_distancia_a_indice_inicio(ht,
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
int hmrh_borra(hm_rr_bs_tabla *ht, const td key) {
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
		hmrh_llena_distancia_a_indice_inicio(ht,
				index_current, &probe_distance);
		if (entrada == NULL || i > probe_distance) {
			break;
		}
		if (entrada->ll == key) {
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
			if (hmrh_llena_distancia_a_indice_inicio(
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
static inline void hmrh_indice_pon_valor(
		hm_rr_bs_tabla *ht, hm_iter indice, enl valor) {
	ass(indice <= ht->probing_max_ && indice >= ht->probing_min_);
	hm_entry *entrada = ht->buckets_[indice].entry;
	ass(entrada);
	entrada->valor = valor;
}
int hmrh_indice_borra(hm_rr_bs_tabla *ht,
		hm_iter indice) {
	ass(indice <= ht->probing_max_ && indice >= ht->probing_min_);
	uint64_t num_cubetas = ht->num_buckets_;
	uint64_t prob_max = ht->probing_max_;
	uint64_t prob_min = ht->probing_max_;
	uint64_t index_current = indice;
	hm_entry *entrada = ht->buckets_[index_current].entry;
	ass(entrada);
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
		if (hmrh_llena_distancia_a_indice_inicio(ht,
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
#endif
#if 1
typedef unsigned long long bitch_vector;
#define BITCH_MAX_NUMEROS_AGREGADOS MAX_NUMEROS
#define BITCH_TAM_MAPA ( (CX_MAX_VALORES_INT / (sizeof(bitch_vector) * 8)) + 1)
enl_sin_signo bitch_numeros_agregados[BITCH_MAX_NUMEROS_AGREGADOS] = { 0 };
nat bitch_numeros_agregados_tam = 0;
bitch_vector *bitch_mapa = NULL;
static inline void bitch_init() {
	bitch_mapa = calloc(BITCH_TAM_MAPA, sizeof(bitch_vector));
	ass(bitch_mapa);
}
#define bitch_checa(bits, posicion, resultado) \
        __asm__ (\
                        "xor %%rdx,%%rdx\n"\
                        "movq %[bitch_posi],%%rax\n"\
                        "movq $64,%%r8\n"\
                        "divq %%r8\n"\
                        "movq $1,%[resul]\n"\
                        "mov %%rdx,%%rcx\n"\
                        "shl %%cl,%[resul]\n"\
                        "andq (%[vectors],%%rax,8),%[resul]\n"\
                        : [resul] "=b" (resultado)\
                        : [bitch_posi] "r" (posicion), [vectors] "r" (bits)\
            :"rax","rdx","rcx","r8")
#endif
#if 1
typedef enum mo_mada_tipo_query {
	mo_mada_actizacion = 'U', mo_mada_consulta = 'Q'
} mo_mada_tipo_query;
typedef struct mo_mada {
	mo_mada_tipo_query tipo;
	nat idx_query;
	nat intervalo_idx_ini;
	nat intervalo_idx_fin;
	nat orden;
	enl resulcaca;
} mo_mada;
nat mo_mada_tam_bloque = 0;
enl mo_mada_resultado = 0;
int mo_mada_ord_bloque(const void *pa, const void *pb) {
	int res = 0;
	mo_mada *a = (mo_mada *) pa;
	mo_mada *b = (mo_mada *) pb;
	nat idx_bloque_a = a->intervalo_idx_ini / mo_mada_tam_bloque;
	nat idx_bloque_b = b->intervalo_idx_ini / mo_mada_tam_bloque;
	nat idx_fin_a = a->intervalo_idx_fin;
	nat idx_fin_b = b->intervalo_idx_fin;
	nat idx_ini_a = a->intervalo_idx_ini;
	nat idx_ini_b = b->intervalo_idx_ini;
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
#define mo_mada_fn_anade_caca cax_anade_caca
#define mo_mada_fn_quita_caca cax_quita_caca
static inline mo_mada *mo_mada_core(mo_mada *consultas, td *numeros,
		nat num_consultas, nat num_numeros) {
	nat idx_izq_act = 0;
	nat idx_der_act = 0;
	mo_mada_tam_bloque = ceil(sqrt(num_numeros));
	qsort(consultas, num_consultas, sizeof(mo_mada), mo_mada_fn_ord_mo);
	idx_izq_act = idx_der_act = (consultas)->intervalo_idx_ini;
	mo_mada_fn_anade_caca((numeros[idx_izq_act]));
	for (int i = 0; i < num_consultas; i++) {
		nat consul_idx_izq = (consultas + i)->intervalo_idx_ini;
		nat consul_idx_der = (consultas + i)->intervalo_idx_fin;
		if ((consultas + i)->tipo == mo_mada_actizacion) {
			continue;
		}
#if 0
		ass(
				ceil(abs((int )idx_izq_act - (int )consul_idx_izq))
				<= mo_mada_tam_bloque * 2);
#endif

		while (idx_izq_act > consul_idx_izq) {
			idx_izq_act--;
			mo_mada_fn_anade_caca(numeros[idx_izq_act]);

		}

		while (idx_der_act < consul_idx_der) {
			idx_der_act++;
			mo_mada_fn_anade_caca(numeros[idx_der_act]);

		}

		while (idx_izq_act < consul_idx_izq) {
			mo_mada_fn_quita_caca(numeros[idx_izq_act]);

			idx_izq_act++;
		}

		while (idx_der_act > consul_idx_der) {
			mo_mada_fn_quita_caca(numeros[idx_der_act]);

			idx_der_act--;
		}

		(consultas + i)->resulcaca = mo_mada_resultado;
	}
	qsort(consultas, num_consultas, sizeof(mo_mada), mo_mada_fn_ord_idx);
	return consultas;
}
#endif
#if 1
#define TRT_MAX_NUMEROS (MAX_NUMEROS+2)
#define TRT_MAX_NODOS (((TRT_MAX_NUMEROS<<1)+2)*100)
#define TRT_MAX_VALOR INT_MAX
typedef struct trt {
	struct trt *hijo_izq;
	struct trt *hijo_der;
	enl valor;
} trt;
trt *nds_trozo = NULL;
nat nds_trozo_usados = 0;
td *trt_numeros = NULL;
nat trt_numeros_tam = 0;
nat trt_idx_ini_buscado = 0;
nat trt_idx_fin_buscado = 0;
nat trt_idx_a_actizar = 0;
static inline trt *trt_nuevo_nd() {
	trt *nd_nuevo = (nds_trozo + (nds_trozo_usados++));
	return nd_nuevo;
}
static inline void trt_contruye(trt **nd_act, nat idx_ini, nat idx_fin) {
	trt *nd_act_int = NULL;
	if (!*nd_act) {
		*nd_act = trt_nuevo_nd();
	}
	nd_act_int = *nd_act;
	if (idx_ini != idx_fin) {
		nat idx_mid = idx_ini + ((idx_fin - idx_ini) >> 1);
		trt_contruye(&nd_act_int->hijo_izq, idx_ini, idx_mid);
		trt_contruye(&nd_act_int->hijo_der, idx_mid + 1, idx_fin);
		nd_act_int->valor = caca_comun_min(
				nd_act_int->hijo_izq->valor,
				nd_act_int->hijo_der->valor);
	} else {
		nd_act_int->valor = trt_numeros[idx_ini];
	}
}
static inline void trt_actiza(trt **nd_act, nat idx_ini, nat idx_fin,
		td nuevo_valor) {
	nat idx_mid = idx_ini + ((idx_fin - idx_ini) >> 1);
	trt *nd_act_int = NULL;
	if (!*nd_act) {
		*nd_act = trt_nuevo_nd();
	}
	nd_act_int = *nd_act;
	if (idx_ini == idx_fin) {
		ass(idx_ini == idx_fin);
		ass(trt_idx_a_actizar == idx_fin);
		nd_act_int->valor += nuevo_valor;

		return;
	}
	if (trt_idx_a_actizar <= idx_mid) {
		trt_actiza(&nd_act_int->hijo_izq, idx_ini, idx_mid, nuevo_valor);
	} else {
		trt_actiza(&nd_act_int->hijo_der, idx_mid + 1, idx_fin,
				nuevo_valor);
	}
	nd_act_int->valor = 0;
	if (nd_act_int->hijo_izq) {

		nd_act_int->valor += nd_act_int->hijo_izq->valor;
	}
	if (nd_act_int->hijo_der) {

		nd_act_int->valor += nd_act_int->hijo_der->valor;
	}
}
static inline enl trt_consulta(trt *nd_act, nat idx_ini, nat idx_fin) {
	enl resul = 0;
	if (trt_idx_ini_buscado > idx_fin) {
		resul = 0;
	} else {
		if (trt_idx_fin_buscado < idx_ini) {
			resul = 0;
		} else {
			if (!nd_act) {
				resul = 0;
			} else {
				if (idx_ini >= trt_idx_ini_buscado
						&& idx_fin <= trt_idx_fin_buscado) {
					resul = nd_act->valor;
				} else {
					nat idx_mid = idx_ini + ((idx_fin - idx_ini) >> 1);
					enl resul_izq = 0;
					enl resul_der = 0;
					resul_izq = trt_consulta(nd_act->hijo_izq, idx_ini,
							idx_mid);
					resul_der = trt_consulta(nd_act->hijo_der, idx_mid + 1,
							idx_fin);
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
	nat num_nds_bit_ch;
//	td nds_bit_ch[BIT_CH_MAX_NODOS + 2];
	trt *nds_bit_ch[BIT_CH_MAX_NODOS + 2];
} bit_ch;
static inline void bit_ch_init(bit_ch *bit, td valor_inicial, nat num_cacas,
		td *numeros) {
	bit->num_nds_bit_ch = num_cacas;
	trt_numeros = numeros;
}
static inline void bit_ch_actiza_trozos(bit_ch *bit, nat idx_bit_ch,
		nat idx_a_actizar_en_trozo, td num_nuevo) {
	trt **trozos = (trt **) bit->nds_bit_ch;
	trt_idx_a_actizar = idx_a_actizar_en_trozo;
	for (nat i = idx_bit_ch; i <= bit->num_nds_bit_ch; i += (i & (-i))) {

		trt_actiza(trozos + i, 1, trt_numeros_tam, num_nuevo);
	}
}
static inline void bit_ch_actiza(bit_ch *bit, nat ocurrencia_anterior,
		nat ocurrencia_a_actizar, nat ocurrencia_sig, td num_nuevo) {
	bit_ch_actiza_trozos(bit, ocurrencia_anterior + 1,
			ocurrencia_a_actizar, num_nuevo);
	bit_ch_actiza_trozos(bit, ocurrencia_anterior + 1, ocurrencia_sig,
			-num_nuevo);
	bit_ch_actiza_trozos(bit, ocurrencia_a_actizar + 1,
			ocurrencia_a_actizar, -num_nuevo);
	bit_ch_actiza_trozos(bit, ocurrencia_a_actizar + 1,
			ocurrencia_sig, num_nuevo);
}
static inline enl bit_ch_consulta_trozos(bit_ch *bit, nat idx_bit_ch,
		nat idx_trozo) {
	enl suma = 0;
	trt **trozos = bit->nds_bit_ch;
	trt_idx_ini_buscado = 1;
	trt_idx_fin_buscado = idx_trozo;
	for (int i = idx_bit_ch; i > 0; i -= (i & (-i))) {
		enl aportacion = 0;

		aportacion = trt_consulta(trozos[i], 1, trt_numeros_tam);
		suma += aportacion;

	}
	return suma;
}
#endif
#if 1
#define AVL_TREE_VALOR_INVALIDO CX_VALOR_INVALIDO
#define AVL_TREE_MAX_RANGO_A_MAPEAR (MAX_VALOR<<1)
struct avt_node_s {
	td ll;
	nat altura;
	nat num_decendientes;
	nat indice_en_arreglo;
	nat ocu;
	td pasajero_oscuro;
	struct avt_node_s *lf;
	struct avt_node_s *rg;
	struct avt_node_s *padre;
};
typedef struct avt_node_s avt_node_t;
struct avt_s {
	nat max_nds;
	nat nds_realmente_en_abl;
	nat nds_usados;
	struct avt_node_s *rt;
	avt_node_t *nds_mem;
	nat *nds_libres_idx;
	unsigned long sig_idx_para_usar;
	unsigned long ultimo_idx_anadido;
//	bitch_vector mapa_recorridos[AVL_TREE_MAX_RANGO_A_MAPEAR / (sizeof(bitch_vector) * 8)];
//	nat numeros_marcados[AVL_TREE_MAX_RANGO_A_MAPEAR];
};
typedef struct avt_s avt_t;
avt_t *avt_create(avt_t **ablin, int max_nds) {
	avt_t *tree = NULL;
	ass(ablin);
	tree = calloc(1, sizeof(avt_t));
	ass(tree);
	tree->max_nds = max_nds;
	tree->nds_mem = calloc(max_nds * 2, sizeof(avt_node_t));
	ass(tree->nds_mem);
	tree->rt = NULL;
	*ablin = tree;
	tree->nds_libres_idx = calloc(max_nds, sizeof(nat));
	memset(tree->nds_libres_idx, 0xffff, sizeof(nat) * max_nds);
	ass(tree->nds_libres_idx);
	return tree;
}
avt_node_t *avt_create_node(avt_t *ablin) {
	avt_node_t *node = NULL;
	ass(
			ablin->sig_idx_para_usar < ablin->ultimo_idx_anadido
					|| ((ablin->sig_idx_para_usar
							== ablin->ultimo_idx_anadido)
							&& ablin->nds_usados < ablin->max_nds));
	if (ablin->sig_idx_para_usar < ablin->ultimo_idx_anadido) {
		node = ablin->nds_mem
				+ ablin->nds_libres_idx[ablin->sig_idx_para_usar
						% ablin->max_nds];
		node->indice_en_arreglo =
				ablin->nds_libres_idx[ablin->sig_idx_para_usar
						% ablin->max_nds];
		ablin->nds_libres_idx[ablin->sig_idx_para_usar % ablin->max_nds] =
				0xffffffff;
		ablin->sig_idx_para_usar++;
	} else {
		node = ablin->nds_mem + ablin->nds_usados++;
		node->indice_en_arreglo = ablin->nds_usados - 1;
	}
	ablin->nds_realmente_en_abl++;
	return node;
}
int avt_node_height(avt_node_t *node) {
	return node ? node->altura : 0;
}
static inline void avt_node_actizar_altura(avt_node_t *node) {
	int height_lf = 0;
	int height_rg = 0;
	if (node->lf) {
		height_lf = node->lf->altura;
	}
	if (node->rg) {
		height_rg = node->rg->altura;
	}
	if (node->lf || node->rg) {
		node->altura = (height_rg > height_lf ? height_rg : height_lf) + 1;
	} else {
		node->altura = 0;
	}
}
static inline void avt_node_actizar_num_decendientes(avt_node_t *node) {
	int conteo_lf = 0;
	int conteo_rg = 0;
	if (node->lf) {
		conteo_lf = node->lf->num_decendientes;
	}
	if (node->rg) {
		conteo_rg = node->rg->num_decendientes;
	}
	if (node->lf || node->rg) {
		node->num_decendientes = conteo_lf + conteo_rg + (node->lf ? 1 : 0)
				+ (node->rg ? 1 : 0);
	} else {
		node->num_decendientes = 0;
	}
}
avt_node_t *avt_rotate_lflf(avt_node_t *node) {
	avt_node_t *a = node;
	avt_node_t *b = a->lf;
	avt_node_t *padre = NULL;
	padre = node->padre;
	a->lf = b->rg;
	b->rg = a;
	avt_node_actizar_altura(a);
	avt_node_actizar_altura(b);
	avt_node_actizar_num_decendientes(a);
	avt_node_actizar_num_decendientes(b);
	a->padre = b;
	b->padre = padre;
	if (a->lf) {
		a->lf->padre = a;
	}
	return (b);
}
avt_node_t *avt_rotate_lfrg(avt_node_t *node) {
	avt_node_t *a = node;
	avt_node_t *b = a->lf;
	avt_node_t *c = b->rg;
	avt_node_t *padre = NULL;
	padre = node->padre;
	a->lf = c->rg;
	b->rg = c->lf;
	c->lf = b;
	c->rg = a;
	avt_node_actizar_altura(a);
	avt_node_actizar_altura(b);
	avt_node_actizar_altura(c);
	avt_node_actizar_num_decendientes(a);
	avt_node_actizar_num_decendientes(b);
	avt_node_actizar_num_decendientes(c);
	a->padre = c;
	b->padre = c;
	c->padre = padre;
	if (a->lf) {
		a->lf->padre = a;
	}
	if (b->rg) {
		b->rg->padre = b;
	}
	return (c);
}
avt_node_t *avt_rotate_rglf(avt_node_t *node) {
	avt_node_t *a = node;
	avt_node_t *b = a->rg;
	avt_node_t *c = b->lf;
	avt_node_t *padre = NULL;
	padre = node->padre;
	a->rg = c->lf;
	b->lf = c->rg;
	c->rg = b;
	c->lf = a;
	avt_node_actizar_altura(a);
	avt_node_actizar_altura(b);
	avt_node_actizar_altura(c);
	avt_node_actizar_num_decendientes(a);
	avt_node_actizar_num_decendientes(b);
	avt_node_actizar_num_decendientes(c);
	a->padre = c;
	b->padre = c;
	c->padre = padre;
	if (a->rg) {
		a->rg->padre = a;
	}
	if (b->lf) {
		b->lf->padre = b;
	}
	return (c);
}
avt_node_t *avt_rotate_rgrg(avt_node_t *node) {
	avt_node_t *a = node;
	avt_node_t *b = a->rg;
	avt_node_t *padre = NULL;
	padre = node->padre;
	a->rg = b->lf;
	b->lf = a;
	avt_node_actizar_altura(a);
	avt_node_actizar_altura(b);
	avt_node_actizar_num_decendientes(a);
	avt_node_actizar_num_decendientes(b);
	a->padre = b;
	b->padre = padre;
	if (a->rg) {
		a->rg->padre = a;
	}
	return (b);
}
int avt_balance_factor(avt_node_t *node) {
	int bf = 0;
	if (node->lf)
		bf += avt_node_height(node->lf);
	if (node->rg)
		bf -= avt_node_height(node->rg);
	return bf;
}
static inline avt_node_t *avt_balance_node_insertar(const avt_node_t *node,
		const td ll_nueva, const td pasajero_oscuro) {
	avt_node_t *newrt = NULL;
	avt_node_t *nd_act = NULL;
	newrt = (avt_node_t *) node;
	nd_act = node->padre;
	while (nd_act) {
		int bf = 0;
		avt_node_t *padre = NULL;
		avt_node_t **rama_padre = NULL;
		bf = avt_balance_factor(nd_act);
		if (bf >= 2) {

			if (ll_nueva > nd_act->lf->ll) {
				newrt = avt_rotate_lfrg(nd_act);
			} else {
				if (ll_nueva < nd_act->lf->ll) {
					newrt = avt_rotate_lflf(nd_act);
				} else {
					if (pasajero_oscuro > nd_act->lf->pasajero_oscuro) {
						newrt = avt_rotate_lfrg(nd_act);
					} else {
						newrt = avt_rotate_lflf(nd_act);
					}
				}
			}
		} else if (bf <= -2) {

			if (ll_nueva < nd_act->rg->ll) {
				newrt = avt_rotate_rglf(nd_act);
			} else {
				if (ll_nueva > nd_act->rg->ll) {
					newrt = avt_rotate_rgrg(nd_act);
				} else {
					if (pasajero_oscuro < nd_act->rg->pasajero_oscuro) {
						newrt = avt_rotate_rglf(nd_act);
					} else {
						newrt = avt_rotate_rgrg(nd_act);
					}
				}
			}
		} else {

			newrt = nd_act;
			avt_node_actizar_altura(nd_act);
		}
		if (newrt->padre) {
			padre = newrt->padre;
			if (ll_nueva < padre->ll) {
				rama_padre = &padre->lf;
			} else {
				if (ll_nueva > padre->ll) {
					rama_padre = &padre->rg;
				} else {
					if (pasajero_oscuro < padre->pasajero_oscuro) {
						rama_padre = &padre->lf;
					} else {
						if (pasajero_oscuro > padre->pasajero_oscuro) {
							rama_padre = &padre->rg;
						} else {
							ass(0);
						}
					}
				}
			}
			*rama_padre = newrt;
		}
		nd_act = nd_act->padre;
	}
	return (newrt);
}
void avt_balance_insertar(avt_t *tree, avt_node_t *nd, td ll_nueva,
		td pasajero_oscuro) {
	avt_node_t *newrt = NULL;
	newrt = avt_balance_node_insertar(nd, ll_nueva, pasajero_oscuro);
	if (newrt != tree->rt) {
		tree->rt = newrt;
	}
}
void avt_insert(avt_t *tree, td value, td pasajero_oscuro) {
	avt_node_t *node = NULL;
	avt_node_t *next = NULL;
	avt_node_t *last = NULL;

	if (tree->rt == NULL ) {
		node = avt_create_node(tree);
		node->ll = value;
		node->pasajero_oscuro = pasajero_oscuro;
		tree->rt = node;

	} else {
		next = tree->rt;
		while (next != NULL ) {
			last = next;
			next->num_decendientes++;
			if (value < next->ll) {
				next = next->lf;
			} else {
				if (value > next->ll) {
					next = next->rg;
				} else {
					if (value == next->ll) {
						if (pasajero_oscuro < next->pasajero_oscuro) {
							next = next->lf;
						} else {
							if (pasajero_oscuro > next->pasajero_oscuro) {
								next = next->rg;
							} else {
								avt_node_t *ancestro_actal = NULL;

								next->ocu++;

								ancestro_actal = next;
								while (ancestro_actal) {

									ancestro_actal->num_decendientes--;
									ancestro_actal = ancestro_actal->padre;
								}
								return;
							}
						}
					} else {

						ass(0);
					}
				}
			}
		}
		node = avt_create_node(tree);
		node->ll = value;
		node->pasajero_oscuro = pasajero_oscuro;
		if (value < last->ll) {
			last->lf = node;
		} else {
			if (value > last->ll) {
				last->rg = node;
			} else {
				if (pasajero_oscuro < last->pasajero_oscuro) {
					last->lf = node;
				} else {
					last->rg = node;
				}
			}
		}
		node->padre = last;
	}
	node->ocu = 1;
	avt_balance_insertar(tree, node, value, pasajero_oscuro);
}
static inline bool avt_es_hijo_perra(avt_node_t *nd) {
	bool es_hijo_perra = falso;
	if (nd->padre) {

		if ((td) nd->padre->lf == (td) nd) {

			es_hijo_perra = verdadero;
		}
	}
	return es_hijo_perra;
}
avt_node_t *avt_find(avt_t *tree, td value, td pasajero_oscuro, bool buscar_ant) {
	avt_node_t *current = tree->rt;
	avt_node_t *last_of_us = NULL;
	while (current) {
		if (value > current->ll) {
//			last_of_us = current;
			current = current->rg;
		} else {
			if (value < current->ll) {
				current = current->lf;
			} else {
				if (pasajero_oscuro != AVL_TREE_VALOR_INVALIDO) {
					if (pasajero_oscuro > current->pasajero_oscuro) {
						if (buscar_ant) {
							last_of_us = current;
						}
						current = current->rg;
					} else {
						if (pasajero_oscuro < current->pasajero_oscuro) {
							if (!buscar_ant) {
								last_of_us = current;
							}
							current = current->lf;
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
void avt_traverse_node_dfs(avt_node_t *node, int depth) {
	int i = 0;
	if (node->lf)
		avt_traverse_node_dfs(node->lf, depth + 2);
	for (i = 0; i < depth; i++)
		putchar(' ');
	printf("%d: %d\n", node->ll, avt_balance_factor(node));
	if (node->rg)
		avt_traverse_node_dfs(node->rg, depth + 2);
}
void avt_traverse_dfs(avt_t *tree) {
	avt_traverse_node_dfs(tree->rt, 0);
}
static inline char *avt_inoder_node_travesti(avt_node_t *nd, char *buf,
		int profundidad_maxima) {
	char num_buf[100] = { '\0' };
	int profundidad = 0;
	int i = 0;
	ass(profundidad_maxima == -1 || profundidad != -1);
	if (nd != NULL ) {
		profundidad = profundidad_maxima - nd->altura;
		ass(!nd->rg || nd->rg->padre == nd);
		avt_inoder_node_travesti(nd->rg, buf, profundidad_maxima);
		if (profundidad_maxima != -1) {
			for (i = 0; i < profundidad; i++) {
				strcat(buf, " ");
			}
		}
		sprintf(num_buf, "%d", nd->ll);
		strcat(buf, num_buf);
		if (profundidad_maxima != -1) {
			strcat(buf, "\n");
		}
		ass(!nd->lf || nd->lf->padre == nd);
		avt_inoder_node_travesti(nd->lf, buf, profundidad_maxima);

	}
	return buf;
}
static inline char *avt_inoder_node_travesti_conteo(avt_node_t *nd, char *buf,
		int profundidad_maxima) {
	char num_buf[100] = { '\0' };
	int profundidad = 0;
	int i = 0;
	ass(profundidad_maxima == -1 || profundidad != -1);
	if (nd != NULL ) {
		profundidad = profundidad_maxima - nd->altura;
		ass(!nd->rg || nd->rg->padre == nd);
		avt_inoder_node_travesti_conteo(nd->rg, buf, profundidad_maxima);
		if (profundidad_maxima != -1) {
			for (i = 0; i < profundidad; i++) {
				strcat(buf, " ");
			}
		}
		sprintf(num_buf, "%d [%u,%u] (%u) ocu %u", nd->ll, (nat ) (nd->ll),
				(nat ) nd->ll, nd->num_decendientes, nd->ocu);
		strcat(buf, num_buf);
		if (profundidad_maxima != -1) {
			strcat(buf, "\n");
		}
		ass(!nd->lf || nd->lf->padre == nd);
		avt_inoder_node_travesti_conteo(nd->lf, buf, profundidad_maxima);

	}
	return buf;
}
static inline avt_node_t* avt_sig_nd_inorder(avt_node_t *node) {
	avt_node_t *current = node;

	while (current->lf != NULL ) {
		current = current->lf;
	}
	return current;
}
static inline avt_node_t *avt_nd_borrar(avt_t *ablini, avt_node_t *rt, td key,
		bool ignora_conteo, td pasajero_oscuro) {
	if (rt == NULL ) {
		return rt;
	}
	if (key < rt->ll) {
		rt->lf = avt_nd_borrar(ablini, rt->lf, key, ignora_conteo,
				pasajero_oscuro);
		ass(!rt->lf || rt->lf->padre == rt);
	} else {
		if (key > rt->ll) {
			rt->rg = avt_nd_borrar(ablini, rt->rg, key, ignora_conteo,
					pasajero_oscuro);
			ass(!rt->rg || rt->rg->padre == rt);
		} else {
			if (pasajero_oscuro == AVL_TREE_VALOR_INVALIDO) {
				if ((rt->ocu - 1) == 0 || ignora_conteo) {
					if (rt->lf == NULL || rt->rg == NULL ) {

						avt_node_t *temp = rt->lf ? rt->lf : rt->rg;
						if (temp == NULL ) {
							temp = rt;
							rt = NULL;
						} else {
							nat idx_en_arreglo = 0;
							avt_node_t *padre = NULL;
							padre = rt->padre;
							idx_en_arreglo = rt->indice_en_arreglo;
							*rt = *temp;
							rt->padre = padre;
							rt->indice_en_arreglo = idx_en_arreglo;
							if (rt->lf) {
								rt->lf->padre = rt;
							}
							if (rt->rg) {
								rt->rg->padre = rt;
							}
						}
						ass(
								ablini->ultimo_idx_anadido
										- ablini->sig_idx_para_usar
										< ablini->max_nds);
						ablini->nds_libres_idx[ablini->ultimo_idx_anadido++
								% ablini->max_nds] = temp->indice_en_arreglo;
						memset(temp, 0, sizeof(avt_node_t));
						temp->ll = AVL_TREE_VALOR_INVALIDO;
						temp->pasajero_oscuro = AVL_TREE_VALOR_INVALIDO;
						ablini->nds_realmente_en_abl--;

					} else {

						avt_node_t *temp = avt_sig_nd_inorder(rt->rg);
						rt->ll = temp->ll;
						rt->pasajero_oscuro = temp->pasajero_oscuro;
						rt->ocu = temp->ocu;
						rt->rg = avt_nd_borrar(ablini, rt->rg, temp->ll,
								verdadero, temp->pasajero_oscuro);
					}
				} else {
					rt->ocu--;
					return rt;
				}
			} else {
				if (pasajero_oscuro < rt->pasajero_oscuro) {
					rt->lf = avt_nd_borrar(ablini, rt->lf, key, ignora_conteo,
							pasajero_oscuro);
					ass(!rt->lf || rt->lf->padre == rt);
				} else {
					if (pasajero_oscuro > rt->pasajero_oscuro) {
						rt->rg = avt_nd_borrar(ablini, rt->rg, key,
								ignora_conteo, pasajero_oscuro);
						ass(!rt->rg || rt->rg->padre == rt);
					} else {
						if ((rt->ocu - 1) == 0 || ignora_conteo) {
							if (rt->lf == NULL || rt->rg == NULL ) {

								avt_node_t *temp = rt->lf ? rt->lf : rt->rg;
								if (temp == NULL ) {
									temp = rt;
									rt = NULL;
								} else {
									nat idx_en_arreglo = 0;
									avt_node_t *padre = NULL;
									padre = rt->padre;
									idx_en_arreglo = rt->indice_en_arreglo;
									*rt = *temp;
									rt->padre = padre;
									rt->indice_en_arreglo = idx_en_arreglo;
									if (rt->lf) {
										rt->lf->padre = rt;
									}
									if (rt->rg) {
										rt->rg->padre = rt;
									}
								}
								ass(
										ablini->ultimo_idx_anadido
												- ablini->sig_idx_para_usar
												< ablini->max_nds);
								ablini->nds_libres_idx[ablini->ultimo_idx_anadido++
										% ablini->max_nds] =
										temp->indice_en_arreglo;
								memset(temp, 0, sizeof(avt_node_t));
								temp->ll = AVL_TREE_VALOR_INVALIDO;
								ablini->nds_realmente_en_abl--;

							} else {

								avt_node_t *temp = avt_sig_nd_inorder(
										rt->rg);
								rt->ll = temp->ll;
								rt->pasajero_oscuro = temp->pasajero_oscuro;
								rt->ocu = temp->ocu;
								rt->rg = avt_nd_borrar(ablini, rt->rg, temp->ll,
										verdadero, temp->pasajero_oscuro);
							}
						} else {
							rt->ocu--;
							return rt;
						}
					}
				}
			}
		}
	}
	if (rt == NULL ) {
		return rt;
	}
	avt_node_actizar_altura(rt);
	avt_node_actizar_num_decendientes(rt);
	int balance = avt_balance_factor(rt);
	if (balance > 1 && avt_balance_factor(rt->lf) >= 0) {
		return avt_rotate_lflf(rt);
	}
	if (balance > 1 && avt_balance_factor(rt->lf) < 0) {
		return avt_rotate_lfrg(rt);
	}
	if (balance < -1 && avt_balance_factor(rt->rg) <= 0) {
		return avt_rotate_rgrg(rt);
	}
	if (balance < -1 && avt_balance_factor(rt->rg) > 0) {
		return avt_rotate_rglf(rt);
	}
	return rt;
}
void avt_borrar(avt_t *tree, td value, td pasajero_oscuro) {
	avt_node_t *newrt = NULL;
	if (!tree->rt) {
		return;
	}
	newrt = avt_nd_borrar(tree, tree->rt, value, falso, pasajero_oscuro);
	if (newrt != tree->rt) {
		tree->rt = newrt;
	}
}
static inline avt_node_t* avt_nd_posicion_anterior(avt_t *ablin,
		avt_node_t *nd_act) {
	avt_node_t *nd_sig;
	nd_sig = nd_act;
	if (nd_sig->lf) {
		nd_sig = nd_sig->lf;
		while (nd_sig->rg) {
			nd_sig = nd_sig->rg;
		}
	} else {
		avt_node_t *last_of_us = nd_sig;
		nd_sig = nd_sig->padre;
		while (nd_sig) {
			if (!avt_es_hijo_perra(last_of_us)) {
				break;
			}
			last_of_us = nd_sig;
			nd_sig = nd_sig->padre;
		}
	}
	return nd_sig;
}
static inline avt_node_t* avt_nd_posicion_sig(avt_t *ablin,
		avt_node_t *nd_act) {
	avt_node_t *nd_sig = NULL;
	nd_sig = nd_act;
	if (nd_sig->rg) {
		nd_sig = nd_sig->rg;
		while (nd_sig->lf) {
			nd_sig = nd_sig->lf;
		}
	} else {
		avt_node_t *last_of_us = nd_sig;
		nd_sig = nd_sig->padre;
		while (nd_sig) {
			if (avt_es_hijo_perra(last_of_us)) {
				break;
			}
			last_of_us = nd_sig;
			nd_sig = nd_sig->padre;
		}
	}
	return nd_sig;
}
#endif
int numeros[MAX_NUMEROS + 2] = { 0 };
nat numeros_tam = 0;
mo_mada consultas[MAX_QUERIES] = { 0 };
nat consultas_tam = 0;
bit_ch *biatch = &(bit_ch ) { 0 };
hm_rr_bs_tabla *ocu_mapa = &(hm_rr_bs_tabla ) { 0 };
hm_rr_bs_tabla *ultimo_idx_mapa = &(hm_rr_bs_tabla ) { 0 };
void cax_anade_caca(td numero) {
	enl_sin_signo cardinalidad_act = 0;
	hm_iter iter = 0;
	iter = hmrh_obten(ocu_mapa, (nat) numero,
			(enl*) &cardinalidad_act);
	if (!cardinalidad_act) {
		mo_mada_resultado += numero;
	}
	hmrh_indice_pon_valor(ocu_mapa, iter,
			cardinalidad_act + 1);
}
void cax_quita_caca(td numero) {
	enl_sin_signo cardinalidad_act = 0;
	hm_iter iter = 0;
	iter = hmrh_obten(ocu_mapa, (nat) numero,
			(enl*) &cardinalidad_act);
	hmrh_indice_pon_valor(ocu_mapa, iter,
			cardinalidad_act - 1);
	ass(cardinalidad_act >= 0);
	if (!(cardinalidad_act - 1)) {
		mo_mada_resultado -= numero;
	}
}
char buf[1000000] = { '\0' };
static inline void cax_main() {
	nat num_queries = 0;
	nat cont_queries = 0;
	char tipo_query = 0;
	nat idx_query_ini = 0;
	nat idx_query_fin = 0;
	avt_t *ablin = NULL;
	bitch_init();
	nds_trozo = calloc(TRT_MAX_NODOS, sizeof(trt));
	scanf("%u\n", &numeros_tam);
	lee_matrix_long_stdin(numeros + 1, &(int ) { 1 }, NULL, 1, numeros_tam + 1);
	scanf("%u\n", &num_queries);
	consultas_tam = num_queries;
	while (cont_queries < consultas_tam) {
		mo_mada *consul_act = consultas + cont_queries;
		scanf("%c %d %d\n", &tipo_query, &idx_query_ini, &idx_query_fin);
		if (tipo_query == 'Q' && idx_query_ini > idx_query_fin) {
			td tmp = 0;
			tmp = idx_query_fin;
			idx_query_fin = idx_query_ini;
			idx_query_ini = tmp;
		}

		setbuf(stdout, NULL );
		consul_act->idx_query = cont_queries;
		consul_act->intervalo_idx_ini = idx_query_ini;
		consul_act->intervalo_idx_fin = idx_query_fin;
		consul_act->orden = cont_queries;
		consul_act->tipo = tipo_query;
		cont_queries++;
	}
	hmrh_init(ocu_mapa, numeros_tam << 2);
	hmrh_init(ultimo_idx_mapa, numeros_tam << 2);
	for (int i = 0; i <= numeros_tam; i++) {
		hmrh_pon(ocu_mapa, (nat) numeros[i], 0,
				&(bool ) { 0 });
		hmrh_pon(ultimo_idx_mapa, (nat) numeros[i], 0,
				&(bool ) { 0 });
	}
	mo_mada_core(consultas, numeros, consultas_tam, numeros_tam);
	setbuf(stdout, NULL );
#ifdef CX_LOG
	for (int i = 0; i < consultas_tam; i++) {

	}
#endif
	avt_create(&ablin, MAX_NUMEROS);
	for (int i = 1; i <= numeros_tam; i++) {
		td numero_act = numeros[i];
		avt_insert(ablin, numero_act, i);
	}
#ifdef CX_LOG
	for (int i = 1; i <= numeros_tam; i++) {
		td numero_act = numeros[i];
		avt_node_t *nd_caca = NULL;

		nd_caca = avt_find(ablin, numero_act, i - 1,verdadero);
		if (nd_caca) {

		} else {

		}

		nd_caca = avt_find(ablin, numero_act, i + 1,falso);
		if (nd_caca) {

		} else {

		}
	}
#endif
	bit_ch_init(biatch, 0, numeros_tam + 2, numeros);
	trt_numeros_tam = numeros_tam + 2;
	for (int i = 0; i < consultas_tam; i++) {
		mo_mada *consul = consultas + i;
		ass(
				consul->tipo == mo_mada_consulta
						|| consul->tipo == mo_mada_actizacion);
		if (consul->tipo == mo_mada_actizacion) {
			nat idx_actizar = consul->intervalo_idx_ini;
			nat idx_viejo_ant = 0;
			nat idx_viejo_pos = numeros_tam + 1;
			nat idx_nuevo_ant = 0;
			nat idx_nuevo_pos = numeros_tam + 1;
			td valor_nuevo = consul->intervalo_idx_fin;
			td viejo_pendejo = numeros[idx_actizar];
			avt_node_t *ocurrencia_viejo_ant = NULL;
			avt_node_t *ocurrencia_viejo_act = NULL;
			avt_node_t *ocurrencia_viejo_pos = NULL;
			avt_node_t *ocurrencia_nuevo_ant = NULL;
			avt_node_t *ocurrencia_nuevo_pos = NULL;

			ocurrencia_viejo_act = avt_find(ablin, viejo_pendejo,
					idx_actizar, verdadero);
			ass(
					ocurrencia_viejo_act
							&& ocurrencia_viejo_act->ll == viejo_pendejo
							&& ocurrencia_viejo_act->pasajero_oscuro
									== idx_actizar);
			ocurrencia_viejo_ant = avt_nd_posicion_anterior(ablin,
					ocurrencia_viejo_act);
			ocurrencia_viejo_pos = avt_nd_posicion_sig(ablin,
					ocurrencia_viejo_act);

			if (ocurrencia_viejo_ant
					&& ocurrencia_viejo_ant->ll == viejo_pendejo) {
				idx_viejo_ant = ocurrencia_viejo_ant->pasajero_oscuro;
			}
			if (ocurrencia_viejo_pos
					&& ocurrencia_viejo_pos->ll == viejo_pendejo) {
				idx_viejo_pos = ocurrencia_viejo_pos->pasajero_oscuro;
			}

			ocurrencia_nuevo_ant = avt_find(ablin, valor_nuevo, idx_actizar,
					verdadero);
			ass(
					!ocurrencia_nuevo_ant
							|| ocurrencia_nuevo_ant->ll != valor_nuevo
							|| ocurrencia_nuevo_ant->pasajero_oscuro
									!= idx_actizar
							|| valor_nuevo == numeros[idx_actizar]);

			if (ocurrencia_nuevo_ant) {
				ocurrencia_nuevo_pos = avt_nd_posicion_sig(ablin,
						ocurrencia_nuevo_ant);
			} else {
				ocurrencia_nuevo_pos = avt_find(ablin, valor_nuevo,
						idx_actizar, falso);
			}

			ass(
					!ocurrencia_nuevo_pos
							|| ocurrencia_nuevo_pos->ll != valor_nuevo
							|| ocurrencia_nuevo_pos->pasajero_oscuro
									!= idx_actizar);
			if (ocurrencia_nuevo_ant
					&& ocurrencia_nuevo_ant->ll == valor_nuevo) {
				idx_nuevo_ant = ocurrencia_nuevo_ant->pasajero_oscuro;
			}
			if (ocurrencia_nuevo_pos
					&& ocurrencia_nuevo_pos->ll == valor_nuevo) {
				idx_nuevo_pos = ocurrencia_nuevo_pos->pasajero_oscuro;
			}

			bit_ch_actiza(biatch, idx_viejo_ant, idx_actizar,
					idx_viejo_pos, -viejo_pendejo);
			bit_ch_actiza(biatch, idx_nuevo_ant, idx_actizar,
					idx_nuevo_pos, valor_nuevo);
			avt_borrar(ablin, viejo_pendejo, idx_actizar);
			avt_insert(ablin, valor_nuevo, idx_actizar);
			numeros[idx_actizar] = valor_nuevo;
		} else {
			enl delta = 0;
			enl resu = 0;
			nat idx_ini = consul->intervalo_idx_ini;
			nat idx_fin = consul->intervalo_idx_fin;

			delta = bit_ch_consulta_trozos(biatch, idx_ini, idx_fin);

			resu = consul->resulcaca + delta;

			printf("%lld\n", resu);
		}
	}
}
int main(void) {
//	puts("he corrido con algo de suerte");
//	sleep(10);
	cax_main();
	return EXIT_SUCCESS;
}
