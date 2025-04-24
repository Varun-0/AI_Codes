#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 3
#define EMPTY ' '

char USER;
char AI;
char MAX_PLAYER = 'X';
char MIN_PLAYER = 'O';

// Function declarations
void PrintingBoard(char board[SIZE][SIZE][SIZE]);
bool CheckGameOver(char board[SIZE][SIZE][SIZE]);
int BoardEvaluation(char board[SIZE][SIZE][SIZE]);
void PlayerMove(char board[SIZE][SIZE][SIZE], int *moveCount);
int MinMax(char board[SIZE][SIZE][SIZE], int depth, bool isMaximizing);
void AIMove(char board[SIZE][SIZE][SIZE], int ply, int *moveCount);

int main()
{
    char board[SIZE][SIZE][SIZE] = {
        {{EMPTY, EMPTY, EMPTY}, {EMPTY, EMPTY, EMPTY}, {EMPTY, EMPTY, EMPTY}},
        {{EMPTY, EMPTY, EMPTY}, {EMPTY, EMPTY, EMPTY}, {EMPTY, EMPTY, EMPTY}},
        {{EMPTY, EMPTY, EMPTY}, {EMPTY, EMPTY, EMPTY}, {EMPTY, EMPTY, EMPTY}}};

    printf("Let's Play 3D Tic-Tac-Toe!\n");

    printf("Choose your symbol (X or O): ");
    scanf(" %c", &USER);

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
    printf("Choose Min-Max search depth (1-ply or 2-ply): ");
    scanf("%d", &ply);

    int moveCount = 0;
    bool gameOver = false;

    while (!gameOver)
    {
        printf("\n");

        if (currentPlayer == 1)
        {
            PlayerMove(board, &moveCount);
        }
        else
        {
            AIMove(board, ply, &moveCount);
        }

        PrintingBoard(board);

        if (CheckGameOver(board))
        {
            int evaluation = BoardEvaluation(board);
            if (evaluation > 0)
            {
                printf("Good game!\n");
            }
            else if (evaluation < 0)
            {
                printf("Good game!\n");
            }
            else
            {
                printf("Good game!\n");
            }
            gameOver = true;
        }
        else if (moveCount == SIZE * SIZE * SIZE)
        {
            printf("Good game!\n");
            gameOver = true;
        }
        currentPlayer = 3 - currentPlayer; // Switch players (1 -> 2, 2 -> 1)
    }

    return 0;
}

// Function to print the 3D Tic-Tac-Toe board
void PrintingBoard(char board[SIZE][SIZE][SIZE])
{
    for (int i = 0; i < SIZE; i++)
    {
        printf("\nLayer %d:\n", i + 1);
        printf("-------------\n");
        for (int j = 0; j < SIZE; j++)
        {
            printf("| ");
            for (int k = 0; k < SIZE; k++)
            {
                printf("%c | ", board[i][j][k]);
            }
            printf("\n-------------\n");
        }
        printf("\n");
    }
}

// Function to check if the game is over
bool CheckGameOver(char board[SIZE][SIZE][SIZE])
{
    // Check rows, columns, and layers for a win
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if ((board[i][j][0] != EMPTY && board[i][j][0] == board[i][j][1] && board[i][j][1] == board[i][j][2]) ||
                (board[i][0][j] != EMPTY && board[i][0][j] == board[i][1][j] && board[i][1][j] == board[i][2][j]) ||
                (board[0][i][j] != EMPTY && board[0][i][j] == board[1][i][j] && board[1][i][j] == board[2][i][j]))
            {
                return true;
            }
        }
    }

    // Check diagonals in each layer
    for (int i = 0; i < SIZE; i++)
    {
        if ((board[i][0][0] != EMPTY && board[i][0][0] == board[i][1][1] && board[i][1][1] == board[i][2][2]) ||
            (board[i][0][2] != EMPTY && board[i][0][2] == board[i][1][1] && board[i][1][1] == board[i][2][0]))
        {
            return true;
        }
    }

    // Check 3D diagonals
    if ((board[0][0][0] != EMPTY && board[0][0][0] == board[1][1][1] && board[1][1][1] == board[2][2][2]) ||
        (board[0][0][2] != EMPTY && board[0][0][2] == board[1][1][1] && board[1][1][1] == board[2][2][0]) ||
        (board[0][2][0] != EMPTY && board[0][2][0] == board[1][1][1] && board[1][1][1] == board[2][0][2]) ||
        (board[0][2][2] != EMPTY && board[0][2][2] == board[1][1][1] && board[1][1][1] == board[2][0][0]))
    {
        return true;
    }

    // Check for a draw
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            for (int k = 0; k < SIZE; k++)
            {
                if (board[i][j][k] == EMPTY)
                    return false; // The game is not over yet
            }
        }
    }

    return true; // It's a draw
}

