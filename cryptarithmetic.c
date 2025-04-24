
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LEN 20
#define MAX_WORDS 10
#define MAX_UNIQUE_CHARS 10 // Max unique characters allowed (0-9 digits)

// Structure to hold puzzle information
typedef struct {
    char words[MAX_WORDS][MAX_LEN]; // Array to store multiple words
    char result[MAX_LEN];           // Result word
    int numWords;                   // Number of words in the puzzle
    char uniqueChars[MAX_UNIQUE_CHARS];
    int numUniqueChars;
    int assigned[MAX_UNIQUE_CHARS]; // Values assigned to each unique character
    bool used[10];                  // Tracks which digits (0-9) have been used
    int solutionCount;              // Count of solutions found
    
    // Constraint arrays
    bool canBeZero[MAX_UNIQUE_CHARS];   // Whether a character can be assigned 0
    bool possibleDigits[MAX_UNIQUE_CHARS][10]; // Possible digits for each char
    bool fixedAssignment[MAX_UNIQUE_CHARS]; // Whether this character has fixed assignment
} Puzzle;

// Function prototypes
bool isConsistent(Puzzle *puzzle, int charIndex, int digit);
bool solveAllSolutions(Puzzle *puzzle, int charIndex);
int evaluateWord(Puzzle *puzzle, char *word);
void findUniqueChars(Puzzle *puzzle);
int getCharIndex(Puzzle *puzzle, char c);
void printSolution(Puzzle *puzzle);
void preComputeConstraints(Puzzle *puzzle);
void analyzeLastDigits(Puzzle *puzzle);
void checkLeadingDigitConstraints(Puzzle *puzzle);
void printConstraintAnalysis(Puzzle *puzzle);

// Check if assigning 'digit' to the character at 'charIndex' is consistent with constraints
bool isConsistent(Puzzle *puzzle, int charIndex, int digit) {
    // If this character has a fixed assignment, only allow that value
    if (puzzle->fixedAssignment[charIndex] && puzzle->assigned[charIndex] != digit) {
        return false;
    }
    
    // Check if digit is already assigned to another character
    if (puzzle->used[digit])
        return false;
    
    // Check pre-computed constraints - is this digit allowed for this character?
    if (!puzzle->possibleDigits[charIndex][digit])
        return false;
    
    // Check zero constraint
    if (digit == 0 && !puzzle->canBeZero[charIndex])
        return false;
    
    // All constraints satisfied
    return true;
}

// Evaluate a word based on current assignments
int evaluateWord(Puzzle *puzzle, char *word) {
    int value = 0;
    for (int i = 0; word[i] != '\0'; i++) {
        int idx = getCharIndex(puzzle, word[i]);
        value = value * 10 + puzzle->assigned[idx];
    }
    return value;
}

// Find the index of a character in the uniqueChars array
int getCharIndex(Puzzle *puzzle, char c) {
    for (int i = 0; i < puzzle->numUniqueChars; i++) {
        if (puzzle->uniqueChars[i] == c) {
            return i;
        }
    }
    return -1;
}

// Extract unique characters from the puzzle
void findUniqueChars(Puzzle *puzzle) {
    bool charExists[26] = {false};
    int uniqueCount = 0;
    
    // Process all words including the result
    for (int w = 0; w < puzzle->numWords; w++) {
        for (int i = 0; puzzle->words[w][i] != '\0'; i++) {
            char c = puzzle->words[w][i];
            if (isalpha(c)) {
                c = toupper(c);
                int idx = c - 'A';
                if (!charExists[idx]) {
                    charExists[idx] = true;
                    puzzle->uniqueChars[uniqueCount++] = c;
                    
                    // Check if we have too many unique characters
                    if (uniqueCount > MAX_UNIQUE_CHARS) {
                        printf("Error: Too many unique characters (max is 10)\n");
                        exit(1);
                    }
                }
            }
        }
    }
    
    // Process the result word
    for (int i = 0; puzzle->result[i] != '\0'; i++) {
        char c = puzzle->result[i];
        if (isalpha(c)) {
            c = toupper(c);
            int idx = c - 'A';
            if (!charExists[idx]) {
                charExists[idx] = true;
                puzzle->uniqueChars[uniqueCount++] = c;
                
                // Check if we have too many unique characters
                if (uniqueCount > MAX_UNIQUE_CHARS) {
                    printf("Error: Too many unique characters (max is 10)\n");
                    exit(1);
                }
            }
        }
    }
    
    puzzle->numUniqueChars = uniqueCount;
}

