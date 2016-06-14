/*
 ============================================================================
 Name        : caca_x.c
 Author      : ernesto
 Version     :
 Copyright   : a veces siento que
 Description : Hello World in C, Ansi-style
 ============================================================================
 tamaño 41333
 https://www.codechef.com/problems/DUPSUM
 http://www.spojtoolkit.com/TestCaseGenerator/
 */

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
#define TAM_MAX_LINEA (MAX_NUMEROS*10+MAX_NUMEROS)
#define MAX_NODOS (1 << 16)
#define CACA_X_VALOR_INVALIDO -1

#define FIESTA_MIERDA_MAX_VALOR_ST_1 1E6
#define FIESTA_MIERDA_MIN_VALOR_ST_1 1
#define FIESTA_MIERDA_MAX_VALOR_ST_2 5E3
#define FIESTA_MIERDA_MIN_VALOR_ST_2 -5E3
#define FIESTA_MIERDA_MAX_NUMS_ST_1 9E6
#define FIESTA_MIERDA_MAX_NUMS_ST_2 9E3
#define FIESTA_MIERDA_MAX_NUMS_REDONDEADO 16777216
#define FIESTA_MIERDA_MAX_PROFUNDIDAD 24

#define CACA_X_VALIDAR_ARBOLINES

int printf_apocrifo(const char * __restrict, ...);
#ifdef ONLINE_JUDGE
#define caca_log_debug printf_apocrifo
#else
#define caca_log_debug printf
#endif

#define assert_timeout(condition) assert(condition);
/*
 #define assert_timeout(condition) 0
 #define assert_timeout(condition) if(!(condition)){printf("fuck\n");sleep(10);}
 */

typedef int tipo_dato;
typedef unsigned int natural;

typedef long long bitch_vector;

typedef enum BOOLEANOS {
	falso = 0, verdadero
} bool;

#define max(x,y) ((x) < (y) ? (y) : (x))
#define min(x,y) ((x) < (y) ? (x) : (y))

#if 1

#define AVL_TREE_VALOR_INVALIDO CACA_X_VALOR_INVALIDO

struct avl_tree_node_s {
	tipo_dato llave;
	natural altura;
	natural num_decendientes;
	natural indice_en_arreglo;
	natural ocurrencias;
	struct avl_tree_node_s *left;
	struct avl_tree_node_s *right;
	struct avl_tree_node_s *padre;
};

typedef struct avl_tree_node_s avl_tree_node_t;

struct avl_tree_s {
	natural max_nodos;
	natural nodos_realmente_en_arbol;
	natural nodos_usados;
	natural nodos_realmente_en_arbol_utiles;
	natural max_nodos_utiles;
	natural ocurrencias_totales;
	struct avl_tree_node_s *root;
	avl_tree_node_t *nodos_mem;
	natural *nodos_libres_idx;
	unsigned long siguiente_idx_para_usar;
	unsigned long ultimo_idx_anadido;
};

typedef struct avl_tree_s avl_tree_t;

typedef struct avl_tree_iterator_t {
	avl_tree_t *arbolin;
	char *contador_visitas;
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
avl_tree_node_t *avl_tree_create_node(avl_tree_t *arbolin, tipo_dato llave) {
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
	node->llave = llave;
	arbolin->nodos_realmente_en_arbol++;
	arbolin->nodos_realmente_en_arbol_utiles++;
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
		const avl_tree_node_t *node, const tipo_dato llave_nueva) {
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
				newroot = avl_tree_rotate_leftleft(nodo_actual);
			}

		} else if (bf <= -2) {
			/* Right Heavy */
			if (llave_nueva < nodo_actual->right->llave) {
				newroot = avl_tree_rotate_rightleft(nodo_actual);
			} else {
				newroot = avl_tree_rotate_rightright(nodo_actual);
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
					assert_timeout(0);
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
		tipo_dato llave_nueva) {

	avl_tree_node_t *newroot = NULL;

	newroot = avl_tree_balance_node_insertar(nodo, llave_nueva);

	if (newroot != tree->root) {
		tree->root = newroot;
	}
}

/* Insert a new node. */
avl_tree_node_t *avl_tree_insert(avl_tree_t *tree, tipo_dato value) {
	avl_tree_node_t *node = NULL;
	avl_tree_node_t *next = NULL;
	avl_tree_node_t *last = NULL;

	tree->ocurrencias_totales++;
	/* Well, there must be a first case */
	if (tree->root == NULL ) {
		node = avl_tree_create_node(tree, value);

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
						return next;
					} else {
						caca_log_debug("verga, no es maior menor ni igual\n");
						assert_timeout(0);

					}
				}
			}
		}

		node = avl_tree_create_node(tree, value);

		if (value < last->llave) {
			last->left = node;
		}
		if (value > last->llave) {
			last->right = node;
		}

		node->padre = last;

	}
	node->ocurrencias = 1;

	avl_tree_balance_insertar(tree, node, value);
	return node;
}

/* Find the node containing a given value */
avl_tree_node_t *avl_tree_find(avl_tree_t *tree, tipo_dato value) {
	avl_tree_node_t *current = tree->root;

	while (current && current->llave != value) {
		if (value > current->llave)
			current = current->right;
		else
			current = current->left;
	}

	return current ? current->llave == value ? current : NULL :NULL;
}

