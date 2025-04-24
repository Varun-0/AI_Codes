#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_BLOCKS 10
#define MAX_STACK_SIZE 100
#define MAX_STRING_LEN 50
#define MAX_STACKS 10
#define MAX_OPERATIONS 50

// Condition types
typedef enum {
    ONTABLE,    // Block is on the table
    ON,         // Block is on another block
    CLEAR,      // Block has nothing on top of it
    HOLDING,    // Robot arm is holding the block
    ARMEMPTY    // Robot arm is empty
} ConditionType;

// Structure for a condition
typedef struct {
    ConditionType type;
    char block1;
    char block2;  // Used only for ON condition
} Condition;

// Structure for an operation
typedef enum {
    STACK,      // Stack block x on block y
    UNSTACK,    // Unstack block x from block y
    PICKUP,     // Pick up block x from the table
    PUTDOWN     // Put down block x on the table
} OperationType;

typedef struct {
    OperationType type;
    char block1;
    char block2;  // Used only for STACK and UNSTACK
} Operation;

// Structure to store operation history
typedef struct {
    Operation operations[MAX_OPERATIONS];
    int count;
} OperationHistory;

// Stack element types
// Add a new stack element type for combined goal
typedef enum {
    GOAL,           // A condition to be satisfied
    ACTION,         // An operation to be performed
    COMBINED_GOAL   // A special marker representing all goals combined
} StackElementType;

// Stack element structure
typedef struct {
    StackElementType type;
    union {
        Condition condition;
        Operation operation;
    } element;
} StackElement;

// World state represented as a list of conditions
typedef struct {
    Condition conditions[MAX_BLOCKS * 3]; // Max possible conditions
    int count;
} WorldState;

// Goal stack
StackElement goalStack[MAX_STACK_SIZE];
int stackTop = -1;

// Operation histories
OperationHistory topToBottomHistory = {0};
OperationHistory bottomToTopHistory = {0};
OperationHistory *currentHistory = NULL;

// Function to push an element to the goal stack
void push(StackElement element) {
    if (stackTop >= MAX_STACK_SIZE - 1) {
        printf("Error: Stack overflow\n");
        exit(1);
    }
    goalStack[++stackTop] = element;
}

// Function to pop an element from the goal stack
StackElement pop() {
    if (stackTop < 0) {
        printf("Error: Stack underflow\n");
        exit(1);
    }
    return goalStack[stackTop--];
}

// Function to print a condition
void printCondition(Condition c) {
    switch (c.type) {
        case ONTABLE:
            printf("ONTABLE(%c)", c.block1);
            break;
        case ON:
            printf("ON(%c,%c)", c.block1, c.block2);
            break;
        case CLEAR:
            printf("CLEAR(%c)", c.block1);
            break;
        case HOLDING:
            printf("HOLDING(%c)", c.block1);
            break;
        case ARMEMPTY:
            printf("ARMEMPTY");
            break;
    }
}

// Function to print an operation
void printOperation(Operation op) {
    switch (op.type) {
        case STACK:
            printf("STACK(%c,%c)", op.block1, op.block2);
            break;
        case UNSTACK:
            printf("UNSTACK(%c,%c)", op.block1, op.block2);
            break;
        case PICKUP:
            printf("PICKUP(%c)", op.block1);
            break;
        case PUTDOWN:
            printf("PUTDOWN(%c)", op.block1);
            break;
    }
}

// Function to add operation to history
void addToHistory(Operation op) {
    if (currentHistory != NULL && currentHistory->count < MAX_OPERATIONS) {
        currentHistory->operations[currentHistory->count++] = op;
    }
}

// Function to print the goal stack
void printStack() {
    printf("\nGoal Stack (from top to bottom):\n");
    for (int i = stackTop; i >= 0; i--) {
        if (goalStack[i].type == GOAL) {
            printf("GOAL: ");
            printCondition(goalStack[i].element.condition);
        } else if (goalStack[i].type == ACTION) {
            printf("ACTION: ");
            printOperation(goalStack[i].element.operation);
        } else if (goalStack[i].type == COMBINED_GOAL) {
            printf("COMBINED_GOAL: [All goals combined]");
        }
        printf("\n");
    }
    printf("\n");
}

