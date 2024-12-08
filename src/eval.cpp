/*
EECS 348 Term Project
Group 3, Fall 2024
Calculator
eval.cpp
*/

#include <iostream>
#include <string>
#include <cmath>
#include <stdexcept>

using namespace std;

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

// performs an operation on two ints stored as strings. returns a string.
string binaryEval(const string& left, char op, const string& right) {
    int l = stoi(left);
    int r = stoi(right);
    int result;
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
    return to_string(result);
}

// returns a bool for if a '-' with index i in str is for negation or subtraction based on the character to its left
// true: negation
// false: subtraction
inline bool minusIsNegation(const string& str, int i) {
    return !(isNumeric(str[i-1]) || str[i-1] == ')');
}

// fills arr with indicies of operators in str
int fillWithOpsInStr(const string& str, int arr[]) {
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
    return count;
}

string parseAndEval(const string& str); // prototype to be used by multipleOpsBlock

string multipleOpsBlock(int ops[], int opCount, int priority, const string& str) {
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
    // signal to caller instance of eval() that no operator of priority exists
    return "no";
}

string parseAndEval(const string& str) { 

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
        else {
            string blockOut;

            // check for ^
            blockOut = multipleOpsBlock(ops, opCount, 1, str);
            if (blockOut != "no") {
                return blockOut;
            } 

            // check for * / %
            blockOut = multipleOpsBlock(ops, opCount, 2, str);
            if (blockOut != "no") {
                return blockOut;
            } 

            // check for + -
            blockOut = multipleOpsBlock(ops, opCount, 3, str);
            if (blockOut != "no") {
                return blockOut;
            }

            // it shouldn't be possible to reach here
            cout << "all blockOuts were no" << endl;
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
        
        // recurse on it
        return parseAndEval(finalStr);
    }

    // The program shouldn't reach here if input is good from lexar
    return "Error";
}

void test(const string& str, int expected) {
    cout << "Testing \"" + str + "\" = " + to_string(expected) + ": ";
    int actual = stoi(parseAndEval(str));
    if (actual == expected) {
        cout << "Passed!" << endl;
    } else {
        cout << "Failed!" << endl;
    }
}

void testCases() {
    test("2+2", 4);
    test("2+2+2", 6);
    test("2^3", 8);
    test("-2+5", 3);
    test("-2-5", -7);
    test("-2--5", 3);
    test("6%4/2", 1);
    test("6%(4/2)", 0);
    test("((3+5)/2)+(3*5+2)", 21);
    test("4^-2", 0); // 1/16 = 0 by int divison
    test("-7*(30/1+17%3)-3^2", (-7*((30/1)+(17%3)))-pow(3,2));
    test("-7*(30/1+17%3)-3^-2", (-7*((30/1)+(17%3)))-pow(3,-2));

    // error cases
    // div by 0
    try {
        cout << "Testing divide by zero: ";
        parseAndEval("10/(6%2)");
        cout << "Failed!" << endl;
    } catch (const exception& e) {
        cout << "Passed!" << endl;
    }
    // mod by 0
    try {
        cout << "Testing mod by zero: ";
        parseAndEval("10%(6-2*3)");
        cout << "Failed!" << endl;
    } catch (const exception& e) {
        cout << "Passed!" << endl;
    }
}

int main() {
    while (true) {
        // prompt and read input
        cout << "Provide an expression to simplify, or enter 't' to run test cases:" << endl;
        string line;
        getline(cin, line);

        // check for and start test cases
        if (line == "t") {
            testCases();
            cout << endl;
            continue;
        }
        
        // find result and handle errors
        string result;
        try {
            result = parseAndEval(line);
            if (result == "Error") {
                cout << "Unknown Error" << endl;
            } else {
                cout << result << endl;
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
        cout << endl;
    }
    return 0;
}