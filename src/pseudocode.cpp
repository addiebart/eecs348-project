/*

Lexar - Clean string:
- Remove white space
- Convert ** to ^
- change 3(4) to 3*(4)
- Check for valid string
- only numbers paras and operators
- paras nest and close properly
- no double operators
- no unary operators execpt negation
- change -(x) to -1*(x)

Evaluator - Evaluate parsed operations
- Find parentheses if they exist
- Recurse on anything in parentheses, simplifying them
- Apply pemdas to recurse on a single operation with one operator
- Handle expressions with 0 or 1 operators in base cases
*/