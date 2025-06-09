#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Maximum limits for various components
#define MAX_RULES 10          // Maximum number of production rules
#define MAX_SYMBOLS 10        // Maximum symbols in a production
#define MAX_TERMINALS 10      // Maximum terminal symbols
#define MAX_NONTERMINALS 10   // Maximum non-terminal symbols
#define MAX_INPUT 100         // Maximum input

// Structure to represent a production rule
typedef struct {
    char lhs;                   // Left-hand side non-terminal
    char rhs[MAX_SYMBOLS][MAX_SYMBOLS];  // Right-hand side productions (multiple alternatives)
    int rhs_count;              // Number of productions for this non-terminal
} ProductionRule;

// Structure to store FIRST and FOLLOW sets for non-terminals
typedef struct {
    char non_terminal;          // The non-terminal symbol
    char first[MAX_TERMINALS];  // FIRST set for this non-terminal
    int first_count;            // Number of elements in FIRST set
    char follow[MAX_TERMINALS]; // FOLLOW set for this non-terminal
    int follow_count;           // Number of elements in FOLLOW set
} FirstFollow;

// Global variables
ProductionRule rules[MAX_RULES];                // Array to store all production rules
FirstFollow firstFollow[MAX_NONTERMINALS];     // Array to store FIRST and FOLLOW sets
char terminals[MAX_TERMINALS] = {'i', '(', ')', '*', '+', '$'};  // Terminal symbols
char non_terminals[MAX_NONTERMINALS] = {'E', 'X', 'T', 'Y', 'F'}; // Non-terminal symbols
int terminal_count = 6;                         // Number of terminal symbols
int non_terminal_count = 5;                     // Number of non-terminal symbols
int rule_count = 0;                             // Current number of rules

// Parsing table: non-terminals x terminals -> production to apply
char parsing_table[MAX_NONTERMINALS][MAX_TERMINALS][MAX_SYMBOLS];

// Stack implementation for parsing
char stack[MAX_INPUT * 2];
int top = -1;

void push(char c) {
    if (top < MAX_INPUT * 2 - 1) {
        stack[++top] = c;
    }
}

char pop() {
    if (top >= 0) {
        return stack[top--];
    }
    return '\0';
}

char peek() {
    if (top >= 0) {
        return stack[top];
    }
    return '\0';
}

// Function to check if a character is a terminal
int is_terminal(char c) {
    for (int i = 0; i < terminal_count; i++) {
        if (terminals[i] == c) return 1;
    }
    return 0;
}

// Function to check if a character is a non-terminal
int is_non_terminal(char c) {
    for (int i = 0; i < non_terminal_count; i++) {
        if (non_terminals[i] == c) return 1;
    }
    return 0;
}

// Function to get the index of a terminal in the terminals array
int get_terminal_index(char c) {
    for (int i = 0; i < terminal_count; i++) {
        if (terminals[i] == c) return i;
    }
    return -1;  // Return -1 if not found
}

// Function to get the index of a non-terminal in the non_terminals array
int get_non_terminal_index(char c) {
    for (int i = 0; i < non_terminal_count; i++) {
        if (non_terminals[i] == c) return i;
    }
    return -1;  // Return -1 if not found
}

// Function to add a symbol to the FIRST set of a non-terminal
void add_to_first(int nt_index, char symbol) {
    // Check if the symbol is already in the FIRST set
    for (int i = 0; i < firstFollow[nt_index].first_count; i++) {
        if (firstFollow[nt_index].first[i] == symbol) return;
    }
    // Add the symbol if not already present
    firstFollow[nt_index].first[firstFollow[nt_index].first_count++] = symbol;
}

// Function to add a symbol to the FOLLOW set of a non-terminal
void add_to_follow(int nt_index, char symbol) {
    // Check if the symbol is already in the FOLLOW set
    for (int i = 0; i < firstFollow[nt_index].follow_count; i++) {
        if (firstFollow[nt_index].follow[i] == symbol) return;
    }
    // Add the symbol if not already present
    firstFollow[nt_index].follow[firstFollow[nt_index].follow_count++] = symbol;
}

// Function to add a production rule to the grammar
void add_rule(char lhs, char *rhs) {
    rules[rule_count].lhs = lhs;            // Set left-hand side
    strcpy(rules[rule_count].rhs[0], rhs);  // Set right-hand side
    rules[rule_count].rhs_count = 1;        // Set production count
    rule_count++;                           // Increment rule counter
}

// Function to check if a FIRST set contains epsilon ('e')
bool contains_epsilon(char *first, int count) {
    for (int i = 0; i < count; i++) {
        if (first[i] == 'e') return true;
    }
    return false;
}