// Function to evaluate the board using the new heuristic
int BoardEvaluation(char board[SIZE][SIZE][SIZE])
{
    int heuristic = 0;

    // Check rows, columns, and layers for potential wins
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            int rowUserCount = 0, rowAICount = 0;
            int colUserCount = 0, colAICount = 0;
            int layerUserCount = 0, layerAICount = 0;

            // Row check
            for (int k = 0; k < SIZE; k++)
            {
                if (board[i][j][k] == MAX_PLAYER)
                    rowUserCount++;
                if (board[i][j][k] == MIN_PLAYER)
                    rowAICount++;
            }
            if (rowUserCount > 0 && rowUserCount < SIZE)
                heuristic++;
            if (rowAICount > 0 && rowAICount < SIZE)
                heuristic--;

            // Column check
            for (int k = 0; k < SIZE; k++)
            {
                if (board[i][k][j] == MAX_PLAYER)
                    colUserCount++;
                if (board[i][k][j] == MIN_PLAYER)
                    colAICount++;
            }
            if (colUserCount > 0 && colUserCount < SIZE)
                heuristic++;
            if (colAICount > 0 && colAICount < SIZE)
                heuristic--;

            // Layer check
            for (int k = 0; k < SIZE; k++)
            {
                if (board[k][i][j] == MAX_PLAYER)
                    layerUserCount++;
                if (board[k][i][j] == MIN_PLAYER)
                    layerAICount++;
            }
            if (layerUserCount > 0 && layerUserCount < SIZE)
                heuristic++;
            if (layerAICount > 0 && layerAICount < SIZE)
                heuristic--;
        }
    }

    // Diagonal checks
    int diag1UserCount = 0, diag1AICount = 0;
    int diag2UserCount = 0, diag2AICount = 0;
    int diag3UserCount = 0, diag3AICount = 0;
    int diag4UserCount = 0, diag4AICount = 0;

    for (int i = 0; i < SIZE; i++)
    {
        if (board[i][i][i] == MAX_PLAYER)
            diag1UserCount++;
        if (board[i][i][i] == MIN_PLAYER)
            diag1AICount++;
        if (board[i][i][SIZE - 1 - i] == MAX_PLAYER)
            diag2UserCount++;
        if (board[i][i][SIZE - 1 - i] == MIN_PLAYER)
            diag2AICount++;
        if (board[i][SIZE - 1 - i][i] == MAX_PLAYER)
            diag3UserCount++;
        if (board[i][SIZE - 1 - i][i] == MIN_PLAYER)
            diag3AICount++;
        if (board[SIZE - 1 - i][i][i] == MAX_PLAYER)
            diag4UserCount++;
        if (board[SIZE - 1 - i][i][i] == MIN_PLAYER)
            diag4AICount++;
    }
    if (diag1UserCount > 0 && diag1UserCount < SIZE)
        heuristic++;
    if (diag1AICount > 0 && diag1AICount < SIZE)
        heuristic--;
    if (diag2UserCount > 0 && diag2UserCount < SIZE)
        heuristic++;
    if (diag2AICount > 0 && diag2AICount < SIZE)
        heuristic--;
    if (diag3UserCount > 0 && diag3UserCount < SIZE)
        heuristic++;
    if (diag3AICount > 0 && diag3AICount < SIZE)
        heuristic--;
    if (diag4UserCount > 0 && diag4UserCount < SIZE)
        heuristic++;
    if (diag4AICount > 0 && diag4AICount < SIZE)
        heuristic--;

    return heuristic;
}

// Function for the user's move
void PlayerMove(char board[SIZE][SIZE][SIZE], int *moveCount)
{
    int layer, row, col;
    printf("Your turn (%c):\n", USER);
    while (1)
    {
        printf("Enter layer , row , and column separated by spaces: ");
        scanf("%d %d %d", &layer, &row, &col);
        if (layer >= 0 && layer < SIZE && row >= 0 && row < SIZE && col >= 0 && col < SIZE && board[layer][row][col] == EMPTY)
        {
            board[layer][row][col] = USER;
            (*moveCount)++;
            break;
        }
        else
        {
            printf("Invalid move. Try again.\n");
        }
    }
}

