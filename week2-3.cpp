//z2-Bracket Matching
#include <iostream>
#include <stack>
using namespace std;
int main() {
    string line;
    cin >> line;
    stack<char> s;
    for (int i = 0; i < line.length(); i++) {
        if (line[i] == '(' || line[i] == '{' || line[i] == '[') {
            s.push(line[i]);
        } else if (line[i] == ')' || line[i] == '}' || line[i] == ']') {
            if(s.empty()){
                cout<<"No"<<endl;
                return 0;
            }
            else if (line[i] == ')') {
                if ('(' == s.top()) {
                    s.pop();
                } else {
                    cout << "No" << endl;
                    return 0;
                }
            }
            else if (line[i] == ']') {
                if ('[' == s.top()) {
                    s.pop();
                } else {
                    cout << "No" << endl;
                    return 0;
                }
            }
            else{
                if ('{' == s.top()) {
                    s.pop();
                } else {
                    cout << "No" << endl;
                    return 0;
                }
            }
        }
    }
    if (s.empty()) cout << "Yes" << endl;
    else cout<<"No"<<endl;
    return 0;
}