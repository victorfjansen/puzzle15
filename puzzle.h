#ifndef PUZZLE_H
#define PUZZLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define N 4
#define BOARD_SIZE 16
#define HASH_TABLE_SIZE 100003 // Número primo para reduzir colisões

// Estrutura da posição
typedef struct
{
    int row;
    int col;
} Position;

// Peça da matriz esparsa
typedef struct
{
    int value;    // valor
    Position pos; // posição
} SparseElement;

// Estado do tabuleiro
typedef struct State
{
    SparseElement elements[BOARD_SIZE - 1]; // 15 peças
    Position zero_pos;                      // Posição do espaço vazio
    int g;                                  // Custo do caminho até este estado
    int h;                                  // Heurística
    int f;                                  // f = g + h
    struct State *parent;                   // mapeamento para elemento pai -> "Árvore"
} State;

// Estrutura para o Heap (Fila de Prioridade do algorítmo A*)
typedef struct
{
    State **data;
    int size;
    int capacity;
} MinHeap;

// Estrutura para a Tabela de Hash - Closed Set - Estados Visitados.
typedef struct HashNode
{
    char *key;
    struct HashNode *next;
} HashNode;

typedef struct
{
    HashNode **nodes;
} HashTable;

// hash
unsigned int hash_function(const char *key);
HashTable *create_hashtable();
int hashtable_contains(HashTable *table, const char *key);
void hashtable_insert(HashTable *table, const char *key);
void free_hashtable(HashTable *table);

// heap
MinHeap *create_minheap(int capacity);
void insert_minheap(MinHeap *heap, State *state);
State *extract_min(MinHeap *heap);
int is_empty(MinHeap *heap);

// Converte o estado (matriz esparsa) em uma string para hashing
void state_to_string(State *state, char *str);

/**
 * Calcula o custo das peças e as soma!
 * Basicamente:
 *  - Custo = distância de cada peça de onde ela realmente deveria estar posicionada.
 *
 *  Ex: peça de valor 1, deveria estar em (0, 0)
 *  Ex: peça de valor 2, deveria estar em (0, 1)
 *  Ex: peça de valor 3, deveria estar em (0, 2)
 */
int calculate_manhattan(State *state);

/**
 * Aplica teorida da paridade e verifica a quantidade de inversões
 * para descobrir se o board é solucionável ou não
 */
int is_solvable(int board[N][N]);

// Gera os vizinhos movendo o espaço vazio
State **get_neighbors(State *current, int *neighbor_count);

// Imprime o estado atual do tabuleiro
void print_state(State *state);

// Imprime a solução completa
void print_solution(State *state);

// Libera a memória dos estados
void free_state(State *state);

/* Fisher-Yates Shuffle */
void shuffle(int board[N][N]);

// Gera um tabuleiro solucionável
void generate_solvable_board(int board[N][N]);

// Resolve o puzzle a partir de um estado inicial
void solve_puzzle(State *initial);

#endif
