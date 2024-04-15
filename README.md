# Expert System for Propositional Calculus

## Overview

This project is an expert system designed to solve problems in propositional calculus using a backward-chaining inference engine. It interprets facts and rules from a given text file to evaluate queries based on the propositional logic provided. The engine supports logical operators like AND, OR, XOR, and NOT, and can handle complex logical expressions with parentheses.

## Features

- **Logical Operators:** Supports AND (+), OR (|), XOR (^), NOT (!), implies (=>), and if and only if (<=>).
- **Backward-Chaining Inference:** Determines the truth value of queries based on given facts and rules.
- **Error Handling:** Alerts the user to contradictions or syntax errors in the input file.
- **Multi-Rule Conclusions:** Can derive multiple conclusions from a single rule.

## Build and Run

The system is implemented in C++ and uses a Makefile for easy compilation and execution.

### Prerequisites

- C++11 compliant compiler

### Compilation

Use the following commands to compile the expert system:

```bash
make all      # Compile the program
make clean    # Remove object files
make fclean   # Remove object files and executable
make re       # Rebuild the project
make debug    # Compile with debug information
```

### Usage

To run the expert system, use the following command:

```bash
./expert_system <file.txt> # I.e. ./expert_system rules2.txt
```

## Input File Format

The input file should contain facts and rules in the following format:

```
# this is a comment
C => E # C implies E
A + B + C => D # A and B and C implies D
...
=ABG # Initial facts: A, B, and G are true. All others are false.
?GVX # Queries: What are G, V, and X?
```