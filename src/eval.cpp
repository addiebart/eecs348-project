#include <iostream>
#include <string>
#include <cmath>

using namespace std;

bool isNumeric(char c) {
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

string binaryOperation(const string& left, char op, const string& right) {
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
            result = l / r;
            break;
        case '%':
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

bool minusIsNegation(const string& str, int i) {
    char leftChar = str[i-1];
    if (isNumeric(leftChar) || leftChar == ')') {
        return false;
    }
    return true;
}

int fillWithOpsInStr(const string& str, int arr[]) { //fills arr
    int j = 0;
    int count = 0;
    for (int i = 0; i < str.length(); i++) {
        switch (str[i]) {
            case '^': // marked p
            case '*': // marked m
            case '/': // marked d
            case '%': // marked r
            case '+': // marked p
                arr[j++] = i;
                count++;
                break;
            case '-':
                if (minusIsNegation(str, i)) {
                    break;
                } else {
                    arr[j++] = i;
                    count++;
                    break;
                }
        }
    }
    arr[j] = 0;
    return count;
}

string multipleOpsBlock(int ops[], int opCount, int priority, const string& str); // proto

string eval(const string& str) { 
    int rightParaI = -1;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == ')') {
            rightParaI = i;
            break;
        }
    } 

    if (rightParaI < 0) {
        // no paras
        int *ops = new int[str.size() + 1];
        int opCount = fillWithOpsInStr(str, ops);

        // no ops - ex: 4 -> 4, sanity case
        if (opCount == 0) {
            delete[] ops;
            ops = nullptr;
            return str;
        }

        else if (opCount == 1) {
            int opI = ops[0];
            string l = str.substr(0, opI);
            string r = str.substr(opI + 1);
            delete[] ops;
            ops = nullptr;
            return binaryOperation(l, str[opI], r);
        }

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

            cout << "all blockOuts were no" << endl;
        }
    }

    else {
        // paras exist
        int leftParaI;
        for (int i = rightParaI; i >= 0; i--) {
            if (str[i] == '(') {
                leftParaI = i;
                break;
            }
        }

        string subExp = str.substr(leftParaI + 1, rightParaI - leftParaI - 1); // get substr inside paras
        subExp = eval(subExp);
        string leftStr = str.substr(0, leftParaI);
        string rightStr = str.substr(rightParaI + 1);
        string finalStr = leftStr + subExp + rightStr;
        
        return eval(finalStr);
    }

    return to_string(0xdeadbeef); // should return in an above block, not here.
    //0xdeadbeef is 3735928559 unsigned dec.
}

string multipleOpsBlock(int ops[], int opCount, int priority, const string& str) {
    for (int i = 0; i < opCount; i++) {
        int opI = ops[i];
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
        if (condition) {
            // find start of substr
            int ssStart;
            if (i == 0) {
                ssStart = 0;
            } else {
                ssStart = ops[i - 1] + 1;
            }
            //find end of substr
            int ssEnd; // last char, not length of substr
            if (i == opCount - 1) {
                ssEnd = str.length() - 1;
            } else {
                ssEnd = ops[i + 1] - 1;
            }

            string l = str.substr(ssStart, opI);
            string r = str.substr(opI + 1, ssEnd - opI); 

            string result = binaryOperation(l, str[opI], r);
            string lwhole = str.substr(0, ssStart);
            string rwhole = str.substr(ssEnd + 1, str.length() - ssEnd - 1);
            string newexp = lwhole + result + rwhole;
            delete[] ops;
            ops = nullptr;
            return eval(newexp);
        }
    }
    return "no";
}

inline void test(const string& str, int expected) {
    cout << "testing \"" + str + "\": ";
    int actual = stoi(eval(str));
    if (actual == expected) {
        cout << "Passed!" << endl;
    } else {
        cout << "Failed!" << endl;
    }
    cout << endl;
}

void testCases() {
    test("-7*(30/1+17%3)-3^2", (-7*((30/1)+(17%3)))-pow(3,2));
}

int main() {
    while (true) {
        cout << "Provide an expression to simplify, or enter 't' to run test cases:" << endl;
        string line;
        getline(cin, line);
        if (line == "t") {
            testCases();
            continue;
        }
        string result = eval(line);
        if (stoi(result) == 0xdeadbeef) {
            cout << "error, got deadbeef" << endl;
        } else {
            cout << result << endl;
        }
        cout << endl;
    }
    return 0;
}