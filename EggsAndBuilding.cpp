#include <iostream>
#include <iomanip>
using namespace std;

class EggDropTest{
    private:
        int critFloor;
        int highestFloor;
        
        int numTries;
        int eggsBroken;
        bool exactFloorTested;
        bool oneAboveTested;
        
    public:
        EggDropTest(){ // do not use this.
            numTries = 0;
            eggsBroken = 0;
            exactFloorTested = false;
            oneAboveTested = false;
        }
        EggDropTest(int cfloor, int tfloor){
            critFloor = cfloor;
            highestFloor = tfloor;
            numTries = 0;
            eggsBroken = 0;
            exactFloorTested = false;
            oneAboveTested = false;
        }
        bool testFloor(int fnum){
            // true if egg survives, false if egg breaks.
            numTries++;
            if(fnum <= critFloor){
                if(fnum==critFloor) exactFloorTested = true;
                return true;
            }
            else{
                if(fnum == critFloor + 1) oneAboveTested = true;
                eggsBroken++;
                return false;
            }
        }
        int getTries(){
            return numTries;
        }
        int getEggsBroken(){
            return eggsBroken;
        }
        bool floorConfirmed(){
            if(critFloor == 0){
                return oneAboveTested;
            }
            else if(critFloor==highestFloor){
                return exactFloorTested;
            }
            else{
                return (oneAboveTested && exactFloorTested);
            }
        }
        
        void setData(int cfloor, int tfloor){
            resetTries();
            critFloor = cfloor;
            highestFloor = tfloor;
        }
        void resetTries(){
            numTries = 0;
            eggsBroken = 0;
            exactFloorTested = false;
            oneAboveTested = false;
        }
};

/* Question 4 UPLOAD START */
// guaranteed at worst 300 floors, and at worst 10 eggs.


// 全局变量（严格对齐你的定义）
const int MAX_EGGS = 10;    // 最大鸡蛋数（1-10）
const int MAX_TRIES = 300;  // 最大测试次数（1-300）
const int MAX_FLOORS = 300; // 最大楼层数（1-300）

// dp[i][j]：i颗蛋、j次测试能覆盖的最大楼层数（你的核心定义）
int dp[MAX_EGGS + 1][MAX_TRIES + 1];

// next_k[i][j][r]：i颗蛋、j次测试、当前待测试范围长度r时，下一步最优测试楼层的偏移量
// r：当前待测试的楼层范围长度（如[1,36]的r=36，[9,36]的r=28）
int next_k[MAX_EGGS + 1][MAX_TRIES + 1][MAX_FLOORS + 1];

// 初始化dp和next_k数组
void initAnswers() {
    // 第一步：初始化dp数组（严格按你的定义）
    // 边界条件1：0颗蛋或0次测试，覆盖0层
    for (int i = 0; i <= MAX_EGGS; i++) dp[i][0] = 0;
    for (int j = 0; j <= MAX_TRIES; j++) dp[0][j] = 0;

    // 边界条件2：1颗蛋、j次测试，最多覆盖j层（只能逐层测）
    for (int j = 1; j <= MAX_TRIES; j++) {
        dp[1][j] = j;
        if (dp[1][j] > MAX_FLOORS) dp[1][j] = MAX_FLOORS + 1; // 超过300层标记为301
    }

    // 边界条件3：i颗蛋、1次测试，最多覆盖1层（只能测1层）
    for (int i = 1; i <= MAX_EGGS; i++) {
        dp[i][1] = 1;
    }

    // 动态规划填表：计算dp[i][j]（i≥2，j≥2）
    // 核心公式：dp[i][j] = dp[i-1][j-1] + dp[i][j-1] + 1
    // 公式含义：
    // 1. dp[i-1][j-1]：当前蛋碎了，用i-1颗蛋、j-1次测下方楼层
    // 2. dp[i][j-1]：当前蛋没碎，用i颗蛋、j-1次测上方楼层
    // 3. +1：当前测试的这一层
    for (int i = 2; i <= MAX_EGGS; i++) {
        for (int j = 2; j <= MAX_TRIES; j++) {
            dp[i][j] = dp[i-1][j-1] + dp[i][j-1] + 1;
            if (dp[i][j] > MAX_FLOORS) dp[i][j] = MAX_FLOORS + 1; // 超过300层标记
        }
    }

    // 第二步：初始化next_k数组（关键！按你的dp定义推导最优偏移量）
    // 遍历所有鸡蛋数、测试次数、待测试范围长度
    for (int i = 1; i <= MAX_EGGS; i++) {
        for (int j = 1; j <= MAX_TRIES; j++) {
            for (int r = 1; r <= MAX_FLOORS; r++) { // r：当前待测试范围长度
                if (r == 0) {
                    next_k[i][j][r] = 0;
                    continue;
                }

                // 情况1：1颗蛋，只能逐层测，偏移量为1（测当前范围的第1层）
                if (i == 1) {
                    next_k[i][j][r] = 1;
                    continue;
                }

                // 情况2：测试次数j=1，只能测当前范围的第1层（偏移量1）
                if (j == 1) {
                    next_k[i][j][r] = 1;
                    continue;
                }

                // 情况3：i≥2，j≥2，推导最优偏移量k
                // 最优k的原则：用当前1次测试，将范围r分成两部分，使两部分都能被剩余测试次数覆盖
                // 最优k = dp[i-1][j-1] + 1（下方最多能覆盖dp[i-1][j-1]层，所以测第k层）
                int optimal_k = dp[i-1][j-1] + 1;

                // 修正：k不能超过当前范围长度r（比如r=5，k不能是8）
                if (optimal_k > r) optimal_k = r;

                next_k[i][j][r] = optimal_k;
            }
        }
    }
}
int minTimesForBuilding(int eggs, int numFloors) {
    if (eggs <= 0 || numFloors <= 0) return 0;
    if (eggs > MAX_EGGS) eggs = MAX_EGGS;
    if (numFloors > MAX_FLOORS) numFloors = MAX_FLOORS;

    // 遍历测试次数j，找到最小的j使dp[eggs][j]≥numFloors
    for (int j = 1; j <= MAX_TRIES; j++) {
        if (dp[eggs][j] >= numFloors) {
            return j;
        }
    }
    return MAX_TRIES; // 最坏情况返回300次
}
// 计算下一步测试的绝对楼层
// 参数说明：
// totalFloors：总楼层数（题目限制≤300）
// eggsLeft：剩余鸡蛋数
// lastSurvival：上一次鸡蛋没碎的楼层（初始0）
// lastBreak：上一次鸡蛋碎了的楼层（初始totalFloors+1）
int nextTestFloor(int totalFloors, int eggsLeft, int lastSurvival, int lastBreak) {
    // 边界条件判断
    if (eggsLeft <= 0 || eggsLeft > MAX_EGGS) return -1;
    if (totalFloors <= 0 || totalFloors > MAX_FLOORS) return -1;
    if (lastSurvival >= lastBreak) return -1; // 范围已锁定，无需测试

    // 1. 计算当前待测试的范围长度r：[lastSurvival+1, lastBreak-1]的长度
    int current_range = lastBreak - lastSurvival - 1;
    if (current_range <= 0) return -1;

    // 2. 找到当前所需的最小测试次数j（即minTimesForBuilding的结果）
    
//	int required_tries = minTimesForBuilding(eggsLeft, totalFloors); 
    int required_tries = minTimesForBuilding(eggsLeft, current_range);

    // 3. 获取最优偏移量k（相对于当前范围的第k层）
    int k = next_k[eggsLeft][required_tries][current_range];

    // 4. 转换为绝对楼层：当前范围的第k层 = lastSurvival + k
    int next_floor = lastSurvival + k;

    // 5. 边界修正（确保在合法范围内）
    if (next_floor < lastSurvival + 1) next_floor = lastSurvival + 1;
    if (next_floor > lastBreak - 1) next_floor = lastBreak - 1;

    return next_floor;
}

