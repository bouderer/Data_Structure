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


// ȫ ֱ      ϸ      Ķ  壩
const int MAX_EGGS = 10;    //   󼦵     1-10  
const int MAX_TRIES = 300;  //      Դ     1-300  
const int MAX_FLOORS = 300; //    ¥      1-300  

// dp[i][j]  i ŵ   j β    ܸ  ǵ    ¥        ĺ  Ķ  壩
int dp[MAX_EGGS + 1][MAX_TRIES + 1];

// next_k[i][j][r]  i ŵ   j β  ԡ   ǰ     Է Χ    rʱ    һ     Ų   ¥   ƫ    
// r    ǰ     Ե ¥ 㷶Χ   ȣ   [1,36]  r=36  [9,36]  r=28  
int next_k[MAX_EGGS + 1][MAX_TRIES + 1][MAX_FLOORS + 1];

//   ʼ  dp  next_k    
void initAnswers() {
    //   һ      ʼ  dp   飨 ϸ   Ķ  壩
    //  ߽     1  0 ŵ   0 β  ԣ     0  
    for (int i = 0; i <= MAX_EGGS; i++) dp[i][0] = 0;
    for (int j = 0; j <= MAX_TRIES; j++) dp[0][j] = 0;

    //  ߽     2  1 ŵ   j β  ԣ   า  j 㣨ֻ     ⣩
    for (int j = 1; j <= MAX_TRIES; j++) {
        dp[1][j] = j;
        if (dp[1][j] > MAX_FLOORS) dp[1][j] = MAX_FLOORS + 1; //     300    Ϊ301
    }

    //  ߽     3  i ŵ   1 β  ԣ   า  1 㣨ֻ ܲ 1 㣩
    for (int i = 1; i <= MAX_EGGS; i++) {
        dp[i][1] = 1;
    }

    //   ̬ 滮        dp[i][j]  i  2  j  2  
    //    Ĺ ʽ  dp[i][j] = dp[i-1][j-1] + dp[i][j-1] + 1
    //   ʽ   壺
    // 1. dp[i-1][j-1]    ǰ     ˣ   i-1 ŵ   j-1 β  · ¥  
    // 2. dp[i][j-1]    ǰ  û 飬  i ŵ   j-1 β  Ϸ ¥  
    // 3. +1    ǰ   Ե   һ  
    for (int i = 2; i <= MAX_EGGS; i++) {
        for (int j = 2; j <= MAX_TRIES; j++) {
            dp[i][j] = dp[i-1][j-1] + dp[i][j-1] + 1;
            if (dp[i][j] > MAX_FLOORS) dp[i][j] = MAX_FLOORS + 1; //     300    
        }
    }

    //  ڶ       ʼ  next_k   飨 ؼ        dp     Ƶ     ƫ      
    //        м          Դ          Է Χ    
    for (int i = 1; i <= MAX_EGGS; i++) {
        for (int j = 1; j <= MAX_TRIES; j++) {
            for (int r = 1; r <= MAX_FLOORS; r++) { // r    ǰ     Է Χ    
                if (r == 0) {
                    next_k[i][j][r] = 0;
                    continue;
                }

                //    1  1 ŵ   ֻ     ⣬ƫ    Ϊ1   ⵱ǰ  Χ ĵ 1 㣩
                if (i == 1) {
                    next_k[i][j][r] = 1;
                    continue;
                }

                //    2     Դ   j=1  ֻ ܲ⵱ǰ  Χ ĵ 1 㣨ƫ    1  
                if (j == 1) {
                    next_k[i][j][r] = 1;
                    continue;
                }

                //    3  i  2  j  2   Ƶ     ƫ    k
                //     k  ԭ   õ ǰ1 β  ԣ     Χr ֳ      ֣ ʹ     ֶ  ܱ ʣ    Դ       
                //     k = dp[i-1][j-1] + 1   ·     ܸ   dp[i-1][j-1] 㣬   Բ  k 㣩
                int optimal_k = dp[i-1][j-1] + 1;

                //       k   ܳ     ǰ  Χ    r      r=5  k      8  
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

    //        Դ   j   ҵ   С  jʹdp[eggs][j]  numFloors
    for (int j = 1; j <= MAX_TRIES; j++) {
        if (dp[eggs][j] >= numFloors) {
            return j;
        }
    }
    return MAX_TRIES; //         300  
}
//       һ     Եľ   ¥  
//     ˵    
// totalFloors    ¥        Ŀ   ơ 300  
// eggsLeft  ʣ ༦    
// lastSurvival    һ μ   û   ¥ 㣨  ʼ0  
// lastBreak    һ μ      ˵ ¥ 㣨  ʼtotalFloors+1  
int nextTestFloor(int totalFloors, int eggsLeft, int lastSurvival, int lastBreak) {
    //  ߽      ж 
    if (eggsLeft <= 0 || eggsLeft > MAX_EGGS) return -1;
    if (totalFloors <= 0 || totalFloors > MAX_FLOORS) return -1;
    if (lastSurvival >= lastBreak) return -1; //   Χ               

    // 1.    㵱ǰ     Եķ Χ    r  [lastSurvival+1, lastBreak-1] ĳ   
    int current_range = lastBreak - lastSurvival - 1;
    if (current_range <= 0) return -1;

    // 2.  ҵ   ǰ       С   Դ   j    minTimesForBuilding Ľ    
    int required_tries = minTimesForBuilding(eggsLeft, totalFloors);

    // 3.   ȡ    ƫ    k      ڵ ǰ  Χ ĵ k 㣩
    int k = next_k[eggsLeft][required_tries][current_range];

    // 4. ת  Ϊ    ¥ 㣺  ǰ  Χ ĵ k   = lastSurvival + k
    int next_floor = lastSurvival + k;

    // 5.  ߽       ȷ   ںϷ   Χ ڣ 
    if (next_floor < lastSurvival + 1) next_floor = lastSurvival + 1;
    if (next_floor > lastBreak - 1) next_floor = lastBreak - 1;

    return next_floor;
}

//     i ŵ   j β    ܸ  ǵ    ¥      ֱ ӷ   dp[i][j]  
int canHandleFloors(int eggs, int numTries) {
    if (eggs <= 0 || numTries <= 0) return 0;
    if (eggs > MAX_EGGS) eggs = MAX_EGGS;
    if (numTries > MAX_TRIES) numTries = MAX_TRIES;
    return dp[eggs][numTries];
}

//     i ŵ   f  ¥       С   Դ         Сjʹdp[i][j]  f  



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
