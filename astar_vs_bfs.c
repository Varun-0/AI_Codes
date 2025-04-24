#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define MAX 50

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point point;
    int f, g, h;
    Point parent; // To track the path
} Node;

typedef struct {
    Node nodes[MAX * MAX];
    int size;
} PriorityQueue;

int dx[] = {-1, 1, 0, 0};
int dy[] = {0, 0, -1, 1};

// Utility functions
bool isValid(int x, int y, int rows, int cols) {
    return (x >= 0 && x < rows && y >= 0 && y < cols);
}

bool isObstacle(int maze[MAX][MAX], int x, int y) {
    return (maze[x][y] == -1);
}

int heuristic(Point a, Point b) {
    return abs(a.x - b.x) + abs(a.y - b.y); // Manhattan distance
}

void initPriorityQueue(PriorityQueue *pq) {
    pq->size = 0;
}

bool isEmpty(PriorityQueue *pq) {
    return pq->size == 0;
}

void insert(PriorityQueue *pq, Node node) {
    pq->nodes[pq->size++] = node;
}

Node removeMin(PriorityQueue *pq) {
    int minIndex = 0;
    for (int i = 1; i < pq->size; i++) {
        if (pq->nodes[i].f < pq->nodes[minIndex].f) {
            minIndex = i;
        }
    }
    Node minNode = pq->nodes[minIndex];
    pq->nodes[minIndex] = pq->nodes[--pq->size];
    return minNode;
}
// Utility functions (remain unchanged except for printMaze)

void printMaze(int maze[MAX][MAX], int rows, int cols, Point current, Point start, Point goal) {
    // Print top border
    for (int j = 0; j < cols; j++) {
        printf("+---");
    }
    printf("+\n");

    for (int i = 0; i < rows; i++) {
        // Print cell content
        for (int j = 0; j < cols; j++) {
            char cell;
            if (i == current.x && j == current.y) {
                cell = '*'; // Current position
            } else if (i == start.x && j == start.y) {
                cell = 'S'; // Start point
            } else if (i == goal.x && j == goal.y) {
                cell = 'G'; // Goal point
            } else if (maze[i][j] == -1) {
                cell = '#'; // Obstacle
            } else if (maze[i][j] == 2) {
                cell = '.'; // Best-First path
            } else if (maze[i][j] == 3) {
                cell = 'o'; // A* path
            } else {
                cell = ' '; // Free space
            }
            printf("| %c ", cell);
        }
        printf("|\n");

        // Print row separator
        for (int j = 0; j < cols; j++) {
            printf("+---");
        }
        printf("+\n");
    }
    printf("\n");
}

// Other functions (bestFirstSearch, aStarSearch, etc.) remain unchanged except:
// 1. Update calls to printMaze to include start and goal
// 2. Modify printFinalMazeWithPath to avoid overriding start/goal symbols

void printFinalMazeWithPath(int maze[MAX][MAX], Node path[MAX][MAX], Point start, Point goal, int rows, int cols, int mode) {
    int originalStart = maze[start.x][start.y];
    int originalGoal = maze[goal.x][goal.y];

    Point p = goal;
    int pathSymbol = (mode == 1) ? 2 : 3;

    while (!(p.x == start.x && p.y == start.y)) {
        if (!(p.x == start.x && p.y == start.y) && !(p.x == goal.x && p.y == goal.y)) {
            maze[p.x][p.y] = pathSymbol;
        }
        p = path[p.x][p.y].parent;
    }

    // Restore original start/goal values
    maze[start.x][start.y] = originalStart;
    maze[goal.x][goal.y] = originalGoal;

    printf("Final Path Visualization:\n");
    printMaze(maze, rows, cols, (Point){-1, -1}, start, goal); // (-1,-1) hides current position
}

void printPath(Node path[MAX][MAX], Point start, Point goal) {
    Point p = goal;
    int cost = 0;

    printf("Path: ");
    while (!(p.x == start.x && p.y == start.y)) {
        printf("(%d, %d) <- ", p.x, p.y);
        p = path[p.x][p.y].parent;
        cost++;
    }
    printf("(%d, %d)\n", start.x, start.y);
    printf("Total cost: %d\n", cost);
}

void printOpenList(PriorityQueue pq) {
    printf("Open List:\n");
    for (int i = 0; i < pq.size; i++) {
        printf("(%d, %d) f: %d\n", pq.nodes[i].point.x, pq.nodes[i].point.y, pq.nodes[i].f);
    }
}

void printClosedList(bool visited[MAX][MAX], int rows, int cols) {
    printf("Closed List:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (visited[i][j]) {
                printf("(%d, %d) ", i, j);
            }
        }
    }
    printf("\n");
}