// Function to compute FIRST sets for all non-terminals
void compute_first() {
    bool changed;  // Flag to track if any FIRST set was modified
    
    // Initialize FIRST sets
    for (int i = 0; i < non_terminal_count; i++) {
        firstFollow[i].non_terminal = non_terminals[i];
        firstFollow[i].first_count = 0;
    }
    
    // Iteratively compute FIRST sets until no changes occur
    do {
        changed = false;
        for (int i = 0; i < rule_count; i++) {
            char lhs = rules[i].lhs;
            char *rhs = rules[i].rhs[0];
            int lhs_index = get_non_terminal_index(lhs);
            
            // Case 1: First symbol of RHS is a terminal
            if (is_terminal(rhs[0])) {
                int old_count = firstFollow[lhs_index].first_count;
                add_to_first(lhs_index, rhs[0]);
                if (firstFollow[lhs_index].first_count > old_count) {
                    changed = true;
                }
            }
            // Case 2: First symbol of RHS is a non-terminal
            else if (is_non_terminal(rhs[0])) {
                int rhs_index = get_non_terminal_index(rhs[0]);
                
                // Add FIRST(rhs[0]) (excluding epsilon) to FIRST(lhs)
                for (int j = 0; j < firstFollow[rhs_index].first_count; j++) {
                    if (firstFollow[rhs_index].first[j] != 'e') {
                        int old_count = firstFollow[lhs_index].first_count;
                        add_to_first(lhs_index, firstFollow[rhs_index].first[j]);
                        if (firstFollow[lhs_index].first_count > old_count) {
                            changed = true;
                        }
                    }
                }
                
                // Check if FIRST(rhs[0]) contains epsilon
                bool has_epsilon = contains_epsilon(firstFollow[rhs_index].first, firstFollow[rhs_index].first_count);
                
                // If epsilon is present and there are more symbols in RHS
                if (has_epsilon && strlen(rhs) > 1) {
                    int pos = 1;
                    // Process subsequent symbols until we find one without epsilon
                    while (pos < strlen(rhs)) {
                        if (is_terminal(rhs[pos])) {
                            // If terminal, add to FIRST(lhs)
                            int old_count = firstFollow[lhs_index].first_count;
                            add_to_first(lhs_index, rhs[pos]);
                            if (firstFollow[lhs_index].first_count > old_count) {
                                changed = true;
                            }
                            break;
                        }
                        else {
                            // If non-terminal, add its FIRST (excluding epsilon)
                            int next_nt_index = get_non_terminal_index(rhs[pos]);
                            for (int j = 0; j < firstFollow[next_nt_index].first_count; j++) {
                                if (firstFollow[next_nt_index].first[j] != 'e') {
                                    int old_count = firstFollow[lhs_index].first_count;
                                    add_to_first(lhs_index, firstFollow[next_nt_index].first[j]);
                                    if (firstFollow[lhs_index].first_count > old_count) {
                                        changed = true;
                                    }
                                }
                            }
                            
                            // Check if this non-terminal has epsilon
                            has_epsilon = contains_epsilon(firstFollow[next_nt_index].first, firstFollow[next_nt_index].first_count);
                            if (!has_epsilon) break;
                            pos++;
                        }
                    }
                    
                    // If all symbols in RHS can derive epsilon, add epsilon to FIRST(lhs)
                    if (pos == strlen(rhs)) {
                        int old_count = firstFollow[lhs_index].first_count;
                        add_to_first(lhs_index, 'e');
                        if (firstFollow[lhs_index].first_count > old_count) {
                            changed = true;
                        }
                    }
                }
            }
            // Case 3: RHS is epsilon
            else if (rhs[0] == 'e') {
                int old_count = firstFollow[lhs_index].first_count;
                add_to_first(lhs_index, 'e');
                if (firstFollow[lhs_index].first_count > old_count) {
                    changed = true;
                }
            }
        }
    } while (changed);  // Continue until no changes in any FIRST set
}

