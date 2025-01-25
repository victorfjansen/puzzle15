#include "puzzle.h"

// gcc main.c puzzle.c hash.c heap.c -o puzzle
// excalidraw: https://excalidraw.com/#json=bHXidH6alr694HCl5JaFa,MGVV0zmYAzuQFloXwP4_Ig

// referências:
// https://github.com/RafaPiveta/JogoDos15
// https://github.com/BigZaphod/AStar/blob/master/AStar.c
// https://www.redblobgames.com/pathfinding/a-star/implementation.html -- the best
// https://www.youtube.com/watch?v=mbMbGjX45_E

int main()
{
    // gerar aleatoriedade no board definindo uma seed para a função rand baseada no tempo.
    srand(time(NULL));

    // cria um board inicial (matrix NxN) -> N definido como 4
    int initial_board[N][N];
    generate_solvable_board(initial_board);

    printf("Tabuleiro inicial:\n\n");
    // Imprime o tabuleiro inicial
    for (int i = 0; i < N; i++)
    {
        printf("+----+----+----+----+\n");
        for (int j = 0; j < N; j++)
        {
            if (initial_board[i][j] == 0)
                printf("|    ");
            else
                printf("| %2d ", initial_board[i][j]);
        }
        printf("|\n");
    }
    printf("+----+----+----+----+\n\n");

    /**
     * Inicia o estado inicial com os dados do tabuleiro gerado.
     * Preenchimento das heurísticas e mapeamento dos itens de posição
     */
    State *initial = (State *)malloc(sizeof(State));
    int elem_index = 0;

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            int val = initial_board[i][j];
            if (val == 0)
            {
                initial->zero_pos.row = i;
                initial->zero_pos.col = j;
            }
            else
            {
                initial->elements[elem_index].value = val;
                initial->elements[elem_index].pos.row = i;
                initial->elements[elem_index].pos.col = j;
                elem_index++;
            }
        }
    }

    // custo do nó inicial, até o estado atual
    initial->g = 0;

    // custo do nó atual até o objetivo
    initial->h = calculate_manhattan(initial);
    initial->f = initial->g + initial->h;

    // inicia a "árvore"
    initial->parent = NULL;

    // Chama a função que resolve o puzzle
    solve_puzzle(initial);

    return 0;
}
