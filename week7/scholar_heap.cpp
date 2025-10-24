#include <iostream>
#include <vector>
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
    student (const student& other)=default;
    student &operator=(const student &other) =default;
    bool operator>(const student &other) { //>表示更优
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
void studentswap(student &a, student &b) {
    student temp = a;
    a = b;
    b = temp;
}
// 0 1 2 3 4 5     6
void siftup(vector<student> &heap) { // size=idx+1
    int size=heap.size();
    int idx = size - 1;
    while (idx > 0) {
        int father = (idx - 1) / 2; // father(idx)
        if (heap[father] > heap[idx]) {
            studentswap(heap[father], heap[idx]);
            idx = father;
        }else break;
    }
}
void siftdown(vector<student> &heap) {
    int size=heap.size();
    int idx=0;
    while(idx<=size/2-1){
        int left=idx*2+1;
        int right=left+1;
        int worse = right<size&&heap[left]>heap[right]? right:left;
        if(heap[idx]>heap[worse]){
            studentswap(heap[idx],heap[worse]);
            idx=worse;
        }
        else break;
    }
}
student poptop(vector<student> &heap) {
    int size=heap.size();
    student temp = heap[0];
    heap[0] = heap[size - 1];
    heap.pop_back();
    siftdown(heap);
    return temp;
}
void insert(vector<student> &heap, const student &a) {
    // 维护小根堆
    heap.push_back(a);
    siftup(heap);
    if (heap.size() == 6) {
        // pop
        poptop(heap);
    }
}

int main() {
    int n;
    while (cin >> n) {
        vector<student> heap;
        for (int i = 0; i < n; i++) {
            int a, b, c;
            cin >> a >> b >> c;
            insert(heap, student(i + 1, a, b, c));
        }
        vector<student> result(5);
        for (int i = 4; i > -1; i--) {
            result[i] = poptop(heap);
        }
        for (int i = 0; i < 5; i++) {
            cout << result[i] << endl;
        }
        cout<<endl;
    }
}