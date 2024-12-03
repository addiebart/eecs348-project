#include <iostream>
#include <string>
#include <cmath>

using namespace std;

bool isNumeric(const char& c) {
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

// takes a lexar-passed string, no errors, and has been marked with paras
int eval(const string& input) {

    string instr = input;

    // remove spaces
    for (int i = 0; i < instr.length(); i++) {
        if (instr[i] == ' ') {
            instr.erase(i--, 1); // remove space
        }
    }

    int rightParaI = -1;
    for (int i = 0; i < instr.length(); i++) {
        if (instr[i] == ')') {
            rightParaI = i;
            break;
        }
    } 

    if (rightParaI < 0) {
        //eval expression with two operands
        int operandI;
        for (int i = 0; i < instr.length(); i++) {
            if (!isNumeric(instr[i])) {
                operandI = i;
            }
        }
        char operand = instr[operandI];
        int leftNum = stoi(instr.substr(0, operandI));
        int rightNum = stoi(instr.substr(operandI + 1));
        int result;
        switch (operand) {
            case '^':
                result = pow(leftNum, rightNum);
                break;
            case '*':
                result = leftNum * rightNum;
                break;
            case '/':
                result = leftNum / rightNum;
                break;
            case '%':
                result = leftNum % rightNum;
                break;
            case '+':
                result = leftNum + rightNum;
                break;
            case '-':
                result = leftNum - rightNum;
                break;
            default:
                cout << "illegal operator" << endl;
        }

        return result;
    }

    else {
        // existing paras remain
        // find left para
        int leftParaI;
        for (int i = rightParaI; i >= 0; i--) {
            if (instr[i] == '(') {
                leftParaI = i;
                break;
            }
        }

        string subExp = instr.substr(leftParaI + 1, rightParaI - leftParaI - 1); // get substr inside paras
        subExp = eval(subExp);
        string leftStr = instr.substr(0, leftParaI);
        string rightStr = instr.substr(rightParaI + 1);
        string finalStr = leftStr + subExp + rightStr;
        return eval(finalStr);
    }
}

int main() {
    // prompt user and stuff
}