#include <iostream>
#include <string>
using namespace std;

// 方向数组：上、右、下、左
const int dx[] = {-1, 0, 1, 0};
const int dy[] = {0, 1, 0, -1};

struct AkariPuzzle {
    int rows, cols;
    char grid[50][50];    // 假设最大尺寸为50x50（可根据需求调整）
    int capable[50][50]; // 标记数字格子上下左右的四个方位是否可以放置灯泡
    int LitCnt[50][50];   // 标记格子被几盏灯点亮
    bool hasLight[50][50];// 标记格子是否放置了灯泡
    bool solved;          // 标记是否找到解
    int Count;     // 记录已经放置的灯泡数量
    int WhiteGrids;   // 记录需要放置的灯泡数量
 
    AkariPuzzle(int r, int c, const string& input) : rows(r), cols(c), solved(false) {
        // 初始化数组
        WhiteGrids = 0;
        Count = 0;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                grid[i][j] = input[i * cols + j];
                capable[i][j]=0;//有数字的格子，如果最后符合条件，也是0.所以初始化为0，最后好比较
                if(grid[i][j]=='.'){
                    WhiteGrids++;
                } 
                if(grid[i][j]>='0'&& grid[i][j]<='4'){
                    capable[i][j]=grid[i][j]-'0';
                }
                LitCnt[i][j] = 0;
                hasLight[i][j] = false;
            }
        }
    }

    // 检查在(x,y)放置灯泡是否合法
    bool isValid(int x, int y) {
        if (grid[x][y] != '.') return false;
        if (LitCnt[x][y]) return false;
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            //添加两行代码，查找当前上下左右有没有数字格子，如果有数字格子且capable<=0，说明不能放灯
            if(nx<0 || nx>=rows || ny<0 || ny>=cols) continue;
            if(grid[nx][ny]>='0'&&grid[nx][ny]<='4' && capable[nx][ny]<=0) return false;
            while (nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
                if (grid[nx][ny] != '.') break;
                if (hasLight[nx][ny]) return false;
                nx += dx[d];
                ny += dy[d];
            }
        }
        return true;
    }

    // 放置灯泡后，更新照亮区域
    void placeLight(int x, int y) {
        for(int d=0;d<4;++d){
            int nx=x+dx[d];
            int ny=y+dy[d];
            if(nx<0 || nx>=rows || ny<0 || ny>=cols) continue;
            if(grid[nx][ny]>='0'&&grid[nx][ny]<='4'){
                capable[nx][ny]--;//capable一定大于0才能运行这个函数。
            }
        }//如果能运行这个函数，证明至少能放灯。寻找这个位置四周有没有数字，有的话数字-1
        hasLight[x][y] = true;
        LitCnt[x][y]++;
        if(LitCnt[x][y]==1) Count++;
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            while (nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
                if (grid[nx][ny] != '.') break;
                LitCnt[nx][ny]++;
                if(LitCnt[nx][ny] == 1) {
                    Count++;
                }
                nx += dx[d];
                ny += dy[d];
            }
        }
    }

    // 移除灯泡及对应的照亮区域
    void removeLight(int x, int y) {
        for(int d=0;d<4;++d){
            int nx=x+dx[d];
            int ny=y+dy[d];
            if(nx<0 || nx>=rows || ny<0 || ny>=cols) continue;
            if(grid[nx][ny]>='0'&&grid[nx][ny]<='4'&& capable[nx][ny]<(grid[nx][ny]-'0')){
                capable[nx][ny]++;
            }
        }//找附近有没有数字。有的话对应位置的capable+1.
        hasLight[x][y] = false;
        LitCnt[x][y]--;
        if(LitCnt[x][y]==0) Count--;
        // 这盏灯找的的所有地方LitCnt-1.
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            while (nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
                if (grid[nx][ny] != '.') break;
                LitCnt[nx][ny]--;
                if(LitCnt[nx][ny] == 0) {
                    Count--;
                }
                nx += dx[d];
                ny += dy[d];
            }
        }
    }

    // 检查黑格数字约束是否满足
    bool checkBlackCells() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if(capable[i][j] != 0) return false;
            }
        }
        return true;
    }

    // 检查是否所有空白格都被照亮
    bool allLit() {
        return Count == WhiteGrids;
    }

    // 回溯法求解
    void backtrack(int x, int y) {
        if (solved) return;
        if (allLit() && checkBlackCells()) {
            solved = true;
            return;
        }
        for (int i = x; i < rows; ++i) {
            for (int j = (i == x ? y : 0); j < cols; ++j) {
                if (grid[i][j] == '.' && !hasLight[i][j] && LitCnt[i][j]==0) {
                    if (isValid(i, j)) {
                        placeLight(i, j);
                        backtrack(i, j);
                        if (solved) return;
                        removeLight(i, j);
                    }
                }
            }
        }
    }

    // 输出结果
    void printResult() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (hasLight[i][j]) {
                    cout << 'L';
                } else {
                    cout << grid[i][j];
                }
            }
        }
        cout << endl;
    }
};

int main() {
    int rows, cols;
    cin >> rows >> cols;
    string input;
    cin >> input;
    AkariPuzzle puzzle(rows, cols, input);
    puzzle.backtrack(0, 0);
    puzzle.printResult();
    return 0;
}

