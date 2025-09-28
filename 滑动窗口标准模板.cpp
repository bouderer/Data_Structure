#include<iostream>
#include<vector>
using namespace std;
int main() {
	long long n,m;
	cin>>n>>m;
	vector<long long> num(n);
	for(int i=0; i<n; i++) cin>>num[i];
	long long head=0,tail=0;
	long long max_length=0;
	long long sum=0;
	while(tail<n) {
		sum+=num[tail];//注意要在这里就直接更新窗口长度
		while(sum>m&&head<=tail) {
			sum-=num[head];
			head++;
		}//直接就要找到符合条件的最大长度
		if (tail-head+1>max_length) max_length = tail-head+1;
		tail++;
	}
	cout<<max_length<<endl;
	return 0;
}
/*滑动窗口模板*/
/*
先向右更新，然后直接移动头指针找到最大长度
其中head要++
最后tail也要++
*/