// Function to compute FOLLOW sets for all non-terminals
void compute_follow() {
    bool changed;  // Flag to track if any FOLLOW set was modified
    
    // Initialize FOLLOW sets
    for (int i = 0; i < non_terminal_count; i++) {
        firstFollow[i].follow_count = 0;
    }
    
    // Rule 1: $ is in FOLLOW(S) where S is the start symbol
    int start_index = get_non_terminal_index('E');
    add_to_follow(start_index, '$');
    
    // Iteratively compute FOLLOW sets until no changes occur
    do {
        changed = false;
        for (int i = 0; i < rule_count; i++) {
            char lhs = rules[i].lhs;
            char *rhs = rules[i].rhs[0];
            int lhs_index = get_non_terminal_index(lhs);
            
            // Process each symbol in the RHS
            for (int j = 0; j < strlen(rhs); j++) {
                if (is_non_terminal(rhs[j])) {
                    int nt_index = get_non_terminal_index(rhs[j]);
                    
                    // Case 1: A → αBβ (β is not ε)
                    if (j < strlen(rhs) - 1) {
                        char next = rhs[j+1];
                        
                        // Subcase 1: β is a terminal
                        if (is_terminal(next)) {
                            int old_count = firstFollow[nt_index].follow_count;
                            add_to_follow(nt_index, next);
                            if (firstFollow[nt_index].follow_count > old_count) {
                                changed = true;
                            }
                        }
                        // Subcase 2: β is a non-terminal
                        else if (is_non_terminal(next)) {
                            int next_index = get_non_terminal_index(next);
                            
                            // Add FIRST(β) (excluding ε) to FOLLOW(B)
                            for (int k = 0; k < firstFollow[next_index].first_count; k++) {
                                if (firstFollow[next_index].first[k] != 'e') {
                                    int old_count = firstFollow[nt_index].follow_count;
                                    add_to_follow(nt_index, firstFollow[next_index].first[k]);
                                    if (firstFollow[nt_index].follow_count > old_count) {
                                        changed = true;
                                    }
                                }
                            }
                            
                            // If FIRST(β) contains ε, add FOLLOW(A) to FOLLOW(B)
                            if (contains_epsilon(firstFollow[next_index].first, firstFollow[next_index].first_count)) {
                                for (int k = 0; k < firstFollow[lhs_index].follow_count; k++) {
                                    int old_count = firstFollow[nt_index].follow_count;
                                    add_to_follow(nt_index, firstFollow[lhs_index].follow[k]);
                                    if (firstFollow[nt_index].follow_count > old_count) {
                                        changed = true;
                                    }
                                }
                            }
                        }
                    }
                    
                    // Case 2: A → αB or A → αBβ where FIRST(β) contains ε
                    if (j == strlen(rhs) - 1 || 
                        (j < strlen(rhs) - 1 && is_non_terminal(rhs[j+1]) && 
                         contains_epsilon(firstFollow[get_non_terminal_index(rhs[j+1])].first, 
                                        firstFollow[get_non_terminal_index(rhs[j+1])].first_count))) {
                        // Add FOLLOW(A) to FOLLOW(B)
                        for (int k = 0; k < firstFollow[lhs_index].follow_count; k++) {
                            int old_count = firstFollow[nt_index].follow_count;
                            add_to_follow(nt_index, firstFollow[lhs_index].follow[k]);
                            if (firstFollow[nt_index].follow_count > old_count) {
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    } while (changed);  // Continue until no changes in any FOLLOW set
}

// Function to create the LL(1) parsing table
void create_parsing_table() {
    // Initialize all entries to empty strings
    for (int i = 0; i < non_terminal_count; i++) {
        for (int j = 0; j < terminal_count; j++) {
            strcpy(parsing_table[i][j], "");
        }
    }

    // Populate the parsing table
    for (int i = 0; i < rule_count; i++) {
        char lhs = rules[i].lhs;
        char *rhs = rules[i].rhs[0];
        int lhs_index = get_non_terminal_index(lhs);

        // Case 1: Production is A → ε
        if (strcmp(rhs, "e") == 0) {
            // For each terminal in FOLLOW(A), add A → ε
            for (int k = 0; k < firstFollow[lhs_index].follow_count; k++) {
                int term_index = get_terminal_index(firstFollow[lhs_index].follow[k]);
                if (term_index != -1) {
                    strcpy(parsing_table[lhs_index][term_index], "e");
                }
            }
        }
        // Case 2: Production starts with a terminal
        else if (is_terminal(rhs[0])) {
            // Add production to table entry for A and the terminal
            int term_index = get_terminal_index(rhs[0]);
            if (term_index != -1) {
                strcpy(parsing_table[lhs_index][term_index], rhs);
            }
        } 
        // Case 3: Production starts with a non-terminal
        else if (is_non_terminal(rhs[0])) {
            int rhs_index = get_non_terminal_index(rhs[0]);

            // For each terminal in FIRST(rhs[0]) (excluding ε), add production
            for (int k = 0; k < firstFollow[rhs_index].first_count; k++) {
                if (firstFollow[rhs_index].first[k] != 'e') {
                    int term_index = get_terminal_index(firstFollow[rhs_index].first[k]);
                    if (term_index != -1) {
                        strcpy(parsing_table[lhs_index][term_index], rhs);
                    }
                }
            }

            // If FIRST(rhs[0]) contains ε, add production for terminals in FOLLOW(A)
            if (contains_epsilon(firstFollow[rhs_index].first, firstFollow[rhs_index].first_count)) {
                for (int k = 0; k < firstFollow[lhs_index].follow_count; k++) {
                    int term_index = get_terminal_index(firstFollow[lhs_index].follow[k]);
                    if (term_index != -1) {
                        strcpy(parsing_table[lhs_index][term_index], rhs);
                    }
                }
            }
        }
    }
}

// Function to print FIRST and FOLLOW sets
void print_first_follow() {
    printf("\nFIRST sets:\n");
    for (int i = 0; i < non_terminal_count; i++) {
        printf("FIRST(%c) = { ", firstFollow[i].non_terminal);
        for (int j = 0; j < firstFollow[i].first_count; j++) {
            printf("%c ", firstFollow[i].first[j]);
        }
        printf("}\n");
    }
    
    printf("\nFOLLOW sets:\n");
    for (int i = 0; i < non_terminal_count; i++) {
        printf("FOLLOW(%c) = { ", firstFollow[i].non_terminal);
        for (int j = 0; j < firstFollow[i].follow_count; j++) {
            printf("%c ", firstFollow[i].follow[j]);
        }
        printf("}\n");
    }
}

// Function to print the LL(1) parsing table
void print_parsing_table() {
    printf("\nLL(1) Parsing Table:\n");
    printf("NonTerminal\\Terminal|");
    for (int i = 0; i < terminal_count; i++) {
        printf("%6c|", terminals[i]);
    }
    printf("\n");
    
    printf("---------------------");
    for (int i = 0; i < terminal_count; i++) {
        printf("-------");
    }
    printf("\n");
    
    for (int i = 0; i < non_terminal_count; i++) {
        printf("%10c\t    |", non_terminals[i]);
        for (int j = 0; j < terminal_count; j++) {
            if (strcmp(parsing_table[i][j], "") != 0) {
                if (strcmp(parsing_table[i][j], "e") == 0) {
                    printf("%3c→e |", non_terminals[i]);
                } else {
                    printf("%3c→%s|", non_terminals[i], parsing_table[i][j]);
                }
            } else {
                printf("%6s|", "");
            }
        }
        printf("\n");
    }
}

// Function to parse input string using the parsing table
bool parse_input(char *input) {
    // Initialize stack with $ and start symbol
    top = -1;
    push('$');
    push('E');  // Start symbol
    
    // Append $ to input
    strcat(input, "$");
    int input_ptr = 0;
    
    printf("\nParsing Steps:\n");
    printf("Stack\t\tInput\t\tAction\n");
    
    while (top >= 0) {
        // Print current stack and input
        printf("%-15s\t%-15s\t", stack, input + input_ptr);
        
        char stack_top = peek();
        char current_input = input[input_ptr];
        
        if (stack_top == current_input) {
            // Match found
            if (stack_top == '$') {
                printf("Accept\n");
                return true;
            }
            printf("Match %c\n", stack_top);
            pop();
            input_ptr++;
        }
        else if (is_terminal(stack_top)) {
            printf("Error: Terminal mismatch\n");
            return false;
        }
        else {
            // Non-terminal on stack - use parsing table
            int nt_index = get_non_terminal_index(stack_top);
            int t_index = get_terminal_index(current_input);
            
            if (nt_index == -1 || t_index == -1) {
                printf("Error: Invalid symbol\n");
                return false;
            }
            
            if (strcmp(parsing_table[nt_index][t_index], "") == 0) {
                printf("Error: No production in parsing table\n");
                return false;
            }
            
            char *production = parsing_table[nt_index][t_index];
            printf("Apply %c -> %s\n", stack_top, production);
            
            pop();  // Remove non-terminal from stack
            
            // Push production in reverse order (except for epsilon)
            if (strcmp(production, "e") != 0) {
                for (int i = strlen(production) - 1; i >= 0; i--) {
                    push(production[i]);
                }
            }
        }
    }
    
    return false;
}

int main() {
    // Initialize grammar
    add_rule('E', "TX");
    add_rule('X', "+TX");
    add_rule('X', "e");
    add_rule('T', "FY");
    add_rule('Y', "*FY");
    add_rule('Y', "e");
    add_rule('F', "(E)");
    add_rule('F', "i");
    
    // Compute FIRST and FOLLOW sets
    compute_first();
    compute_follow();
    
    // Print the computed sets
    print_first_follow();
    
    // Create and print the parsing table
    create_parsing_table();
    print_parsing_table();
    
    // Get input from user
    char input[MAX_INPUT];
    printf("\nEnter input string to parse: ");
    scanf("%s", input);
    
    // Parse the input
    if (parse_input(input)) {
        printf("\nInput '%s' is ACCEPTED by the grammar\n", input);
    } else {
        printf("\nInput '%s' is REJECTED by the grammar\n", input);
    }
    
    return 0;
}