avl_tree_node_t *avl_tree_find_descartando(avl_tree_node_t *nodo_raiz,
		avl_tree_node_t **primer_nodo_mayor_o_igual, tipo_dato value,
		tipo_dato tope, bool *tope_topado) {
	avl_tree_node_t *current = NULL;
	avl_tree_node_t *primer_nodo_mayor = NULL;

	current = nodo_raiz;
	assert_timeout(!tope_topado || tope || !nodo_raiz->llave);

	assert_timeout(!tope_topado || tope >= value);

	if (tope_topado) {
		*tope_topado = falso;
	}

	do {
		if (tope_topado) {
			if ((current->llave > tope && !current->left)
					|| current->llave == tope) {
				*tope_topado = verdadero;
				break;
			}
		}
		if (value > current->llave) {
			current = current->right;
		} else {
			if (value < current->llave) {
				if (!primer_nodo_mayor) {
					primer_nodo_mayor = current;
				}
				current = current->left;
			} else {
				break;
			}
		}
	} while (current && current->llave != value);

	*primer_nodo_mayor_o_igual = primer_nodo_mayor;
	if (!*primer_nodo_mayor_o_igual) {
		if (current && (current->llave == value)) {
			*primer_nodo_mayor_o_igual = current;
		}
	}

	if (tope_topado && current && current->llave >= tope && value >= tope) {
		*tope_topado = verdadero;
	}

	return current ? current->llave == value ? current : NULL :NULL;
}

	/* Do a depth first traverse of a node. */
void avl_tree_traverse_node_dfs(avl_tree_node_t *node, int depth) {
	int i = 0;

	if (node->left)
		avl_tree_traverse_node_dfs(node->left, depth + 2);

	for (i = 0; i < depth; i++)
		putchar(' ');
	printf("%u: %d\n", node->llave, avl_tree_balance_factor(node));

	if (node->right)
		avl_tree_traverse_node_dfs(node->right, depth + 2);
}

/* Do a depth first traverse of a tree. */
void avl_tree_traverse_dfs(avl_tree_t *tree) {
	avl_tree_traverse_node_dfs(tree->root, 0);
}

static inline void avl_tree_iterador_ini(avl_tree_t *arbolin,
		avl_tree_iterator_t *iter) {
	iter->contador_visitas = calloc(arbolin->nodos_usados, sizeof(char));
	assert_timeout(iter->contador_visitas);
	iter->arbolin = arbolin;
}

static inline void avl_tree_iterador_fini(avl_tree_iterator_t *iter) {
	free(iter->contador_visitas);
}

static inline bool avl_tree_iterador_hay_siguiente(avl_tree_iterator_t *iter) {
	return iter->arbolin->root
			&& !(iter->nodo_actual == iter->arbolin->root
					&& iter->contador_visitas[iter->arbolin->root->indice_en_arreglo]
							== 2);
}

static inline avl_tree_node_t* avl_tree_iterador_siguiente(
		avl_tree_iterator_t *iter) {
	int contador_actual = 0;
	avl_tree_node_t *nodo = NULL;
	avl_tree_node_t *last_of_us = NULL;
	avl_tree_node_t *nodo_actual = NULL;

	if (!iter->nodo_actual) {
		nodo_actual = iter->nodo_actual = iter->arbolin->root;
	}

	if (!avl_tree_iterador_hay_siguiente(iter)) {
		return NULL ;
	}

	contador_actual =
			iter->contador_visitas[iter->nodo_actual->indice_en_arreglo];

	iter->contador_visitas[iter->nodo_actual->indice_en_arreglo]++;

	switch (contador_actual) {
	case 0:
	case 1:
		if (!contador_actual) {
			nodo_actual = iter->nodo_actual->left;
			if (!nodo_actual) {
				return iter->nodo_actual;
			}
		} else {
			nodo_actual = iter->nodo_actual->right;
			if (!nodo_actual) {
				nodo_actual = iter->nodo_actual->padre;
				while (nodo_actual
						&& iter->contador_visitas[nodo_actual->indice_en_arreglo]
								== 2) {
					last_of_us = nodo_actual;
					nodo_actual = nodo_actual->padre;
				}
				if (!nodo_actual) {
					if (last_of_us) {
						iter->nodo_actual = last_of_us;
					}
				} else {
					iter->nodo_actual = nodo_actual;
				}
				return nodo_actual;
			}
		}

		while (nodo_actual) {
			last_of_us = nodo_actual;
			iter->contador_visitas[nodo_actual->indice_en_arreglo]++;
			nodo_actual = last_of_us->left;
		}

		nodo = iter->nodo_actual = last_of_us;

		break;
	default:
		assert_timeout(0)
		;
		break;
	}

	return nodo;
}

