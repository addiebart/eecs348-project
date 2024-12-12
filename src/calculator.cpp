/*
EECS 348 Term Project
Group 3, Fall 2024
Calculator
calculator.cpp
*/

#include <iostream>
#include <string>
#include <cmath>
#include <stdexcept>
#include <regex>

using namespace std;

// takes a char, returns true if a number 0-9
inline bool isNumeric(char c) {
    switch (c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return true;
    }
    return false;
}

// returns a bool for if a '-' with index i in str is for negation or subtraction based on the character to its left
// true: negation
// false: subtraction
inline bool minusIsNegation(const string &str, int i) {
    return !(isNumeric(str[i-1]) || str[i-1] == ')');
}

string lexer(const string &input) {
    // copy str
    string str = input;

    // check for only valid characters
    regex validChars("([0-9+\\-*/%() ]+)");
    if (!regex_match(str, validChars)) {
        throw runtime_error("Invalid character in input.");
    }

    // remove spaces, change ** to ^
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '*' && str[i+1] == '*') {
            str[i] = '^';
            str.erase(i+1, 1);
        }
        if (str[i] == ' ') {
            str.erase(i--, 1); // remove space
        }
    }

    // add implied multiplication
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '(' && i > 0 && (isNumeric(str[i - 1]) || str[i - 1] == ')')) {
            str.insert(i++, "*");
        }
        if (str[i] == ')' && i + 1 < str.length() && (isNumeric(str[i + 1]) || str[i + 1] == '(')) {
            str.insert(++i, "*");
        }
    }

    // change all negation to multiplication by negative 1
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '-') {
            string l = str.substr(0, i);
            string r = str.substr(i + 1);
            str = l + "-1*" + r;
        }
    }

    // check for matching parentheses
    int paraTracker = 0; // use a counter to check paraenthetical matching, stores left paras only
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '(') { 
            paraTracker++;
        } else if (str[i] == ')') { // if recived right, remove a left
            if (paraTracker <= 0) { // if you cant remove a corresponding left, there is an imbalance
                throw runtime_error("Unmatched closing parenthesis.");
            }
            paraTracker--;
        }
    }
    if (paraTracker != 0) { // if not all paras got removed from tracker, it means that a para went unmatched.
        throw runtime_error("Unmatched opening parenthesis.");
    }

    // handle unary + for inverse negation
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '+' && minusIsNegation(str, i)) {
            str.erase(i--, 1);
        }
    }

    // check for invalid operator placement
    for (int i = 0; i < str.length(); i++) {
        char c = str[i];
        if ((c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '%')) {
            // input is invalid if non unary operator at beginning
            if (i == 0 && c != '-' && c != '+') {
                throw runtime_error("Invalid operator usage.");
            }
            // input is invalid if any operator at end
            if (i == str.length() - 1) {
                throw runtime_error("Invalid operator usage.");
            }
            // account for unary operator execptions
            if ((c == '-' || c == '+') && minusIsNegation(str, i)) {
                continue;
            }
            // check for operator placement using general rules
            bool leftCharCompliant = isNumeric(str[i - 1]) || str[i - 1] == ')'; // conditions for left to be valid
            bool rightCharCompliant = isNumeric(str[i + 1]) || str[i + 1] == '(' || ((str[i + 1] == '+' || str[i + 1] == '-') && minusIsNegation(str, i + 1)); // conditions for right to be valid
            if (!(leftCharCompliant && rightCharCompliant)) {
                throw runtime_error("Invalid operator usage.");
            }
        }
    }

    return str;
}