// Pre-compute constraints to narrow down the search space
void preComputeConstraints(Puzzle *puzzle) {
    // Initialize all characters to allow all digits initially
    for (int i = 0; i < puzzle->numUniqueChars; i++) {
        puzzle->canBeZero[i] = true;
        puzzle->fixedAssignment[i] = false;
        puzzle->assigned[i] = -1; // Unassigned
        
        // All digits are possible initially
        for (int d = 0; d <= 9; d++) {
            puzzle->possibleDigits[i][d] = true;
        }
    }
    
    // Check which characters cannot be zero (leading digits)
    checkLeadingDigitConstraints(puzzle);
    
    // Analyze last digits for constraints
    analyzeLastDigits(puzzle);
    
    // Look for any characters that can only be one specific digit
    for (int i = 0; i < puzzle->numUniqueChars; i++) {
        int possibleCount = 0;
        int lastPossibleDigit = -1;
        
        for (int d = 0; d <= 9; d++) {
            if (puzzle->possibleDigits[i][d]) {
                possibleCount++;
                lastPossibleDigit = d;
            }
        }
        
        // If only one possibility exists, mark as fixed
        if (possibleCount == 1 && lastPossibleDigit != -1) {
            puzzle->fixedAssignment[i] = true;
            puzzle->assigned[i] = lastPossibleDigit;
            puzzle->used[lastPossibleDigit] = true;
            printf("Pre-assigned: %c = %d (fixed by constraint analysis)\n", 
                   puzzle->uniqueChars[i], lastPossibleDigit);
        }
    }
}

// Check for leading digit constraints (cannot be zero)
void checkLeadingDigitConstraints(Puzzle *puzzle) {
    // First letter of any word cannot be assigned zero
    for (int i = 0; i < puzzle->numWords; i++) {
        char firstChar = puzzle->words[i][0];
        int idx = getCharIndex(puzzle, firstChar);
        puzzle->canBeZero[idx] = false;
        puzzle->possibleDigits[idx][0] = false;
    }
    
    // First letter of result cannot be assigned zero
    char resultFirstChar = puzzle->result[0];
    int resultIdx = getCharIndex(puzzle, resultFirstChar);
    puzzle->canBeZero[resultIdx] = false;
    puzzle->possibleDigits[resultIdx][0] = false;
}

// Analyze last digits for possible constraints
void analyzeLastDigits(Puzzle *puzzle) {
    // Extract last digits
    char inputLastDigits[MAX_WORDS];
    char resultLastDigit;
    
    // Get result's last digit
    int resultLen = strlen(puzzle->result);
    if (resultLen > 0) {
        resultLastDigit = puzzle->result[resultLen - 1];
    } else {
        return; // Can't analyze empty result
    }
    
    // Get last digits of input words
    int wordCount = 0;
    for (int i = 0; i < puzzle->numWords; i++) {
        int len = strlen(puzzle->words[i]);
        if (len > 0) {
            inputLastDigits[wordCount++] = puzzle->words[i][len - 1];
        }
    }
    
    // If all last digits are the same character and match result's last digit
    bool allSame = true;
    for (int i = 1; i < wordCount; i++) {
        if (inputLastDigits[i] != inputLastDigits[0]) {
            allSame = false;
            break;
        }
    }
    
    if (allSame && wordCount > 0) {
        char commonChar = inputLastDigits[0];
        
        // If last digits of inputs and result are the same character
        if (commonChar == resultLastDigit) {
            int idx = getCharIndex(puzzle, commonChar);
            // This character can only be 0
            for (int d = 1; d <= 9; d++) {
                puzzle->possibleDigits[idx][d] = false;
            }
            printf("Constraint: %c must be 0 (all last digits)\n", commonChar);
        }
        
        // If last digits of inputs differ from result
        else {
            // More complex logic here - could be 0 or 5 depending on count
            // For simplicity, we'll skip detailed analysis here
        }
    }
    
    // Advanced sum constraint analysis would go here
    // This would involve modular arithmetic to limit possible values
}

// Print constraint analysis for debugging
void printConstraintAnalysis(Puzzle *puzzle) {
    printf("\nConstraint Analysis:\n");
    
    for (int i = 0; i < puzzle->numUniqueChars; i++) {
        char c = puzzle->uniqueChars[i];
        printf("%c: ", c);
        
        printf("Possible digits [");
        int possibleCount = 0;
        for (int d = 0; d <= 9; d++) {
            if (puzzle->possibleDigits[i][d]) {
                printf("%d ", d);
                possibleCount++;
            }
        }
        printf("] (%d possibilities) ", possibleCount);
        
        if (!puzzle->canBeZero[i]) {
            printf("Cannot be 0 ");
        }
        
        if (puzzle->fixedAssignment[i]) {
            printf("Fixed: %d", puzzle->assigned[i]);
        }
        
        printf("\n");
    }
    printf("\n");
}

