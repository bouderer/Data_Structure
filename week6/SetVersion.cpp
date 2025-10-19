//z6-明明的随机数
#include <iostream>
#include <set>
using namespace std;
int main() {
    int N;
    while (cin >> N) {
        set<int> s;
        for (int i = 0; i < N; i++) {
            int m;
            cin >> m;
            s.insert(m);
        }
        cout << s.size()<<endl;
        for (auto val : s) { cout << val << " "; }
    }
    return 0;
}