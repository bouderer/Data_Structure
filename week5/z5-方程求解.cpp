//z5-方程求解
#include<iostream>
#include<cmath>
#include"solve.h"
using namespace std;
long double solve(long double y){
    long double bottom = 0.5;
    long double top =1;
    while(exp(top)+log(top)-1-y<0){
        top*=2;
    }
    long double mid =(bottom + top)/2;
    while(fabsl(exp(mid)+log(mid)-1-y)>=1e-6){
        if(exp(mid)+log(mid)-1-y>0){
            top =mid;
        }
        else {
            bottom = mid;
        }
        mid = (top+bottom)/2;
    }
    return mid;
}