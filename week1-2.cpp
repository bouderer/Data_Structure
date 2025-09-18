//z2-中缀表达式转后缀表达
#include<iostream>
#include<string>
#include<stack>
using namespace std;
int main(){
    string str;
    stack<char> s;
    cin>>str;
    string result;
    for (int i=0; i<str.length();i++){
        if (str[i]>='A'&&str[i]<='Z' ||str[i]>='a'&&str[i]<='z' ){//直接输出
            result +=str[i];
        }
        else if(str[i]=='+'||str[i]=='-'){//+ -
            while((!s.empty())&&s.top()!='('){//非空且最上面不是左括号，这个时候需要把里面的符号压出来
                result+=s.top();
                s.pop();
            }
            s.push(str[i]);
        }
        else if(str[i]=='*'||str[i]=='/'){//*  /
            while((!s.empty())&&(s.top()=='*'||s.top()=='/')){//非空且最上面是*或者/，这个时候需要把里面的符号压出来
                result+=s.top();
                s.pop();
            }
            s.push(str[i]);
        }
        else if (str[i]=='('){
            s.push(str[i]);
        }
        else if (str[i]==')'){
            while(true){
                if (s.top()=='('){
                    s.pop();
                    break;
                }
                else{
                    result+=s.top();
                    s.pop();
                }
            }
        }
    }
    while(!s.empty()){
        result+=s.top();
        s.pop();
    }
    cout<<result<<endl;
    return 0;
}