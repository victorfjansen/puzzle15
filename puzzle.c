#include "puzzle.h"

// converte o estado (matriz esparsa) em uma string para hashing
// considerando os valores dos elementos da matriz esparsa (posições e valores).

void state_to_string(State *state, char *str)
{
    int index = 0;
    // Ordena as peças por valor para garantir consistência
    SparseElement sorted_elements[BOARD_SIZE - 1];
    memcpy(sorted_elements, state->elements, sizeof(sorted_elements));

    for (int i = 0; i < BOARD_SIZE - 2; i++)
    {
        for (int j = i + 1; j < BOARD_SIZE - 1; j++)
        {
            if (sorted_elements[i].value > sorted_elements[j].value)
            {
                SparseElement temp = sorted_elements[i];
                sorted_elements[i] = sorted_elements[j];
                sorted_elements[j] = temp;
            }
        }
    }

    // Concatena os valores e posições das peças
    for (int i = 0; i < BOARD_SIZE - 1; i++)
    {
        index += sprintf(str + index, "%d%d%d",
                         sorted_elements[i].value,
                         sorted_elements[i].pos.row,
                         sorted_elements[i].pos.col);
    }

    // Adiciona a posição do espaço vazio
    index += sprintf(str + index, "z%d%d", state->zero_pos.row, state->zero_pos.col);
    str[index] = '\0';
}

/**
 * Calcula o custo das peças e as soma!
 * Basicamente:
 *  - Custo = distância de cada peça de onde ela realmente deveria estar posicionada.
 *
 *  Ex: peça de valor 1, deveria estar em (0, 0)
 *  Ex: peça de valor 2, deveria estar em (0, 1)
 *  Ex: peça de valor 3, deveria estar em (0, 2)
 */
int calculate_manhattan(State *state)
{
    int distance = 0;
    for (int i = 0; i < BOARD_SIZE - 1; i++)
    {
        // valor atual
        int val = state->elements[i].value;

        // posições que se encontram
        int current_row = state->elements[i].pos.row;
        int current_col = state->elements[i].pos.col;

        int target_row = (val - 1) / N;
        int target_col = (val - 1) % N;

        distance += abs(current_row - target_row) + abs(current_col - target_col);
    }
    return distance;
}

/**
 * Aplica teorida da paridade e verifica a quantidade de inversões
 * para descorbir se o board é solucionável ou não
 *
 * Verifica a linha vazia --> Dado que é um tabuleiro de N par, a paridade sempre será
 * comparada a partir da linha.
 *
 * Se o número da linha for par e as inversões forem ímpares, é inversível
 * Se o número da linha for impar e as inversões pares, é inversível
 *
 * (tem que ser inversível para poder resolver o tabuleiro)
 *
 * Stack exchange:
 * - https://math.stackexchange.com/questions/635188/what-is-the-parity-of-permutation-in-the-15-puzzle
 */
int is_solvable(int board[N][N])
{
    // estado flatted para calcular as inversões
    int flat_list[BOARD_SIZE - 1];

    // index inicial
    int index = 0;

    // número de inversões iniciais
    int inversions = 0;

    // número da linha vazia (cálculo de ímpares e pares)
    int empty_row = 0;

    /**
     * Preenche flatted list para cálcular inversões
     */
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (board[i][j] == 0)
            {
                empty_row = N - i;
                continue;
            }
            flat_list[index++] = board[i][j];
        }
    }

    /**
     * Verifica todas as casas da flatted list, e passa verificando se o posterior é menor que o anterior
     */
    for (int i = 0; i < BOARD_SIZE - 1; i++)
    {
        for (int j = i + 1; j < BOARD_SIZE - 1; j++)
        {
            if (flat_list[i] > flat_list[j])
                inversions++;
        }
    }

    if (empty_row % 2 == 0)
        return (inversions % 2 == 1);
    else
        return (inversions % 2 == 0);
}

// Gera os vizinhos a partir de todas as movimentações possíveis
State **get_neighbors(State *current, int *neighbor_count)
{
    // pega linha e coluna do estado 0
    int row = current->zero_pos.row;
    int col = current->zero_pos.col;

    // faz uma matriz de posições possíveis na movimentação  do 0
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    // Os 4 possíveis vizinhos
    State **neighbors = (State **)malloc(4 * sizeof(State *));

    // reseta quantidade de vizinhos
    *neighbor_count = 0;

    // para todas as possibilidades de movimentação
    for (int i = 0; i < 4; i++)
    {
        // pega a nova provavel linha e nova provavel coluna
        int new_r = row + directions[i][0];
        int new_c = col + directions[i][1];

        // se estiver dentro do tabuleiro respeitando os limites de N
        if (new_r >= 0 && new_r < N && new_c >= 0 && new_c < N)
        {
            // Cria um novo estado a partir do anterior
            // alterando a posição do estado de 0 e adicionando
            // o nobo estado a possível árvore!
            State *new_state = (State *)malloc(sizeof(State));
            memcpy(new_state, current, sizeof(State));

            new_state->g = current->g + 1;
            new_state->parent = current;

            // nova posição de 0 já é atribuida aqui
            new_state->zero_pos.row = new_r;
            new_state->zero_pos.col = new_c;

            // Percorre a lista de elementos e preenche o valor do espaço vazio anterior
            for (int j = 0; j < BOARD_SIZE - 1; j++)
            {
                if (new_state->elements[j].pos.row == new_r && new_state->elements[j].pos.col == new_c)
                {
                    new_state->elements[j].pos.row = row;
                    new_state->elements[j].pos.col = col;
                    break;
                }
            }

            // novos cálculos das heurísticas de manhattan
            new_state->h = calculate_manhattan(new_state);
            new_state->f = new_state->g + new_state->h;

            // e contagem de vizinhos
            neighbors[*neighbor_count] = new_state;
            (*neighbor_count)++;
        }
    }
    return neighbors;
}

