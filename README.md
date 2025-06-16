# LL(1) Predictive Parser for Arithmetic Expressions

## Overview

An **LL(1) Predictive Parser** is a top-down parsing technique used in compiler design to parse a subset of context-free grammars. The parser computes FIRST and FOLLOW sets, constructs an LL(1) parsing table, and uses it to parse input strings, determining whether they are valid according to the grammar.

The term **LL(1)** refers to:

- **L**: Left-to-right scanning of the input.
- **L**: Leftmost derivation of the grammar.
- **1**: One symbol of lookahead is used to make parsing decisions.

The grammar supports expressions like:  
- `i`  
- `i+i`  
- `(i*i)`  
- `i+i*i`  

Where:
- `i` represents an identifier
- `$` marks the end of input

## Grammar

The grammar is defined as follows:

`E → T X`
`X → + T X | ε`
T → F Y
Y → * F Y | ε
F → ( E ) | i

- **Non-terminals:** E (expression), X (expression tail), T (term), Y (term tail), F (factor)
- **Terminals:** `i`, `+`, `*`, `(`, `)`, `$` (end marker)
- **Start symbol:** `E`

## Program Description

The program performs the following tasks:

1. **Grammar Definition:**  
   Defines the grammar using production rules stored in a `ProductionRule` structure.

2. **FIRST and FOLLOW Sets:**  
   Computes the FIRST and FOLLOW sets for all non-terminals to facilitate parsing table construction.

3. **Parsing Table:**  
   Builds an LL(1) parsing table based on the FIRST and FOLLOW sets, mapping non-terminals and terminals to productions.

4. **Parsing:**  
   Uses a stack-based predictive parsing algorithm to process input strings and determine if they are valid.

5. **Output:**  
   Displays:
   - The FIRST and FOLLOW sets
   - The parsing table
   - Step-by-step parsing actions for the input string
   - The final acceptance or rejection of the input string

## Example

### FIRST sets:

FIRST(E) = { i, ( }
FIRST(X) = { +, ε }
FIRST(T) = { i, ( }
FIRST(Y) = { *, ε }
FIRST(F) = { i, ( }


### FOLLOW sets:

FOLLOW(E) = { ), $ }
FOLLOW(X) = { ), $ }
FOLLOW(T) = { +, ), $ }
FOLLOW(Y) = { +, ), $ }
FOLLOW(F) = { *, +, ), $ }


### LL(1) Parsing Table:

| NonTerminal \ Terminal | i   | (   | )  | *  | +  | $  |
|-------------------------|-----|-----|----|----|----|----|
| E                       | E→TX| E→TX|    |    |    |    |
| X                       |     |     | X→ε|    | X→+TX| X→ε|
| T                       | T→FY| T→FY|    |    |    |    |
| Y                       |     |     | Y→ε| Y→*FY| Y→ε| Y→ε|
| F                       | F→i | F→(E)|   |    |    |    |

### Parsing Example:

**Input string:** `i+i`

#### Parsing Steps:

| Stack  | Input  | Action  |
|--------|--------|---------|
| E$     | i+i$   | Apply E → TX |
| TX$    | i+i$   | Apply T → FY |
| FYX$   | i+i$   | Apply F → i |
| iYX$   | i+i$   | Match i |
| YX$    | +i$    | Apply Y → ε |
| X$     | +i$    | Apply X → +TX |
| +TX$   | +i$    | Match + |
| TX$    | i$     | Apply T → FY |
| FYX$   | i$     | Apply F → i |
| iYX$   | i$     | Match i |
| YX$    | $      | Apply Y → ε |
| X$     | $      | Apply X → ε |
| $      | $      | Accept |

**Result:**  
Input `'i+i'` is **ACCEPTED** by the grammar.