// Function to check if a condition is in the world state
int conditionExists(WorldState *state, Condition c) {
    for (int i = 0; i < state->count; i++) {
        Condition current = state->conditions[i];
        
        if (current.type != c.type) continue;
        
        switch (c.type) {
            case ONTABLE:
            case CLEAR:
            case HOLDING:
                if (current.block1 == c.block1) return 1;
                break;
            case ON:
                if (current.block1 == c.block1 && current.block2 == c.block2) return 1;
                break;
            case ARMEMPTY:
                return 1;
                break;
        }
    }
    return 0;
}

// Function to add a condition to the world state
void addCondition(WorldState *state, Condition c) {
    if (!conditionExists(state, c)) {
        state->conditions[state->count++] = c;
    }
}

// Function to remove a condition from the world state
void removeCondition(WorldState *state, Condition c) {
    for (int i = 0; i < state->count; i++) {
        Condition current = state->conditions[i];
        
        if (current.type != c.type) continue;
        
        int match = 0;
        switch (c.type) {
            case ONTABLE:
            case CLEAR:
            case HOLDING:
                match = (current.block1 == c.block1);
                break;
            case ON:
                match = (current.block1 == c.block1 && current.block2 == c.block2);
                break;
            case ARMEMPTY:
                match = 1;
                break;
        }
        
        if (match) {
            // Remove by shifting all elements down
            for (int j = i; j < state->count - 1; j++) {
                state->conditions[j] = state->conditions[j + 1];
            }
            state->count--;
            return;
        }
    }
}

// Function to update the world state based on an operation
void executeOperation(WorldState *state, Operation op) {
    Condition c;
    
    printf("Executing: ");
    printOperation(op);
    printf("\n");
    
     // Check if trying to pick up a block while already holding another
     if ((op.type == PICKUP || op.type == UNSTACK) && !conditionExists(state, (Condition){.type = ARMEMPTY})) {
        // Find what block is being held
        char heldBlock = '\0';
        for (int i = 0; i < state->count; i++) {
            if (state->conditions[i].type == HOLDING) {
                heldBlock = state->conditions[i].block1;
                break;
            }
        }
        
        if (heldBlock != '\0') {
            printf("WARNING: Arm is already holding block %c. Putting it down first.\n", heldBlock);
            
            // Create and execute a PUTDOWN operation for the held block
            Operation putdownOp = {.type = PUTDOWN, .block1 = heldBlock};
            
            // Remove HOLDING condition
            c.type = HOLDING;
            c.block1 = heldBlock;
            removeCondition(state, c);
            
            // Add ARMEMPTY, ONTABLE and CLEAR conditions
            c.type = ARMEMPTY;
            addCondition(state, c);
            
            c.type = ONTABLE;
            c.block1 = heldBlock;
            addCondition(state, c);
            
            c.type = CLEAR;
            c.block1 = heldBlock;
            addCondition(state, c);
            
            // Add putdown to history as well
            addToHistory(putdownOp);
            
            printf("Automatically executed: PUTDOWN(%c)\n", heldBlock);
        }
    }

    // Add to operation history
    addToHistory(op);
    
   
    
    // Now proceed with the original operation
    switch (op.type) {
        case STACK:
            // Remove preconditions
            c.type = HOLDING;
            c.block1 = op.block1;
            removeCondition(state, c);
            
            c.type = CLEAR;
            c.block1 = op.block2;
            removeCondition(state, c);
            
            // Add effects
            c.type = ARMEMPTY;
            addCondition(state, c);
            
            c.type = ON;
            c.block1 = op.block1;
            c.block2 = op.block2;
            addCondition(state, c);
            
            c.type = CLEAR;
            c.block1 = op.block1;
            addCondition(state, c);
            break;
            
        case UNSTACK:
            // Remove preconditions
            c.type = ARMEMPTY;
            removeCondition(state, c);
            
            c.type = ON;
            c.block1 = op.block1;
            c.block2 = op.block2;
            removeCondition(state, c);
            
            c.type = CLEAR;
            c.block1 = op.block1;
            removeCondition(state, c);
            
            // Add effects
            c.type = HOLDING;
            c.block1 = op.block1;
            addCondition(state, c);
            
            c.type = CLEAR;
            c.block1 = op.block2;
            addCondition(state, c);
            break;
            
        case PICKUP:
            // Remove preconditions
            c.type = ARMEMPTY;
            removeCondition(state, c);
            
            c.type = ONTABLE;
            c.block1 = op.block1;
            removeCondition(state, c);
            
            c.type = CLEAR;
            c.block1 = op.block1;
            removeCondition(state, c);
            
            // Add effects
            c.type = HOLDING;
            c.block1 = op.block1;
            addCondition(state, c);
            break;
            
        case PUTDOWN:
            // Remove preconditions
            c.type = HOLDING;
            c.block1 = op.block1;
            removeCondition(state, c);
            
            // Add effects
            c.type = ARMEMPTY;
            addCondition(state, c);
            
            c.type = ONTABLE;
            c.block1 = op.block1;
            addCondition(state, c);
            
            c.type = CLEAR;
            c.block1 = op.block1;
            addCondition(state, c);
            break;
    }
}