// performs an operation on two ints stored as strings. returns a string.
string binaryEval(const string &left, char op, const string &right) {
    long l, r;
    try {
        l = stol(left);
        r = stol(right);
    } catch (const exception &e) {
        throw runtime_error("Could not convert string to number. This is likely because a number outside the integer size bounds was provided by the user.");
    }
    long result;
    bool negative;
    unsigned long check;
    switch (op) {
        case '^':
            result = pow(l, r);
            break;
        case '*':
            result = l * r;
            if (result != 0) {
                check = abs(l) * abs(r);
                // check that the abs is the same and use algebra to determine if an overflow as ocurred
                if (abs(result) != check || l != result / r) {
                    throw runtime_error("Integer over/underflow.");
                }
            }
            break;
        case '/':
            // check for division by zero
            if (r == 0) {
                throw runtime_error("Division by zero occurred.");
            }
            result = l / r;
            break;
        case '%':
            // check for divison by zero
            if (r == 0) {
                throw runtime_error("Division by zero occurred.");
            }
            result = l % r;
            break;
        case '+':
            result = l + r;
            // if adding like signs (because the signs will stay the same)
            check = ((l >= 0 && r >= 0) || (l <= 0 && r <= 0)) ? abs(l) + abs(r) : abs(l + r);
            if (abs(result) != check) {
                throw runtime_error("Integer over/underflow.");
            }
            break;
        case '-':
            result = l - r;
            // if subtracting from a negative number, or adding (by subtracting a negative) to a positive number
            check = ((l <= 0 && r >= 0) || (l >= 0 && r <= 0)) ? abs(l) + abs(r) : abs(l - r);
            if (abs(result) != check) {
                throw runtime_error("Integer over/underflow.");
            }
            break;
        default:
            throw runtime_error("Invalid opeator in binaryOperation.");
    }
    string resultstr = to_string(result);
    if (resultstr == "-9223372036854775808") {
        throw runtime_error("Integer over/underflow.");
    }
    return resultstr;
}

// fills arr with indicies of operators in str
int fillWithOpsInStr(const string &str, int arr[]) {
    int j = 0; // for array slots
    int count = 0; // count ops to be able to return arr length to caller
    for (int i = 0; i < str.length(); i++) {
        switch (str[i]) {
            case '^':
            case '*':
            case '/':
            case '%':
            case '+':
                arr[j++] = i; // use then increment
                count++;
                break;
            case '-': 
                // check if '-' is subtraction or negation
                if (!minusIsNegation(str, i)) {
                    arr[j++] = i;
                    count++;
                }
                break;
        }
    }
    // return the number of operators to the caller
    return count;
}

// takes a string from the lexar and simplifies it. returns a single integer as a string.
string parseAndEval(const string &str) { 
    // find the index of the first ')'
    int rightParaI = -1;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == ')') {
            rightParaI = i;
            break;
        }
    } 

    // if no () exists
    if (rightParaI < 0) {
        // dynamically allocate array and fill with indicies of operators in str
        int *ops = new int[str.size() + 1];
        int opCount = fillWithOpsInStr(str, ops);

        // no ops, ex: 4 -> 4, sanity case
        if (opCount == 0) {
            // clean dynamically allocated array
            delete[] ops;
            ops = nullptr;

            // return scalar
            return str;
        }

        // one operator, most common base case
        else if (opCount == 1) {
            // find the index of the operator
            int opI = ops[0];

            // get the operands as strings
            string l = str.substr(0, opI);
            string r = str.substr(opI + 1);

            // clean dynamically allocated array
            delete[] ops;
            ops = nullptr;

            // solve and return
            return binaryEval(l, str[opI], r);
        }

        // multiple operators, recursive step
        // this block needs to run three times but the code is almost entirely the same
        else {
            // cycle in pemdas order: 1 for ^, 2 for % / *, 3 for + -
           for (int priority = 1; priority <= 3; priority++) {
                // cycle indices of operators in ops[]
                for (int i = 0; i < opCount; i++) {
                    // get the index of the operator in the string
                    int opI = ops[i];

                    // determine a condition to select operator(s) based on priority
                    bool condition;
                    switch (priority) {
                        case 1:
                            condition = str[opI] == '^';
                            break;
                        case 2:
                            condition = str[opI] == '/' || str[opI] == '*' || str[opI] == '%';
                            break;
                        case 3:
                            condition = str[opI] == '+' || str[opI] == '-';
                    }

                    // if no operators of priority, skip this block
                    if (condition) {
                        // find bounds of substr, inclusive
                        int ssStart = (i == 0) ? 0 : ops[i-1] + 1;
                        int ssEnd = (i == opCount - 1) ? str.length() - 1 : ops[i+1] - 1;

                        // find the operands as strings
                        string l = str.substr(ssStart, opI);
                        string r = str.substr(opI + 1, ssEnd - opI); 

                        // execute operation
                        string result = binaryEval(l, str[opI], r);
                        string lwhole = str.substr(0, ssStart);
                        string rwhole = str.substr(ssEnd + 1, str.length() - ssEnd - 1);
                        string newexp = lwhole + result + rwhole;

                        // handle dynamically allocated array
                        delete[] ops;
                        ops = nullptr;

                        //recurse
                        return parseAndEval(newexp);
                    }
                }
            }
        }
    }

    // () exist
    else {
        // find the corresponding ')'
        int leftParaI;
        for (int i = rightParaI; i >= 0; i--) {
            if (str[i] == '(') {
                leftParaI = i;
                break;
            }
        }

        // get substr inside ()
        string subExp = str.substr(leftParaI + 1, rightParaI - leftParaI - 1);

        // recurse on substring
        subExp = parseAndEval(subExp);

        // get parts of instr to the left and right of ()
        string leftStr = str.substr(0, leftParaI);
        string rightStr = str.substr(rightParaI + 1);

        // mash strings together to make a new "str"
        string finalStr = leftStr + subExp + rightStr;
        
        // recurse on the new "str"
        return parseAndEval(finalStr);
    }

    // The program shouldn't reach here if input is good from lexar
    return "Error";
}

