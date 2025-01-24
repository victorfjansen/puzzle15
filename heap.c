#include "puzzle.h"

/* Funções de Heap */

static void swap_states(State **a, State **b)
{
    State *temp = *a;
    *a = *b;
    *b = temp;
}

static void heapify_up(MinHeap *heap, int index)
{
    // Espécie de "busca binária" dentro da BinaryHeap.
    // Pega elemento do meio e verifica se
    // é maior do que o filho (que acabou de ser adicionado)
    // se for, faz um swap e entra em recursividade.

    // A questão é ordernar pelo F, obedecendo a fila de prioridade
    // da BinaryHeap
    if (index && heap->data[(index - 1) / 2]->f > heap->data[index]->f)
    {
        swap_states(&heap->data[index], &heap->data[(index - 1) / 2]);
        heapify_up(heap, (index - 1) / 2);
    }
}

static void heapify_down(MinHeap *heap, int index)
{

    // Como  na estrutura de BinaryHeap a ordem de elementos tem que seguir
    // sendo do menor pro maior (raiz tem que ser menor que nós diretos e esquerdos)
    // faz uma busca recursiva pra ordernar pela heurística F do menor pro maior.

    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->size && heap->data[left]->f < heap->data[smallest]->f)
        smallest = left;
    if (right < heap->size && heap->data[right]->f < heap->data[smallest]->f)
        smallest = right;
    if (smallest != index)
    {
        swap_states(&heap->data[index], &heap->data[smallest]);
        heapify_down(heap, smallest);
    }
}

MinHeap *create_minheap(int capacity)
{
    MinHeap *heap = (MinHeap *)malloc(sizeof(MinHeap));
    heap->data = (State **)malloc(sizeof(State *) * capacity);
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

void insert_minheap(MinHeap *heap, State *state)
{
    // realocação dinâmica para aumentar o valor
    if (heap->size == heap->capacity)
    {
        heap->capacity *= 2;
        heap->data = (State **)realloc(heap->data, sizeof(State *) * heap->capacity);
    }

    heap->data[heap->size] = state;
    heapify_up(heap, heap->size);
    heap->size++;
}

State *extract_min(MinHeap *heap)
{
    if (heap->size == 0)
        return NULL;

    // pega o primeiro estado da fila de prioridade (menor "F" possível)
    State *min = heap->data[0];

    // reorganiza para colocar o segundo estado como primeiro
    heap->data[0] = heap->data[heap->size - 1];

    // diminui a referência
    heap->size--;

    heapify_down(heap, 0);
    return min;
}

int is_empty(MinHeap *heap)
{
    return (heap->size == 0);
}
