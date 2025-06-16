# LL(1) Predictive Parser

An **LL(1) Predictive Parser** is a top-down parsing technique used in compiler design to parse a subset of context-free grammars.

The term **LL(1)** refers to:

- **L**: Left-to-right scanning of the input.
- **L**: Leftmost derivation of the grammar.
- **1**: One symbol of lookahead is used to make parsing decisions.

This type of parser uses a parsing table and stack to guide the parsing process, making it efficient for a certain class of grammars known as LL(1) grammars.

# LL(1) Predictive Parser for Arithmetic Expressions

## Overview

This C program implements an LL(1) predictive parser for a context-free grammar that recognizes simple arithmetic expressions over the alphabet `{ i, (, ), *, +, $ }`.  
The parser computes FIRST and FOLLOW sets, constructs an LL(1) parsing table, and uses it to parse input strings, determining whether they are valid according to the grammar.

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