// Function to print the world state
void printWorldState(WorldState state) {
    printf("Current World State:\n");
    for (int i = 0; i < state.count; i++) {
        printCondition(state.conditions[i]);
        printf("\n");
    }
    printf("\n");
}

// Function to parse a stack string and add conditions to the world state
void parseStack(WorldState *state, char *str) {
    char buffer[MAX_STRING_LEN];
    strncpy(buffer, str, MAX_STRING_LEN);
    
    // Remove trailing newline if present
    buffer[strcspn(buffer, "\n")] = 0;
    
    // Convert to uppercase
    for (int i = 0; buffer[i]; i++) {
        buffer[i] = toupper(buffer[i]);
    }
    
    // Count number of blocks in this stack
    int length = strlen(buffer);
    int blockCount = 0;
    for (int i = 0; i < length; i++) {
        if (isalpha(buffer[i])) {
            blockCount++;
        }
    }
    
    if (blockCount == 0) {
        return; // Empty stack
    }
    
    // Process the stack from bottom to top
    char blocks[MAX_BLOCKS];
    int blockIndex = 0;
    
    for (int i = 0; i < length; i++) {
        if (isalpha(buffer[i])) {
            blocks[blockIndex++] = buffer[i];
        }
    }
    
    // Bottom block is on table
    Condition c;
    c.type = ONTABLE;
    c.block1 = blocks[0];
    addCondition(state, c);
    
    // Remaining blocks are stacked on each other
    for (int i = 1; i < blockCount; i++) {
        c.type = ON;
        c.block1 = blocks[i];
        c.block2 = blocks[i-1];
        addCondition(state, c);
    }
    
    // Top block is clear
    c.type = CLEAR;
    c.block1 = blocks[blockCount-1];
    addCondition(state, c);
}