// Recursive backtracking solver to find all solutions
bool solveAllSolutions(Puzzle *puzzle, int charIndex) {
    // Skip characters that already have fixed assignments
    while (charIndex < puzzle->numUniqueChars && puzzle->fixedAssignment[charIndex]) {
        charIndex++;
    }
    
    // Base case: all characters have been assigned
    if (charIndex >= puzzle->numUniqueChars) {
        // Check if the equation is satisfied
        int sum = 0;
        for (int i = 0; i < puzzle->numWords; i++) {
            sum += evaluateWord(puzzle, puzzle->words[i]);
        }
        int result = evaluateWord(puzzle, puzzle->result);
        
        if (sum == result) {
            // Found a valid solution, print it
            puzzle->solutionCount++;
            printf("\nSolution #%d:\n", puzzle->solutionCount);
            printSolution(puzzle);
            return true; // Continue searching for more solutions
        }
        return false;
    }
    
    bool foundAnySolution = false;
    
    // Try each possible digit for the current character
    for (int digit = 0; digit <= 9; digit++) {
        if (isConsistent(puzzle, charIndex, digit)) {
            // Assign this digit and mark it as used
            puzzle->assigned[charIndex] = digit;
            puzzle->used[digit] = true;
            
            // Recursively try to solve the rest
            bool found = solveAllSolutions(puzzle, charIndex + 1);
            if (found) {
                foundAnySolution = true;
                // Don't return here, continue to find all solutions
            }
            
            // Backtrack
            puzzle->used[digit] = false;
            
            // Only reset if not a fixed assignment
            if (!puzzle->fixedAssignment[charIndex]) {
                puzzle->assigned[charIndex] = -1;
            }
        }
    }
    
    return foundAnySolution;
}

// Print the solution
void printSolution(Puzzle *puzzle) {
    // Print the mapping
    for (int i = 0; i < puzzle->numUniqueChars; i++) {
        printf("%c = %d\n", puzzle->uniqueChars[i], puzzle->assigned[i]);
    }
    
    // Print the equation with values
    printf("\nEquation:\n");
    for (int i = 0; i < puzzle->numWords; i++) {
        if (i > 0) {
            printf("+ ");
        }
        printf("%s (%d)\n", puzzle->words[i], evaluateWord(puzzle, puzzle->words[i]));
    }
    printf("= %s (%d)\n", puzzle->result, evaluateWord(puzzle, puzzle->result));
}

int main() {
    Puzzle puzzle;
    
    // Initialize
    memset(puzzle.used, false, sizeof(puzzle.used));
    puzzle.solutionCount = 0;
    
    // Get number of words from the user
    printf("Enter the number of words in the equation (max %d): ", MAX_WORDS - 1);
    scanf("%d", &puzzle.numWords);
    
    if (puzzle.numWords < 1 || puzzle.numWords >= MAX_WORDS) {
        printf("Invalid number of words. Must be between 1 and %d.\n", MAX_WORDS - 1);
        return 1;
    }
    
    // Get input words from the user
    for (int i = 0; i < puzzle.numWords; i++) {
        printf("Enter word %d: ", i + 1);
        scanf("%s", puzzle.words[i]);
        
        // Convert to uppercase
        for (int j = 0; puzzle.words[i][j]; j++) {
            puzzle.words[i][j] = toupper(puzzle.words[i][j]);
        }
    }
    
    // Get the result word
    printf("Enter the result word: ");
    scanf("%s", puzzle.result);
    
    // Convert to uppercase
    for (int i = 0; puzzle.result[i]; i++) {
        puzzle.result[i] = toupper(puzzle.result[i]);
    }
    
    // Find unique characters
    findUniqueChars(&puzzle);
    
    // Check if the problem is solvable
    if (puzzle.numUniqueChars > 10) {
        printf("Error: Too many unique characters. Maximum allowed is 10.\n");
        return 1;
    }
    
    printf("Puzzle has %d unique characters: ", puzzle.numUniqueChars);
    for (int i = 0; i < puzzle.numUniqueChars; i++) {
        printf("%c ", puzzle.uniqueChars[i]);
    }
    printf("\n");
    
    // Pre-compute constraints before backtracking
    printf("\nAnalyzing constraints...\n");
    preComputeConstraints(&puzzle);
    printConstraintAnalysis(&puzzle);
    
    printf("Starting backtracking with pre-computed constraints...\n");
    
    // Solve the puzzle to find all solutions
    bool solved = solveAllSolutions(&puzzle, 0);
    
    if (puzzle.solutionCount > 0) {
        printf("\nTotal solutions found: %d\n", puzzle.solutionCount);
    } else {
        printf("\nNo solution exists for this puzzle.\n");
    }
    
    return 0;
} 