
//z5-最大值最小化
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;
typedef long long ll;
int main() {
    ll n, m;
    while(cin>>n) {
        cin >> m;
        vector<ll> num;
        ll sum = 0;
        ll max = -1;
        for (ll i = 0; i < n; i++) {
            ll a;
            cin >> a;
            sum += a;
            num.push_back(a);
            if(a>=max) max=a;
        }
        ll bottom = max, top = sum;
        ll x = (bottom + top) / 2;
        while (bottom < top) {
            // 求得区间最大值
            ll sum = 0;
            ll times = m - 1; // 可以划几根棒子分割
            for (ll j = 0; j < n && times >= 0;) {
                if (sum + num[j] <= x) {
                    sum += num[j];
                    j++;
                } else {
                    sum = 0;
                    times--;
                }
            }
            if (times >= 0) { // 当前x太大
                top = x;
            } else {
                bottom = x + 1;
            }
            x = (bottom + top) / 2;
        }
        cout << top << endl;
    }
    return 0;
}