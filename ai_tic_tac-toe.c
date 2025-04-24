#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 3
#define EMPTY ' '

char USER;
char AI;
char MAX_PLAYER = 'X';
char MIN_PLAYER = 'O';
void printBoard(char board[SIZE][SIZE]);
bool isGameOver(char board[SIZE][SIZE]);
int cal(char board[SIZE][SIZE]);
void userM(char board[SIZE][SIZE], int *moveCount);
int minMax(char board[SIZE][SIZE], int depth, bool isMaximizing);
void aiM(char board[SIZE][SIZE], int ply, int *moveCount);

int main()
{
    char board[SIZE][SIZE] = {
        {EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY}};

    printf("Choose your symbol (X/O): ");
    scanf(" %c", &USER);
    while (USER != 'X' && USER != 'O')
    {
        printf("Invalid input. Please enter X or O: ");
        scanf(" %c", &USER);
    }
    

    if (USER == 'X' || USER == 'x')
    {
        USER = 'X';
        AI = 'O';
    }
    else
    {
        USER = 'O';
        AI = 'X';
    }

    int currentPlayer = (USER == 'X') ? 1 : 2;

    int ply;
    printf("Choose Min-Max search depth 1 or 2 : ");
    scanf("%d", &ply);
    while(ply != 1 && ply != 2)
    {
        printf("Invalid input. Please enter 1 or 2: ");
        scanf("%d", &ply);
    }
    int moveCount = 0;
    bool gameOver = false;

    while (!gameOver)
    {
        printf("\n");

        if (currentPlayer == 1)
        {
            userM(board, &moveCount);
        }
        else
        {
            aiM(board, ply, &moveCount);
        }

        printBoard(board);

        if (isGameOver(board))
        {
            int evaluation = cal(board);
            if (evaluation > 0)
            {
                printf("Congratulations! %c wins!\n", MIN_PLAYER);
            }
            else if (evaluation < 0)
            {
                
                printf("Congratulations! %c wins!\n", MAX_PLAYER);
            }
            else
            {
                printf("It's a draw!\n");
            }
            gameOver = true;
        }
        else if (moveCount == SIZE * SIZE)
        {
            printf("It's a draw!\n");
            gameOver = true;
        }
        currentPlayer = 3 - currentPlayer; 
    }

    return 0;
}   

void printBoard(char board[SIZE][SIZE]) {
    printf("\n");
    for (int i = 0; i < SIZE; i++) {
        printf(" %c | %c | %c \n", board[i][0], board[i][1], board[i][2]);
        if (i < SIZE - 1) {
            printf("---|---|---\n");
        }
    }
    printf("\n");
}

bool isGameOver(char board[SIZE][SIZE])
{
    for (int i = 0; i < SIZE; i++)
    {
        if ((board[i][0] != EMPTY && board[i][0] == board[i][1] && board[i][1] == board[i][2]) ||
            (board[0][i] != EMPTY && board[0][i] == board[1][i] && board[1][i] == board[2][i]))
        {
            return true;
        }
    }

    if ((board[0][0] != EMPTY && board[0][0] == board[1][1] && board[1][1] == board[2][2]) ||
        (board[0][2] != EMPTY && board[0][2] == board[1][1] && board[1][1] == board[2][0]))
    {
        return true;
    }

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] == EMPTY)
                return false; 
        }
    }

    return true; 
}

int cal(char board[SIZE][SIZE])
{
    int heuristic = 0;

    for (int i = 0; i < SIZE; i++)
    {
        int rowXCount = 0, rowOCount = 0;
        int colXCount = 0, colOCount = 0;

        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] == MAX_PLAYER)
                rowXCount++;
            if (board[i][j] == MIN_PLAYER)
                rowOCount++;
        }
        if (rowXCount > 0 && rowXCount < SIZE)
            heuristic++;
        if (rowOCount > 0 && rowOCount < SIZE)
            heuristic--;

        for (int j = 0; j < SIZE; j++)
        {
            if (board[j][i] == MAX_PLAYER)
                colXCount++;
            if (board[j][i] == MIN_PLAYER)
                colOCount++;
        }
        if (colXCount > 0 && colXCount < SIZE)
            heuristic++;
        if (colOCount > 0 && colOCount < SIZE)
            heuristic--;
    }

    int diag1XCount = 0, diag1OCount = 0;
    int diag2XCount = 0, diag2OCount = 0;

    for (int i = 0; i < SIZE; i++)
    {
        if (board[i][i] == MAX_PLAYER)
            diag1XCount++;
        if (board[i][i] == MIN_PLAYER)
            diag1OCount++;
        if (board[i][SIZE - 1 - i] == MAX_PLAYER)
            diag2XCount++;
        if (board[i][SIZE - 1 - i] == MIN_PLAYER)
            diag2OCount++;
    }
    if (diag1XCount > 0 && diag1XCount < SIZE)
        heuristic++;
    if (diag1OCount > 0 && diag1OCount < SIZE)
        heuristic--;
    if (diag2XCount > 0 && diag2XCount < SIZE)
        heuristic++;
    if (diag2OCount > 0 && diag2OCount < SIZE)
        heuristic--;

    return heuristic;
}

