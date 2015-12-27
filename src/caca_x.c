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
#define MAX_NUMEROS_REDONDEADO 65536
#define MAX_VALOR INT_MAX
#define MAX_QUERIES 100000
#define TAM_MAX_LINEA (MAX_NUMEROS*10+MAX_NUMEROS)
#define MAX_NODOS (1 << 16)
#define CACA_X_VALOR_INVALIDO -1

#define CACA_X_VALIDAR_ARBOLINES

#define caca_log_debug(formato, args...) 0
/*
 #define caca_log_debug printf
 */

/*
 #define assert_timeout(condition) assert(condition);
 #define assert_timeout(condition) if(!(condition)){printf("fuck\n");sleep(10);}
 */
#define assert_timeout(condition) 0


typedef int tipo_dato;

typedef long long bitch_vector;

typedef enum BOOLEANOS {
	falso = 0, verdadero
} bool;

#if 1

struct avl_node_s {
	long llave;
	int altura;
	int indice_en_arreglo;
	struct avl_node_s *left;
	struct avl_node_s *right;
	struct avl_node_s *padre;
};

typedef struct avl_node_s avl_node_t;

struct avl_tree_s {
	int max_nodos;
	int nodos_usados;
	int nodos_realmente_en_arbol;
	int nodos_realmente_creados;
	int ultimo_nodo_liberado_idx;
	struct avl_node_s *root;
	avl_node_t *nodos_mem;
};

typedef struct avl_tree_s avl_tree_t;

typedef struct avl_tree_iterator_t {
	avl_tree_t *arbolin;
	char *contador_visitas;
	avl_node_t *nodo_actual;
} avl_tree_iterator_t;

#endif

typedef struct caca_x_numeros_unicos_en_rango {
	avl_tree_t *arbolazo;
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
	int num_popeado;
	caca_x_numeros_unicos_en_rango *nodo;
} caca_x_estado_recursion;

caca_x_numeros_unicos_en_rango *arbol_numeros_unicos = NULL;

caca_x_estado_recursion *estado = NULL;

#define max(x,y) ((x) < (y) ? (y) : (x))
#define min(x,y) ((x) < (y) ? (x) : (y))

#if 1

/* Create a new AVL tree. */
avl_tree_t *avl_create(avl_tree_t **arbolin, int max_nodos) {
	avl_tree_t *tree = NULL;

	assert_timeout(arbolin);

	tree = calloc(1, sizeof(avl_tree_t));

	assert_timeout(tree);

	tree->max_nodos = max_nodos;

	tree->nodos_mem = calloc(max_nodos * 2, sizeof(avl_node_t));

	assert_timeout(tree->nodos_mem);

	tree->root = NULL;

	*arbolin = tree;

	return tree;
}

static inline void avl_destroy(avl_tree_t *arbolin) {
	free(arbolin->nodos_mem);
	free(arbolin);

}

/* Initialize a new node. */
avl_node_t *avl_create_node(avl_tree_t *arbolin) {
	avl_node_t *node = NULL;

	assert(
			arbolin->ultimo_nodo_liberado_idx
					|| arbolin->nodos_usados <= arbolin->max_nodos);

	if (arbolin->ultimo_nodo_liberado_idx) {
		node = arbolin->nodos_mem + arbolin->ultimo_nodo_liberado_idx;
		node->indice_en_arreglo = arbolin->ultimo_nodo_liberado_idx;
		arbolin->ultimo_nodo_liberado_idx = 0;
	} else {
		node = arbolin->nodos_mem + arbolin->nodos_usados++;
		node->indice_en_arreglo = arbolin->nodos_usados - 1;

		arbolin->nodos_realmente_en_arbol++;
		arbolin->nodos_realmente_creados++;
	}
	return node;
}

/* Find the height of an AVL no repulsivamente */
int avl_node_height(avl_node_t *node) {
	return node ? node->altura : 0;
}

