#include <stdio.h>
#include <stdbool.h>

#define SIZE 3

void initializeBoard(int board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = 2;
        }
    }
}

void displayBoard(int board[SIZE][SIZE]) {
    printf("\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            char symbol = (board[i][j] == 3) ? 'X' : (board[i][j] == 5) ? 'O' : ' ';
            printf(" %c ", symbol);
            if (j < SIZE - 1) printf("|");
        }
        printf("\n");
        if (i < SIZE - 1) printf("---+---+---\n");
    }
    printf("\n");
}

int getPlayerChoice() {
    int choice;
    while (true) {
        printf("Enter your choice (1 for X, 2 for O): ");
        if (scanf("%d", &choice) == 1 && (choice == 1 || choice == 2)) {
            return choice;
        } else {
            printf("Invalid input! Please enter 1 or 2.\n");
            while (getchar() != '\n');
        }
    }
}

bool isWinningMove(int board[SIZE][SIZE], int player) {
    int winProduct = player * player * player;
    for (int i = 0; i < SIZE; i++) {
        if (board[i][0] * board[i][1] * board[i][2] == winProduct) return true;
    }
    for (int j = 0; j < SIZE; j++) {
        if (board[0][j] * board[1][j] * board[2][j] == winProduct) return true;
    }
    if (board[0][0] * board[1][1] * board[2][2] == winProduct) return true;
    if (board[0][2] * board[1][1] * board[2][0] == winProduct) return true;
    return false;
}

bool isFull(int board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 2) return false;
        }
    }
    return true;
}

void playerMove(int board[SIZE][SIZE], int player) {
    int row, col;
    while (true) {
        printf("Enter row and column (1-3): ");
        if (scanf("%d %d", &row, &col) == 2) {
            row--; col--;
            if (row >= 0 && row < SIZE && col >= 0 && col < SIZE && board[row][col] == 2) {
                board[row][col] = player;
                break;
            } else {
                printf("Invalid move! Cell is already occupied or out of bounds. Try again.\n");
            }
        } else {
            printf("Invalid input! Please enter numbers only.\n");
            while (getchar() != '\n');
        }
    }
}

void machineMove(int board[SIZE][SIZE], int player) {
    int bestRow = -1, bestCol = -1;
    int priority[SIZE][SIZE] = {
        {3, 2, 3},
        {2, 4, 2},
        {3, 2, 3}
    };
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 2) {
                board[i][j] = player;
                if (isWinningMove(board, player)) {
                    bestRow = i;
                    bestCol = j;
                    board[i][j] = 2;
                    goto move;
                }
                board[i][j] = (player == 3) ? 5 : 3;
                if (isWinningMove(board, (player == 3) ? 5 : 3)) {
                    bestRow = i;
                    bestCol = j;
                    board[i][j] = 2;
                    goto move;
                }
                board[i][j] = 2;
            }
        }
    }
    int maxPriority = -1;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 2 && priority[i][j] > maxPriority) {
                maxPriority = priority[i][j];
                bestRow = i;
                bestCol = j;
            }
        }
    }
move:
    board[bestRow][bestCol] = player;
}

int main() {
    int board[SIZE][SIZE];
    initializeBoard(board);
    printf("Choose your symbol:\n1. X\n2. O\n");
    int playerChoice = getPlayerChoice();
    int player = (playerChoice == 1) ? 3 : 5;
    int machine = (player == 3) ? 5 : 3;
    int currentPlayer = 3;
    while (true) {
        displayBoard(board);
        if (currentPlayer == player) {
            printf("\nYour turn:\n");
            playerMove(board, player);
        } else {
            printf("\nMachine's turn:\n");
            machineMove(board, machine);
        }
        if (isWinningMove(board, currentPlayer)) {
            displayBoard(board);
            printf("\nPlayer %c wins!\n", currentPlayer == 3 ? 'X' : 'O');
            break;
        }
        if (isFull(board)) {
            displayBoard(board);
            printf("\nIt's a draw!\n");
            break;
        }
        currentPlayer = (currentPlayer == 3) ? 5 : 3;
    }
    return 0;
}