static inline avl_tree_node_t* avl_tree_iterador_obtener_actual(
		avl_tree_iterator_t *iter) {
	avl_tree_node_t *nodo = NULL;

	if (!iter->nodo_actual) {
		avl_tree_iterador_siguiente(iter);
	}
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
		sprintf(num_buf, "%u", nodo->llave);
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
#ifdef ONLINE_JUDGE
	return NULL;
#endif
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
		/*
		 sprintf(num_buf, "%u [%u,%u] (%u) ocu %u", nodo->llave,
		 (natural )(nodo->llave >> 32), (natural )nodo->llave,
		 nodo->num_decendientes, nodo->ocurrencias);
		 */
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

static inline void avl_tree_validar_arbolin_indices_int(avl_tree_t *arbolin,
		avl_tree_node_t *nodo, natural *contador_total) {
	if (nodo) {
		assert_timeout(&arbolin->nodos_mem[nodo->indice_en_arreglo] == nodo);
		assert_timeout(!nodo->left || nodo->left->padre == nodo);
		avl_tree_validar_arbolin_indices_int(arbolin, nodo->left,
				contador_total);
		assert_timeout(!nodo->right || nodo->right->padre == nodo);
		avl_tree_validar_arbolin_indices_int(arbolin, nodo->right,
				contador_total);
		assert_timeout(nodo->ocurrencias > 0);
		assert_timeout(nodo->ocurrencias <= arbolin->max_nodos);
		(*contador_total) += nodo->ocurrencias;
	}
}

static inline void avl_tree_validar_arbolin_indices_ex(avl_tree_t *arbolin,
		avl_tree_node_t *nodo, bool validar_conteo_total) {
	natural contador_total = 0;

	avl_tree_validar_arbolin_indices_int(arbolin, nodo, &contador_total);

#if 0
	assert_timeout(
			!validar_conteo_total
			|| contador_total == arbolin->max_nodos_utiles);
	assert_timeout(
			!validar_conteo_total
			|| arbolin->ocurrencias_totales
			== arbolin->max_nodos_utiles);
#endif

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
		avl_tree_node_t *root, tipo_dato key, bool ignora_conteo) {
	// STEP 1: PERFORM STANDARD BST DELETE

	if (root == NULL ) {
		return root;
	}

	if (key < root->llave) {
		// If the key to be deleted is smaller than the root's key,
		// then it lies in left subtree
		root->left = avl_tree_nodo_borrar(arbolini, root->left, key,
				ignora_conteo);
		assert_timeout(!root->left || root->left->padre == root);
	} else {
		// If the key to be deleted is greater than the root's key,
		// then it lies in right subtree
		if (key > root->llave) {
			root->right = avl_tree_nodo_borrar(arbolini, root->right, key,
					ignora_conteo);
			assert_timeout(!root->right || root->right->padre == root);
		} else {
			if (!ignora_conteo) {
				arbolini->ocurrencias_totales--;
			}

			if ((root->ocurrencias - 1) == 0 || ignora_conteo) {
				if (root->left == NULL || root->right == NULL ) {
					avl_tree_node_t *temp =
							root->left ? root->left : root->right;
					// No child case
					if (temp == NULL ) {
						temp = root;
						root = NULL;
					} else {
						// One child case
						natural idx_en_arreglo = 0;
						avl_tree_node_t *padre = NULL;

						padre = root->padre;
						idx_en_arreglo = root->indice_en_arreglo;
						*root = *temp;// Copy the contents of the non-empty child
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
							% arbolini->max_nodos] = temp->indice_en_arreglo;
					memset(temp, 0, sizeof(avl_tree_node_t));
					temp->llave = AVL_TREE_VALOR_INVALIDO;
					arbolini->nodos_realmente_en_arbol--;
					arbolini->nodos_realmente_en_arbol_utiles--;
					caca_log_debug("disminuiendo nodos realmente en arbol a %u",
							arbolini->nodos_realmente_en_arbol);
				} else {
					// node with two children: Get the inorder successor (smallest
					// in the right subtree)
					avl_tree_node_t *temp = avl_tree_siguiente_nodo_inorder(
							root->right);

					// Copy the inorder successor's data to this node
					root->llave = temp->llave;
					root->ocurrencias = temp->ocurrencias;

					// Delete the inorder successor
					root->right = avl_tree_nodo_borrar(arbolini, root->right,
							temp->llave, verdadero);
				}
			} else {
				root->ocurrencias--;
				return root;
			}
		}
	}

// If the tree had only one node then return
	if (root == NULL ) {
		return root;
	}

// STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
	avl_tree_node_actualizar_altura(root);
	avl_tree_node_actualizar_num_decendientes(root);

// STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to check whether
//  this node became unbalanced)
	int balance = avl_tree_balance_factor(root);

// If this node becomes unbalanced, then there are 4 cases

// Left Left Case
	if (balance > 1 && avl_tree_balance_factor(root->left) >= 0) {
		return avl_tree_rotate_leftleft(root);
	}

// Left Right Case
	if (balance > 1 && avl_tree_balance_factor(root->left) < 0) {
		return avl_tree_rotate_leftright(root);
	}

// Right Right Case
	if (balance < -1 && avl_tree_balance_factor(root->right) <= 0) {
		return avl_tree_rotate_rightright(root);
	}

// Right Left Case
	if (balance < -1 && avl_tree_balance_factor(root->right) > 0) {
		return avl_tree_rotate_rightleft(root);
	}

	return root;
}

void avl_tree_borrar(avl_tree_t *tree, tipo_dato value) {

	avl_tree_node_t *newroot = NULL;

	if (!tree->root) {
		return;
	}
	newroot = avl_tree_nodo_borrar(tree, tree->root, value, falso);

	if (newroot != tree->root) {
		tree->root = newroot;
	}
}

#endif

typedef struct caca_x_numeros_unicos_en_rango {
	avl_tree_t *arbolazo;
	natural max_numeros;
	natural max_num_esperados;
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
	int num_popeado;
	caca_x_numeros_unicos_en_rango *nodo;
} caca_x_estado_recursion;

caca_x_estado_recursion *estado = NULL;

typedef enum fiesta_mierda_tipo_sub_tarea {
	st_tipo_desconocido, st_tipo_1, st_tipo_2
} fiesta_mierda_tipo_sub_tarea;

int printf_apocrifo(const char * cad, ...) {
	return 0;
}
static inline int lee_matrix_long_stdin(tipo_dato *matrix, int *num_filas,
		int *num_columnas, int num_max_filas, int num_max_columnas,
		fiesta_mierda_tipo_sub_tarea *tipo_st) {
	int indice_filas = 0;
	int indice_columnas = 0;
	tipo_dato numero = 0;
	char *siguiente_cadena_numero = NULL;
	char *cadena_numero_actual = NULL;
	char *linea = NULL;

	linea = calloc(TAM_MAX_LINEA, sizeof(char));

	if (tipo_st) {
		*tipo_st = st_tipo_desconocido;
	}
	while (indice_filas < num_max_filas && fgets(linea, TAM_MAX_LINEA, stdin)) {
		indice_columnas = 0;
		cadena_numero_actual = linea;
		for (siguiente_cadena_numero = linea;; siguiente_cadena_numero =
				cadena_numero_actual) {
			numero = strtol(siguiente_cadena_numero, &cadena_numero_actual, 10);
			if (cadena_numero_actual == siguiente_cadena_numero) {
				break;
			}
			if (tipo_st) {
				if (numero < FIESTA_MIERDA_MIN_VALOR_ST_1) {
					assert_timeout(*tipo_st != st_tipo_1);
					*tipo_st = st_tipo_2;
					caca_log_debug("debido a %u el tipo1 es %u\n", numero,
							*tipo_st);
				}
				if (numero > FIESTA_MIERDA_MAX_VALOR_ST_2) {
					assert_timeout(*tipo_st != st_tipo_2);
					*tipo_st = st_tipo_1;
					caca_log_debug("debido a %u el tipo2 es %u\n", numero,
							*tipo_st);
				}
			}
			*(matrix + indice_filas * num_max_columnas + indice_columnas) =
					numero;
			caca_log_debug("en col %d, fil %d, el valor %u\n", indice_columnas,
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
	avl_tree_create(&nodo->arbolazo, nodo->max_numeros);
	nodo->idx = idx_nodo;
	nodo->limite_izq = limite_izq;
	nodo->limite_der = limite_der;

}

static inline void caca_x_clona_nodos(avl_tree_t *arbol_dst,
		avl_tree_node_t *nodo_ori, avl_tree_node_t *nodo_dst_padre,
		avl_tree_node_t **apuntador_hijo) {
	int idx_en_arreglo_real = 0;
	avl_tree_node_t *nodo_nuevo = NULL;

	if (!nodo_ori) {
		return;
	}

	nodo_nuevo = avl_tree_create_node(arbol_dst, nodo_ori->llave);
	idx_en_arreglo_real = nodo_nuevo->indice_en_arreglo;

	memcpy(nodo_nuevo, nodo_ori, offsetof(avl_tree_node_t,left));

	nodo_nuevo->indice_en_arreglo = idx_en_arreglo_real;

	if (!arbol_dst->root) {
		arbol_dst->root = nodo_nuevo;
	}

	if (nodo_dst_padre) {
		nodo_nuevo->padre = nodo_dst_padre;
		*apuntador_hijo = nodo_nuevo;
	}

	caca_x_clona_nodos(arbol_dst, nodo_ori->left, nodo_nuevo,
			&nodo_nuevo->left);
	caca_x_clona_nodos(arbol_dst, nodo_ori->right, nodo_nuevo,
			&nodo_nuevo->right);
}

static inline void caca_x_clona_arbol(avl_tree_t *arbol_dst,
		avl_tree_t *arbol_ori) {
	int tam_arbol_dst = 0;
	int tam_arbol_ori = 0;

	tam_arbol_dst = arbol_dst->max_nodos;
	tam_arbol_ori = arbol_ori->nodos_realmente_en_arbol_utiles;

	assert_timeout(tam_arbol_dst >= tam_arbol_ori);
	assert_timeout(!arbol_dst->nodos_usados);

	caca_x_clona_nodos(arbol_dst, arbol_ori->root, NULL, NULL );

	arbol_dst->ocurrencias_totales = arbol_ori->ocurrencias_totales;
}

static inline void caca_x_mergear_arboles(avl_tree_t *arbolin_izq,
		avl_tree_t *arbolin_der, avl_tree_t *arbolin_resultante) {
	int tam_arbol_izq = 0;
	int tam_arbol_der = 0;
	avl_tree_iterator_t *iter = &(avl_tree_iterator_t ) { 0 };
	avl_tree_t *arbol_menor = NULL;
	avl_tree_t *arbol_mayor = NULL;

	assert_timeout(arbolin_izq && arbolin_der);

	tam_arbol_izq = arbolin_izq->nodos_realmente_en_arbol_utiles;
	tam_arbol_der = arbolin_der->nodos_realmente_en_arbol_utiles;

	if (tam_arbol_izq <= tam_arbol_der) {
		arbol_menor = arbolin_izq;
		arbol_mayor = arbolin_der;
	} else {
		arbol_menor = arbolin_der;
		arbol_mayor = arbolin_izq;
	}

	caca_x_clona_arbol(arbolin_resultante, arbol_mayor);

	assert_timeout(
			arbolin_resultante->nodos_realmente_en_arbol_utiles
					== arbol_mayor->nodos_realmente_en_arbol_utiles);

	avl_tree_iterador_ini(arbol_menor, iter);

	while (avl_tree_iterador_hay_siguiente(iter)) {
		int numero_actual = 0;
		avl_tree_node_t *nodo_actual = NULL;
		avl_tree_node_t *nodo_encontrado = NULL;

		nodo_actual = avl_tree_iterador_obtener_actual(iter);
		numero_actual = nodo_actual->llave;

		if (!(nodo_encontrado = avl_tree_find(arbolin_resultante, numero_actual))) {
			nodo_encontrado = avl_tree_insert(arbolin_resultante,
					numero_actual);
			nodo_encontrado->ocurrencias = nodo_actual->ocurrencias;
			arbolin_resultante->ocurrencias_totales += nodo_actual->ocurrencias
					- 1;
		} else {
			nodo_encontrado->ocurrencias += nodo_actual->ocurrencias;
			arbolin_resultante->ocurrencias_totales += nodo_actual->ocurrencias;
		}

		avl_tree_iterador_siguiente(iter);
	}

	avl_tree_iterador_fini(iter);
}

static inline void caca_x_construye_arbol_binario_segmentado(int *numeros,
		caca_x_numeros_unicos_en_rango *arbol_numeros_unicos, int num_numeros,
		int max_profundidad, natural ultimo_indice_numero_valido) {
	int profundidad = -1;

	estado->idx_ini = 0;
	estado->idx_fin = num_numeros;
	estado->idx_nodo = 0;
	estado->nodo = NULL;
	estado->profundidad = 0;
	estado->num_popeado = 0;

	profundidad++;

	while (profundidad != -1) {
		int idx_ini = 0;
		int idx_fin = 0;
		int idx_nodo = 0;
		int altura = 0;
		caca_x_numeros_unicos_en_rango *nodo = NULL;
		caca_x_estado_recursion *estado_actual = NULL;

		estado_actual = estado + profundidad;

		assert_timeout(estado_actual->profundidad == profundidad);
		caca_log_debug("perfume %d\n", profundidad);

		idx_ini = estado_actual->idx_ini;
		idx_fin = estado_actual->idx_fin;
		idx_nodo = estado_actual->idx_nodo;

		altura = max_profundidad - profundidad;

		caca_log_debug("en altura %d de pseudo rec idx %d\n", altura, idx_nodo);

		if (!estado_actual->num_popeado) {
			nodo = arbol_numeros_unicos + idx_nodo;

			caca_x_inicializar_nodo(nodo, altura, idx_nodo, idx_ini, idx_fin);
		}

		estado_actual->num_popeado++;

		profundidad--;

		if (altura) {
			int distancia_media = 0;
			int idx_medio = 0;
			int ultimo_num_popeado = 0;
			caca_x_estado_recursion *estado_futuro = NULL;

			if (estado_actual->num_popeado == 1
					|| estado_actual->num_popeado == 2) {

				caca_log_debug("creando nueva profundidad \n");
				estado_futuro = estado + estado_actual->profundidad + 1;

				caca_log_debug("num de popeos d estado actual es %d\n",
						estado_actual->num_popeado);

				if (estado_actual->num_popeado == 1) {
					estado_actual->nodo = nodo;
				}

				memset(estado_futuro, 0, sizeof(caca_x_estado_recursion));

				estado_futuro->profundidad = estado_actual->profundidad + 1;

				distancia_media = (idx_fin - idx_ini) / 2;
				idx_medio = idx_ini + distancia_media;

			}

			ultimo_num_popeado = estado_actual->num_popeado;

			switch (estado_actual->num_popeado) {
			case 1:
				estado_futuro->idx_ini = idx_ini;
				estado_futuro->idx_fin = idx_medio;
				estado_futuro->idx_nodo = 2 * idx_nodo + 1;
				caca_log_debug("segmentando izq de %d a %d en alt %d\n",
						idx_ini, idx_medio, altura);
				break;
			case 2:
				estado_futuro->idx_ini = idx_medio + 1;
				estado_futuro->idx_fin = idx_fin;
				estado_futuro->idx_nodo = 2 * idx_nodo + 2;
				caca_log_debug("segmentando der de %d a %d en alt %d\n",
						idx_medio + 1, idx_fin, altura);
				break;
			case 3: {
				caca_x_numeros_unicos_en_rango *hijo_izq = NULL;
				caca_x_numeros_unicos_en_rango *hijo_der = NULL;
				avl_tree_t *arbolin_izq = NULL;
				avl_tree_t *arbolin_der = NULL;

				hijo_izq = arbol_numeros_unicos + (idx_nodo * 2 + 1);
				hijo_der = arbol_numeros_unicos + (idx_nodo * 2 + 2);

				arbolin_izq = hijo_izq->arbolazo;
				arbolin_der = hijo_der->arbolazo;

				caca_x_mergear_arboles(arbolin_izq, arbolin_der,
						estado_actual->nodo->arbolazo);

				estado_actual->nodo->max_num_esperados =
						(ultimo_indice_numero_valido
								>= estado_actual->nodo->limite_der) ?
								estado_actual->nodo->max_numeros :
						(ultimo_indice_numero_valido
								>= estado_actual->nodo->limite_izq) ?
								(ultimo_indice_numero_valido
										- estado_actual->nodo->limite_izq + 1) :
								0;
				estado_actual->nodo->arbolazo->max_nodos_utiles =
						estado_actual->nodo->max_num_esperados;

#ifdef CACA_X_VALIDAR_ARBOLINES
				avl_tree_validar_arbolin_indices_ex(
						estado_actual->nodo->arbolazo,
						estado_actual->nodo->arbolazo->root, verdadero);
#endif

				memset(estado_actual, 0, sizeof(caca_x_estado_recursion));
				caca_log_debug("ultimo 'recursion' de nodo %d, nada q acer\n",
						idx_nodo);
			}
				break;
			default:
				assert_timeout(0)
				;
				break;
			}

			if (ultimo_num_popeado < 3) {
				profundidad += 2;
				caca_log_debug("aumentando profundidad a %d\n", profundidad);
			}

		} else {
			int numero_actual = 0;

			assert_timeout(idx_ini == idx_fin);
			assert_timeout(nodo);

			numero_actual = numeros[idx_ini];

			assert_timeout(nodo->max_numeros == 1);
			assert_timeout(nodo->limite_izq == nodo->limite_der);
			assert_timeout(nodo->arbolazo->max_nodos == 1);
			assert_timeout(!nodo->arbolazo->nodos_realmente_en_arbol_utiles);

			assert_timeout(!avl_tree_find(nodo->arbolazo, numero_actual));

			avl_tree_insert(nodo->arbolazo, (tipo_dato) numero_actual);
			nodo->arbolazo->max_nodos_utiles = nodo->max_num_esperados = 1;

			caca_log_debug("asignado unico numero %d a indice de arbol %d\n",
					numero_actual, idx_nodo);

			memset(estado_actual, 0, sizeof(caca_x_estado_recursion));
		}
	}
}

static inline void caca_x_suma_unicos(long *sumas_arbol_segmentado,
		caca_x_numeros_unicos_en_rango *arbol_numeros_unicos, int num_nodos) {
	int *numeros_unicos = NULL;
	char *buf = NULL;

	buf = calloc(1000, sizeof(char));

	numeros_unicos = calloc(MAX_NODOS, sizeof(int));
	assert(numeros_unicos);

	for (int i = 0; i < num_nodos; i++) {
		int num_numeros_unicos = 0;

		memset(buf, '\0', 1000);

		caca_x_numeros_unicos_en_rango *nodo = NULL;
		avl_tree_t *arbolazo_actual = NULL;
		avl_tree_iterator_t * iterador = &(avl_tree_iterator_t ) { 0 };

		nodo = arbol_numeros_unicos + i;

		if (!nodo->arbolazo) {
			continue;
		}

		assert_timeout(i == nodo->idx);

		arbolazo_actual = nodo->arbolazo;

		caca_log_debug("arbol en idx %d \n %s\n", i,
				avl_tree_sprint(arbolazo_actual, buf));
		caca_log_debug("aaa\n");

		avl_tree_iterador_ini(arbolazo_actual, iterador);

		sumas_arbol_segmentado[i] = 0;

		while (avl_tree_iterador_hay_siguiente(iterador)) {
			int numero_unico_actual = 0;
			avl_tree_node_t *nodo_arbol_actual = NULL;

			nodo_arbol_actual =
					(avl_tree_node_t*) avl_tree_iterador_obtener_actual(
							iterador);

			if (nodo_arbol_actual) {

				numero_unico_actual = (int) nodo_arbol_actual->llave;

				sumas_arbol_segmentado[i] += numero_unico_actual;
				numeros_unicos[num_numeros_unicos++] = numero_unico_actual;

			}
			avl_tree_iterador_siguiente(iterador);
		}

		caca_log_debug("los numeros unicos en %d, altura %d, son %s\n", i,
				nodo->altura,
				caca_arreglo_a_cadena(numeros_unicos, num_numeros_unicos, buf));
#if 0
		assert_timeout(
				nodo->max_num_esperados
				>= arbolazo_actual->nodos_realmente_en_arbol_utiles);
#endif

		caca_log_debug("la suma es %ld\n", sumas_arbol_segmentado[i]);
		avl_tree_iterador_fini(iterador);
	}

	free(buf);
	free(numeros_unicos);
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

static inline unsigned long caca_x_generar_suma_unicos(
		caca_x_numeros_unicos_en_rango *arbol_numeros_unicos,
		long *sumas_arbol_segmentado, int *indices, int num_indices) {
	unsigned long suma_unicos = 0;
	avl_tree_t *arbolin_unicos = NULL;

	caca_log_debug("sumando unicos\n");

	avl_tree_create(&arbolin_unicos, MAX_NODOS);

	for (int i = 0; i < num_indices; i++) {
		avl_tree_t *arbolin_actual = NULL;

		arbolin_actual = arbol_numeros_unicos[indices[i]].arbolazo;

		if (!i) {
			caca_log_debug("primer arbol en nodo %d\n", i);
			caca_x_clona_arbol(arbolin_unicos, arbolin_actual);
			suma_unicos += sumas_arbol_segmentado[indices[i]];
		} else {
			avl_tree_iterator_t *iter_actual = &(avl_tree_iterator_t ) { 0 };
			avl_tree_node_t *nodo_actual = NULL;

			avl_tree_iterador_ini(arbolin_actual, iter_actual);

			while (avl_tree_iterador_hay_siguiente(iter_actual)) {
				int numero_actual = 0;

				nodo_actual = avl_tree_iterador_obtener_actual(iter_actual);
				numero_actual = nodo_actual->llave;

				if (!avl_tree_find(arbolin_unicos, numero_actual)) {
					caca_log_debug("añadiendo %d al arbol de unicos\n%s\n",
							numero_actual,
							avl_tree_sprint_identado(arbolin_unicos,
									(char[100] ) { '\0' }));
					avl_tree_insert(arbolin_unicos, numero_actual);
					suma_unicos += numero_actual;
				} else {
					caca_log_debug(
							"numero %d, se encontro que es duplicado en \n%s, proviene de segmento actual %d:\n%s\n",
							numero_actual,
							avl_tree_sprint_identado(arbolin_unicos,
									(char[100] ) { '\0' }), i,
							avl_tree_sprint_identado(arbolin_actual,
									(char[100] ) { '\0' }));
				}

				avl_tree_iterador_siguiente(iter_actual);
			}

			avl_tree_iterador_fini(iter_actual);
		}
	}

	avl_tree_destroy(arbolin_unicos);

	caca_log_debug("en total la suma de repetidos es %ld\n", suma_unicos);
	return suma_unicos;
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

static inline long caca_x_suma_segmento(long *sumas_arbol_segmentado,
		caca_x_numeros_unicos_en_rango *arbol_numeros_unicos, int limite_izq,
		int limite_der) {
	long res = 0;
	int num_indices_nodos = 0;
	int *indices_nodos = (int[30] ) { 0 };
	char buf[100] = { '\0' };

	caca_x_encuentra_indices_segmento(arbol_numeros_unicos, 0, limite_izq,
			limite_der, indices_nodos, &num_indices_nodos);

	assert_timeout(num_indices_nodos < 30);

	qsort(indices_nodos, num_indices_nodos, sizeof(int),
			caca_comun_compara_enteros);
	caca_log_debug("indices de segmento %d:%d %s\n", limite_izq, limite_der,
			caca_arreglo_a_cadena(indices_nodos, num_indices_nodos, buf));

	/*
	 for (int i = 0; i < num_indices_nodos; i++) {
	 caca_log_debug("segmento %d aporta %lu\n", indices_nodos[i],
	 sumas_arbol_segmentado[indices_nodos[i]]);
	 res += sumas_arbol_segmentado[indices_nodos[i]];
	 }

	 res -= caca_x_generar_suma_repetidos(arbol_numeros_unicos, indices_nodos,
	 num_indices_nodos);
	 */

	res = caca_x_generar_suma_unicos(arbol_numeros_unicos,
			sumas_arbol_segmentado, indices_nodos, num_indices_nodos);
	caca_log_debug("La suma es %ld\n", res);

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

static inline void caca_x_validar_segmentos_int(
		caca_x_numeros_unicos_en_rango *arbolin_segs, natural idx,
		natural *conteo_por_nivel, natural num_nodos) {

	caca_x_numeros_unicos_en_rango *arbolin_actual = NULL;

	arbolin_actual = arbolin_segs + idx;

	if (!arbolin_actual->arbolazo
			|| !arbolin_actual->arbolazo->nodos_realmente_en_arbol_utiles) {
		caca_log_debug(
				"oe o ea, la familia telo agradecera. encontrado arbol pendejo");
		return;
	}
#if 0
	assert_timeout(
			arbolin_actual->max_num_esperados
			>= arbolin_actual->arbolazo->nodos_realmente_en_arbol_utiles);
#endif
	conteo_por_nivel[arbolin_actual->altura] +=
			arbolin_actual->arbolazo->ocurrencias_totales;

	if (arbolin_actual->idx * 2 + 1 < num_nodos) {
		caca_x_validar_segmentos_int(arbolin_segs, arbolin_actual->idx * 2 + 1,
				conteo_por_nivel, num_nodos);
		if (arbolin_actual->idx * 2 + 2 < num_nodos) {
			caca_x_validar_segmentos_int(arbolin_segs,
					arbolin_actual->idx * 2 + 2, conteo_por_nivel, num_nodos);
		}
	}
}

static inline void caca_x_validar_segmentos(
		caca_x_numeros_unicos_en_rango *arbolin_segs, natural num_numeros,
		natural num_nodos) {
	natural conteo_por_nivel[19] = { 0 };

	caca_x_validar_segmentos_int(arbolin_segs, 0, conteo_por_nivel, num_nodos);

	for (int i = 0; i <= arbolin_segs->altura; i++) {
		assert_timeout(conteo_por_nivel[i] == num_numeros);
	}
}

static inline void caca_x_main() {
	int *matriz_nums = NULL;
	int num_filas = 0;
	natural num_casos = 0;

	char buf[100] = { '\0' };

	matriz_nums = calloc(FIESTA_MIERDA_MAX_NUMS_REDONDEADO, sizeof(tipo_dato));
	assert_timeout(matriz_nums);

	lee_matrix_long_stdin(matriz_nums, &num_filas, NULL, 1, 1, NULL );
	num_casos = *matriz_nums;

#ifdef USA_MALLOC
	estado = malloc((max_profundidad +2) * sizeof(caca_x_estado_recursion));
	assert_timeout(estado);
	memset(estado, 0, (max_profundidad+2 )* sizeof(caca_x_estado_recursion));
#else
	estado = calloc(FIESTA_MIERDA_MAX_PROFUNDIDAD + 1,
			sizeof(caca_x_estado_recursion));
	assert_timeout(estado);
#endif

	for (int i = 0; i < num_casos; i++) {
		long sum = 0;
		natural max_profundidad = 0;
		natural num_numeros_redondeado = 0;
		natural num_nodos = 0;
		natural num_numeros = 0;
		fiesta_mierda_tipo_sub_tarea tipo_st = st_tipo_desconocido;
		long *sumas_arbol_segmentado = NULL;
		tipo_dato *numeros = NULL;
		caca_x_numeros_unicos_en_rango *arbol_numeros_unicos = NULL;

		lee_matrix_long_stdin(matriz_nums, &num_filas, NULL, 1, 1, NULL );

		num_numeros = *matriz_nums;

		caca_log_debug("a vece siento q %d\n", num_numeros);

		lee_matrix_long_stdin(matriz_nums, &num_filas, NULL, 1,
				FIESTA_MIERDA_MAX_NUMS_REDONDEADO, &tipo_st);

		numeros = matriz_nums;

		caca_log_debug("as corrido con algo de s %s\n",
				caca_arreglo_a_cadena(numeros, num_numeros, buf));
		if (num_numeros > FIESTA_MIERDA_MAX_NUMS_ST_2) {
			assert_timeout(tipo_st != st_tipo_2);
			tipo_st = st_tipo_1;
		}

		while ((num_numeros >> max_profundidad)) {
			max_profundidad++;
		}
		num_numeros_redondeado = (1 << max_profundidad);

		caca_log_debug("en estas paginas %s\n",
				caca_arreglo_a_cadena(numeros, num_numeros_redondeado, buf));

		num_nodos = (2 << (max_profundidad + 0));

		caca_log_debug("el numero de nodos %d\n", num_nodos);

		arbol_numeros_unicos = calloc(num_nodos,
				sizeof(caca_x_numeros_unicos_en_rango));
		assert_timeout(arbol_numeros_unicos);

		sumas_arbol_segmentado = calloc(num_nodos, sizeof(long));
		assert_timeout(sumas_arbol_segmentado);

		caca_log_debug("llamando a func rec con max prof %d\n",
				max_profundidad + 2);

		caca_x_construye_arbol_binario_segmentado(numeros, arbol_numeros_unicos,
				num_numeros_redondeado - 1, max_profundidad, num_numeros - 1);

#ifdef CACA_X_VALIDAR_ARBOLINES
		caca_x_validar_segmentos(arbol_numeros_unicos, num_numeros_redondeado,
				num_nodos);
		for (int i = 0; i < num_numeros; i++) {
			if (tipo_st == st_tipo_1) {
				assert_timeout(numeros[i] >=FIESTA_MIERDA_MIN_VALOR_ST_1);
				assert_timeout(numeros[i] <=FIESTA_MIERDA_MAX_VALOR_ST_1);
				assert_timeout(num_numeros<=FIESTA_MIERDA_MAX_NUMS_ST_1);
			}
			if (tipo_st == st_tipo_2) {
				assert_timeout(numeros[i] >=FIESTA_MIERDA_MIN_VALOR_ST_2);
				assert_timeout(numeros[i] <=FIESTA_MIERDA_MAX_VALOR_ST_2);
				assert_timeout(num_numeros<=FIESTA_MIERDA_MAX_NUMS_ST_2);
			}
		}
#endif

		caca_x_suma_unicos(sumas_arbol_segmentado, arbol_numeros_unicos,
				num_nodos);

		sum = caca_x_suma_segmento(sumas_arbol_segmentado, arbol_numeros_unicos,
				0, num_numeros_redondeado);

		printf("%ld\n", sum);

		for (int i = 0; i < num_nodos; i++) {
			caca_x_numeros_unicos_en_rango *nodo_segmento_actual = NULL;

			nodo_segmento_actual = arbol_numeros_unicos + i;
			if (nodo_segmento_actual->arbolazo) {
				avl_tree_destroy(nodo_segmento_actual->arbolazo);
			}
		}
		free(arbol_numeros_unicos);
		free(sumas_arbol_segmentado);
	}
	free(matriz_nums);
	free(estado);
}

int main(void) {
//	puts("he corrido con algo de suerte"); /* prints he corrido con algo de suerte */
//	sleep(10);
	caca_x_main();
	return EXIT_SUCCESS;
}