// Function to add the preconditions of an operation to the stack
void addPreconditions(Operation op) {
    StackElement element;
    Condition c;
    
    printf("Replacing operation with preconditions: ");
    printOperation(op);
    printf("\n");
    
    switch (op.type) {
        case STACK:
            // Preconditions for STACK(x,y): HOLDING(x), CLEAR(y)
            element.type = GOAL;
            element.element.condition.type = CLEAR;
            element.element.condition.block1 = op.block2;
            push(element);
            
            element.type = GOAL;
            element.element.condition.type = HOLDING;
            element.element.condition.block1 = op.block1;
            push(element);
            break;
            
        case UNSTACK:
            // Preconditions for UNSTACK(x,y): ARMEMPTY, ON(x,y), CLEAR(x)
            element.type = GOAL;
            element.element.condition.type = CLEAR;
            element.element.condition.block1 = op.block1;
            push(element);
            
            element.type = GOAL;
            element.element.condition.type = ON;
            element.element.condition.block1 = op.block1;
            element.element.condition.block2 = op.block2;
            push(element);
            
            element.type = GOAL;
            element.element.condition.type = ARMEMPTY;
            push(element);
            break;
            
        case PICKUP:
            // Preconditions for PICKUP(x): ARMEMPTY, ONTABLE(x), CLEAR(x)
            element.type = GOAL;
            element.element.condition.type = CLEAR;
            element.element.condition.block1 = op.block1;
            push(element);
            
            element.type = GOAL;
            element.element.condition.type = ONTABLE;
            element.element.condition.block1 = op.block1;
            push(element);
            
            element.type = GOAL;
            element.element.condition.type = ARMEMPTY;
            push(element);
            break;
            
        case PUTDOWN:
            // Preconditions for PUTDOWN(x): HOLDING(x)
            element.type = GOAL;
            element.element.condition.type = HOLDING;
            element.element.condition.block1 = op.block1;
            push(element);
            break;
    }
}

// Function to achieve a goal (condition)
void achieveGoal(WorldState *state, Condition goal) {
    printf("Trying to achieve goal: ");
    printCondition(goal);
    printf("\n");
    
    // Check if the goal is already satisfied
    if (conditionExists(state, goal)) {
        printf("Goal already satisfied.\n");
        return;
    }
    
    StackElement element;
    Operation op;
    
    switch (goal.type) {
        case ON:
            // To achieve ON(x,y), use STACK(x,y)
            op.type = STACK;
            op.block1 = goal.block1;
            op.block2 = goal.block2;
            
            element.type = ACTION;
            element.element.operation = op;
            push(element);
            printStack();
            
            addPreconditions(op);
            printStack();
            break;
            
        case ONTABLE:
            // To achieve ONTABLE(x), use PUTDOWN(x)
            op.type = PUTDOWN;
            op.block1 = goal.block1;
            
            element.type = ACTION;
            element.element.operation = op;
            push(element);
            printStack();
            
            addPreconditions(op);
            printStack();
            break;
            
        case CLEAR:
            // To achieve CLEAR(x), find what's on x and remove it
            for (int i = 0; i < state->count; i++) {
                if (state->conditions[i].type == ON && state->conditions[i].block2 == goal.block1) {
                    char blockOnTop = state->conditions[i].block1;
                    
                    // Use UNSTACK to clear the block
                    op.type = UNSTACK;
                    op.block1 = blockOnTop;
                    op.block2 = goal.block1;
                    
                    element.type = ACTION;
                    element.element.operation = op;
                    push(element);
                    printStack();
                    
                    addPreconditions(op);
                    printStack();
                    return;
                }
            }
            printf("Error: Cannot clear block %c as nothing is on it\n", goal.block1);
            break;
            
        case HOLDING:
            // To achieve HOLDING(x), use PICKUP or UNSTACK depending on where x is
            for (int i = 0; i < state->count; i++) {
                if (state->conditions[i].type == ONTABLE && state->conditions[i].block1 == goal.block1) {
                    // Block is on table, use PICKUP
                    op.type = PICKUP;
                    op.block1 = goal.block1;
                    
                    element.type = ACTION;
                    element.element.operation = op;
                    push(element);
                    printStack();
                    
                    addPreconditions(op);
                    printStack();
                    return;
                } else if (state->conditions[i].type == ON && state->conditions[i].block1 == goal.block1) {
                    // Block is on another block, use UNSTACK
                    op.type = UNSTACK;
                    op.block1 = goal.block1;
                    op.block2 = state->conditions[i].block2;
                    
                    element.type = ACTION;
                    element.element.operation = op;
                    push(element);
                    printStack();
                    
                    addPreconditions(op);
                    printStack();
                    return;
                }
            }
            printf("Error: Cannot find block %c to hold\n", goal.block1);
            break;
            
        case ARMEMPTY:
            // To achieve ARMEMPTY, find what the arm is holding and put it down
            for (int i = 0; i < state->count; i++) {
                if (state->conditions[i].type == HOLDING) {
                    char blockHeld = state->conditions[i].block1;
                    
                    // Use PUTDOWN to achieve ARMEMPTY
                    op.type = PUTDOWN;
                    op.block1 = blockHeld;
                    
                    element.type = ACTION;
                    element.element.operation = op;
                    push(element);
                    printStack();
                    
                    addPreconditions(op);
                    printStack();
                    return;
                }
            }
            printf("Error: Arm is already empty\n");
            break;
    }
}