void userM(char board[SIZE][SIZE], int *moveCount)
{
    int row, col;
    printf("Your turn (%c):\n", USER);
    while (1)
    {
        printf("Enter row and column (0-2) separated by space: ");
        scanf("%d %d", &row, &col);
        if (row >= 0 && row < SIZE && col >= 0 && col < SIZE && board[row][col] == EMPTY)
        {
            board[row][col] = USER;
            (*moveCount)++;
            break;
        }
        else
        {
            printf("Invalid move. Try again.\n");
        }
    }
}

int minMax(char board[SIZE][SIZE], int depth, bool isMaximizing)
{
    int score = cal(board);

    if (score > 0) 
        return score;
    if (score < 0)
        return score;
    if (isGameOver(board))
        return 0; 

    if (isMaximizing)
    {
        int maxEval = -1000;
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                if (board[i][j] == EMPTY)
                {
                    board[i][j] = AI;
                    int eval = minMax(board, depth + 1, false);
                    board[i][j] = EMPTY; 
                    if (eval > maxEval)
                        maxEval = eval;
                }
            }
        }
        return maxEval;
    }
    else
    {
        int minEval = 1000;
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                if (board[i][j] == EMPTY)
                {
                    board[i][j] = USER;
                    int eval = minMax(board, depth + 1, true);
                    board[i][j] = EMPTY; 
                    if (eval < minEval)
                        minEval = eval;
                }
            }
        }
        return minEval;
    }
}

void aiM(char board[SIZE][SIZE], int ply, int *moveCount)
{
    int bestMoveRow = -1;
    int bestMoveCol = -1;
    int bestScore = 1000; 
    int bestScore1 = -1000;
    int currentMoveCount = 0;

    printf("AI evaluating moves:\n");

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] == EMPTY)
            {
                board[i][j] = AI;
                int score = -1000;
                int score1 = 1000;

                if (ply > 1)
                {
                    for (int k = 0; k < SIZE; k++)
                    {
                        for (int l = 0; l < SIZE; l++)
                        {
                            if (board[k][l] == EMPTY)
                            {
                                board[k][l] = USER;
                                int moveScore = minMax(board, 0, false);

                                printf("AI's move (%d, %d) followed by USER's move (%d, %d) Heuristic: %d\n", i, j, k, l, moveScore);
                                printBoard(board);

                                board[k][l] = EMPTY; 

                                if (USER == 'X')
                                {
                                    if (board[k][l] == EMPTY)
                                    {
                                        if (moveScore > score)
                                            score = moveScore;
                                    }
                                }

                                if (USER == 'O')
                                {
                                    if (board[k][l] == EMPTY)
                                    {
                                        if (moveScore < score1)
                                            score1 = moveScore;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (USER == 'X')
                        score = minMax(board, 0, false);
                    if (USER == 'O')
                        score1 = minMax(board, 0, false);
                }

                if (USER == 'X')
                    printf("Move %d: (%d, %d) Heuristic: %d\n", ++currentMoveCount, i, j, score);
                if (USER == 'O')
                    printf("Move %d: (%d, %d) Heuristic: %d\n", ++currentMoveCount, i, j, score1);

                board[i][j] = AI;
                printBoard(board);
                board[i][j] = EMPTY; 

                if (USER == 'X')
                {
                    if (score < bestScore)
                    {
                        bestScore = score;
                        bestMoveRow = i;
                        bestMoveCol = j;
                    }
                }
                if (USER == 'O')
                {
                    if (score1 > bestScore1)
                    {
                        bestScore1 = score1;
                        bestMoveRow = i;
                        bestMoveCol = j;
                    }
                }
            }
        }
    }
    if (USER == 'X')
        printf("selected move: (%d, %d) with Heuristic: %d\n", bestMoveRow, bestMoveCol, bestScore);

    if (USER == 'O')
        printf("selected move: (%d, %d) with Heuristic: %d\n", bestMoveRow, bestMoveCol, bestScore1);

    board[bestMoveRow][bestMoveCol] = AI;
    (*moveCount)++;
}