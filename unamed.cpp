#include<iostream>
#include<string>
#include<vector>
using namespace std;
bool match(string& s, int i , string& p, int j,vector<vector<int>>& hash);
int simplePatternMatch(string s, string p){
	//直接使用深搜可能会超时
	//增加哈希表,存储计算过的节点，用空间换时间，避免重复计算
	//所有位置初始化为-1，表示没有找到过;0表示s[i]与p[j]匹配失败，1表示成功
	//i指向s，j指向p
	vector<vector<int>> hash(s.length()+1,vector<int>(p.length()+1,-1));
	for (int i=0;i<=s.length();i++){
		if(match(s,i,p,0,hash)) return i;
	}
	return -1;//  
} 
bool match(string& s, int i, string& p, int j, vector<vector<int>>& hash){
	// 
	if(hash[i][j]>=0) return hash[i][j];        
	if(j==p.length()){
		hash[i][j]=1;
		return true;// 终止条件	
	} 
	else if (i==s.length()){
		if (p[j]=='*'){
			bool temp =match(s,i,p,j+1,hash);
			hash[i][j] = temp;
			return temp;
		} 
		else{
			hash[i][j]=0;
			return false;
		}
	}
	else if (p[j]=='*'){
			bool skip_0 = match(s,i,p,j+1,hash);//*号匹配0个字符
			bool skip_1 = match(s,i+1,p,j,hash);//*号匹配一个字符，且j依然指向*。如果一个不行，下一个递归可以再跳过一个
			int temp =  skip_0||skip_1;
			hash[i][j]=temp;
			return temp;
			
	}
	else{//
		if(p[j]==s[i]){
			bool temp = match(s,i+1,p,j+1,hash);
			hash[i][j] = temp;
			return temp;
		} 
		else{
			hash[i][j]=0;
			return false;
		} 
	} 
}
int main(){
	cout << simplePatternMatch(
        "njkljkljjkj;laghiaghna;bi;fdhxjsfhgsdfjgkfgsjkghbjkafdnb;aojfdhnbuoafdhbabfdajkbha;jfksdhfkdsjgds;b;ifdhbg;idfhbgkhgbjklhnvjahbiopauiafbjkghfajkbganhlafukhsfasdfdasfgdefg",
        "h**********gh**na*******b********gsdfjgkfgs;***bu*******oafdhbab******ajkbha;jfks*********ifdhbg;idfhbg****nvjahbio***pauiafbjfuk**********hsfasdfdasf*"
    ) << endl;	
	return 0;
}
