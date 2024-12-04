#include <iostream>
#include <string>
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

string cleanForParsing(const string& input) {
    string instr = input;

    // remove spaces
    for (int i = 0; i < instr.length(); i++) {
        if (instr[i] == ' ') {
            instr.erase(i--, 1); // remove space
        }
    }

    // add implied multiplication
    for (int i = 0; i < instr.length(); i++) {
        if (instr[i] == '(' && i > 0 && isNumeric(instr[i-1])) {
            instr.insert(i++, "*");
            continue;
        }
        if (instr[i] == ')' && i < instr.length()-1 && isNumeric(instr[i-1])) {
            instr.insert(++i, "*");
        }
    }

    return instr;
}

int main() {
    /*
    cout << "Enter an expression" << endl;
    string instr;
    getline(cin, instr);
    */
    string instr = "8 ^ (1+ 2) /22%8" // should be 
    string instr = cleanForParsing(instr);
    cout << instr << endl;
}