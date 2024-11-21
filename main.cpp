#include <iostream>
#include <stack>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

float performOperation(char operation, float number1, float number2) {
    if (operation == '+') {
        return number1 + number2;
    }

    if (operation == '-') {
        return number1 - number2;
    }

    if (operation == '*') {
        return number1 * number2;
    }

    if (operation == '/') {
        if (number2 == 0) {
            cout << "nan" << endl;
            exit(-1);
        }
        return number1 / number2;
    }

    // this could potentially cause a bug, but technically this shouldn't be a problem due to prior error checking
    return -1;
}

// I wasn't sure exactly how to implement precedence, so I provided chatGPT with my code and asked to provide ideas
// While this was not the only part that it provided (also the precedence procedure in the for loop at calculate function), it did work correctly
int precedence(char op) {
    if (op == '+' || op == '-') {
        return 1;
    }

    if (op == '*' || op == '/') {
        return 2;
    }
    return 0;
}

float calculateExpression(string input) {
    // I decided to use 2 stacks one for holding values/subvalues and other for operator symbols to keep up the track for relevant operations
    stack<float> values;
    stack<char> operatorStack;

    unordered_map<char, bool> numbers = {
        {'1', true}, {'2', true}, {'3', true},
        {'4', true}, {'5', true}, {'6', true},
        {'7', true}, {'8', true}, {'9', true},
        {'0', true}
    };

    unordered_map<char, bool> operators = {
        {'+', true}, {'-', true}, {'*', true}, {'/', true},
    };

    for (int i = 0; i < input.length(); i++) {
        if (isspace(input[i])) continue; // Ignore all whitespace

        if (numbers[input[i]]) {
            // calculates the current number value till the next operator
            float value = 0;
            while (i < input.length() && numbers[input[i]]) {
                value = (value * 10) + (input[i] - '0');
                i++;
            }
            values.push(value);
            i--; // Step back to handle the next character

        } else if (input[i] == '(') {
            operatorStack.push(input[i]);
        } else if (input[i] == ')') {
            // when a closing parenthesis is found, the loop evaluates all operators until an opening parenthesis is found
            while (!operatorStack.empty() && operatorStack.top() != '(') {
                float number2 = values.top();
                values.pop();

                float number1 = values.top();
                values.pop();

                char op = operatorStack.top();
                operatorStack.pop();
                values.push(performOperation(op, number1, number2));
            }

            if (!operatorStack.empty() && operatorStack.top() == '(') {
                operatorStack.pop();
            } else {
                cout << "Error: Unmatched parenthesis" << endl;
                return -1;
            }

        } else if (operators[input[i]]) {
            if (i > 0 && (operators[input[i - 1]] || input[i - 1] == '(')) {
                cout << "Error: Invalid operator sequence" << endl;
                return -1;
            }

            // If operator precedence is correct, then at least 2 values are popped from the stack and their value is calculated after which it is pushed back
            // This is to calculate the expression value up until this point
            while (!operatorStack.empty() && precedence(operatorStack.top()) >= precedence(input[i])) {
                float number2 = values.top();
                values.pop();

                float number1 = values.top();
                values.pop();

                char op = operatorStack.top();
                operatorStack.pop();
                values.push(performOperation(op, number1, number2));
            }
            operatorStack.push(input[i]);

        } else {
            cout << "error" << endl;
            return -1;
        }
    }

    // Doing final value calculations
    while (!operatorStack.empty()) {
        if (operatorStack.top() == '(') {
            cout << "Error: Unmatched parenthesis" << endl;
            return -1;
        }

        float number2 = values.top();
        values.pop();

        float number1 = values.top();
        values.pop();

        char op = operatorStack.top();
        operatorStack.pop();
        values.push(performOperation(op, number1, number2));
    }

    return values.top();
}

//Asked chatGPT to trim text, and read everything as a string
string readAndStripText(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filePath << endl;
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string text = buffer.str();

    // Remove spaces and newline characters
    text.erase(remove_if(text.begin(), text.end(), [](char c) {
        return isspace(c);
    }), text.end());

    return text;
}

int main() {
    string filePath = "C:\\path\\to\\project\\sequence_with_parenthesis.txt"; // Replace with your file path
    // Result for sequence with paranthesis is 227
    string strippedText = readAndStripText(filePath);
    float result = calculateExpression(strippedText);
    cout << result << endl;
}
