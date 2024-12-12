/*
EECS 348 Term Project
Group 3, Fall 2024
Calculator
calculator.cpp
*/

#include <iostream>
#include <string>
#include <cmath> // for pow() only
#include <stdexcept>
#include <regex>
#include <stack>

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
    regex validChars("([0-9+\\-*/%()]+)");
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
    stack <char> paraStack; // use a stack to check paraenthetical matching, stores left paras only
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '(') { 
            paraStack.push('(');
        } else if (str[i] == ')') { // if recived right, remove a left
            if (paraStack.empty()) { // if you cant remove a corresponding left, there is an imbalance
                throw runtime_error("Unmatched closing parenthesis.");
            }
            paraStack.pop();
        }
    }
    if (!paraStack.empty()) { // if not all paras got popped, it means that a para went unmatched.
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
        if ((c == '+' || c == '-' || c == '*' || c == '/' || c == '^')) {
            // input is invalid if non unary operator at beginning
            if (i == 0 && c != '-' && c != '+') {
                throw runtime_error("Invalid operator usage. Index=" + to_string(i));
            }
            // input is invalid if any operator at end
            if (i == str.length() - 1) {
                throw runtime_error("Invalid operator usage. Index=" + to_string(i));
            }
            // account for unary operator execptions
            if ((c == '-' || c == '+') && minusIsNegation(str, i)) {
                continue;
            }
            // check for operator placement using general rules
            bool leftCharCompliant = isNumeric(str[i - 1]) || str[i - 1] == ')'; // conditions for left to be valid
            bool rightCharCompliant = isNumeric(str[i + 1]) || str[i + 1] == '(' || ((str[i + 1] == '+' || str[i + 1] == '-') && minusIsNegation(str, i + 1)); // conditions for right to be valid
            if (!(leftCharCompliant && rightCharCompliant)) {
                throw runtime_error("Invalid operator usage. Index=" + to_string(i));
            }
        }
    }

    return str;
}

// performs an operation on two ints stored as strings. returns a string.
string binaryEval(const string &left, char op, const string &right) {
    long l = stol(left);
    long r = stol(right);
    long result;
    switch (op) {
        case '^':
            result = pow(l, r);
            break;
        case '*':
            result = l * r;
            break;
        case '/':
            // check for division by zero
            if (r == 0) {
                throw runtime_error("Division by zero occurred!");
            }
            result = l / r;
            break;
        case '%':
            // check for divison by zero
            if (r == 0) {
                throw runtime_error("Division by zero occurred!");
            }
            result = l % r;
            break;
        case '+':
            result = l + r;
            break;
        case '-':
            result = l - r;
            break;
        default:
            cout << "invalid op in binaryOperation" << endl;
    }
    string resultstr = to_string(result);
    if (resultstr == "-9223372036854775808") {
        throw runtime_error("Long int overflow/underflow!");
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

inline void test(const string& str, long expected) {
    cout << "Testing \"" + str + "\" = " + to_string(expected) + ": ";
    long actual = stol(parseAndEval(lexer(str)));
    cout << ((actual == expected) ? "Passed!" : "Failed!") << endl;
}

void testCases() {
    test("2+2", 4);
    test("2+2+2", 6);
    test("2**3", 8);
    test("-2+5", 3);
    test("-2-5", -7);
    test("-2--5", 3);
    test("2--5", 7);
    test("6472%100", 72);
    test("6%4/2", 1);
    test("6%(4/2)", 0);
    test("((3+5)/2)+(3*5+2)", 21);
    test("4**-2", 0); // = 1/16 = 0 by int divison
    test("-2**2", -4);
    test("(-2)**2", 4);
    test("-7*(30/1+17%3)-3**2", (-7*((30/1)+(17%3)))-pow(3,2));
    test("-7*(30/1+17%3)-3**-2", (-7*((30/1)+(17%3)))-pow(3,-2));

    // error cases
    // div by 0
    try {
        cout << "Testing divide by zero: ";
        parseAndEval("10/(6%2)");
        cout << "Failed!" << endl;
    } catch (const runtime_error &e) {
        cout << "Passed!" << endl;
    }
    // mod by 0
    try {
        cout << "Testing mod by zero: ";
        parseAndEval("10%(6-2*3)");
        cout << "Failed!" << endl;
    } catch (const runtime_error &e) {
        cout << "Passed!" << endl;
    }
    // overflow
    try {
        cout << "Overflow: ";
        parseAndEval("200^75");
        cout << "Failed!" << endl;
    } catch (const runtime_error &e) {
        cout << "Passed!" << endl;
    }
    // underflow
    try {
        cout << "Underflow: ";
        parseAndEval("-200^75");
        cout << "Failed!" << endl;
    } catch (const runtime_error &e) {
        cout << "Passed!" << endl;
    }
    // invalid input
    try {
        cout << "Invalid Input (\"/*x\"): ";
        parseAndEval("/*x");
        cout << "Failed!" << endl;
    } catch (const exception &e) {
        cout << "Passed!" << endl;
    }
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
            cout << "Lexed: |" << result << "|" << endl;
            result = parseAndEval(result);
            if (result == "Error") {
                cout << "Unknown Error" << endl;
            } else {
                cout << result << endl;
            }
        } catch (const runtime_error &e) {
            cout << "Error: " << e.what() << endl;
        } catch (const exception &e) {
            cout << "Error: Invalid Input" << endl;
        }
        cout << endl;       
    }
    return 0;
}