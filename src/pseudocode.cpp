/*

Lexar - check for syntax errors

Parser - Add parenthesis according to pemdas
    1. looks existing ().
    2. run parser in parenthesis recursively
    3. apply parser to ops out of () by pemdas

Evaluator - Evaluate parsed operations
    1. find first right parenthesis
    2. find the corresponding left parenthesis
    3. take note of string index of '('
    3. observer the operator character
    4. use switch statement on said character to find operation
    5. preform the operation
    6. store the character at the old index, thereby replacing the microexpression with its simplification
    7. repeat from 1 on the new string assuming there are no more operators
*/