// Min-Max algorithm for AI's move with ply search
int MinMax(char board[SIZE][SIZE][SIZE], int depth, bool isMaximizing)
{
    int score = BoardEvaluation(board);

    // Base cases
    if (score > 0) // AI is the maximizing player
        return score - depth;
    if (score < 0) // User is the minimizing player
        return score + depth;
    if (CheckGameOver(board))
        return 0; // Draw

    // If it's the AI's turn
    if (isMaximizing)
    {
        int maxEval = -1000;
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                for (int k = 0; k < SIZE; k++)
                {
                    if (board[i][j][k] == EMPTY)
                    {
                        board[i][j][k] = AI;
                        int eval = MinMax(board, depth + 1, false);
                        board[i][j][k] = EMPTY; // Undo the move
                        if (eval > maxEval)
                            maxEval = eval;
                    }
                }
            }
        }
        return maxEval;
    }
    // If it's the user's turn
    else
    {
        int minEval = 1000;
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                for (int k = 0; k < SIZE; k++)
                {
                    if (board[i][j][k] == EMPTY)
                    {
                        board[i][j][k] = USER;
                        int eval = MinMax(board, depth + 1, true);
                        board[i][j][k] = EMPTY; // Undo the move
                        if (eval < minEval)
                            minEval = eval;
                    }
                }
            }
        }
        return minEval;
    }
}

// Function for the AI's move using Min-Max algorithm with ply search
void AIMove(char board[SIZE][SIZE][SIZE], int ply, int *moveCount)
{
    int bestMoveLayer = -1, bestMoveRow = -1, bestMoveCol = -1;
    int bestScore = 1000; // Initialize with a very high value (since we're minimizing)
    int bestScore1 = -1000;
    int currentMoveCount = 0;

    printf("AI evaluating moves:\n");

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            for (int k = 0; k < SIZE; k++)
            {
                if (board[i][j][k] == EMPTY)
                {
                    board[i][j][k] = AI;
                    int score = -1000;
                    int score1 = 1000;

                    if (ply == 2)
                    {
                        // Evaluate all user responses for 2-ply search
                        for (int l = 0; l < SIZE; l++)
                        {
                            for (int m = 0; m < SIZE; m++)
                            {
                                for (int n = 0; n < SIZE; n++)
                                {
                                    if (board[l][m][n] == EMPTY)
                                    {
                                        board[l][m][n] = USER;
                                        int moveScore = MinMax(board, 0, false);

                                        // Print the board configuration after user's possible move
                                        printf("AI's move (%d, %d, %d) followed by USER's move (%d, %d, %d) -> Heuristic: %d\n", i, j, k, l, m, n, moveScore);
                                        PrintingBoard(board);

                                        board[l][m][n] = EMPTY; // Undo the move

                                        if (USER == 'X')
                                        {
                                            if (board[l][m][n] == EMPTY)
                                            {
                                                if (moveScore > score)
                                                    score = moveScore;
                                            }
                                        }

                                        if (USER == 'O')
                                        {
                                            if (board[l][m][n] == EMPTY)
                                            {
                                                if (moveScore < score1)
                                                    score1 = moveScore;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if (USER == 'X')
                            score = MinMax(board, 0, false);
                        if (USER == 'O')
                            score1 = MinMax(board, 0, false);
                    }

                    if (USER == 'X')
                        printf("Move %d: (%d, %d, %d) -> Heuristic: %d\n", ++currentMoveCount, i, j, k, score);
                    if (USER == 'O')
                        printf("Move %d: (%d, %d, %d) -> Heuristic: %d\n", ++currentMoveCount, i, j, k, score1);

                    // Print the board configuration with the possible move
                    board[i][j][k] = AI;
                    PrintingBoard(board);
                    board[i][j][k] = EMPTY; // Undo the move

                    if (USER == 'X')
                    {
                        if (score < bestScore)
                        {
                            bestScore = score;
                            bestMoveLayer = i;
                            bestMoveRow = j;
                            bestMoveCol = k;
                        }
                    }
                    if (USER == 'O')
                    {
                        if (score1 > bestScore1)
                        {
                            bestScore1 = score1;
                            bestMoveLayer = i;
                            bestMoveRow = j;
                            bestMoveCol = k;
                        }
                    }
                }
            }
        }
    }

    // Apply the best move
    if (USER == 'X')
        printf("AI chooses move: (%d, %d, %d) with Heuristic: %d\n", bestMoveLayer, bestMoveRow, bestMoveCol, bestScore);

    if (USER == 'O')
        printf("AI chooses move: (%d, %d, %d) with Heuristic: %d\n", bestMoveLayer, bestMoveRow, bestMoveCol, bestScore1);

    board[bestMoveLayer][bestMoveRow][bestMoveCol] = AI;
    (*moveCount)++;
}