static inline void avl_node_actualizar_altura(avl_node_t *node) {
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

/* Find the balance of an AVL node */
int avl_balance_factor(avl_node_t *node) {
	int bf = 0;

	if (node->left)
		bf += avl_node_height(node->left);
	if (node->right)
		bf -= avl_node_height(node->right);

	return bf;
}

/* Left Left Rotate */
avl_node_t *avl_rotate_leftleft(avl_node_t *node) {
	avl_node_t *a = node;
	avl_node_t *b = a->left;
	avl_node_t *padre = NULL;

	padre = node->padre;

	a->left = b->right;
	b->right = a;

	avl_node_actualizar_altura(a);
	avl_node_actualizar_altura(b);

	a->padre = b;
	b->padre = padre;
	if (a->left) {
		a->left->padre = a;
	}

	return (b);
}

/* Left Right Rotate */
avl_node_t *avl_rotate_leftright(avl_node_t *node) {
	avl_node_t *a = node;
	avl_node_t *b = a->left;
	avl_node_t *c = b->right;
	avl_node_t *padre = NULL;

	padre = node->padre;

	a->left = c->right;
	b->right = c->left;
	c->left = b;
	c->right = a;

	avl_node_actualizar_altura(a);
	avl_node_actualizar_altura(b);
	avl_node_actualizar_altura(c);

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
avl_node_t *avl_rotate_rightleft(avl_node_t *node) {
	avl_node_t *a = node;
	avl_node_t *b = a->right;
	avl_node_t *c = b->left;
	avl_node_t *padre = NULL;

	padre = node->padre;

	a->right = c->left;
	b->left = c->right;
	c->right = b;
	c->left = a;

	avl_node_actualizar_altura(a);
	avl_node_actualizar_altura(b);
	avl_node_actualizar_altura(c);

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
avl_node_t *avl_rotate_rightright(avl_node_t *node) {
	avl_node_t *a = node;
	avl_node_t *b = a->right;
	avl_node_t *padre = NULL;

	padre = node->padre;

	a->right = b->left;
	b->left = a;

	avl_node_actualizar_altura(a);
	avl_node_actualizar_altura(b);

	a->padre = b;
	b->padre = padre;
	if (a->right) {
		a->right->padre = a;
	}

	return (b);
}

static inline avl_node_t *avl_balance_node_insertar(const avl_node_t *node,
		const long llave_nueva) {
	avl_node_t *newroot = NULL;
	avl_node_t *nodo_actual = NULL;

	newroot = (avl_node_t *) node;
	nodo_actual = node->padre;
	while (nodo_actual) {

		int bf = 0;
		avl_node_t *padre = NULL;
		avl_node_t **rama_padre = NULL;

		bf = avl_balance_factor(nodo_actual);

		if (bf >= 2) {
			/* Left Heavy */

			if (llave_nueva > nodo_actual->left->llave) {
				newroot = avl_rotate_leftright(nodo_actual);
			} else {
				newroot = avl_rotate_leftleft(nodo_actual);
			}

		} else if (bf <= -2) {
			/* Right Heavy */
			if (llave_nueva < nodo_actual->right->llave) {
				newroot = avl_rotate_rightleft(nodo_actual);
			} else {
				newroot = avl_rotate_rightright(nodo_actual);
			}

		} else {
			/* This node is balanced -- no change. */

			newroot = nodo_actual;
			avl_node_actualizar_altura(nodo_actual);
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
void avl_balance_insertar(avl_tree_t *tree, avl_node_t *nodo, long llave_nueva) {

	avl_node_t *newroot = NULL;

	newroot = avl_balance_node_insertar(nodo, llave_nueva);

	if (newroot != tree->root) {
		tree->root = newroot;
	}
}

/* Insert a new node. */
void avl_insert(avl_tree_t *tree, long value) {
	avl_node_t *node = NULL;
	avl_node_t *next = NULL;
	avl_node_t *last = NULL;

	/* Well, there must be a first case */
	if (tree->root == NULL ) {
		node = avl_create_node(tree);
		node->llave = value;

		tree->root = node;

		/* Okay.  We have a root already.  Where do we put this? */
	} else {
		next = tree->root;

		while (next != NULL ) {
			last = next;

			if (value < next->llave) {
				next = next->left;

			} else if (value > next->llave) {
				next = next->right;

				/* Have we already inserted this node? */
			} else if (value == next->llave) {
				caca_log_debug("llave ya existe, no insertada\n");
				assert_timeout(0);
			}
		}

		node = avl_create_node(tree);
		node->llave = value;

		if (value < last->llave) {
			last->left = node;
		}
		if (value > last->llave) {
			last->right = node;
		}

		node->padre = last;

	}

	avl_balance_insertar(tree, node, value);
}

/* Find the node containing a given value */
avl_node_t *avl_find(avl_tree_t *tree, long value) {
	avl_node_t *current = tree->root;

	while (current && current->llave != value) {
		if (value > current->llave)
			current = current->right;
		else
			current = current->left;
	}

	return current ? current->llave == value ? current : NULL :NULL;
}

avl_node_t *avl_find_descartando(avl_node_t *nodo_raiz,
		avl_node_t **primer_nodo_mayor_o_igual, long value, int tope,
		bool *tope_topado) {
	avl_node_t *current = NULL;
	avl_node_t *primer_nodo_mayor = NULL;

	current = nodo_raiz;
	assert_timeout(!tope_topado || tope);

	if (tope_topado) {
		*tope_topado = falso;
	}

	while (current && current->llave != value) {
		if (tope_topado) {
			if (current->llave >= tope && value >= tope) {
				*tope_topado = verdadero;
				break;
			}
		}
		if (value > current->llave) {
			current = current->right;
		} else {
			if (!primer_nodo_mayor) {
				primer_nodo_mayor = current;
			}
			current = current->left;
		}
	}

	*primer_nodo_mayor_o_igual = primer_nodo_mayor;
	if (!primer_nodo_mayor_o_igual) {
		if (current && (current->llave == value)) {
			*primer_nodo_mayor_o_igual = current;
		}
	}

	return current ? current->llave == value ? current : NULL :NULL;
}

	/* Do a depth first traverse of a node. */
void avl_traverse_node_dfs(avl_node_t *node, int depth) {
	int i = 0;

	if (node->left)
		avl_traverse_node_dfs(node->left, depth + 2);

	for (i = 0; i < depth; i++)
		putchar(' ');
	printf("%ld: %d\n", node->llave, avl_balance_factor(node));

	if (node->right)
		avl_traverse_node_dfs(node->right, depth + 2);
}

/* Do a depth first traverse of a tree. */
void avl_traverse_dfs(avl_tree_t *tree) {
	avl_traverse_node_dfs(tree->root, 0);
}

static inline void avl_tree_iterador_ini(avl_tree_t *arbolin,
		avl_tree_iterator_t *iter) {
	iter->contador_visitas = calloc(arbolin->nodos_realmente_creados,
			sizeof(char));
	assert_timeout(iter->contador_visitas);
	iter->arbolin = arbolin;
}

static inline void avl_tree_iterador_fini(avl_tree_iterator_t *iter) {
	free(iter->contador_visitas);
}

static inline bool avl_tree_iterador_hay_siguiente(avl_tree_iterator_t *iter) {
	return !(iter->nodo_actual == iter->arbolin->root
			&& iter->contador_visitas[iter->arbolin->root->indice_en_arreglo]
					== 2);
}

static inline avl_node_t* avl_tree_iterador_siguiente(avl_tree_iterator_t *iter) {
	int contador_actual = 0;
	avl_node_t *nodo = NULL;
	avl_node_t *last_of_us = NULL;
	avl_node_t *nodo_actual = NULL;

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
		assert_timeout(0);
		break;
	}

	return nodo;
}

static inline avl_node_t* avl_tree_iterador_obtener_actual(
		avl_tree_iterator_t *iter) {
	avl_node_t *nodo = NULL;

	if (!iter->nodo_actual) {
		avl_tree_iterador_siguiente(iter);
	}
	nodo = iter->nodo_actual;

	return nodo;

}

static inline char *avl_tree_inoder_node_travesti(avl_node_t *nodo, char *buf,
		int profundidad_maxima) {
	char num_buf[100] = { '\0' };
	int profundidad = 0;

	assert_timeout(profundidad_maxima == -1 || profundidad != -1);

	if (nodo != NULL ) {
		profundidad = profundidad_maxima - nodo->altura;

		assert_timeout(!nodo->right || nodo->right->padre == nodo);
		avl_tree_inoder_node_travesti(nodo->right, buf, profundidad_maxima);

		if (profundidad_maxima != -1) {
			for (int i = 0; i < profundidad; i++) {
				strcat(buf, " ");
			}
		}
		sprintf(num_buf, "%ld", nodo->llave);
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

static inline char* avl_tree_sprint_identado(avl_tree_t *arbolini, char *buf) {
	int profundidad_maxima = 0;

	if (!arbolini->root) {
		return buf;
	}

	profundidad_maxima = arbolini->root->altura;
	avl_tree_inoder_node_travesti(arbolini->root, buf, profundidad_maxima);
	return buf;
}

static inline avl_node_t* avl_tree_iterador_asignar_actual(
		avl_tree_iterator_t *iter, int llave) {
	avl_tree_t *arbolazo = NULL;
	avl_node_t *nodo_actual = NULL;
	avl_node_t *last_of_us = NULL;

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

static inline avl_node_t* avl_tree_max_min(avl_tree_t *arbolin, bool max) {
	avl_node_t *nodo_actual = NULL;
	avl_node_t *last_of_us = NULL;

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
		avl_node_t *nodo) {
	if (nodo != NULL ) {
		assert_timeout(&arbolin->nodos_mem[nodo->indice_en_arreglo] == nodo);
		assert_timeout(!nodo->left || nodo->left->padre == nodo);
		avl_tree_validar_arbolin_indices(arbolin, nodo->left);
		assert_timeout(!nodo->right || nodo->right->padre == nodo);
		avl_tree_validar_arbolin_indices(arbolin, nodo->right);
	}
}

/* Balance a given node */
/* Given a non-empty binary search tree, return the node with minimum
 key value found in that tree. Note that the entire tree does not
 need to be searched. */
static inline avl_node_t* avl_siguiente_nodo_inorder(avl_node_t *node) {
	avl_node_t *current = node;

	/* loop down to find the leftmost leaf */
	while (current->left != NULL ) {
		current = current->left;
	}

	return current;
}

static inline avl_node_t *avl_nodo_borrar(avl_tree_t *arbolini,
		avl_node_t *root, int key) {
	// STEP 1: PERFORM STANDARD BST DELETE

	if (root == NULL ) {
		return root;
	}

	if (key < root->llave) {
		// If the key to be deleted is smaller than the root's key,
		// then it lies in left subtree
		root->left = avl_nodo_borrar(arbolini, root->left, key);
		assert_timeout(!root->left || root->left->padre == root);
	} else {
		// If the key to be deleted is greater than the root's key,
		// then it lies in right subtree
		if (key > root->llave) {
			root->right = avl_nodo_borrar(arbolini, root->right, key);
			assert_timeout(!root->right || root->right->padre == root);
		} else {
			if (root->left == NULL || root->right == NULL ) {
				avl_node_t *temp = root->left ? root->left : root->right;

				// No child case
				if (temp == NULL ) {
					temp = root;
					root = NULL;
				} else {
					// One child case
					int idx_en_arreglo = 0;
					avl_node_t *padre = NULL;

					padre = root->padre;
					idx_en_arreglo = root->indice_en_arreglo;
					*root = *temp; // Copy the contents of the non-empty child
					root->padre = padre;
					root->indice_en_arreglo = idx_en_arreglo;
					if (root->left) {
						root->left->padre = root;
					}
					if (root->right) {
						root->right->padre = root;
					}
				}

				assert_timeout(!arbolini->ultimo_nodo_liberado_idx);
				arbolini->ultimo_nodo_liberado_idx = temp->indice_en_arreglo;
				memset(temp, 0, sizeof(avl_node_t));
				temp->llave = CACA_X_VALOR_INVALIDO;

			} else {
				// node with two children: Get the inorder successor (smallest
				// in the right subtree)
				avl_node_t *temp = avl_siguiente_nodo_inorder(root->right);

				// Copy the inorder successor's data to this node
				root->llave = temp->llave;

				// Delete the inorder successor
				root->right = avl_nodo_borrar(arbolini, root->right,
						temp->llave);
			}
		}
	}

// If the tree had only one node then return
	if (root == NULL ) {
		return root;
	}

// STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
	avl_node_actualizar_altura(root);

// STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to check whether
//  this node became unbalanced)
	int balance = avl_balance_factor(root);

// If this node becomes unbalanced, then there are 4 cases

// Left Left Case
	if (balance > 1 && avl_balance_factor(root->left) >= 0) {
		return avl_rotate_leftleft(root);
	}

// Left Right Case
	if (balance > 1 && avl_balance_factor(root->left) < 0) {
		return avl_rotate_leftright(root);
	}

// Right Right Case
	if (balance < -1 && avl_balance_factor(root->right) <= 0) {
		return avl_rotate_rightright(root);
	}

// Right Left Case
	if (balance < -1 && avl_balance_factor(root->right) > 0) {
		return avl_rotate_rightleft(root);
	}

	return root;
}

void avl_borrar(avl_tree_t *tree, int value) {

	avl_node_t *newroot = NULL;

	newroot = avl_nodo_borrar(tree, tree->root, value);

	if (newroot != tree->root) {
		tree->root = newroot;
	}
	tree->nodos_realmente_en_arbol--;
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
	*(ap_buffer + characteres_escritos) = '\0';
	return ap_buffer;
}

static inline void caca_x_inicializar_nodo(caca_x_numeros_unicos_en_rango *nodo,
		int altura, int idx_nodo, int limite_izq, int limite_der) {
	nodo->altura = altura;
	nodo->max_numeros = 1 << nodo->altura;
	avl_create(&nodo->arbolazo, nodo->max_numeros);
	nodo->idx = idx_nodo;
	nodo->limite_izq = limite_izq;
	nodo->limite_der = limite_der;

}

static inline void caca_x_clona_nodos(avl_tree_t *arbol_dst,
		avl_node_t *nodo_ori, avl_node_t *nodo_dst_padre,
		avl_node_t **apuntador_hijo) {
	int idx_en_arreglo_real = 0;
	avl_node_t *nodo_nuevo = NULL;

	if (!nodo_ori) {
		return;
	}

	nodo_nuevo = avl_create_node(arbol_dst);
	idx_en_arreglo_real = nodo_nuevo->indice_en_arreglo;

	memcpy(nodo_nuevo, nodo_ori, offsetof(avl_node_t,left));

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
	tam_arbol_ori = arbol_ori->nodos_realmente_en_arbol;

	assert_timeout(tam_arbol_dst >= tam_arbol_ori);
	assert_timeout(!arbol_dst->nodos_usados);

	caca_x_clona_nodos(arbol_dst, arbol_ori->root, NULL, NULL );

}

static inline void caca_x_mergear_arboles(avl_tree_t *arbolin_izq,
		avl_tree_t *arbolin_der, avl_tree_t *arbolin_resultante) {
	int tam_arbol_izq = 0;
	int tam_arbol_der = 0;
	int tam_arbol_menor = 0;
	avl_tree_iterator_t *iter = &(avl_tree_iterator_t ) { 0 };
	avl_tree_t *arbol_menor = NULL;
	avl_tree_t *arbol_mayor = NULL;

	assert_timeout(arbolin_izq && arbolin_der);

	tam_arbol_izq = arbolin_izq->nodos_realmente_en_arbol;
	tam_arbol_der = arbolin_der->nodos_realmente_en_arbol;

	if (tam_arbol_izq <= tam_arbol_der) {
		tam_arbol_menor = tam_arbol_izq;
		arbol_menor = arbolin_izq;
		arbol_mayor = arbolin_der;
	} else {
		tam_arbol_menor = tam_arbol_der;
		arbol_menor = arbolin_der;
		arbol_mayor = arbolin_izq;
	}

	caca_x_clona_arbol(arbolin_resultante, arbol_mayor);

	avl_tree_iterador_ini(arbol_menor, iter);

	while (avl_tree_iterador_hay_siguiente(iter)) {
		int numero_actual = 0;
		avl_node_t *nodo_actual = NULL;

		nodo_actual = avl_tree_iterador_obtener_actual(iter);
		numero_actual = nodo_actual->llave;

		if (!avl_find(arbolin_resultante, numero_actual)) {
			avl_insert(arbolin_resultante, numero_actual);
		}

		avl_tree_iterador_siguiente(iter);
	}

	avl_tree_iterador_fini(iter);
}

static inline void caca_x_construye_arbol_binario_segmentado(int *numeros,
		int num_numeros, int max_profundidad) {
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

#ifndef CACA_X_VALIDAR_ARBOLINES
				avl_tree_validar_arbolin_indices(estado_actual->nodo->arbolazo,
						estado_actual->nodo->arbolazo->root);
#endif

				estado_actual->nodo->num_numeros =
						estado_actual->nodo->arbolazo->nodos_realmente_en_arbol;

				memset(estado_actual, 0, sizeof(caca_x_estado_recursion));
				caca_log_debug("ultimo 'recursion' de nodo %d, nada q acer\n",
						idx_nodo);
			}
				break;
			default:
				assert_timeout(0);
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

			if (!avl_find(nodo->arbolazo, numero_actual)) {
				avl_insert(nodo->arbolazo, (long) numero_actual);
				nodo->num_numeros++;
			}

			caca_log_debug("asignado unico numero %d a indice de arbol %d\n",
					numero_actual, idx_nodo);

			memset(estado_actual, 0, sizeof(caca_x_estado_recursion));
		}
	}
}

static inline void caca_x_suma_unicos(long *sumas_arbol_segmentado,
		int num_nodos) {
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
		/*
		 avltree_print2(arbolazo_actual);
		 */

		avl_tree_iterador_ini(arbolazo_actual, iterador);

		sumas_arbol_segmentado[i] = 0;

		while (avl_tree_iterador_hay_siguiente(iterador)) {
			int numero_unico_actual = 0;
			avl_node_t *nodo_arbol_actual = NULL;

			nodo_arbol_actual = (avl_node_t*) avl_tree_iterador_obtener_actual(
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

		assert_timeout(
				nodo->num_numeros == arbolazo_actual->nodos_realmente_en_arbol);

		caca_log_debug("la suma es %ld\n", sumas_arbol_segmentado[i]);
	}

	free(buf);
	free(numeros_unicos);
}

static inline int caca_x_calcular_suma_intersexion(
		caca_x_numeros_unicos_en_rango *nodo_izq,
		caca_x_numeros_unicos_en_rango *nodo_der) {
	int res = 0;
	int num_arbolazo_mayor = 0;
	int num_arbolazo_menor = 0;

	avl_tree_iterator_t *iterador = &(avl_tree_iterator_t ) { 0 };
	avl_tree_t *arbolazo_izq = NULL;
	avl_tree_t *arbolazo_der = NULL;
	avl_tree_t *arbolazo_mayor = NULL;
	avl_tree_t *arbolazo_menor = NULL;

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

	avl_tree_iterador_ini(arbolazo_menor, iterador);

	while (avl_tree_iterador_hay_siguiente(iterador)) {
		int numero_actual = 0;
		long numero_encontrado = 0;
		avl_node_t *nodo_arbol_actual = NULL;

		nodo_arbol_actual = (avl_node_t *) avl_tree_iterador_obtener_actual(
				iterador);

		if (nodo_arbol_actual) {

			numero_actual = (int) nodo_arbol_actual->llave;

			caca_log_debug("el numero %d se va a buscar \n", numero_actual);

			numero_encontrado =
					avl_find(arbolazo_mayor, (long) numero_actual)->llave;
			if (numero_encontrado) {
				caca_log_debug("el numero %d encontrado en intersexion\n",
						numero_actual);
				res += numero_actual;
			}
		}

		avl_tree_iterador_siguiente(iterador);
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
							nodo_ancestro->limite_izq <= limite_intervalo && limite_intervalo <= nodo_ancestro->limite_der);

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

static inline long caca_x_generar_suma_repetidos(
		caca_x_numeros_unicos_en_rango *arbol_numeros_unicos, int *indices,
		int num_indices) {
	long suma_repetidos = 0;
	avl_tree_t *arbolin_unicos = NULL;

	caca_log_debug("sumando repetidos\n");

	avl_create(&arbolin_unicos, MAX_NODOS);

	for (int i = 0; i < num_indices; i++) {
		avl_tree_t *arbolin_actual = NULL;

		arbolin_actual = arbol_numeros_unicos[indices[i]].arbolazo;

		if (!i) {
			caca_log_debug("primer arbol en nodo %d\n", i);
			caca_x_clona_arbol(arbolin_unicos, arbolin_actual);
		} else {
			bool tope_topado = falso;
			int tope = 0;
			int numero_minimo_arbol_actual = 0;
			int numero_maximo_arbol_unicos = 0;
			int num_encontrados_en_unicos = 0;
			int num_encontrados_en_actual = 0;
			avl_tree_iterator_t *iter_actual = &(avl_tree_iterator_t ) { 0 };
			avl_tree_iterator_t *iter_unicos = &(avl_tree_iterator_t ) { 0 };
			avl_node_t *nodo_minimo_arbol_actual = NULL;
			avl_node_t *nodo_maximo_arbol_unicos = NULL;
			avl_node_t *nodo_raiz_arbol_actual = NULL;

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
					int numero_unicos = 0;
					avl_node_t *nodo_unicos = NULL;
					avl_node_t *nodo_nueva_raiz_arbol_actual = NULL;

					nodo_unicos = avl_tree_iterador_obtener_actual(iter_unicos);
					numero_unicos = nodo_unicos->llave;

					caca_log_debug("buscando %d en\n%s", numero_unicos,
							avl_tree_inoder_node_travesti( nodo_raiz_arbol_actual, (char[100] ) { '\0' }, nodo_raiz_arbol_actual->altura));

					if (avl_find_descartando(nodo_raiz_arbol_actual,
							&nodo_nueva_raiz_arbol_actual, numero_unicos,
							numero_maximo_arbol_unicos, &tope_topado)) {
						caca_log_debug(
								"numero %d, se encontro que es duplicado en segment %d \n%s, proviene de unicos:\n%s\n",
								numero_unicos, i,
								avl_tree_inoder_node_travesti( nodo_raiz_arbol_actual, (char[100] ) { '\0' }, nodo_raiz_arbol_actual->altura),
								avl_tree_sprint_identado(arbolin_unicos, (char[100] ) { '\0' }));
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
						caca_log_debug(
								"cambiada la raiz a buscar de %ld a %ld\n",
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

			while (avl_tree_iterador_hay_siguiente(iter_actual)) {
				int numero_actual = 0;
				avl_node_t *nodo_actual = NULL;

				nodo_actual = avl_tree_iterador_obtener_actual(iter_actual);
				numero_actual = nodo_actual->llave;

				if (!avl_find(arbolin_unicos, numero_actual)) {
					caca_log_debug("añadiendo %d al arbol de unicos\n%s\n",
							numero_actual,
							avl_tree_sprint_identado(arbolin_unicos, (char[100] ) { '\0' }));
					avl_insert(arbolin_unicos, numero_actual);
				} else {
					caca_log_debug(
							"numero %d, se encontro que es duplicado en \n%s, proviene de segmento actual %d:\n%s\n",
							numero_actual,
							avl_tree_sprint_identado(arbolin_unicos, (char[100] ) { '\0' }),
							i,
							avl_tree_sprint_identado(arbolin_actual, (char[100] ) { '\0' }));
					num_encontrados_en_unicos++;
				}

				avl_tree_iterador_siguiente(iter_actual);
			}

			assert_timeout(
					num_encontrados_en_actual == num_encontrados_en_unicos);

			avl_tree_iterador_fini(iter_actual);
			avl_tree_iterador_fini(iter_unicos);
		}
	}

	avl_destroy(arbolin_unicos);

	caca_log_debug("en total la suma de repetidos es %ld\n", suma_repetidos);
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

static inline long caca_x_suma_segmento(long *sumas_arbol_segmentado,
		int limite_izq, int limite_der) {
	long res = 0;
	int num_indices_nodos = 0;
	int *indices_nodos = (int[30] ) { 0 };
	caca_x_numeros_unicos_en_rango *nodo_izq = NULL;
	caca_x_numeros_unicos_en_rango *nodo_der = NULL;
	char buf[100] = { '\0' };

	caca_x_encuentra_indices_segmento(arbol_numeros_unicos, 0, limite_izq,
			limite_der, indices_nodos, &num_indices_nodos);

	assert_timeout(num_indices_nodos < 30);

	qsort(indices_nodos, num_indices_nodos, sizeof(int),
			caca_comun_compara_enteros);
	caca_log_debug("indices de segmento %d:%d %s\n", limite_izq, limite_der,
			caca_arreglo_a_cadena(indices_nodos, num_indices_nodos, buf));

	for (int i = 0; i < num_indices_nodos; i++) {
		caca_log_debug("segmento %d aporta %d\n", indices_nodos[i],
				sumas_arbol_segmentado[indices_nodos[i]]);
		res += sumas_arbol_segmentado[indices_nodos[i]];
	}

	res -= caca_x_generar_suma_repetidos(arbol_numeros_unicos, indices_nodos,
			num_indices_nodos);
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
		avl_tree_t *arbolazo_nodo_lateral = NULL;

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

		numero_encontrado = (bool) avl_find(arbolazo_nodo_lateral,
				(long) viejo_valor);

		if (numero_encontrado) {
			matriz_sumas_coincidencias[idx_matrix_izq][idx_matrix_der] -=
					viejo_valor;
		}

		numero_encontrado = (bool) avl_find(arbolazo_nodo_lateral,
				(long) nuevo_valor);

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
		char buf1[100] = { '\0' };

		idx_a_actualizar = indices_afectados[i];

		caca_x_encuentra_indices_a_actualizar_laterales(arbol_numeros_unicos,
				idx_a_actualizar, indices_izq, indices_der, &num_indices_izq,
				&num_indices_der, num_nodos);

		caca_log_debug("los indices afectados de lado izq %s, der %s para %d\n",
				caca_arreglo_a_cadena(indices_izq, num_indices_izq, buf),
				caca_arreglo_a_cadena(indices_der, num_indices_der, buf1),
				idx_a_actualizar);

		caca_x_actualiza_intersexxxiones_lateral(arbol_numeros_unicos,
				matriz_sumas_coincidencias, indices_izq, num_indices_izq,
				nuevo_valor, viejo_valor, idx_a_actualizar, verdadero);
		caca_x_actualiza_intersexxxiones_lateral(arbol_numeros_unicos,
				matriz_sumas_coincidencias, indices_der, num_indices_der,
				nuevo_valor, viejo_valor, idx_a_actualizar, falso);

	}
}

static inline bool caca_comun_checa_bit(bitch_vector *bits, int posicion) {
	bool res = falso;
	int idx_arreglo = 0;
	int idx_registro = 0;

	idx_arreglo = posicion / 64;
	idx_registro = posicion % 64;

	res = !!(bits[idx_arreglo] & (bitch_vector) (1 << idx_registro));

	return res;
}

static inline void caca_comun_asigna_bit(bitch_vector *bits, int posicion) {
	int idx_arreglo = 0;
	int idx_registro = 0;

	idx_arreglo = posicion / 64;
	idx_registro = posicion % 64;

	bits[idx_arreglo] |= (bitch_vector) (1 << idx_registro);

}

static inline void caca_comun_limpia_bit(bitch_vector *bits, int posicion) {
	int idx_arreglo = 0;
	int idx_registro = 0;

	idx_arreglo = posicion / 64;
	idx_registro = posicion % 64;

	bits[idx_arreglo] &= ~((bitch_vector) (1 << idx_registro));

}

static inline void caca_x_actualiza_arbol_numeros_unicos(
		caca_x_numeros_unicos_en_rango *arbol_numeros_unicos,
		int *indices_a_actualizar, int num_indices_a_actualizar,
		int viejo_pendejo, int nuevo_valor, bitch_vector *nuevo_ya_existente) {

	for (int i = 0; i < num_indices_a_actualizar; i++) {
		int idx_a_actualizar = 0;
		caca_x_numeros_unicos_en_rango *nodo_a_actualizar = NULL;
		avl_tree_t *arbolazo = NULL;

		idx_a_actualizar = indices_a_actualizar[i];
		nodo_a_actualizar = arbol_numeros_unicos + idx_a_actualizar;
		arbolazo = nodo_a_actualizar->arbolazo;

		caca_log_debug("borrando %d de seg %d. Antes de borra arbol es\n%s",
				viejo_pendejo, idx_a_actualizar,
				avl_tree_sprint_identado(arbolazo, (char[100] ) { '\0' }));

		arbolazo->ultimo_nodo_liberado_idx = 0;

		assert(avl_find(arbolazo, viejo_pendejo));
		avl_borrar(arbolazo, viejo_pendejo);
#ifndef CACA_X_VALIDAR_ARBOLINES
		avl_tree_validar_arbolin_indices(arbolazo, arbolazo->root);
#endif
		caca_log_debug(
				"borrado %d de seg %d. Despues de borra arbol es\n%s.\n Ahora insertando %d.\n",
				viejo_pendejo, idx_a_actualizar,
				avl_tree_sprint_identado(arbolazo, (char[100] ) { '\0' }),
				nuevo_valor);
		if (!avl_find(arbolazo, nuevo_valor)) {
			avl_insert(arbolazo, (long) nuevo_valor);
		} else {
			caca_comun_asigna_bit(nuevo_ya_existente, i);
			caca_log_debug("no se añadio %d a seg %d, ya estaba\n", nuevo_valor,
					idx_a_actualizar);
		}
#ifndef CACA_X_VALIDAR_ARBOLINES
		avl_tree_validar_arbolin_indices(arbolazo, arbolazo->root);
#endif
		caca_log_debug(
				"insertado %d en seg %d. Despues de insertar el arbol es\n%s",
				nuevo_valor, idx_a_actualizar,
				avl_tree_sprint_identado(arbolazo, (char[100] ) { '\0' }));
	}
}

static inline void caca_x_actualiza_sumas_arbol_segmentado(
		caca_x_numeros_unicos_en_rango *arbol_numeros_unicos,
		long *sumas_arbol_segmentado, int *indices_a_actualizar,
		int num_indices_a_actualizar, int nuevo_valor, int viejo_pendejo,
		bitch_vector *nuevo_ya_existente) {

	for (int i = 0; i < num_indices_a_actualizar; i++) {
		int idx_a_actualizar = 0;
		avl_tree_t *arbolin_a_actualizar = NULL;

		idx_a_actualizar = indices_a_actualizar[i];
		arbolin_a_actualizar = arbol_numeros_unicos[idx_a_actualizar].arbolazo;

		caca_log_debug("el valor original de la suma en seg %d es %d\n",
				idx_a_actualizar, sumas_arbol_segmentado[idx_a_actualizar]);

		sumas_arbol_segmentado[idx_a_actualizar] -= viejo_pendejo;
		caca_log_debug("despues de restar %d en seg %d la suma en seg %d \n",
				viejo_pendejo, idx_a_actualizar,
				sumas_arbol_segmentado[idx_a_actualizar]);
		if (!caca_comun_checa_bit(nuevo_ya_existente, i)) {
			sumas_arbol_segmentado[idx_a_actualizar] += nuevo_valor;
			caca_log_debug(
					"despues de ahora sumar %d en seg %d la suma en seg %d \n",
					nuevo_valor, idx_a_actualizar,
					sumas_arbol_segmentado[idx_a_actualizar]);
		} else {
			caca_log_debug("segmento %d no se le añadira %d por que ia sta\n",
					idx_a_actualizar, nuevo_valor);
		}
	}
}

static inline void caca_x_actualiza_estado(int *numeros,
		caca_x_numeros_unicos_en_rango *arbol_numeros_unicos,
		long *sumas_arbol_segmentado, int idx_actualizado, int nuevo_valor,
		int num_nodos) {
	bitch_vector nuevo_ya_existente = 0;
	int num_indices_afectados_actualizacion = 0;
	int viejo_pendejo = 0;
	int *indices_afectados_actualizacion = (int[18] ) { 0 };
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

	assert_timeout(num_indices_afectados_actualizacion < 18);

	caca_log_debug("los idx afectados %s\n",
			caca_arreglo_a_cadena(indices_afectados_actualizacion, num_indices_afectados_actualizacion, buf));

	caca_log_debug("el viejo %d y el nuevo %d\n", viejo_pendejo, nuevo_valor);

	caca_x_actualiza_arbol_numeros_unicos(arbol_numeros_unicos,
			indices_afectados_actualizacion,
			num_indices_afectados_actualizacion, viejo_pendejo, nuevo_valor,
			&nuevo_ya_existente);

	caca_x_actualiza_sumas_arbol_segmentado(arbol_numeros_unicos,
			sumas_arbol_segmentado, indices_afectados_actualizacion,
			num_indices_afectados_actualizacion, nuevo_valor, viejo_pendejo,
			&nuevo_ya_existente);

	numeros[idx_actualizado] = nuevo_valor;
	/*
	 */

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
	long *sumas_arbol_segmentado = NULL;
	int *matriz_sumas_coincidencias = NULL;

	char buf[100] = { '\0' };

	matriz_nums = calloc(MAX_NUMEROS_REDONDEADO * 3, sizeof(int));
	assert_timeout(matriz_nums);

	matriz_sumas_coincidencias = calloc(MAX_NODOS * 16, sizeof(int));
	assert_timeout(matriz_sumas_coincidencias);

	num_filas = 3;
	lee_matrix_long_stdin(matriz_nums, &num_filas, NULL, 3,
			MAX_NUMEROS_REDONDEADO);

	num_numeros = *matriz_nums;
	numeros = matriz_nums + MAX_NUMEROS_REDONDEADO;
	num_queries = *(numeros + MAX_NUMEROS_REDONDEADO);

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

	caca_log_debug("el numero de nodos %d\n", num_nodos);

	arbol_numeros_unicos = calloc(num_nodos,
			sizeof(caca_x_numeros_unicos_en_rango));
	assert_timeout(arbol_numeros_unicos);

	sumas_arbol_segmentado = calloc(num_nodos, sizeof(long));
	assert_timeout(sumas_arbol_segmentado);

	caca_log_debug("llamando a func rec con max prof %d\n",
			max_profundidad + 2);

#ifdef USA_MALLOC
	estado = malloc((max_profundidad +2) * sizeof(caca_x_estado_recursion));
	assert_timeout(estado);
	memset(estado, 0, (max_profundidad+2 )* sizeof(caca_x_estado_recursion));
#else
	estado = calloc(max_profundidad + 2, sizeof(caca_x_estado_recursion));
	assert_timeout(estado);
#endif

	caca_x_construye_arbol_binario_segmentado(numeros,
			num_numeros_redondeado - 1, max_profundidad);

	caca_x_suma_unicos(sumas_arbol_segmentado, num_nodos);

	while (cont_queries < num_queries) {
		int idx_actualizado = 0;
		int nuevo_valor = 0;
		long sum = 0;
		scanf("%c %d %d\n", &tipo_query, &idx_query_ini, &idx_query_fin);
		caca_log_debug("q: %c, ini %d, fin %d\n", tipo_query, idx_query_ini,
				idx_query_fin);

		switch (tipo_query) {
		case 'Q':
			sum = caca_x_suma_segmento(sumas_arbol_segmentado,
					idx_query_ini - 1, idx_query_fin - 1);
			printf("%ld\n", sum);
			break;
		case 'U':

			idx_actualizado = idx_query_ini - 1;
			nuevo_valor = idx_query_fin;
			caca_x_actualiza_estado(numeros, arbol_numeros_unicos,
					sumas_arbol_segmentado, idx_actualizado, nuevo_valor,
					(2 << (max_profundidad + 0)) - 2);
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
