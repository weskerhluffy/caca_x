#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stddef.h>
#include <unistd.h>
#define MAX_NUMEROS 50000
#define MAX_NUMEROS_REDONDEADO 65536
#define MAX_VALOR INT_MAX
#define MAX_QUERIES 100000
// 12 es 10 de el num, 1 del espacio 1 de signo
#define TAM_MAX_LINEA (MAX_NUMEROS*12) 
#define MAX_NODOS (1 << 16)
#define CACA_X_VALOR_INVALIDO -1
#define CACA_X_MAX_VALORES_INT ((unsigned int)((unsigned int)INT_MAX-(INT_MIN)))
#define CACA_X_BUF_STATICO_DUMP_ARBOL (char[1000] ) { '\0' }
#define caca_log_debug(formato, args...) 0
#define ass(condition) assert(condition);
typedef int td;
typedef unsigned int nat;
typedef long long entero_largo;
typedef unsigned long long bitch_vector;
typedef enum BOOLEANOS {
	falso = 0, verdadero
} bool;
#define max(x,y) ((x) < (y) ? (y) : (x))
#define min(x,y) ((x) < (y) ? (x) : (y))
#if 1
#define AVL_TREE_VALOR_INVALIDO CACA_X_VALOR_INVALIDO
struct avt_node_s {
	td ll;
	nat altura;
	nat num_decendientes;
	nat indice_en_arreglo;
	nat ocu;
	struct avt_node_s *lf;
	struct avt_node_s *rg;
	struct avt_node_s *padre;
};
typedef struct avt_node_s avt_node_t;
struct avt_s {
	nat max_nds;
	nat nds_realmente_en_abl;
	nat nds_usados;
	nat nds_realmente_en_abl_utiles;
	nat max_nds_utiles;
	nat ocu_totales;
	struct avt_node_s *rt;
	avt_node_t *nds_mem;
	nat *nds_libres_idx;
	unsigned long siguiente_idx_para_usar;
	unsigned long ultimo_idx_anadido;
};
typedef struct avt_s avt_t;
typedef struct avt_iterator_t {
	avt_t *ablin;
	char *contador_visitas;
	avt_node_t *nd_actual;
} avt_iterator_t;
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
static inline void avt_destroy(avt_t *ablin) {
	free(ablin->nds_mem);
	free(ablin->nds_libres_idx);
	free(ablin);
}
avt_node_t *avt_create_node(avt_t *ablin, td ll) {
	avt_node_t *node = NULL;
	ass(
			ablin->siguiente_idx_para_usar < ablin->ultimo_idx_anadido
					|| ((ablin->siguiente_idx_para_usar
							== ablin->ultimo_idx_anadido)
							&& ablin->nds_usados < ablin->max_nds));
	if (ablin->siguiente_idx_para_usar < ablin->ultimo_idx_anadido) {
		node = ablin->nds_mem
				+ ablin->nds_libres_idx[ablin->siguiente_idx_para_usar
						% ablin->max_nds];
		node->indice_en_arreglo =
				ablin->nds_libres_idx[ablin->siguiente_idx_para_usar
						% ablin->max_nds];
		ablin->nds_libres_idx[ablin->siguiente_idx_para_usar
				% ablin->max_nds] = 0xffffffff;
		ablin->siguiente_idx_para_usar++;
	} else {
		node = ablin->nds_mem + ablin->nds_usados++;
		node->indice_en_arreglo = ablin->nds_usados - 1;
	}
	node->ll = ll;
	ablin->nds_realmente_en_abl++;
	ablin->nds_realmente_en_abl_utiles++;
	return node;
}
int avt_node_height(avt_node_t *node) {
	return node ? node->altura : 0;
}
static inline void avt_node_actualizar_altura(avt_node_t *node) {
	int height_lf = 0;
	int height_rg = 0;
	if (node->lf) {
		height_lf = node->lf->altura;
	}
	if (node->rg) {
		height_rg = node->rg->altura;
	}
	if (node->lf || node->rg) {
		node->altura = (height_rg > height_lf ? height_rg : height_lf)
				+ 1;
	} else {
		node->altura = 0;
	}
}
static inline void avt_node_actualizar_num_decendientes(
		avt_node_t *node) {
	int conteo_lf = 0;
	int conteo_rg = 0;
	if (node->lf) {
		conteo_lf = node->lf->num_decendientes;
	}
	if (node->rg) {
		conteo_rg = node->rg->num_decendientes;
	}
	if (node->lf || node->rg) {
		node->num_decendientes = conteo_lf + conteo_rg
				+ (node->lf ? 1 : 0) + (node->rg ? 1 : 0);
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
	avt_node_actualizar_altura(a);
	avt_node_actualizar_altura(b);
	avt_node_actualizar_num_decendientes(a);
	avt_node_actualizar_num_decendientes(b);
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
	avt_node_actualizar_altura(a);
	avt_node_actualizar_altura(b);
	avt_node_actualizar_altura(c);
	avt_node_actualizar_num_decendientes(a);
	avt_node_actualizar_num_decendientes(b);
	avt_node_actualizar_num_decendientes(c);
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
	avt_node_actualizar_altura(a);
	avt_node_actualizar_altura(b);
	avt_node_actualizar_altura(c);
	avt_node_actualizar_num_decendientes(a);
	avt_node_actualizar_num_decendientes(b);
	avt_node_actualizar_num_decendientes(c);
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
	avt_node_actualizar_altura(a);
	avt_node_actualizar_altura(b);
	avt_node_actualizar_num_decendientes(a);
	avt_node_actualizar_num_decendientes(b);
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
static inline avt_node_t *avt_balance_node_insertar(
		const avt_node_t *node, const td ll_nueva) {
	avt_node_t *newrt = NULL;
	avt_node_t *nd_actual = NULL;
	newrt = (avt_node_t *) node;
	nd_actual = node->padre;
	while (nd_actual) {
		int bf = 0;
		avt_node_t *padre = NULL;
		avt_node_t **rama_padre = NULL;
		bf = avt_balance_factor(nd_actual);
		if (bf >= 2) {
			
			if (ll_nueva > nd_actual->lf->ll) {
				newrt = avt_rotate_lfrg(nd_actual);
			} else {
				newrt = avt_rotate_lflf(nd_actual);
			}
		} else if (bf <= -2) {
			
			if (ll_nueva < nd_actual->rg->ll) {
				newrt = avt_rotate_rglf(nd_actual);
			} else {
				newrt = avt_rotate_rgrg(nd_actual);
			}
		} else {
			
			newrt = nd_actual;
			avt_node_actualizar_altura(nd_actual);
		}
		if (newrt->padre) {
			padre = newrt->padre;
			if (ll_nueva < padre->ll) {
				rama_padre = &padre->lf;
			} else {
				if (ll_nueva > padre->ll) {
					rama_padre = &padre->rg;
				} else {
					ass(0);
				}
			}
			*rama_padre = newrt;
		}
		nd_actual = nd_actual->padre;
	}
	return (newrt);
}
void avt_balance_insertar(avt_t *tree, avt_node_t *nd,
		td ll_nueva) {
	avt_node_t *newrt = NULL;
	newrt = avt_balance_node_insertar(nd, ll_nueva);
	if (newrt != tree->rt) {
		tree->rt = newrt;
	}
}
avt_node_t *avt_insert(avt_t *tree, td value) {
	avt_node_t *node = NULL;
	avt_node_t *next = NULL;
	avt_node_t *last = NULL;
	tree->ocu_totales++;
	
	if (tree->rt == NULL ) {
		node = avt_create_node(tree, value);
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
						avt_node_t *ancestro_actal = NULL;
						
						next->ocu++;
					
						ancestro_actal = next;
						while (ancestro_actal) {
						
							ancestro_actal->num_decendientes--;
							ancestro_actal = ancestro_actal->padre;
						}
						return next;
					} else {
					
						ass(0);
					}
				}
			}
		}
		node = avt_create_node(tree, value);
		if (value < last->ll) {
			last->lf = node;
		}
		if (value > last->ll) {
			last->rg = node;
		}
		node->padre = last;
	}
	node->ocu = 1;
	avt_balance_insertar(tree, node, value);
	return node;
}
avt_node_t *avt_find(avt_t *tree, td value) {
	avt_node_t *current = tree->rt;
	while (current && current->ll != value) {
		if (value > current->ll)
			current = current->rg;
		else
			current = current->lf;
	}
	return current ? current->ll == value ? current : NULL :NULL;
}
avt_node_t *avt_find_descartando(avt_node_t *nd_raiz,
		avt_node_t **primer_nd_mayor_o_igual, td value,
		td tope, bool *tope_topado) {
	avt_node_t *current = NULL;
	avt_node_t *primer_nd_mayor = NULL;
	current = nd_raiz;
	ass(!tope_topado || tope || !nd_raiz->ll);
	ass(!tope_topado || tope >= value);
	if (tope_topado) {
		*tope_topado = falso;
	}
	do {
		if (tope_topado) {
			if ((current->ll > tope && !current->lf)
					|| current->ll == tope) {
				*tope_topado = verdadero;
				break;
			}
		}
		if (value > current->ll) {
			current = current->rg;
		} else {
			if (value < current->ll) {
				if (!primer_nd_mayor) {
					primer_nd_mayor = current;
				}
				current = current->lf;
			} else {
				break;
			}
		}
	} while (current && current->ll != value);
	*primer_nd_mayor_o_igual = primer_nd_mayor;
	if (!*primer_nd_mayor_o_igual) {
		if (current && (current->ll == value)) {
			*primer_nd_mayor_o_igual = current;
		}
	}
	if (tope_topado && current && current->ll >= tope && value >= tope) {
		*tope_topado = verdadero;
	}
	return current ? current->ll == value ? current : NULL :NULL;
}
	