// Imprime o estado atual do tabuleiro
void print_state(State *state)
{
    int board[N][N] = {0};
    // Preenche o tabuleiro com as peças da matriz esparsa
    for (int i = 0; i < BOARD_SIZE - 1; i++)
    {
        int row = state->elements[i].pos.row;
        int col = state->elements[i].pos.col;
        board[row][col] = state->elements[i].value;
    }
    // O espaço vazio é zero, já inicializado

    // Imprime o tabuleiro
    for (int i = 0; i < N; i++)
    {
        printf("+----+----+----+----+\n");
        for (int j = 0; j < N; j++)
        {
            if (board[i][j] == 0)
                printf("|    ");
            else
                printf("| %2d ", board[i][j]);
        }
        printf("|\n");
    }
    printf("+----+----+----+----+\n\n");
}

// Imprime a solução completa (do primeiro estado até o atual)
void print_solution(State *state)
{
    if (state == NULL)
        return;
    print_solution(state->parent);
    print_state(state);
}

// Libera a memória dos estados recursivamente
void free_state(State *state)
{
    if (state == NULL)
        return;
    free_state(state->parent);
    free(state);
}

/* Fisher-Yates Shuffle */
void shuffle(int board[N][N])
{
    int numbers[BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; i++)
        numbers[i] = i;

    // Embaralha os números
    for (int i = BOARD_SIZE - 1; i > 0; i--)
    {
        /**
         * Pego o número na posição j (de 1 a 16)
         * e faço um swap nas posições entre o número
         * existente e o número aleatório.
         */
        int j = rand() % (i + 1);
        int temp = numbers[i];

        numbers[i] = numbers[j];
        numbers[j] = temp;
    }

    // Preenche o tabuleiro (0 representa o espaço vazio)
    int index = 0;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            board[i][j] = numbers[index++];
        }
    }
}

void generate_solvable_board(int board[N][N])
{
    do
    {
        shuffle(board);
    } while (!is_solvable(board));
}

void solve_puzzle(State *initial)
{
    // Cria o min_heap -> estados não explorados totalmente.
    // guardar sempre o estado com o menor F em ordem de prioridade (função do min_heap)
    // 1000 para alocação dinâmica. Se precisar, será acrescido.

    // OBS; estruturausada para o min_heap é o heap binario ou BinaryHeap
    // https://napoleon.com.br/glossario/o-que-e-binary-heap/
    MinHeap *heap = create_minheap(1000);
    insert_minheap(heap, initial);

    // registro do estado -- qualquer um que já tenha existido
    HashTable *closed_set = create_hashtable();

    char key[256];
    state_to_string(initial, key);

    // registro do primeiro estado
    hashtable_insert(closed_set, key);

    while (!is_empty(heap))
    {
        // pega o primeiro da binary heap (menor valor de F)
        State *current = extract_min(heap);

        // Se for o estado final, finaliza a função.
        if (current->h == 0)
        {
            // Onde G é o número do estado atual
            printf("Solução encontrada em %d movimentos:\n\n", current->g);
            print_solution(current);

            // Libera memória
            free_hashtable(closed_set);
            free(heap->data);
            free(heap);
            free_state(current);
            return;
        }

        // Geração dos vizinhos (A*)
        int neighbor_count = 0;
        State **neighbors = get_neighbors(current, &neighbor_count);

        for (int i = 0; i < neighbor_count; i++)
        {
            // calcula a hash para dicionar no closed set caso não exista.
            // e adiciona a jogada na minheap (ordenando pela heurística F, obedencendo os critérios
            // do Binary Heap )
            char neighbor_key[256];
            state_to_string(neighbors[i], neighbor_key);

            if (!hashtable_contains(closed_set, neighbor_key))
            {
                insert_minheap(heap, neighbors[i]);
                hashtable_insert(closed_set, neighbor_key);
            }
            else
            {
                // Estado já visitado, libera a memória
                free(neighbors[i]);
            }
        }
        free(neighbors);
    }

    printf("Nenhuma solução encontrada.\n");

    // Libera memória
    free_hashtable(closed_set);
    free(heap->data);
    free(heap);
}
