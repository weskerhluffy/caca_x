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

#define CACA_X_VALIDAR_ARBOLINES

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

#define AVL_TREE_VALOR_INVALIDO CACA_X_VALOR_INVALIDO

struct avl_tree_node_s {
	tipo_dato llave;
	natural altura;
#ifdef CACA_X_VALIDAR_ARBOLINES
	natural num_decendientes;
#endif
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
	avl_tree_node_t *node;
	unsigned long arbolin_siguiente_idx_para_usar =
			arbolin->siguiente_idx_para_usar;

	assert_timeout(
			arbolin->siguiente_idx_para_usar < arbolin->ultimo_idx_anadido
					|| ((arbolin->siguiente_idx_para_usar
							== arbolin->ultimo_idx_anadido)
							&& arbolin->nodos_usados < arbolin->max_nodos));

	if (arbolin_siguiente_idx_para_usar < arbolin->ultimo_idx_anadido) {
		natural *arbolin_nodos_libres_idx = arbolin->nodos_libres_idx;
		unsigned long idx_para_usar = arbolin_siguiente_idx_para_usar
				% arbolin->max_nodos;
		natural idx_nodo_libre = arbolin_nodos_libres_idx[idx_para_usar];

		node = arbolin->nodos_mem + idx_nodo_libre;
		node->indice_en_arreglo = idx_nodo_libre;

#ifdef CACA_X_VALIDAR_ARBOLINES
		arbolin_nodos_libres_idx[idx_para_usar] = 0xffffffff;
#endif
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

static inline void avl_tree_node_actualizar_altura(avl_tree_node_t *node) {
	int height_left = 0;
	int height_right = 0;
	avl_tree_node_t *node_left = node->left;
	avl_tree_node_t *node_right = node->right;

	node->altura = 0;
	if (node->left || node->right) {
		if (node_left) {
			height_left = node_left->altura;
		}
		if (node_right) {
			height_right = node_right->altura;
		}
		node->altura = (height_right > height_left ? height_right : height_left)
				+ 1;
	}
}

static inline void avl_tree_node_actualizar_num_decendientes(
		avl_tree_node_t *node) {
	int conteo_left = 0;
	int conteo_right = 0;

#ifdef CACA_X_VALIDAR_ARBOLINES
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
#endif
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
#ifdef CACA_X_VALIDAR_ARBOLINES
	avl_tree_node_actualizar_num_decendientes(a);
	avl_tree_node_actualizar_num_decendientes(b);
#endif

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
static inline int avl_tree_balance_factor(avl_tree_node_t *node) {
	int bf = 0;

	avl_tree_node_t *node_left = node->left;
	avl_tree_node_t *node_right = node->right;

	if (node_left) {
		bf = node_left->altura;
	}
	if (node_right) {
		bf -= node_right->altura;
	}

	return bf;
}

static inline avl_tree_node_t *avl_tree_balance_node_insertar(
		const avl_tree_node_t *node, const tipo_dato llave_nueva) {
	avl_tree_node_t *newroot = (avl_tree_node_t *) node;
	avl_tree_node_t *nodo_actual = node->padre;

	while (nodo_actual) {

		int bf = avl_tree_balance_factor(nodo_actual);

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
			avl_tree_node_t *padre = newroot->padre;

			assert_timeout(llave_nueva != padre->llave);
			if (llave_nueva < padre->llave) {
				padre->left = newroot;
			} else {
				padre->right = newroot;
			}
		}

		nodo_actual = nodo_actual->padre;
	}

	return (newroot);
}

/* Balance a given tree */
void avl_tree_balance_insertar(avl_tree_t *tree, avl_tree_node_t *nodo,
		tipo_dato llave_nueva) {

	avl_tree_node_t *newroot = avl_tree_balance_node_insertar(nodo,
			llave_nueva);

	if (newroot != tree->root) {
		tree->root = newroot;
	}
}

/* Insert a new node. */
avl_tree_node_t *avl_tree_insert(avl_tree_t *tree, tipo_dato value) {
	avl_tree_node_t *node;
	avl_tree_node_t *next;
	avl_tree_node_t *last;

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
			tipo_dato next_llave = next->llave;

#ifdef CACA_X_VALIDAR_ARBOLINES
			next->num_decendientes++;
#endif

			if (value < next_llave) {
				next = next->left;
			} else {
				if (value > next_llave) {
					next = next->right;
				} else {
					assert_timeout(value == next_llave);
					avl_tree_node_t *ancestro_actal = next;
					/* Have we already inserted this node? */
					next->ocurrencias++;
					caca_log_debug(
							"llave ya existe, aumentando contador a carajo %u\n",
							next->ocurrencias);

#ifdef CACA_X_VALIDAR_ARBOLINES
					while (ancestro_actal) {
						caca_log_debug("bajando decendientes de %u\n",
								ancestro_actal->llave);
						ancestro_actal->num_decendientes--;
						ancestro_actal = ancestro_actal->padre;
					}
#endif
					return next;
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

	if (current && current->llave == value) {
		return current;
	}

	return NULL ;
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
#ifdef ONLINE_JUDGE
	return NULL;
#endif

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

#ifdef CACA_X_LOG
#ifndef ONLINE_JUDGE
		sprintf(num_buf, "%u [%u,%u]  ocu %u", nodo->llave,
				(natural )(nodo->llave >> 32), (natural )nodo->llave,
				nodo->ocurrencias);
#endif
#endif

		strcat(buf, num_buf);
		if (profundidad_maxima != -1) {
			strcat(buf, "\n");
		}

		assert_timeout(!nodo->left || nodo->left->padre == nodo);
		avl_tree_inoder_node_travesti_conteo(nodo->left, buf,
				profundidad_maxima);

		if (profundidad_maxima != -1) {
			strcat(buf, "\n");
			for (int i = 0; i <= profundidad; i++) {
				strcat(buf, " ");
			}
		}
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
		avl_tree_iterator_t *iter, tipo_dato llave) {
	avl_tree_t *arbolazo = NULL;
	avl_tree_node_t *nodo_actual = NULL;
	avl_tree_node_t *last_of_us = NULL;

	arbolazo = iter->arbolin;

	nodo_actual = arbolazo->root;

	while (nodo_actual) {
		last_of_us = nodo_actual;
		if (llave < nodo_actual->llave) {
			iter->contador_visitas[nodo_actual->indice_en_arreglo] = 1;
			nodo_actual = nodo_actual->left;
		} else {
			iter->contador_visitas[nodo_actual->indice_en_arreglo] = 2;
			if (llave > nodo_actual->llave) {
				nodo_actual = nodo_actual->right;
			} else {
				break;
			}

		}
	}

	if (nodo_actual) {
		iter->nodo_actual = nodo_actual;
	} else {
		iter->nodo_actual = last_of_us;
	}
	iter->contador_visitas[iter->nodo_actual->indice_en_arreglo] = 1;
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

static inline void avl_tree_validar_arbolin_indices(avl_tree_t *arbolin,
		avl_tree_node_t *nodo) {
	avl_tree_validar_arbolin_indices_ex(arbolin, nodo, falso);
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
		avl_tree_node_t **nodo_sobreviviente) {
	// STEP 1: PERFORM STANDARD BST DELETE

	if (root == NULL ) {
		return root;
	}

	if (key < root->llave) {
		// If the key to be deleted is smaller than the root's key,
		// then it lies in left subtree
		root->left = avl_tree_nodo_borrar(arbolini, root->left, key,
				ignora_conteo, nodo_sobreviviente);
		assert_timeout(!root->left || root->left->padre == root);
	} else {
		// If the key to be deleted is greater than the root's key,
		// then it lies in right subtree
		if (key > root->llave) {
			root->right = avl_tree_nodo_borrar(arbolini, root->right, key,
					ignora_conteo, nodo_sobreviviente);
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
					caca_log_debug(
							"disminuiendo nodos realmente en arbol a %u\n",
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
							temp->llave, verdadero, NULL );
				}
			} else {
				root->ocurrencias--;
				*nodo_sobreviviente = root;
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

avl_tree_node_t *avl_tree_borrar(avl_tree_t *tree, tipo_dato value) {

	avl_tree_node_t *newroot = NULL;
	avl_tree_node_t *nodo_sobreviviente = NULL;

	if (!tree->root) {
		return NULL ;
	}
	newroot = avl_tree_nodo_borrar(tree, tree->root, value, falso,
			&nodo_sobreviviente);

	if (newroot != tree->root) {
		tree->root = newroot;
	}

	return nodo_sobreviviente;
}

#endif

typedef struct caca_x_numeros_unicos_en_rango {
	avl_tree_t *arbolazo;
	natural max_numeros;
	natural max_num_esperados;
	entero_largo suma;
	natural altura;
	natural idx;
	natural limite_izq;
	natural limite_der;
} caca_x_numeros_unicos_en_rango;

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
		avl_tree_create(&nodo->arbolazo, nodo->max_numeros);
	}
	nodo->idx = idx_nodo;
	nodo->limite_izq = limite_izq;
	nodo->limite_der = limite_der;

}

static inline void caca_x_clona_nodos(avl_tree_t *arbol_dst,
		avl_tree_node_t *nodo_ori, avl_tree_node_t *nodo_dst_padre,
		avl_tree_node_t **apuntador_hijo) {

	if (!nodo_ori) {
		return;
	}

	avl_tree_node_t *nodo_nuevo = avl_tree_create_node(arbol_dst,
			nodo_ori->llave);
	natural idx_en_arreglo_real = nodo_nuevo->indice_en_arreglo;

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

	assert_timeout(arbolin_izq || arbolin_der);
	if (arbolin_izq) {
		if (arbolin_der) {
			int tam_arbol_izq = arbolin_izq->nodos_realmente_en_arbol_utiles;
			int tam_arbol_der = arbolin_der->nodos_realmente_en_arbol_utiles;
			avl_tree_iterator_t *iter = &(avl_tree_iterator_t ) { 0 };
			avl_tree_t *arbol_menor;
			avl_tree_t *arbol_mayor;

			if (tam_arbol_izq <= tam_arbol_der) {
				arbol_menor = arbolin_izq;
				arbol_mayor = arbolin_der;
			} else {
				arbol_menor = arbolin_der;
				arbol_mayor = arbolin_izq;
			}

			caca_x_clona_arbol(arbolin_resultante, arbol_mayor);
			caca_log_debug(
					"ay arbol der, el res de la copia del mas grande es %s\n",
					avl_tree_sprint_identado(arbolin_resultante, CACA_X_BUF_STATICO_DUMP_ARBOL));

			assert_timeout(
					arbolin_resultante->nodos_realmente_en_arbol_utiles
							== arbol_mayor->nodos_realmente_en_arbol_utiles);

			avl_tree_iterador_ini(arbol_menor, iter);

			while (avl_tree_iterador_hay_siguiente(iter)) {
				avl_tree_node_t *nodo_actual = avl_tree_iterador_obtener_actual(
						iter);
				tipo_dato numero_actual = nodo_actual->llave;
				avl_tree_node_t *nodo_encontrado;

				if (!(nodo_encontrado = avl_tree_find(arbolin_resultante,
						numero_actual))) {
					caca_log_debug("el num %u no se encontraba, añadiendo\n",
							numero_actual);

					nodo_encontrado = avl_tree_insert(arbolin_resultante,
							numero_actual);
					nodo_encontrado->ocurrencias = nodo_actual->ocurrencias;
					arbolin_resultante->ocurrencias_totales +=
							nodo_actual->ocurrencias - 1;
				} else {
					caca_log_debug("el num %u ya estaba\n", numero_actual);
					nodo_encontrado->ocurrencias += nodo_actual->ocurrencias;
					arbolin_resultante->ocurrencias_totales +=
							nodo_actual->ocurrencias;
				}

				avl_tree_iterador_siguiente(iter);
			}

			avl_tree_iterador_fini(iter);

			caca_log_debug("el arbol mergeado es %s\n",
					avl_tree_sprint_identado(arbolin_resultante, CACA_X_BUF_STATICO_DUMP_ARBOL));
		} else {
			caca_x_clona_arbol(arbolin_resultante, arbolin_izq);
			caca_log_debug("no ay arbol der, el res es %s\n",
					avl_tree_sprint_identado(arbolin_resultante, CACA_X_BUF_STATICO_DUMP_ARBOL));
			assert_timeout(
					arbolin_resultante->nodos_realmente_en_arbol_utiles
							== arbolin_izq->nodos_realmente_en_arbol_utiles);
		}
	}
}

static inline avl_tree_t *caca_x_construye_arbol_binario_segmentado(
		natural idx_nodo, natural idx_num_ini, natural idx_num_fin,
		natural altura) {

	caca_x_numeros_unicos_en_rango *nodo = arbol_numeros_unicos + idx_nodo;
	caca_x_inicializar_nodo(nodo, altura, idx_nodo, idx_num_ini, idx_num_fin);

	if (idx_num_ini <= idx_numeros_max) {
		caca_log_debug("construyendo intervalo %u-%u\n", idx_num_ini,
				idx_num_fin);
		if (idx_num_ini != idx_num_fin) {
			natural idx_hijo_izq = idx_nodo << 1 | 1;
			natural altura_sig = altura - 1;
			natural idx_medio = idx_num_ini
					+ ((idx_num_fin - idx_num_ini) >> 1);
			avl_tree_t *arbolazo = nodo->arbolazo;

			caca_log_debug(
					"esto no es una elegia, digo oja, idx %u, altura %u\n",
					idx_nodo, altura);
			caca_x_mergear_arboles(
					caca_x_construye_arbol_binario_segmentado(idx_hijo_izq,
							idx_num_ini, idx_medio, altura_sig),
					caca_x_construye_arbol_binario_segmentado(idx_hijo_izq + 1,
							idx_medio + 1, idx_num_fin, altura_sig), arbolazo);

			if (idx_numeros_max >= idx_num_fin) {
				nodo->max_num_esperados = nodo->max_numeros;
			} else {
				if (idx_numeros_max >= idx_num_ini) {
					nodo->max_num_esperados =
							(idx_numeros_max - idx_num_ini + 1);
				}
			}

			if (arbolazo) {
				caca_log_debug("hay arbolin %s\n",
						avl_tree_sprint_identado(arbolazo, CACA_X_BUF_STATICO_DUMP_ARBOL));
				entero_largo suma_unicos = 0;
				avl_tree_iterator_t * iterador = &(avl_tree_iterator_t ) { 0 };

				avl_tree_iterador_ini(arbolazo, iterador);

				while (avl_tree_iterador_hay_siguiente(iterador)) {
					avl_tree_node_t *nodo_arbol_actual =
							(avl_tree_node_t*) avl_tree_iterador_obtener_actual(
									iterador);
					tipo_dato numero_unico_actual = nodo_arbol_actual->llave;
					caca_log_debug("sumando %u a suma aunicos \n",
							numero_unico_actual);
					suma_unicos += numero_unico_actual;
					caca_log_debug("la suma aora es %lld\n", suma_unicos);

					avl_tree_iterador_siguiente(iterador);
				}

				avl_tree_iterador_fini(iterador);
				caca_log_debug("la suma final es %lld\n", suma_unicos);

				nodo->suma = suma_unicos;
			}

			return arbolazo;
		} else {
			tipo_dato numero_actual = numeros[idx_num_ini];

			assert_timeout(altura == 0);
			nodo->suma = numero_actual;

			assert_timeout(!avl_tree_find(nodo->arbolazo, numero_actual));

#ifdef CACA_X_LOG
			avl_tree_node_t *res=avl_tree_insert(nodo->arbolazo, numero_actual);
			assert_timeout(
					res->ocurrencias
					== 1);
#else
			avl_tree_insert(nodo->arbolazo, numero_actual);
#endif
			nodo->arbolazo->max_nodos_utiles = nodo->max_num_esperados = 1;
			return nodo->arbolazo;
		}
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

static inline entero_largo caca_x_generar_suma_repetidos() {
	entero_largo suma_repetidos = 0;
	avl_tree_t *arbolin_unicos = NULL;

	caca_log_debug("sumando repetidos\n");

	avl_tree_create(&arbolin_unicos, CACA_X_MAX_NODOS);

	for (int i = 0; i < num_indices_nodos; i++) {
		avl_tree_t *arbolin_actual =
				arbol_numeros_unicos[indices_nodos[i]].arbolazo;

		if (!i) {
			caca_log_debug("primer arbol en nodo %d\n", i);
			caca_x_clona_arbol(arbolin_unicos, arbolin_actual);
		} else {
			bool tope_topado = falso;
			natural tope = 0;
			natural numero_minimo_arbol_actual = 0;
			natural numero_maximo_arbol_unicos = 0;
			natural num_encontrados_en_unicos = 0;
			natural num_encontrados_en_actual = 0;
			avl_tree_iterator_t *iter_actual = &(avl_tree_iterator_t ) { 0 };
			avl_tree_iterator_t *iter_unicos = &(avl_tree_iterator_t ) { 0 };
			avl_tree_node_t *nodo_minimo_arbol_actual = NULL;
			avl_tree_node_t *nodo_maximo_arbol_unicos = NULL;
			avl_tree_node_t *nodo_raiz_arbol_actual = NULL;

			nodo_raiz_arbol_actual = arbolin_actual->root;

			nodo_minimo_arbol_actual = avl_tree_max_min(arbolin_actual, falso);
			numero_minimo_arbol_actual = nodo_minimo_arbol_actual->llave;

			nodo_maximo_arbol_unicos = avl_tree_max_min(arbolin_unicos,
					verdadero);
			numero_maximo_arbol_unicos = nodo_maximo_arbol_unicos->llave;

			caca_log_debug(
					"nodo minimo de actual %d, nodo maximo de unicos %d\n",
					numero_minimo_arbol_actual, numero_maximo_arbol_unicos);

			avl_tree_iterador_ini(arbolin_actual, iter_actual);
			avl_tree_iterador_ini(arbolin_unicos, iter_unicos);

			if (numero_minimo_arbol_actual <= numero_maximo_arbol_unicos) {
				avl_tree_iterador_asignar_actual(iter_unicos,
						numero_minimo_arbol_actual);

				while (avl_tree_iterador_hay_siguiente(iter_unicos)) {
					natural numero_unicos = 0;
					avl_tree_node_t *nodo_unicos = NULL;
					avl_tree_node_t *nodo_nueva_raiz_arbol_actual = NULL;

					nodo_unicos = avl_tree_iterador_obtener_actual(iter_unicos);
					numero_unicos = nodo_unicos->llave;

					caca_log_debug("buscando %d en\n%s", numero_unicos,
							avl_tree_inoder_node_travesti( nodo_raiz_arbol_actual, CACA_X_BUF_STATICO_DUMP_ARBOL, nodo_raiz_arbol_actual->altura));

					if (avl_tree_find_descartando(nodo_raiz_arbol_actual,
							&nodo_nueva_raiz_arbol_actual, numero_unicos,
							numero_maximo_arbol_unicos, &tope_topado)) {
						caca_log_debug(
								"numero %d, se encontro que es duplicado en segment %d \n%s, proviene de unicos:\n%s\n",
								numero_unicos, i,
								avl_tree_inoder_node_travesti( nodo_raiz_arbol_actual, (char[100] ) { '\0' }, nodo_raiz_arbol_actual->altura),
								avl_tree_sprint_identado(arbolin_unicos, CACA_X_BUF_STATICO_DUMP_ARBOL));
						suma_repetidos += numero_unicos;
						num_encontrados_en_actual++;
					}

					if (tope_topado) {
						caca_log_debug(
								"se alcanzo el tope %d con el numero %d\n",
								numero_maximo_arbol_unicos, numero_unicos);
						break;
					}
					if (nodo_nueva_raiz_arbol_actual) {
						caca_log_debug("cambiada la raiz a buscar de %u a %u\n",
								nodo_raiz_arbol_actual->llave,
								nodo_nueva_raiz_arbol_actual->llave);
						nodo_raiz_arbol_actual = nodo_nueva_raiz_arbol_actual;
					}

					avl_tree_iterador_siguiente(iter_unicos);
				}

				caca_log_debug("termino de buscar unicos en actual\n");
			} else {
				caca_log_debug("no ay coincidencias entre segmentos\n");
			}

#ifdef CACA_X_VALIDAR_ARBOLINES
			while (avl_tree_iterador_hay_siguiente(iter_actual)) {
				natural numero_actual = 0;
				avl_tree_node_t *nodo_actual = NULL;

				nodo_actual = avl_tree_iterador_obtener_actual(iter_actual);
				numero_actual = nodo_actual->llave;

				if (!avl_tree_find(arbolin_unicos, numero_actual)) {
					caca_log_debug("añadiendo %d al arbol de unicos\n%s\n",
							numero_actual,
							avl_tree_sprint_identado(arbolin_unicos, CACA_X_BUF_STATICO_DUMP_ARBOL));
					avl_tree_insert(arbolin_unicos, numero_actual);
				} else {
					caca_log_debug(
							"numero %d, se encontro que es duplicado en \n%s, proviene de segmento actual %d:\n%s\n",
							numero_actual,
							avl_tree_sprint_identado(arbolin_unicos, CACA_X_BUF_STATICO_DUMP_ARBOL),
							i,
							avl_tree_sprint_identado(arbolin_actual, CACA_X_BUF_STATICO_DUMP_ARBOL));
					num_encontrados_en_unicos++;
				}

				avl_tree_iterador_siguiente(iter_actual);
			}

			assert_timeout(
					num_encontrados_en_actual == num_encontrados_en_unicos);

#endif
			avl_tree_iterador_fini(iter_actual);
			avl_tree_iterador_fini(iter_unicos);
		}
	}

	avl_tree_destroy(arbolin_unicos);

	caca_log_debug("en total la suma de repetidos es %lld\n", suma_repetidos);
	return suma_repetidos;
}

static inline entero_largo caca_x_generar_suma_unicos() {
	entero_largo suma_unicos;
	avl_tree_t *arbolin_unicos;

#ifdef CACA_X_LOG
	char buf[1000]=CACA_X_BUF_STATICO_DUMP_ARBOL;
#endif

	caca_log_debug("sumando unicos\n");

	avl_tree_create(&arbolin_unicos, CACA_X_MAX_NODOS);

	caca_x_numeros_unicos_en_rango *primer_arbol = arbol_numeros_unicos
			+ indices_nodos[0];
	caca_x_clona_arbol(arbolin_unicos, primer_arbol->arbolazo);
	suma_unicos = primer_arbol->suma;

	caca_log_debug("los indices afectados son %u: %s\n", num_indices_nodos,
			caca_arreglo_a_cadena((tipo_dato*) indices_nodos, num_indices_nodos, buf));

	for (int i = 1; i < num_indices_nodos; i++) {
		avl_tree_t *arbolin_actual =
				arbol_numeros_unicos[indices_nodos[i]].arbolazo;

		avl_tree_iterator_t *iter_actual = &(avl_tree_iterator_t ) { 0 };

		avl_tree_iterador_ini(arbolin_actual, iter_actual);

		while (avl_tree_iterador_hay_siguiente(iter_actual)) {
			avl_tree_node_t *nodo_actual = avl_tree_iterador_obtener_actual(
					iter_actual);
			entero_largo numero_actual = nodo_actual->llave;

			if (!avl_tree_find(arbolin_unicos, numero_actual)) {
				caca_log_debug("añadiendo %d al arbol de unicos\n%s\n",
						numero_actual,
						avl_tree_sprint_identado(arbolin_unicos, CACA_X_BUF_STATICO_DUMP_ARBOL));
				avl_tree_insert(arbolin_unicos, numero_actual);
				suma_unicos += numero_actual;
			} else {
				caca_log_debug(
						"numero %d, se encontro que es duplicado en \n%s, proviene de segmento actual %d:\n%s\n",
						numero_actual,
						avl_tree_sprint_identado(arbolin_unicos, CACA_X_BUF_STATICO_DUMP_ARBOL),
						i,
						avl_tree_sprint_identado(arbolin_actual, CACA_X_BUF_STATICO_DUMP_ARBOL));
			}

			avl_tree_iterador_siguiente(iter_actual);
		}

	}

	avl_tree_destroy(arbolin_unicos);

	caca_log_debug("en total la suma de repetidos es %lld\n", suma_unicos);
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

static inline entero_largo caca_x_suma_segmento() {
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
			caca_arreglo_a_cadena((tipo_dato *) indices_nodos, num_indices_nodos, buf));

#if 1
	for (int i = 0; i < num_indices_nodos; i++) {
		caca_log_debug("segmento %d aporta %lu\n", indices_nodos[i],
				arbol_numeros_unicos[indices_nodos[i]].suma);
		res += arbol_numeros_unicos[indices_nodos[i]].suma;
	}

	res -= caca_x_generar_suma_repetidos();
#endif

#if 0
	res = caca_x_generar_suma_unicos();
	caca_log_debug("La suma es %lld\n", res);

#endif
	return res;
}

static inline void caca_x_actualiza_segmentos(natural idx_nodo) {
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

	avl_tree_t *arbolini = nodo->arbolazo;

	if (nodo_lim_izq != nodo_lim_der) {
		natural idx_nodo_hijo_izq = (idx_nodo << 1) | 1;
		natural idx_nodo_hijo_der = idx_nodo_hijo_izq + 1;
		caca_log_debug(
				"antes de actualizar recursivo nodo %u %u->%u suma %lld\n",
				idx_nodo, nodo_lim_izq, nodo_lim_der, nodo->suma);

		caca_x_actualiza_segmentos(idx_nodo_hijo_izq);
		caca_x_actualiza_segmentos(idx_nodo_hijo_der);

		if (!avl_tree_borrar(arbolini, viejo_pendejo)) {
			nodo->suma -= viejo_pendejo;
		}
		if (avl_tree_insert(arbolini, nuevo_valor)->ocurrencias == 1) {
			nodo->suma += nuevo_valor;
		}
		caca_log_debug(
				"despues de actualizar recursivo nodo %u %u->%u suma %lld\n",
				idx_nodo, nodo_lim_izq, nodo_lim_der, nodo->suma);
	} else {
		if (!avl_tree_borrar(arbolini, viejo_pendejo)) {
			nodo->suma -= viejo_pendejo;
		}
		if (avl_tree_insert(arbolini, nuevo_valor)->ocurrencias == 1) {
			nodo->suma += nuevo_valor;
		}
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

			avl_tree_node_t *nodo_act = NULL;
			avl_tree_iterator_t *iter = &(avl_tree_iterator_t ) { 0 };

			suma_presumado = nodo->suma;

			for (natural j = num_idx_ini;
					j
							<= (num_idx_fin > idx_numeros_max ?
									idx_numeros_max : num_idx_fin); j++) {
				if (!caca_comun_checa_bit(mapa_unicos,
						(unsigned long) (numeros[j]
								+ (unsigned long) ((unsigned long) INT_MAX + 1)))) {
					suma_mapa_unicos += numeros[j];
					caca_comun_asigna_bit(mapa_unicos,
							(unsigned long) (numeros[j]
									+ (unsigned long) ((unsigned long) INT_MAX
											+ 1)));
					assert_timeout(avl_tree_find(nodo->arbolazo, numeros[j]));
					encontrados_en_mapa += 1;
				}
			}

			avl_tree_iterador_ini(nodo->arbolazo, iter);

			while (avl_tree_iterador_hay_siguiente(iter)) {
				nodo_act = avl_tree_iterador_obtener_actual(iter);
				suma_arboles += nodo_act->llave;

				assert_timeout(
						caca_comun_checa_bit(mapa_unicos, (unsigned long) (nodo_act->llave + (unsigned long) ((unsigned long) INT_MAX + 1))));
				avl_tree_iterador_siguiente(iter);
				encontrados_en_arbol += 1;
				caca_comun_limpia_bit(mapa_unicos,
						(unsigned long) (nodo_act->llave
								+ (unsigned long) ((unsigned long) INT_MAX + 1)));
			}

			assert_timeout(
					encontrados_en_mapa
							== nodo->arbolazo->nodos_realmente_en_arbol_utiles);

			assert_timeout(encontrados_en_arbol == encontrados_en_mapa);

			avl_tree_iterador_fini(iter);

			for (natural j = num_idx_ini; j <= num_idx_fin; j++) {
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
	natural conteo_por_nivel[CACA_X_MAX_PROFUNDIDAD] = { 0 };

	caca_x_validar_segmentos_int(arbolin_segs, 0, conteo_por_nivel, num_nodos);

	for (int i = 0; i <= arbolin_segs->altura; i++) {
		assert_timeout(conteo_por_nivel[i] == num_numeros);
	}

	caca_x_valida_segmentos_sumas(arbolin_segs, numeros, num_nodos, num_numeros,
			indices, num_indices);

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

	free(matriz_nums);
	free(arbol_numeros_unicos);
}

int main(void) {
//	puts("he corrido con algo de suerte");
//	sleep(10);
	caca_x_main();
	return EXIT_SUCCESS;
}