// 计算i颗蛋、j次测试能覆盖的最大楼层数（直接返回dp[i][j]）
int canHandleFloors(int eggs, int numTries) {
    if (eggs <= 0 || numTries <= 0) return 0;
    if (eggs > MAX_EGGS) eggs = MAX_EGGS;
    if (numTries > MAX_TRIES) numTries = MAX_TRIES;
    return dp[eggs][numTries];
}

// 计算i颗蛋测f层楼所需的最小测试次数（找最小j使dp[i][j]≥f）



/* Question 4 UPLOAD END */

void minTimesTest(){
    for(int i=1;i<=10;i++){
        cout << right << setw(5) << i;
    }
    cout << endl;
    for(int j=1;j<=25;j++){
        for(int i=1;i<=10;i++)
            cout << right << setw(5) << minTimesForBuilding(i,j);
        cout << endl;
    }
    int testings[8] = {30,35,40,45,50,100,200,300};
    for(int j=0;j<8;j++){
        for(int i=1;i<=10;i++)
            cout << right << setw(5) << minTimesForBuilding(i,testings[j]);
        cout << endl;
    }
    cout << endl;
}

void handleFloorTest(){
    cout << right << setw(5) << " ";
    for(int i=1;i<=10;i++){
        cout << right << setw(5) << i;
    }
    cout << endl;
    for(int i=1;i<=8;i++){
        cout << right << setw(5) << i;
        for(int j=1;j<=10;j++){
            cout << right << setw(5) << canHandleFloors(j,i);
        }
        cout << endl;
    }
    cout << endl;
}

void setUpCase(int eggCount, int cFloor, int hFloor, int target){
    int lastSurvival = 0;
    int lastBreak = hFloor+1;
    int nextTest = 0;
    int eggsLeft = eggCount;
    EggDropTest edt(cFloor,hFloor);
    while(nextTest!=-1){
        nextTest = nextTestFloor(hFloor,eggsLeft,lastSurvival,lastBreak);
        if(nextTest==-1) break;
        if(edt.testFloor(nextTest)){
            lastSurvival = nextTest;
        }
        else{
            eggsLeft--;
            lastBreak = nextTest;
            if(eggsLeft==0) break;
        }
    }
    if(edt.floorConfirmed() && edt.getTries()<= target){
        cout << "Good" << endl;
    }
    else{
        cout << "Bad" << endl;
    }
}

void specificTests(int i){
    int floors[6] = {0,20,40,60,80,100};
    int counts[11] = {0,0,14,9,8,7,7,7,7,7,7};
    switch(i){
        case 10:
        case 9:
        case 8:
        case 7:
        case 6:
        case 5:
        case 4:
        case 3:
        case 2:
            for(int j=0;j<6;j++)
                setUpCase(i,floors[j],100,counts[i]);
            break;
        case 1: // reality check
            setUpCase(1,0,100,1);
            setUpCase(1,100,100,100);
            setUpCase(1,50,300,51);
            setUpCase(1,89,200,90);
        default:
            break;
    }
}

int main(){
    int testNum = 0;
    initAnswers();
    cin >> testNum;
    switch(testNum){
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            specificTests(testNum);
            break;
        default:
            minTimesTest();
            handleFloorTest();
    }    
    return 0;
}
