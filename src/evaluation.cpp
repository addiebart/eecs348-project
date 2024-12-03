#include <iostream>
#include <string>

using namespace std;

// takes a lexar-passed string, no errors
string parse(const string& input) {

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
        // apply pemdas and place back into str
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
        subExp = parse(subExp);
        string leftStr = instr.substr(0, leftParaI);
        string rightStr = instr.substr(rightParaI + 1);
        string finalStr = leftStr + subExp + rightStr;
    }
}

int main() {

}