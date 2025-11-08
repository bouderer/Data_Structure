//z8-爸爸去哪儿之一
#include <iostream>
#include <queue>
#include<vector>
using namespace std;
class student {
  private:
    int id = 0;
    int sum = 0;
    int chinese = 0;
    int math = 0;
    int english = 0;

  public:
    student() {}
    student(int id, int chinese, int math, int english)
        : id(id), chinese(chinese), math(math), english(english) {
        sum = chinese + math + english;
    }
    bool operator>(const student &other) const{ //>表示更优
        if (this->sum != other.sum)
            return this->sum > other.sum;
        if (this->chinese != other.chinese)
            return this->chinese > other.chinese;
        return this->id < other.id;
    }
    friend ostream &operator<<(ostream &os, const student a) {
        os << a.id << " " << a.sum;
        return os;
    }
};
class worsefirst{
    public:
    bool operator()(const student& a, const student& b)const{
        return a>b;
    }
};
int main() {
    int n;
    while (cin >> n) {
        priority_queue<student,vector<student>,worsefirst> heap;
        for (int i = 0; i < n; i++) {
            int a, b, c;
            cin >> a >> b >> c;
            heap.push(student(i+1,a,b,c));
            if(heap.size()>5) heap.pop();
        }
        vector<student> result(5);
        for (int i = 4; i > -1; i--) {
            result[i] = heap.top();
            heap.pop();
        }
        for (int i = 0; i < 5; i++) {
            cout << result[i] << endl;
        }
        cout<<endl;
    }
}