inline bool test(const string &msg, const string &str, long expected) {
    cout << msg << ": ";
    bool pass = expected == stol(parseAndEval(lexer(str)));
    cout << ((pass) ? "Passed!" : "Failed!") << endl;
    return pass;
}

inline bool errorTest(const string &msg, const string &in, const string &errmsg) {
    cout << msg << ": ";
    bool pass = false;
    try {
        parseAndEval(lexer(in));
    } catch (const exception &e) {
        pass = e.what() == errmsg;
    }
    cout << ((pass) ? "Passed!" : "Failed!") << endl;
    return pass;
}

void testCases() {
    bool allPass = true;

    allPass = test("Addition", "3 + 4", 7) && allPass;
    allPass = test("Subtraction with Parentheses", "8 - (5 - 2)",5) && allPass;
    allPass = test("Multiplication and Division", "10 * 2 / 5", 4) && allPass;
    allPass = test("Exponentiation", "2 ** 3", 8) && allPass;
    allPass = test("Mixed Operators 1", "4 * (3 + 2) % 7 - 1", 5) && allPass;
    allPass = test("Complex Addition with Extraneous Operators", "(((2 + 3))) + (((1 + 2)))", 8) && allPass;
    allPass = test("Mixed Operators with Extraneous Parentheses", "(5 * 2) - (((3 / 1) + ((4 % 3))))", 6) && allPass;
    allPass = test("Nested Parentheses with Exponents", "(((2 ** (1 + 1)) + ((3 - 1) ** 2)) / ((4 / 2) % 3))", 4) && allPass;
    allPass = test("Combination of Extraneous and Necessary Parentheses", "(((((5 - 3))) * (((2 + 1))) + ((2 * 3))))", 12) && allPass;
    allPass = test("Effects of Integer Division 1", "10 * (3 / 2)", 10) && allPass;
    allPass = test("Combining Unary Operators with Arithmetic Operations", "+(-2) * (-3) - ((-4) / (+5))", 6) && allPass;
    allPass = test("Unary Negation and Addition in Parentheses", "-(+1) + (+2)", 1) && allPass;
    allPass = test("Negation and Addition with Negated Parentheses", "-(-(-3)) + (-4) + (+5)", -2) && allPass;
    allPass = test("Unary Negation, Exponentiation, and Int Division", "+2 ** (-3)", 0) && allPass;
    allPass = test("Combining Unary Operators with Parentheses", "-(+2) * (+3) - (-4) / (-5)", -6) && allPass;
    allPass = test("Implied Multiplication", "4(5)", 20) && allPass;
    allPass = errorTest("Unmatched Parentheses", "2 * (4 + 3 - 1", "Unmatched opening parenthesis.") && allPass;
    allPass = errorTest("Operators Without Operands 1", "* 5 + 2", "Invalid operator usage.") && allPass;
    allPass = errorTest("Divide by Zero", "4 / 0", "Division by zero occurred.") && allPass;
    allPass = errorTest("Modulo by Zero", "4 % 0", "Division by zero occurred.") && allPass;
    allPass = errorTest("Invalid Characters", "7 & 3", "Invalid character in input.") && allPass;
    allPass = errorTest("Mismatched Parentheses 1", "(((3 + 4) - 2) + (1)", "Unmatched opening parenthesis.") && allPass;
    allPass = errorTest("Divide by Zero Multiplication Product", "(5 + 2) / (3 * 0)", "Division by zero occurred.") && allPass;
    allPass = errorTest("Incorrect Operator Sequence", "((2-) 1 + 3)",  "Invalid operator usage.") && allPass;
    allPass = errorTest("Operators Without Operands 2", "((4 * 2) + ( - ))", "Invalid operator usage.") && allPass;
    allPass = test("Multiply by Zero", "4 % 2 (1) + 3 ** 5", 243) && allPass;
    allPass = test("Exponent and Modulo", "2 ** 5 % 4", 0) && allPass;
    allPass = errorTest("Overflow by Addition", "9223372036854775806+12", "Integer over/underflow.") && allPass;
    allPass = errorTest("Underflow by Addition", "-9223372036854775806+-12", "Integer over/underflow.") && allPass;
    allPass = errorTest("Overflow by Subtraction", "9223372036854775806--12",  "Integer over/underflow.") && allPass;
    allPass = errorTest("Underflow by Subtraction", "-9223372036854775806-12", "Integer over/underflow.") && allPass;
    allPass = errorTest("Overflow by Multiplication", "4611686018427387915 * 2", "Integer over/underflow.") && allPass;
    allPass = errorTest("Underflow by Multiplication", "-4611686018427387915 * 2", "Integer over/underflow.") && allPass;
    allPass = errorTest("Overflow by Exponentiation", "200 ** 75", "Integer over/underflow.") && allPass;
    allPass = errorTest("Underflow by Exponentiation", "(-200) ** 75", "Integer over/underflow.") && allPass;
    allPass = test("Complex Mixed Operators 1", "(((4+2)-6)**2) + ((2*4)-(2**3)) * ((64/8) % 3)", 0) && allPass;
    allPass = test("Complex Mixed Operators 2", "(((5**3)/14)+27)*((17-15)**2)-(57+((7*3)-18))", 80) && allPass;
    allPass = test("Modulo and Exponent", "72 % 7 ** 2", 23) && allPass;
    allPass = test("Divide by Zero Modulo Product", "72 % 7 ** 2", 23) && allPass;
    allPass = test("Mixed Operators 2", "15 - 27 * 2 + 38 % 12", -37) && allPass;
    allPass = test("Combining Unary Operators with Parentheses and Implied Multiplication", "(+4)(-2)+(+6)(-3)-(-8)", -18) && allPass;
    allPass = errorTest("Mismatched Parentheses 2", "((7-2) % 3))", "Unmatched closing parenthesis.") && allPass;
    allPass = errorTest("Operators Without Operands 3", "**7+3", "Invalid operator usage.") && allPass;
    allPass = errorTest("Operators Without Operands 4", "% 5 - (9-0) /2 +5", "Invalid operator usage.") && allPass;
    allPass = test("Mixed Operators 3", "3 + 9 - 2 * 2 **2 % 2 ", 12) && allPass;
    allPass = test("Effects of Integer Division 2", "(7 % 4) ** 3 / 4", 6) && allPass;
    allPass = test("Implied Multiplication and Exponentiation 1", "(5) 7 ** 2", 245) && allPass;
    allPass = test("Implied Multiplication and Exponentiation 2", "8 ** 5 (6)", 196608) && allPass;
    allPass = test("Complex Exponentiation", "2 ** 3 ** 4", 4096) && allPass;
    allPass = errorTest("Complex Exponentiation Overflow", "2 ** (3 ** 4)", "Integer over/underflow.") && allPass;

    cout << ((allPass) ? "All test cases passed." : "At least one test case failed.") << endl;
}

int main() {
    while (true) {
        // prompt and read input
        cout << "Provide an expression to simplify, enter 't' to run test cases, or 'q' to quit:" << endl;
        string line;
        getline(cin, line);

        // check for quit signal
        if (line == "q") {
            return 0;
        }

        // check for and start test cases
        if (line == "t") {
            testCases();
            cout << endl;
            continue;
        }
        
        // find result and handle errors
        try {
            string result = lexer(line);
            result = parseAndEval(result);
            if (result == "Error") {
                cout << "Unknown Error" << endl;
            } else {
                cout << result << endl;
            }
        } catch (const runtime_error &e) {
            cout << "Error: " << e.what() << endl;
        } catch (const exception &e) {
            cout << "Error: Unknown Error" << endl;
        }
        cout << endl;       
    }
    return 0;
}