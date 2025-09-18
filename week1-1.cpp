#include<iostream>
#include<stack>
#include<string>
#include<iomanip>
using namespace std;
int main(){
    string str;
    cin>>str;//初始化一个string类型的变量，存储用户输入
    stack<double> s;
    //初始化一个存储double的栈
    //之所以是double，是为了进行除法运算的时候不出错
    for (int i=0;i<str.length();i++){//表达式有数字和符号的区分
    //如果是数字，直接压栈；如果是符号，取出栈顶的两个元素计算
        if(str[i]>=97&&str[i]<=122){
            s.push(str[i]-96);
            //数字直接压栈。但是因为这里的数字是char类型，而且是小写字母，需要-96，转化成大于等于1的整数再压栈。
        }
        else if(str[i]=='+'){//处理加法
            double a = s.top();
            s.pop();//取出第一个数，并从栈中移除
            double b = s.top();
            s.pop();//取出第二个数，并从栈中移除
            s.push(a+b);//将两个数相加后重新压栈
        }
        else if(str[i]=='*'){
            double a = s.top();
            s.pop();//同上
            double b = s.top();
            s.pop();//同上
            s.push(a*b);//相乘后重新压栈
        }
        else if(str[i]=='-'){
            double a = s.top();
            s.pop();
            double b = s.top();
            s.pop();
            s.push(b-a);//注意操作数顺序
        }
        else if(str[i]=='/'){
            double a = s.top();
            s.pop();
            double b = s.top();
            s.pop();
            s.push(b/a);//注意操作数顺序
        }
    }
    cout<<fixed<<setprecision(2)<<s.top()<<endl;
    //栈内只有一个元素，直接将它输出
    return 0;
}