void avt_traverse_node_dfs(avt_node_t *node, int depth) {
	int i = 0;
	if (node->lf)
		avt_traverse_node_dfs(node->lf, depth + 2);
	for (i = 0; i < depth; i++)
		putchar(' ');
	printf("%u: %d\n", node->ll, avt_balance_factor(node));
	if (node->rg)
		avt_traverse_node_dfs(node->rg, depth + 2);
}
void avt_traverse_dfs(avt_t *tree) {
	avt_traverse_node_dfs(tree->rt, 0);
}
static inline void avt_iterador_ini(avt_t *ablin,
		avt_iterator_t *iter) {
	iter->contador_visitas = calloc(ablin->nds_usados, sizeof(char));
	ass(iter->contador_visitas);
	iter->ablin = ablin;
}
static inline void avt_iterador_fini(avt_iterator_t *iter) {
	free(iter->contador_visitas);
}
static inline bool avt_iterador_hay_siguiente(avt_iterator_t *iter) {
	return iter->ablin->rt
			&& !(iter->nd_actual == iter->ablin->rt
					&& iter->contador_visitas[iter->ablin->rt->indice_en_arreglo]
							== 2);
}
static inline avt_node_t* avt_iterador_siguiente(
		avt_iterator_t *iter) {
	int contador_actual = 0;
	avt_node_t *nd = NULL;
	avt_node_t *last_of_us = NULL;
	avt_node_t *nd_actual = NULL;
	if (!iter->nd_actual) {
		nd_actual = iter->nd_actual = iter->ablin->rt;
	}
	if (!avt_iterador_hay_siguiente(iter)) {
		return NULL ;
	}
	contador_actual =
			iter->contador_visitas[iter->nd_actual->indice_en_arreglo];
	iter->contador_visitas[iter->nd_actual->indice_en_arreglo]++;
	switch (contador_actual) {
	case 0:
	case 1:
		if (!contador_actual) {
			nd_actual = iter->nd_actual->lf;
			if (!nd_actual) {
				return iter->nd_actual;
			}
		} else {
			nd_actual = iter->nd_actual->rg;
			if (!nd_actual) {
				nd_actual = iter->nd_actual->padre;
				while (nd_actual
						&& iter->contador_visitas[nd_actual->indice_en_arreglo]
								== 2) {
					last_of_us = nd_actual;
					nd_actual = nd_actual->padre;
				}
				if (!nd_actual) {
					if (last_of_us) {
						iter->nd_actual = last_of_us;
					}
				} else {
					iter->nd_actual = nd_actual;
				}
				return nd_actual;
			}
		}
		while (nd_actual) {
			last_of_us = nd_actual;
			iter->contador_visitas[nd_actual->indice_en_arreglo]++;
			nd_actual = last_of_us->lf;
		}
		nd = iter->nd_actual = last_of_us;
		break;
	default:
		ass(0)
		;
		break;
	}
	return nd;
}
static inline avt_node_t* avt_iterador_obtener_actual(
		avt_iterator_t *iter) {
	avt_node_t *nd = NULL;
	if (!iter->nd_actual) {
		avt_iterador_siguiente(iter);
	}
	nd = iter->nd_actual;
	return nd;
}
static inline avt_node_t* avt_siguiente_nd_inorder(
		avt_node_t *node) {
	avt_node_t *current = node;
	
	while (current->lf != NULL ) {
		current = current->lf;
	}
	return current;
}
static inline avt_node_t *avt_nd_borrar(avt_t *ablini,
		avt_node_t *rt, td key, bool ignora_conteo) {
	// STEP 1: PERFORM STANDARD BST DELETE
	if (rt == NULL ) {
		return rt;
	}
	if (key < rt->ll) {
		// If the key to be deleted is smaller than the rt's key,
		// then it lies in lf subtree
		rt->lf = avt_nd_borrar(ablini, rt->lf, key,
				ignora_conteo);
		ass(!rt->lf || rt->lf->padre == rt);
	} else {
		// If the key to be deleted is greater than the rt's key,
		// then it lies in rg subtree
		if (key > rt->ll) {
			rt->rg = avt_nd_borrar(ablini, rt->rg, key,
					ignora_conteo);
			ass(!rt->rg || rt->rg->padre == rt);
		} else {
			if (!ignora_conteo) {
				ablini->ocu_totales--;
			}
			if ((rt->ocu - 1) == 0 || ignora_conteo) {
				if (rt->lf == NULL || rt->rg == NULL ) {
					avt_node_t *temp =
							rt->lf ? rt->lf : rt->rg;
					// No child case
					if (temp == NULL ) {
						temp = rt;
						rt = NULL;
					} else {
						// One child case
						nat idx_en_arreglo = 0;
						avt_node_t *padre = NULL;
						padre = rt->padre;
						idx_en_arreglo = rt->indice_en_arreglo;
						*rt = *temp;// Copy the contents of the non-empty child
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
									- ablini->siguiente_idx_para_usar
									< ablini->max_nds);
					ablini->nds_libres_idx[ablini->ultimo_idx_anadido++
							% ablini->max_nds] = temp->indice_en_arreglo;
					memset(temp, 0, sizeof(avt_node_t));
					temp->ll = AVL_TREE_VALOR_INVALIDO;
					ablini->nds_realmente_en_abl--;
					ablini->nds_realmente_en_abl_utiles--;
				
				} else {
					// node with two children: Get the inorder successor (smallest
					// in the rg subtree)
					avt_node_t *temp = avt_siguiente_nd_inorder(
							rt->rg);
					// Copy the inorder successor's data to this node
					rt->ll = temp->ll;
					rt->ocu = temp->ocu;
					// Delete the inorder successor
					rt->rg = avt_nd_borrar(ablini, rt->rg,
							temp->ll, verdadero);
				}
			} else {
				rt->ocu--;
				return rt;
			}
		}
	}
// If the tree had only one node then return
	if (rt == NULL ) {
		return rt;
	}
// STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
	avt_node_actualizar_altura(rt);
	avt_node_actualizar_num_decendientes(rt);
// STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to check whether
//  this node became unbalanced)
	int balance = avt_balance_factor(rt);
// If this node becomes unbalanced, then there are 4 cases
// Left Left Case
	if (balance > 1 && avt_balance_factor(rt->lf) >= 0) {
		return avt_rotate_lflf(rt);
	}
// Left Right Case
	if (balance > 1 && avt_balance_factor(rt->lf) < 0) {
		return avt_rotate_lfrg(rt);
	}
// Right Right Case
	if (balance < -1 && avt_balance_factor(rt->rg) <= 0) {
		return avt_rotate_rgrg(rt);
	}
// Right Left Case
	if (balance < -1 && avt_balance_factor(rt->rg) > 0) {
		return avt_rotate_rglf(rt);
	}
	return rt;
}
void avt_borrar(avt_t *tree, td value) {
	avt_node_t *newrt = NULL;
	if (!tree->rt) {
		return;
	}
	newrt = avt_nd_borrar(tree, tree->rt, value, falso);
	if (newrt != tree->rt) {
		tree->rt = newrt;
	}
}
#endif
typedef struct cax_numeros_unicos_en_rango {
	avt_t *ablazo;
	nat max_numeros;
	nat max_num_esperados;
	int altura;
	int idx;
	int limite_izq;
	int limite_der;
} cax_numeros_unicos_en_rango;
typedef struct cax_estado_recursion {
	int profundidad;
	int idx_ini;
	int idx_fin;
	int idx_nd;
	int num_popeado;
	cax_numeros_unicos_en_rango *nd;
} cax_estado_recursion;
cax_estado_recursion *estado = NULL;
static inline int lee_matrix_long_stdin(td *matrix, int *num_filas,
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
		
			indice_columnas++;
		
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
static inline void cax_inicializar_nd(cax_numeros_unicos_en_rango *nd,
		int altura, int idx_nd, int limite_izq, int limite_der) {
	nd->altura = altura;
	nd->max_numeros = 1 << nd->altura;
	avt_create(&nd->ablazo, nd->max_numeros);
	nd->idx = idx_nd;
	nd->limite_izq = limite_izq;
	nd->limite_der = limite_der;
}
static inline void cax_clona_nds(avt_t *abl_dst,
		avt_node_t *nd_ori, avt_node_t *nd_dst_padre,
		avt_node_t **apuntador_hijo) {
	int idx_en_arreglo_real = 0;
	avt_node_t *nd_nuevo = NULL;
	if (!nd_ori) {
		return;
	}
	nd_nuevo = avt_create_node(abl_dst, nd_ori->ll);
	idx_en_arreglo_real = nd_nuevo->indice_en_arreglo;
	memcpy(nd_nuevo, nd_ori, offsetof(avt_node_t,lf));
	nd_nuevo->indice_en_arreglo = idx_en_arreglo_real;
	if (!abl_dst->rt) {
		abl_dst->rt = nd_nuevo;
	}
	if (nd_dst_padre) {
		nd_nuevo->padre = nd_dst_padre;
		*apuntador_hijo = nd_nuevo;
	}
	cax_clona_nds(abl_dst, nd_ori->lf, nd_nuevo,
			&nd_nuevo->lf);
	cax_clona_nds(abl_dst, nd_ori->rg, nd_nuevo,
			&nd_nuevo->rg);
}
static inline void cax_clona_abl(avt_t *abl_dst,
		avt_t *abl_ori) {
	int tam_abl_dst = 0;
	int tam_abl_ori = 0;
	tam_abl_dst = abl_dst->max_nds;
	tam_abl_ori = abl_ori->nds_realmente_en_abl_utiles;
	ass(tam_abl_dst >= tam_abl_ori);
	ass(!abl_dst->nds_usados);
	cax_clona_nds(abl_dst, abl_ori->rt, NULL, NULL );
	abl_dst->ocu_totales = abl_ori->ocu_totales;
}
static inline void cax_mergear_ables(avt_t *ablin_izq,
		avt_t *ablin_der, avt_t *ablin_resultante) {
	int tam_abl_izq = 0;
	int tam_abl_der = 0;
	avt_iterator_t *iter = &(avt_iterator_t ) { 0 };
	avt_t *abl_menor = NULL;
	avt_t *abl_mayor = NULL;
	ass(ablin_izq && ablin_der);
	tam_abl_izq = ablin_izq->nds_realmente_en_abl_utiles;
	tam_abl_der = ablin_der->nds_realmente_en_abl_utiles;
	if (tam_abl_izq <= tam_abl_der) {
		abl_menor = ablin_izq;
		abl_mayor = ablin_der;
	} else {
		abl_menor = ablin_der;
		abl_mayor = ablin_izq;
	}
	cax_clona_abl(ablin_resultante, abl_mayor);
	ass(
			ablin_resultante->nds_realmente_en_abl_utiles
					== abl_mayor->nds_realmente_en_abl_utiles);
	avt_iterador_ini(abl_menor, iter);
	while (avt_iterador_hay_siguiente(iter)) {
		int numero_actual = 0;
		avt_node_t *nd_actual = NULL;
		avt_node_t *nd_encontrado = NULL;
		nd_actual = avt_iterador_obtener_actual(iter);
		numero_actual = nd_actual->ll;
		if (!(nd_encontrado = avt_find(ablin_resultante, numero_actual))) {
			nd_encontrado = avt_insert(ablin_resultante,
					numero_actual);
			nd_encontrado->ocu = nd_actual->ocu;
			ablin_resultante->ocu_totales += nd_actual->ocu
					- 1;
		} else {
			nd_encontrado->ocu += nd_actual->ocu;
			ablin_resultante->ocu_totales += nd_actual->ocu;
		}
		avt_iterador_siguiente(iter);
	}
	avt_iterador_fini(iter);
}
static inline void cax_construye_abl_binario_segmentado(int *numeros,
		cax_numeros_unicos_en_rango *abl_numeros_unicos, int num_numeros,
		int max_profundidad, nat ultimo_indice_numero_valido) {
	int profundidad = -1;
	estado->idx_ini = 0;
	estado->idx_fin = num_numeros;
	estado->idx_nd = 0;
	estado->nd = NULL;
	estado->profundidad = 0;
	estado->num_popeado = 0;
	profundidad++;
	while (profundidad != -1) {
		int idx_ini = 0;
		int idx_fin = 0;
		int idx_nd = 0;
		int altura = 0;
		cax_numeros_unicos_en_rango *nd = NULL;
		cax_estado_recursion *estado_actual = NULL;
		estado_actual = estado + profundidad;
		ass(estado_actual->profundidad == profundidad);
	
		idx_ini = estado_actual->idx_ini;
		idx_fin = estado_actual->idx_fin;
		idx_nd = estado_actual->idx_nd;
		altura = max_profundidad - profundidad;
	
		if (!estado_actual->num_popeado) {
			nd = abl_numeros_unicos + idx_nd;
			cax_inicializar_nd(nd, altura, idx_nd, idx_ini, idx_fin);
		}
		estado_actual->num_popeado++;
		profundidad--;
		if (altura) {
			int distancia_media = 0;
			int idx_medio = 0;
			int ultimo_num_popeado = 0;
			cax_estado_recursion *estado_futuro = NULL;
			if (estado_actual->num_popeado == 1
					|| estado_actual->num_popeado == 2) {
			
				estado_futuro = estado + estado_actual->profundidad + 1;
			
				if (estado_actual->num_popeado == 1) {
					estado_actual->nd = nd;
				}
				memset(estado_futuro, 0, sizeof(cax_estado_recursion));
				estado_futuro->profundidad = estado_actual->profundidad + 1;
				distancia_media = (idx_fin - idx_ini) / 2;
				idx_medio = idx_ini + distancia_media;
			}
			ultimo_num_popeado = estado_actual->num_popeado;
			switch (estado_actual->num_popeado) {
			case 1:
				estado_futuro->idx_ini = idx_ini;
				estado_futuro->idx_fin = idx_medio;
				estado_futuro->idx_nd = 2 * idx_nd + 1;
			
				break;
			case 2:
				estado_futuro->idx_ini = idx_medio + 1;
				estado_futuro->idx_fin = idx_fin;
				estado_futuro->idx_nd = 2 * idx_nd + 2;
			
				break;
			case 3: {
				cax_numeros_unicos_en_rango *hijo_izq = NULL;
				cax_numeros_unicos_en_rango *hijo_der = NULL;
				avt_t *ablin_izq = NULL;
				avt_t *ablin_der = NULL;
				hijo_izq = abl_numeros_unicos + (idx_nd * 2 + 1);
				hijo_der = abl_numeros_unicos + (idx_nd * 2 + 2);
				ablin_izq = hijo_izq->ablazo;
				ablin_der = hijo_der->ablazo;
				cax_mergear_ables(ablin_izq, ablin_der,
						estado_actual->nd->ablazo);
				estado_actual->nd->max_num_esperados =
						(ultimo_indice_numero_valido
								>= estado_actual->nd->limite_der) ?
								estado_actual->nd->max_numeros :
						(ultimo_indice_numero_valido
								>= estado_actual->nd->limite_izq) ?
								(ultimo_indice_numero_valido
										- estado_actual->nd->limite_izq + 1) :
								0;
				estado_actual->nd->ablazo->max_nds_utiles =
						estado_actual->nd->max_num_esperados;
#ifdef CACA_X_VALIDAR_ARBOLINES
				avt_validar_ablin_indices_ex(
						estado_actual->nd->ablazo,
						estado_actual->nd->ablazo->rt, verdadero);
#endif
				memset(estado_actual, 0, sizeof(cax_estado_recursion));
			
			}
				break;
			default:
				ass(0)
				;
				break;
			}
			if (ultimo_num_popeado < 3) {
				profundidad += 2;
			
			}
		} else {
			int numero_actual = 0;
			ass(idx_ini == idx_fin);
			ass(nd);
			numero_actual = numeros[idx_ini];
			ass(nd->max_numeros == 1);
			ass(nd->limite_izq == nd->limite_der);
			ass(nd->ablazo->max_nds == 1);
			ass(!nd->ablazo->nds_realmente_en_abl_utiles);
			ass(!avt_find(nd->ablazo, numero_actual));
			avt_insert(nd->ablazo, (td) numero_actual);
			nd->ablazo->max_nds_utiles = nd->max_num_esperados = 1;
		
			memset(estado_actual, 0, sizeof(cax_estado_recursion));
		}
	}
}
static inline void cax_suma_unicos(entero_largo *sumas_abl_segmentado,
		cax_numeros_unicos_en_rango *abl_numeros_unicos, int num_nds) {
	int *numeros_unicos = NULL;
	char *buf = NULL;
	buf = calloc(1000, sizeof(char));
	numeros_unicos = calloc(MAX_NODOS, sizeof(int));
	assert(numeros_unicos);
	for (int i = 0; i < num_nds; i++) {
		int num_numeros_unicos = 0;
		memset(buf, '\0', 1000);
		cax_numeros_unicos_en_rango *nd = NULL;
		avt_t *ablazo_actual = NULL;
		avt_iterator_t * iterador = &(avt_iterator_t ) { 0 };
		nd = abl_numeros_unicos + i;
		if (!nd->ablazo) {
			continue;
		}
		ass(i == nd->idx);
		ablazo_actual = nd->ablazo;
	
	
		avt_iterador_ini(ablazo_actual, iterador);
		sumas_abl_segmentado[i] = 0;
		while (avt_iterador_hay_siguiente(iterador)) {
			int numero_unico_actual = 0;
			avt_node_t *nd_abl_actual = NULL;
			nd_abl_actual =
					(avt_node_t*) avt_iterador_obtener_actual(
							iterador);
			if (nd_abl_actual) {
				ass(num_numeros_unicos<MAX_NODOS-1);
				numero_unico_actual = (int) nd_abl_actual->ll;
				sumas_abl_segmentado[i] += numero_unico_actual;
				numeros_unicos[num_numeros_unicos++] = numero_unico_actual;
			}
			avt_iterador_siguiente(iterador);
		}
	
#if 0
		ass(
				nd->max_num_esperados
				>= ablazo_actual->nds_realmente_en_abl_utiles);
#endif
	
		avt_iterador_fini(iterador);
	}
	free(buf);
	free(numeros_unicos);
}
static inline void cax_encuentra_indices_segmento(
		cax_numeros_unicos_en_rango *nds, int idx_nd, int limite_izq,
		int limite_der, int *indices, int *num_indices) {
	cax_numeros_unicos_en_rango *nd = NULL;
	nd = nds + idx_nd;
	if (limite_izq <= nd->limite_izq && nd->limite_der <= limite_der) {
	
		indices[(*num_indices)++] = idx_nd;
	} else {
		if (nd->limite_der < limite_izq || limite_der < nd->limite_izq) {
		
		} else {
		
			cax_encuentra_indices_segmento(nds, 2 * idx_nd + 1,
					limite_izq, limite_der, indices, num_indices);
			cax_encuentra_indices_segmento(nds, 2 * idx_nd + 2,
					limite_izq, limite_der, indices, num_indices);
		}
	}
}
static inline entero_largo cax_generar_suma_unicos(
		cax_numeros_unicos_en_rango *abl_numeros_unicos,
		entero_largo *sumas_abl_segmentado, int *indices, int num_indices) {
	entero_largo suma_unicos = 0;
	avt_t *ablin_unicos = NULL;
	avt_create(&ablin_unicos, MAX_NODOS);
	for (int i = 0; i < num_indices; i++) {
		avt_t *ablin_actual = NULL;
		ablin_actual = abl_numeros_unicos[indices[i]].ablazo;
		if (!i) {
		
			cax_clona_abl(ablin_unicos, ablin_actual);
			suma_unicos += sumas_abl_segmentado[indices[i]];
		} else {
			avt_iterator_t *iter_actual = &(avt_iterator_t ) { 0 };
			avt_node_t *nd_actual = NULL;
			avt_iterador_ini(ablin_actual, iter_actual);
			while (avt_iterador_hay_siguiente(iter_actual)) {
				int numero_actual = 0;
				nd_actual = avt_iterador_obtener_actual(iter_actual);
				numero_actual = nd_actual->ll;
				if (!avt_find(ablin_unicos, numero_actual)) {
				
					avt_insert(ablin_unicos, numero_actual);
					suma_unicos += numero_actual;
				} else {
				
				}
				avt_iterador_siguiente(iter_actual);
			}
			avt_iterador_fini(iter_actual);
		}
	}
	avt_destroy(ablin_unicos);
	return suma_unicos;
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
static inline entero_largo cax_suma_segmento(entero_largo *sumas_abl_segmentado,
		cax_numeros_unicos_en_rango *abl_numeros_unicos, int limite_izq,
		int limite_der) {
	entero_largo res = 0;
	int num_indices_nds = 0;
	int *indices_nds = (int[30] ) { 0 };
	char buf[100] = { '\0' };
	cax_encuentra_indices_segmento(abl_numeros_unicos, 0, limite_izq,
			limite_der, indices_nds, &num_indices_nds);
	ass(num_indices_nds < 30);
	qsort(indices_nds, num_indices_nds, sizeof(int),
			caca_comun_compara_enteros);
	
	res = cax_generar_suma_unicos(abl_numeros_unicos,
			sumas_abl_segmentado, indices_nds, num_indices_nds);
	return res;
}
static inline void cax_encuentra_indices_afectados_por_actualizacion(
		cax_numeros_unicos_en_rango *nds, nat idx_nd,
		nat idx_actualizado, nat *indices, nat *num_indices) {
	cax_numeros_unicos_en_rango *nd = NULL;
	nd = nds + idx_nd;
	if (nd->limite_izq == idx_actualizado
			&& idx_actualizado == nd->limite_der) {
	
		indices[(*num_indices)++] = idx_nd;
	} else {
		if (nd->limite_der < idx_actualizado
				|| idx_actualizado < nd->limite_izq) {
		
		} else {
		
			indices[(*num_indices)++] = idx_nd;
			cax_encuentra_indices_afectados_por_actualizacion(nds,
					2 * idx_nd + 1, idx_actualizado, indices, num_indices);
			cax_encuentra_indices_afectados_por_actualizacion(nds,
					2 * idx_nd + 2, idx_actualizado, indices, num_indices);
		}
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
static inline void cax_actualiza_abl_numeros_unicos(
		cax_numeros_unicos_en_rango *abl_numeros_unicos,
		nat *indices_a_actualizar, nat num_indices_a_actualizar,
		td viejo_pendejo, td nuevo_valor,
		bitch_vector *nuevo_ya_existente, bitch_vector *viejo_aun_presente) {
	for (int i = 0; i < num_indices_a_actualizar; i++) {
		int idx_a_actualizar = 0;
		cax_numeros_unicos_en_rango *nd_a_actualizar = NULL;
		avt_t *ablazo = NULL;
		idx_a_actualizar = indices_a_actualizar[i];
		nd_a_actualizar = abl_numeros_unicos + idx_a_actualizar;
		ablazo = nd_a_actualizar->ablazo;
	
		assert(avt_find(ablazo, viejo_pendejo));
		avt_borrar(ablazo, viejo_pendejo);
		if (avt_find(ablazo, viejo_pendejo)) {
			caca_comun_asigna_bit(viejo_aun_presente, i);
		}
#ifdef CACA_X_VALIDAR_ARBOLINES
		avt_validar_ablin_indices(ablazo, ablazo->rt);
#endif
	
		if (!avt_find(ablazo, nuevo_valor)) {
		
		} else {
			caca_comun_asigna_bit(nuevo_ya_existente, i);
		
		}
		avt_insert(ablazo, (td) nuevo_valor);
#ifdef CACA_X_VALIDAR_ARBOLINES
		avt_validar_ablin_indices_ex(ablazo, ablazo->rt,
				verdadero);
#endif
	
	}
}
static inline void cax_actualiza_sumas_abl_segmentado(
		cax_numeros_unicos_en_rango *abl_numeros_unicos,
		entero_largo *sumas_abl_segmentado, nat *indices_a_actualizar,
		nat num_indices_a_actualizar, td nuevo_valor,
		td viejo_pendejo, bitch_vector *nuevo_ya_existente,
		bitch_vector *viejo_aun_presente) {
	for (nat i = 0; i < num_indices_a_actualizar; i++) {
		int idx_a_actualizar = 0;
		avt_t *ablin_a_actualizar = NULL;
		idx_a_actualizar = indices_a_actualizar[i];
		ablin_a_actualizar = abl_numeros_unicos[idx_a_actualizar].ablazo;
	
		if (!caca_comun_checa_bit(nuevo_ya_existente, i)) {
			sumas_abl_segmentado[idx_a_actualizar] += nuevo_valor;
		
		} else {
		
		}
		if (!caca_comun_checa_bit(viejo_aun_presente, i)) {
			sumas_abl_segmentado[idx_a_actualizar] -= viejo_pendejo;
		
		} else {
		
		}
		//3320749897
	}
}
static inline void cax_actualiza_estado(td *numeros,
		cax_numeros_unicos_en_rango *abl_numeros_unicos,
		entero_largo *sumas_abl_segmentado, nat idx_actualizado,
		td nuevo_valor, nat num_nds,
		nat num_numeros_redondeado) {
	bitch_vector nuevo_ya_existente = 0;
	bitch_vector viejo_aun_presente = 0;
	nat num_indices_afectados_actualizacion = 0;
	td viejo_pendejo = 0;
	nat *indices_afectados_actualizacion = (nat[18] ) { 0 };
	char buf[100];
	viejo_pendejo = numeros[idx_actualizado];
	if (viejo_pendejo == nuevo_valor) {
	
		return;
	}
	cax_encuentra_indices_afectados_por_actualizacion(abl_numeros_unicos,
			0, idx_actualizado, indices_afectados_actualizacion,
			&num_indices_afectados_actualizacion);
	ass(num_indices_afectados_actualizacion < 18);
	cax_actualiza_abl_numeros_unicos(abl_numeros_unicos,
			indices_afectados_actualizacion,
			num_indices_afectados_actualizacion, viejo_pendejo, nuevo_valor,
			&nuevo_ya_existente, &viejo_aun_presente);
	cax_actualiza_sumas_abl_segmentado(abl_numeros_unicos,
			sumas_abl_segmentado, indices_afectados_actualizacion,
			num_indices_afectados_actualizacion, nuevo_valor, viejo_pendejo,
			&nuevo_ya_existente, &viejo_aun_presente);
	numeros[idx_actualizado] = nuevo_valor;
#ifdef CACA_X_VALIDAR_ARBOLINES
	cax_validar_segmentos(abl_numeros_unicos, numeros,
			sumas_abl_segmentado, indices_afectados_actualizacion, 0,
			num_numeros_redondeado, num_nds + 1,
			num_indices_afectados_actualizacion);
#endif
}
static inline void cax_main() {
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
	nat num_numeros_redondeado = 0;
	int num_nds = 0;
	entero_largo *sumas_abl_segmentado = NULL;
	cax_numeros_unicos_en_rango *abl_numeros_unicos = NULL;
	char buf[100] = { '\0' };
	matriz_nums = calloc(MAX_NUMEROS_REDONDEADO * 3, sizeof(int));
	ass(matriz_nums);
	num_filas = 3;
	lee_matrix_long_stdin(matriz_nums, &num_filas, NULL, 3,
			MAX_NUMEROS_REDONDEADO);
	num_numeros = *matriz_nums;
	numeros = matriz_nums + MAX_NUMEROS_REDONDEADO;
	num_queries = *(numeros + MAX_NUMEROS_REDONDEADO);
	while ((num_numeros >> max_profundidad)) {
		max_profundidad++;
	}
	num_numeros_redondeado = (1 << max_profundidad);
	num_nds = (2 << (max_profundidad + 0)) - 1;
	abl_numeros_unicos = calloc(num_nds,
			sizeof(cax_numeros_unicos_en_rango));
	ass(abl_numeros_unicos);
	sumas_abl_segmentado = calloc(num_nds, sizeof(entero_largo));
	ass(sumas_abl_segmentado);
#ifdef USA_MALLOC
	estado = malloc((max_profundidad +2) * sizeof(cax_estado_recursion));
	ass(estado);
	memset(estado, 0, (max_profundidad+2 )* sizeof(cax_estado_recursion));
#else
	estado = calloc(max_profundidad + 2, sizeof(cax_estado_recursion));
	ass(estado);
#endif
	cax_construye_abl_binario_segmentado(numeros, abl_numeros_unicos,
			num_numeros_redondeado - 1, max_profundidad, num_numeros - 1);
	cax_suma_unicos(sumas_abl_segmentado, abl_numeros_unicos, num_nds);
#ifdef CACA_X_VALIDAR_ARBOLINES
	cax_validar_segmentos(abl_numeros_unicos, numeros,
			sumas_abl_segmentado, NULL, num_numeros, num_numeros_redondeado,
			num_nds, 0);
#endif
	while (cont_queries < num_queries) {
		int idx_actualizado = 0;
		int nuevo_valor = 0;
		entero_largo sum = 0;
		scanf("%c %d %d\n", &tipo_query, &idx_query_ini, &idx_query_fin);
		if (tipo_query == 'Q' && idx_query_ini > idx_query_fin) {
			td tmp = 0;
			tmp = idx_query_fin;
			idx_query_fin = idx_query_ini;
			idx_query_ini = tmp;
		}
	
		switch (tipo_query) {
		case 'Q':
			sum = cax_suma_segmento(sumas_abl_segmentado,
					abl_numeros_unicos, idx_query_ini - 1, idx_query_fin - 1);
			printf("%lld\n", sum);
			break;
		case 'U':
			idx_actualizado = idx_query_ini - 1;
			nuevo_valor = idx_query_fin;
			cax_actualiza_estado(numeros, abl_numeros_unicos,
					sumas_abl_segmentado, idx_actualizado, nuevo_valor,
					num_nds - 1, num_numeros_redondeado);
			break;
		default:
			abort();
			break;
		}
		cont_queries++;
	}
	free(matriz_nums);
	free(abl_numeros_unicos);
	free(sumas_abl_segmentado);
	free(estado);
}
int main(void) {
//	puts("he corrido con algo de suerte");
//	sleep(10);
	cax_main();
	return EXIT_SUCCESS;
}