// Best First Search
bool bestFirstSearch(int maze[MAX][MAX], int rows, int cols, Point start, Point goal) {
    PriorityQueue pq;
    initPriorityQueue(&pq);

    Node startNode = {start, heuristic(start, goal), 0, heuristic(start, goal), start};
    insert(&pq, startNode);

    bool visited[MAX][MAX] = {false};
    visited[start.x][start.y] = true;

    Node path[MAX][MAX];

    while (!isEmpty(&pq)) {
        Node current = removeMin(&pq);

        printMaze(maze, rows, cols, current.point, start, goal);
        printOpenList(pq);
        printClosedList(visited, rows, cols);

        if (current.point.x == goal.x && current.point.y == goal.y) {
            printf("Path found with Best First Search.\n");
            printPath(path, start, goal);
            printFinalMazeWithPath(maze, path, start, goal, rows, cols, 1); // 1 for Best First Search
            return true;
        }

        for (int i = 0; i < 4; i++) {
            int nx = current.point.x + dx[i];
            int ny = current.point.y + dy[i];

            if (isValid(nx, ny, rows, cols) && !visited[nx][ny] && !isObstacle(maze, nx, ny)) {
                visited[nx][ny] = true;
                Node neighbor = {{nx, ny}, heuristic((Point){nx, ny}, goal), 0, heuristic((Point){nx, ny}, goal), current.point};
                path[nx][ny] = neighbor;
                insert(&pq, neighbor);
            }
        }
    }

    printf("No path found with Best First Search.\n");
    return false;
}

// A* Search
bool aStarSearch(int maze[MAX][MAX], int rows, int cols, Point start, Point goal) {
    PriorityQueue pq;
    initPriorityQueue(&pq);

    Node startNode = {start, heuristic(start, goal), 0, heuristic(start, goal), start};
    insert(&pq, startNode);

    bool visited[MAX][MAX] = {false};
    visited[start.x][start.y] = true;

    Node path[MAX][MAX];

    while (!isEmpty(&pq)) {
        Node current = removeMin(&pq);
        printMaze(maze, rows, cols, current.point, start, goal);
        printOpenList(pq);
        printClosedList(visited, rows, cols);

        if (current.point.x == goal.x && current.point.y == goal.y) {
            printf("Path found with A* Search.\n");
            printPath(path, start, goal);
            printFinalMazeWithPath(maze, path, start, goal, rows, cols, 2); // 2 for A* Search
            return true;
        }

        for (int i = 0; i < 4; i++) {
            int nx = current.point.x + dx[i];
            int ny = current.point.y + dy[i];

            if (isValid(nx, ny, rows, cols) && !visited[nx][ny] && !isObstacle(maze, nx, ny)) {
                visited[nx][ny] = true;
                int g = current.g + 1;
                int h = heuristic((Point){nx, ny}, goal);
                Node neighbor = {{nx, ny}, g + h, g, h, current.point};
                path[nx][ny] = neighbor;
                insert(&pq, neighbor);
            }
        }
    }

    printf("No path found with A* Search.\n");
    return false;
}

int main() {
    int rows, cols;
    int maze[MAX][MAX];
    Point start, goal;
    int numObstacles;

    FILE *file = fopen("input.txt", "r");
    if (file == NULL) {
        printf("Error opening input.txt file.\n");
        return 1;
    }

    // Read rows and columns
    if (fscanf(file, "%d %d", &rows, &cols) != 2) {
        printf("Error reading rows and columns from file.\n");
        fclose(file);
        return 1;
    }

    // Initialize maze to 0
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            maze[i][j] = 0;
        }
    }

    // Read number of obstacles
    if (fscanf(file, "%d", &numObstacles) != 1) {
        printf("Error reading number of obstacles from file.\n");
        fclose(file);
        return 1;
    }

    // Read obstacle coordinates
    for (int i = 0; i < numObstacles; i++) {
        int x, y;
        if (fscanf(file, "%d %d", &x, &y) != 2) {
            printf("Error reading obstacle %d from file.\n", i + 1);
            fclose(file);
            return 1;
        }
        if (!isValid(x, y, rows, cols)) {
            printf("Invalid obstacle position (%d, %d). Exiting...\n", x, y);
            fclose(file);
            return 1;
        }
        maze[x][y] = -1;
    }

    // Read start point
    if (fscanf(file, "%d %d", &start.x, &start.y) != 2) {
        printf("Error reading start point from file.\n");
        fclose(file);
        return 1;
    }

    if (!isValid(start.x, start.y, rows, cols)) {
        printf("Invalid start position (%d, %d). Exiting...\n", start.x, start.y);
        fclose(file);
        return 1;
    }

    // Read goal point
    if (fscanf(file, "%d %d", &goal.x, &goal.y) != 2) {
        printf("Error reading goal point from file.\n");
        fclose(file);
        return 1;
    }

    if (!isValid(goal.x, goal.y, rows, cols)) {
        printf("Invalid goal position (%d, %d). Exiting...\n", goal.x, goal.y);
        fclose(file);
        return 1;
    }

    fclose(file);

    int choice;
    do {
        printf("\n--- Menu ---\n");
        printf("1. Best First Search\n");
        printf("2. A* Search\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                bestFirstSearch(maze, rows, cols, start, goal);
                break;
            case 2:
                aStarSearch(maze, rows, cols, start, goal);
                break;
            case 3:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 3);

    return 0;
}