// Function to visualize the stacks
void visualizeStacks(WorldState state) {
    // Find all blocks that are on the table
    char tableBlocks[MAX_BLOCKS];
    int tableBlockCount = 0;
    
    for (int i = 0; i < state.count; i++) {
        if (state.conditions[i].type == ONTABLE) {
            tableBlocks[tableBlockCount++] = state.conditions[i].block1;
        }
    }
    
    printf("Stack Visualization:\n");
    
    // For each block on the table, build and print its stack
    for (int i = 0; i < tableBlockCount; i++) {
        char stack[MAX_BLOCKS];
        int stackSize = 0;
        
        // Add the bottom block (on table)
        stack[stackSize++] = tableBlocks[i];
        
        // Find blocks on top recursively
        char currentTop = tableBlocks[i];
        while (1) {
            int found = 0;
            for (int j = 0; j < state.count; j++) {
                if (state.conditions[j].type == ON && state.conditions[j].block2 == currentTop) {
                    currentTop = state.conditions[j].block1;
                    stack[stackSize++] = currentTop;
                    found = 1;
                    break;
                }
            }
            if (!found) break;
        }
        
        // Print this stack from bottom to top
        printf("Stack %d: ", i + 1);
        for (int j = 0; j < stackSize; j++) {
            printf("%c", stack[j]);
            if (j < stackSize - 1) printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

// Function to print operation history
void printOperationHistory(OperationHistory history, const char* title) {
    printf("\n%s:\n", title);
    if (history.count == 0) {
        printf("No operations performed.\n");
        return;
    }
    
    printf("Total operations: %d\n", history.count);
    for (int i = 0; i < history.count; i++) {
        printf("%d. ", i + 1);
        printOperation(history.operations[i]);
        printf("\n");
    }
    printf("\n");
}

// Function to reset the goal stack
void resetGoalStack() {
    stackTop = -1;
}

// Function to copy a world state
void copyWorldState(WorldState *dest, WorldState *src) {
    dest->count = 0;
    for (int i = 0; i < src->count; i++) {
        addCondition(dest, src->conditions[i]);
    }
}

// Function to check if current state matches the goal state
int matchesGoalState(WorldState *currentState, WorldState *goalState) {
    // Check if all goal conditions are satisfied in current state
    for (int i = 0; i < goalState->count; i++) {
        if (!conditionExists(currentState, goalState->conditions[i])) {
            return 0; // Found a goal condition that's not satisfied
        }
    }
    return 1; // All goal conditions are satisfied
}

// Function to perform goal stack planning with goals added top-to-bottom
void performTopToBottomPlanning(WorldState *initialState, WorldState *goalState) {
    WorldState currentState = {0};
    copyWorldState(&currentState, initialState);
    
    // Set current history to top-to-bottom
    currentHistory = &topToBottomHistory;
    currentHistory->count = 0;
    
    printf("APPROACH 1: TOP-TO-BOTTOM GOAL STACK PLANNING\n");
    
    resetGoalStack();
    
    // First push the combined goal marker
    StackElement combinedGoal;
    combinedGoal.type = COMBINED_GOAL;
    push(combinedGoal);
    
    // Then add each goal condition to the stack from top to bottom (last to first)
    for (int i = goalState->count - 1; i >= 0; i--) {
        StackElement element;
        element.type = GOAL;
        element.element.condition = goalState->conditions[i];
        push(element);
    }
    
    printf("Starting Goal Stack Planning (Top-to-Bottom)\n");
    printStack();
    
    // Main planning loop
    while (stackTop >= 0) {
        // Check if only the combined goal remains
        if (stackTop == 0 && goalStack[0].type == COMBINED_GOAL) {
            printf("Checking if goal state has been reached...\n");
            if (matchesGoalState(&currentState, goalState)) {
                printf("Goal state reached! Planning complete.\n");
                break;
            } else {
                printf("Goal state not yet reached. Pushing goals back onto stack.\n");
                // Add each goal condition to the stack from top to bottom again
                for (int i = goalState->count - 1; i >= 0; i--) {
                    StackElement element;
                    element.type = GOAL;
                    element.element.condition = goalState->conditions[i];
                    push(element);
                }
                printStack();
                continue;
            }
        }
        
        StackElement top = pop();
        
        if (top.type == GOAL) {
            // If it's a goal, try to achieve it
            Condition goal = top.element.condition;
            
            if (conditionExists(&currentState, goal)) {
                printf("Goal already satisfied: ");
                printCondition(goal);
                printf("\n");
            } else {
                // Push it back and try to achieve it
                push(top);
                achieveGoal(&currentState, goal);
            }
        } else if (top.type == ACTION) {
            // If it's an action, execute it
            Operation action = top.element.operation;
            executeOperation(&currentState, action);
            printWorldState(currentState);
            visualizeStacks(currentState);
        }
        // Skip COMBINED_GOAL as it's just a marker
        
        printStack();
    }
    
    printf("Planning complete! Final state (Top-to-Bottom):\n");
    printWorldState(currentState);
    visualizeStacks(currentState);
}

// Function to perform goal stack planning with goals added bottom-to-top
void performBottomToTopPlanning(WorldState *initialState, WorldState *goalState) {
    WorldState currentState = {0};
    copyWorldState(&currentState, initialState);
    
    // Set current history to bottom-to-top
    currentHistory = &bottomToTopHistory;
    currentHistory->count = 0;
    
    printf("\n\n====================================\n");
    printf("APPROACH 2: BOTTOM-TO-TOP GOAL STACK PLANNING\n");
    printf("====================================\n\n");
    
    resetGoalStack();
    
    // First push the combined goal marker
    StackElement combinedGoal;
    combinedGoal.type = COMBINED_GOAL;
    push(combinedGoal);
    
    // Then add each goal condition to the stack from bottom to top (first to last)
    for (int i = 0; i < goalState->count; i++) {
        StackElement element;
        element.type = GOAL;
        element.element.condition = goalState->conditions[i];
        push(element);
    }
    
    printf("Starting Goal Stack Planning (Bottom-to-Top)\n");
    printStack();
    
    // Main planning loop
    while (stackTop >= 0) {
        // Check if only the combined goal remains
        if (stackTop == 0 && goalStack[0].type == COMBINED_GOAL) {
            printf("Checking if goal state has been reached...\n");
            if (matchesGoalState(&currentState, goalState)) {
                printf("Goal state reached! Planning complete.\n");
                break;
            } else {
                printf("Goal state not yet reached. Pushing goals back onto stack.\n");
                // Add each goal condition to the stack from bottom to top again
                for (int i = 0; i < goalState->count; i++) {
                    StackElement element;
                    element.type = GOAL;
                    element.element.condition = goalState->conditions[i];
                    push(element);
                }
                printStack();
                continue;
            }
        }
        
        StackElement top = pop();
        
        if (top.type == GOAL) {
            // If it's a goal, try to achieve it
            Condition goal = top.element.condition;
            
            if (conditionExists(&currentState, goal)) {
                printf("Goal already satisfied: ");
                printCondition(goal);
                printf("\n");
            } else {
                // Push it back and try to achieve it
                push(top);
                achieveGoal(&currentState, goal);
            }
        } else if (top.type == ACTION) {
            // If it's an action, execute it
            Operation action = top.element.operation;
            executeOperation(&currentState, action);
            printWorldState(currentState);
            visualizeStacks(currentState);
        }
        // Skip COMBINED_GOAL as it's just a marker
        
        printStack();
    }
    
    printf("Planning complete! Final state (Bottom-to-Top):\n");
    printWorldState(currentState);
    visualizeStacks(currentState);  
}

// Function to print summary comparison of both approaches
void printOperationSummary() {
    printf("\n\n====================================\n");
    printf("COMPARISON OF BOTH APPROACHES\n");
    printf("====================================\n");
    
    printOperationHistory(topToBottomHistory, "Operations performed by Top-to-Bottom approach");
    printOperationHistory(bottomToTopHistory, "Operations performed by Bottom-to-Top approach");
    
    // Compare number of operations
    printf("Top-to-Bottom approach used %d operations.\n", topToBottomHistory.count);
    printf("Bottom-to-Top approach used %d operations.\n", bottomToTopHistory.count);
    
    if (topToBottomHistory.count < bottomToTopHistory.count) {
        printf("Top-to-Bottom approach was more efficient by %d operations.\n", 
               bottomToTopHistory.count - topToBottomHistory.count);
    } else if (bottomToTopHistory.count < topToBottomHistory.count) {
        printf("Bottom-to-Top approach was more efficient by %d operations.\n",
               topToBottomHistory.count - bottomToTopHistory.count);
    } else {
        printf("Both approaches used the same number of operations.\n");
    }
}

int main() {
    WorldState initialState = {0};
    WorldState goalState = {0};
    char input[MAX_STRING_LEN];
    int i;
    
    // Initialize the arm as empty
    Condition armEmptyCondition;
    armEmptyCondition.type = ARMEMPTY;
    addCondition(&initialState, armEmptyCondition);
    
    // Get number of blocks
    printf("Enter the number of blocks: ");
    int numBlocks;
    scanf("%d", &numBlocks);
    getchar(); // Clear newline
    
    if (numBlocks <= 0 || numBlocks > MAX_BLOCKS) {
        printf("Error: Invalid number of blocks. Must be between 1 and %d.\n", MAX_BLOCKS);
        return 1;
    }
    
    // Get initial state
    printf("Enter the number of stacks in the initial state: ");
    int numInitialStacks;
    scanf("%d", &numInitialStacks);
    getchar(); // Clear newline
    
    if (numInitialStacks <= 0 || numInitialStacks > numBlocks) {
        printf("Error: Invalid number of stacks. Must be between 1 and %d.\n", numBlocks);
        return 1;
    }
    
    printf("Enter the initial stacks (e.g., \"A B\" for B on top of A):\n");
    for (i = 0; i < numInitialStacks; i++) {
        printf("Stack %d: ", i + 1);
        fgets(input, MAX_STRING_LEN, stdin);
        parseStack(&initialState, input);
    }
    
    // Get goal state
    printf("\nEnter the number of stacks in the goal state: ");
    int numGoalStacks;
    scanf("%d", &numGoalStacks);
    getchar(); // Clear newline
    
    if (numGoalStacks <= 0 || numGoalStacks > numBlocks) {
        printf("Error: Invalid number of stacks. Must be between 1 and %d.\n", numBlocks);
        return 1;
    }
    
    printf("Enter the goal stacks (e.g., \"A B\" for B on top of A):\n");
    for (i = 0; i < numGoalStacks; i++) {
        printf("Stack %d: ", i + 1);
        fgets(input, MAX_STRING_LEN, stdin);
        parseStack(&goalState, input);
    }
    
    // Print the initial and goal states
    printf("\nInitial State:\n");
    printWorldState(initialState);
    visualizeStacks(initialState);
    
    printf("Goal State:\n");
    printWorldState(goalState);
    visualizeStacks(goalState);
    
    // Perform planning with both approaches
    performTopToBottomPlanning(&initialState, &goalState);
    performBottomToTopPlanning(&initialState, &goalState);
    
    // Print summary of operations
    printOperationSummary();
    
    return 0;
} 