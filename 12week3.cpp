#include <iostream>
#include <iomanip>
#include <cstring>
#include <string>
using namespace std;


template <class T>
struct Node{
    T data;
    Node<T> *next;
};


template <class T>
class VecList{
private:
    int capacity;
    int length;
    T* arr;
    void doubleListSize(){
        T * oldArr = arr;
        arr = new T[2*capacity];
        capacity = 2 * capacity;
        for(int i=0;i<length;i++){
            arr[i] = oldArr[i];
        }
        delete [] oldArr;
    }
public:
    VecList(){
        length = 0;
        capacity = 100;
        arr = new T[capacity];
    }
    VecList(T* a, int n){
        length = n;
        capacity = 100 + 2*n;
        arr = new T[capacity];
        for(int i=0;i<n;i++){
            arr[i] = a[i];
        }
    }
    ~VecList(){
        delete [] arr;
    }
    int getLength(){
        return length;
    }
    bool isEmpty(){
        return length==0;
    }
    void insertEleAtPos(int i, T x){
        if(length==capacity)
            doubleListSize();
        if(i > length || i < 0)
            throw "Illegal position";
        for(int j=length;j>i;j--)
            arr[j] = arr[j-1];
        arr[i] = x;
        length++;
    }
    T deleteEleAtPos(int i){
        if(i >= length || i < 0)
            throw "Illegal position";
        T tmp = arr[i];
        for(int j=i;j<length-1;j++)
            arr[j] = arr[j+1];
        length--;
        return tmp;
    }
    void setEleAtPos(int i, T x){
        if(i >= length || i < 0)
            throw "Illegal position";
        arr[i] = x;
    }
    T getEleAtPos(int i){
        if(i >= length || i < 0)
            throw "Illegal position";
        return arr[i];
    }
    int locateEle(T x){
        for(int i=0;i<length;i++){
            if(arr[i]==x)
                return i;
        }
        return -1;
    }
    void insertLast(T x){
        insertEleAtPos(length,x);
    }
    void printList(){
        for(int i=0;i<length;i++)
            cout << arr[i] << " ";
    }
};


template <class T>
struct DNode{
    T data;
    DNode<T>* next;
};

template <class T>
class LinkStack{
private:
    DNode<T>* top;
    int length;
public:
    LinkStack(){
        top = NULL;
        length = 0;
    }
    ~LinkStack(){
        while(top!=NULL){
            DNode<T>* tmp = top;
            top = top->next;
            delete tmp;
        }
    }
    int getLength(){
        return length;
    }
    bool isEmpty(){
        return length==0;
    }
    void push(T x){
        DNode<T>* tmp = new DNode<T>;
        tmp->data = x;
        tmp->next = top;
        top = tmp;
        length++;
    }
    T pop(){
        if(length==0) throw "Stack Empty!";
        DNode<T>* tmp = top;
        top = top->next;
        T tmpData = tmp->data;
        delete tmp;
        length--;
        return tmpData;
    }
    T getTop(){
        if(length==0) throw "Stack Empty!";
        return top->data;
    }
    void printStack(){
        cout << "Stack top: ";
        DNode<T>* tmp = top;
        while(tmp!=NULL){
            cout << tmp->data << " ";
            tmp = tmp->next;
        }
        cout << ":stack bottom" << endl;
    }
};


template <class T>
class LinkQueue{
private:
    DNode<T>* front;
    DNode<T>* back;
    int length;
public:
    LinkQueue(){
        front = new DNode<T>;
        front->next = NULL;
        back = front;
        length = 0;
    }
    ~LinkQueue(){
        while(front!=NULL){
            back = front;
            front = front->next;
            delete back;
        }
    }
    int getLength(){
        return length;
    }
    bool isEmpty(){
        return length==0;
    }
    void enQueue(T x){
        DNode<T>* tmpN = new DNode<T>;
        tmpN->data = x;
        tmpN->next = NULL;
        back->next = tmpN;
        back = tmpN;
        length++;
    }
    T deQueue(){
        if(length==0) throw "Queue Empty!";
        DNode<T>* tmpN = front->next;
        front->next = tmpN->next;
        T tmpD = tmpN->data;
        delete tmpN;
        length--;
        if(length==0) back = front;
        return tmpD;
    }
    T peekQueue(){
        if(length==0) throw "Queue Empty!";
        return front->next->data;
    }
    void printQueue(){
        cout << "Front of queue: ";
        DNode<T>* tmp = front->next;
        while(tmp!=NULL){
            cout << tmp->data << " ";
            tmp = tmp->next;
        }
        cout << ": back of queue" << endl;
    }
};


template <class T>
struct Edge{
    T start;
    T end;
    int weight;
};


template <class T>
class AMGraph{
private:
    int numVer, numEdge;
    VecList<T> verList; 
    int** adjMatrix;
    bool directed;


    void BFShelper(int st, bool* visited){
        visited[st] = true;
        cout << verList.getEleAtPos(st) << " ";
        LinkQueue<int> q;
        q.enQueue(st);
        while(!q.isEmpty()){
            int tmp = q.deQueue();
            for(int k=0;k<numVer;k++){
                if(adjMatrix[tmp][k]==0) continue;
                if(visited[k]) continue;
                visited[k] = true;
                cout << verList.getEleAtPos(k) << " ";
                q.enQueue(k);
            }
        }
    }

    void DFShelper(int st, bool* visited){
        visited[st] = true;
        cout << verList.getEleAtPos(st) << " ";
        for(int k=0;k<numVer;k++){
            if(adjMatrix[st][k]==0) continue;
            if(visited[k]) continue;
            DFShelper(k,visited);
        }
    }

public:
    AMGraph(){} 


    AMGraph(T* arr, int n, bool dir=false){
        numVer = n;
        numEdge = 0;
        directed = dir;
        for(int i=0;i<n;i++){
            verList.insertLast(arr[i]);
        }

    
        adjMatrix = new int*[n];
        for(int i=0;i<n;i++){
            adjMatrix[i] = new int[n];
            for(int j=0;j<n;j++)
                adjMatrix[i][j] = 0;
        }
    }

    
    AMGraph(T* arr, int n, Edge<T>** eArr, int e,bool dir=false){
        numVer = n;
        numEdge = 0;
        for(int i=0;i<n;i++){
            verList.insertLast(arr[i]);
        }
        directed = dir;

        adjMatrix = new int*[n];
        for(int i=0;i<n;i++){
            adjMatrix[i] = new int[n];
            for(int j=0;j<n;j++)
                adjMatrix[i][j] = 0;
        }

        for(int i=0;i<e;i++){
            addEdge(eArr[i]->start,eArr[i]->end,eArr[i]->weight);
        }
    }

    ~AMGraph(){
        for(int i=0;i<numVer;i++){
            delete [] adjMatrix[i];
        }
        delete [] adjMatrix;
    }

    
    void addEdge(Edge<T> e){
        addEdge(e.start,e.end,e.weight);
    }

    void addEdge(T st, T en, int w = 1){
        int sIndex = verList.locateEle(st);
        int eIndex = verList.locateEle(en);
        if(sIndex == -1 || eIndex == -1) return; 
        if(adjMatrix[sIndex][eIndex]!=0) return;
        numEdge++;
        adjMatrix[sIndex][eIndex] = w;
        if(!directed) adjMatrix[eIndex][sIndex] = w;
    }


    void removeEdge(Edge<T> e){
        removeEdge(e.start,e.end);
    }

    void removeEdge(T st, T en){
        int sIndex = verList.locateEle(st);
        int eIndex = verList.locateEle(en);
        if(adjMatrix[sIndex][eIndex]==0) return;
        numEdge--;
        adjMatrix[sIndex][eIndex] = 0;
        if(!directed) adjMatrix[eIndex][sIndex] = 0;
    }

    
    int getVertexIndex(T v) { return verList.locateEle(v); }
    T getVertexName(int idx) {
        if(idx < 0 || idx >= numVer) throw "Invalid i ndex";
        return verList.getEleAtPos(idx);
    }
    int getVertexCount() { return numVer; }
    int getEdgeWeight(int from, int to) {
        if(from < 0 || from >= numVer || to < 0 || to >= numVer) return 0;
        return adjMatrix[from][to];
    }

    
    void printGraph(){
        cout << "Vertices:" << endl;
        for(int i=0;i<numVer;i++)
            cout << verList.getEleAtPos(i) << " ";

        cout << endl << "Edges:" << endl;
        char sLeft = (directed ? '<' : '(');
        char sRight = (directed ? '>' : ')');
        for(int i=0;i<numVer;i++){
            for(int j=i+1;j<numVer;j++){
                if(adjMatrix[i][j] != 0) cout << sLeft << verList.getEleAtPos(i) << "," << verList.getEleAtPos(j) << sRight << ", weight = " << adjMatrix[i][j] <<endl;
            }
        }
        if(!directed) return;
        for(int i=0;i<numVer;i++){
            for(int j=0;j<i;j++){
                if(adjMatrix[i][j] !=0) cout << sLeft << verList.getEleAtPos(i) << "," << verList.getEleAtPos(j) << sRight<< ", weight = " << adjMatrix[i][j]  <<endl;
            }
        }
    }

    void printBFS(){
        bool visited[numVer];
        for(int i=0;i<numVer;i++)
            visited[i] = false;

        for(int i=0;i<numVer;i++)
            if(!visited[i])
                BFShelper(i,visited);
        cout << endl;
    }

    void printDFS(){
        bool visited[numVer];
        for(int i=0;i<numVer;i++)
            visited[i] = false;

        for(int i=0;i<numVer;i++)
            if(!visited[i])
                DFShelper(i,visited);
        cout << endl;
    }

    AMGraph<T>* PrimMST(){
        T vers[numVer];
        for(int i=0;i<numVer;i++){
            vers[i] = verList.getEleAtPos(i);
        }
        AMGraph<T>* res = new AMGraph<T>(vers,numVer,false);

        bool isInU[numVer];
        int minDist[numVer];
        int minIdx[numVer];
        isInU[0] = true;
        for(int i=1;i<numVer;i++){
            isInU[i] = false;
            minDist[i] = (adjMatrix[0][i]!=0 ? adjMatrix[0][i] : -1);
            minIdx[i] = (adjMatrix[0][i]!=0 ? 0 : -1);
        }

        int numInU = 1;
        while(numInU < numVer){
            int minD = -1;
            int minIndex = -1;
            for(int i=0;i<numVer;i++){
                if(isInU[i]) continue;
                if(minDist[i]==-1) continue;
                if(minD==-1 || minDist[i] < minD){
                    minD = minDist[i];
                    minIndex = i;
                }
            }

            isInU[minIndex] = true;
            numInU++;
            res->addEdge(verList.getEleAtPos(minIndex),verList.getEleAtPos(minIdx[minIndex]),minD);
            cout << "Adding Edge: (" << verList.getEleAtPos(minIndex);
            cout << "," << verList.getEleAtPos(minIdx[minIndex]) << "), cost = " << minD << endl;

            for(int i=0;i<numVer;i++){
                if(isInU[i]) continue;
                if(adjMatrix[minIndex][i]==0) continue;
                if(minDist[i]==-1 || adjMatrix[minIndex][i] < minDist[i]){
                    minDist[i] = adjMatrix[minIndex][i];
                    minIdx[i] = minIndex;
                }
            }
        }
        return res;
    }

    AMGraph<T>* KruskalMST(){
        T vers[numVer];
        for(int i=0;i<numVer;i++){
            vers[i] = verList.getEleAtPos(i);
        }
        AMGraph<T>* res = new AMGraph<T>(vers,numVer,false);

        int eStart[numEdge];
        int eEnd[numEdge];
        int eWeight[numEdge];
        int idx = 0;

        for(int i=0;i<numVer;i++){
            for(int j=i+1;j<numVer;j++){
                if(adjMatrix[i][j]==0) continue;
                eStart[idx] = i;
                eEnd[idx] = j;
                eWeight[idx] = adjMatrix[i][j];
                idx++;
            }
        }

        for(int i=0;i<numEdge;i++){
            for(int j=0;j<numEdge-1;j++){
                if(eWeight[j] > eWeight[j+1]){
                    int tmp = eStart[j];
                    eStart[j] = eStart[j+1];
                    eStart[j+1] = tmp;
                    tmp = eEnd[j];
                    eEnd[j] = eEnd[j+1];
                    eEnd[j+1] = tmp;
                    tmp = eWeight[j];
                    eWeight[j] = eWeight[j+1];
                    eWeight[j+1] = tmp;
                }
            }
        }

        idx = 0;
        int compNum[numVer];
        int numComponents = numVer;
        for(int i=0;i<numVer;i++){
            compNum[i] = i;
        }

        while(numComponents > 1){
            int sComp = compNum[eStart[idx]];
            int eComp = compNum[eEnd[idx]];
            if(sComp==eComp){
                cout << "Dropping Edge: (" << verList.getEleAtPos(eStart[idx]);
                cout << "," << verList.getEleAtPos(eEnd[idx]) << "), cost = " << eWeight[idx] << endl;
                idx++;
                continue;
            }

            cout << "Adding Edge: (" << verList.getEleAtPos(eStart[idx]);
            cout << "," << verList.getEleAtPos(eEnd[idx]) << "), cost = " << eWeight[idx] << endl;
            res->addEdge(verList.getEleAtPos(eStart[idx]),verList.getEleAtPos(eEnd[idx]), eWeight[idx]);
            numComponents--;
            idx++;

            int minComp = (eComp < sComp ? eComp : sComp);
            for(int i=0;i<numVer;i++){
                if(compNum[i]==eComp || compNum[i]==sComp){
                    compNum[i] = minComp;
                }
            }
        }
        return res;
    }

    void printTopo(){
        if(!directed){
            cout << "Directed graphs only!" << endl;
            return;
        }

        bool isActive[numVer];
        int inDeg[numVer];

        for(int i=0;i<numVer;i++){
            isActive[i] = true;
            inDeg[i] = 0;
            for(int j=0;j<numVer;j++){
                if(adjMatrix[j][i]==0) continue;
                inDeg[i]++;
            }
        }

        LinkQueue<int> q;
        int numInQ = 0;

        bool changes = true;
        while(changes){
            changes = false;
            for(int i=0;i<numVer;i++){
                if(!isActive[i]) continue;
                if(inDeg[i]!=0) continue;
                q.enQueue(i);
                isActive[i] = false;
                numInQ++;
                changes = true;
                for(int j=0;j<numVer;j++){
                    if(adjMatrix[i][j]==0) continue;
                    inDeg[j]--;
                }
            }
        }

        if(numInQ < numVer){
            cout << "Loop detected." << endl;
            return;
        }

        cout << "Topo ordering: ";
        while(!q.isEmpty()){
            cout << verList.getEleAtPos(q.deQueue()) << " ";
        }
        cout << endl;
    }

    int** getMatrix(){
        return adjMatrix;
    }
};


const int MAX_PATH_LEN = 10;
const int MAX_PATHS = 20;
struct PathInfo {
    string path[MAX_PATH_LEN];
    int pathLen;
    int totalWeight;

    PathInfo(){
        pathLen = 0;
        totalWeight = 0;
    }
};


AMGraph<string>* graph = NULL;
string banList[10];
int banCount = 0;
int maxTrans = -1;


bool isBanned(string v) {
    for(int i=0;i<banCount;i++){
        if(banList[i] == v) return true;
    }
    return false;
}


bool isValidPath(PathInfo& path) {
    int trans = path.pathLen - 2;
    if(maxTrans != -1 && trans > maxTrans) return false;
    for(int i=1;i<path.pathLen-1;i++){
        if(isBanned(path.path[i])) return false;
    }
    return true;
}


bool isInPath(PathInfo& path, string v) {
    for(int i=0;i<path.pathLen;i++){
        if(path.path[i] == v) return true;
    }
    return false;
}


void swapPath(PathInfo& a, PathInfo& b) {
    PathInfo tmp = a;
    a = b;
    b = tmp;
}


void sortPaths(PathInfo paths[], int count) {
    for(int i=0;i<count-1;i++){
        for(int j=0;j<count-i-1;j++){
            if(paths[j].totalWeight > paths[j+1].totalWeight){
                swapPath(paths[j], paths[j+1]);
            }
        }
    }
}

int removeDuplicatePaths(PathInfo paths[], int count) {
    if(count <= 1) return count;
    int uniqueCount = 1;
    for(int i=1;i<count;i++){
        bool duplicate = false;
        for(int j=0;j<uniqueCount;j++){
            if(paths[i].pathLen != paths[j].pathLen) continue;
            bool same = true;
            for(int k=0;k<paths[i].pathLen;k++){
                if(paths[i].path[k] != paths[j].path[k]){
                    same = false;
                    break;
                }
            }
            if(same) {
                duplicate = true;
                break;
            }
        }
        if(!duplicate) {
            paths[uniqueCount++] = paths[i];
        }
    }
    return uniqueCount;
}


void dfsFindPaths(int startIdx, int endIdx, int currentIdx,
                  PathInfo currentPath, PathInfo result[], int& resultCount) {
    if(currentPath.pathLen >= MAX_PATH_LEN || resultCount >= MAX_PATHS) return;
    
    if(currentIdx == endIdx) {
        if(isValidPath(currentPath) && currentPath.pathLen >= 2) {
            result[resultCount++] = currentPath;
        }
        return;
    }

    int verCount = graph->getVertexCount();
    for(int i=0;i<verCount;i++){
        int weight = graph->getEdgeWeight(currentIdx, i);
        if(weight == 0) continue;
        
        string vName = graph->getVertexName(i);
        if(isInPath(currentPath, vName)) continue;

        
        PathInfo newPath = currentPath;
        newPath.path[newPath.pathLen] = vName;
        newPath.pathLen++;
        newPath.totalWeight += weight;
        
        dfsFindPaths(startIdx, endIdx, i, newPath, result, resultCount);
    }
}


int getKShortestPaths(string start, string end, int k, PathInfo result[]) {
    int resultCount = 0;
    if(graph == NULL) return 0;

    int startIdx = graph->getVertexIndex(start);
    int endIdx = graph->getVertexIndex(end);
    if(startIdx == -1 || endIdx == -1) return 0;

    PathInfo initPath;
    initPath.path[0] = start;
    initPath.pathLen = 1;
    initPath.totalWeight = 0;

    dfsFindPaths(startIdx, endIdx, startIdx, initPath, result, resultCount);

    resultCount = removeDuplicatePaths(result, resultCount);
    sortPaths(result, resultCount);

    return (resultCount > k) ? k : resultCount;
}


void printPaths(PathInfo paths[], int count) {
    cout << "There are " << count << " paths." << endl;
    for(int i=0;i<count;i++){
        cout << "Path " << i << ": ";
        for(int j=0;j<paths[i].pathLen;j++){
            if(j > 0) cout << " -> ";
            cout << paths[i].path[j];
        }
        cout << ", distance = " << paths[i].totalWeight << endl;
    }
}


void printPrompt() {
    cout << endl << "Please enter your next request." << endl;
    cout << "Options include: ban X, unban X, maxTrans Y, paths A B k, quit." << endl << endl;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    
    int m, n;
    cin >> m >> n;

    
    string* vertices = new string[m];
    for(int i=0;i<m;i++){
        cin >> vertices[i];
    }

    
    graph = new AMGraph<string>(vertices, m, false);


    for(int i=0;i<n;i++){
        string s1, s2;
        int w;
        cin >> s1 >> s2 >> w;
        graph->addEdge(s1, s2, w);
    }


    delete[] vertices;

    
    char cmd[20];
    printPrompt();
    while(true) {
        cin >> cmd;

        if(strcmp(cmd, "quit") == 0) break;
        else if(strcmp(cmd, "ban") == 0) {
            string s;
            cin >> s;
            if(!isBanned(s)){
                banList[banCount++] = s;
            }
            printPrompt();
        }
        else if(strcmp(cmd, "unban") == 0) {
            string s;
            cin >> s;
            for(int i=0;i<banCount;i++){
                if(banList[i] == s){
                    for(int j=i;j<banCount-1;j++){
                        banList[j] = banList[j+1];
                    }
                    banCount--;
                    break;
                }
            }
            printPrompt();
        }
        else if(strcmp(cmd, "maxTrans") == 0) {
            int y;
            cin >> y;
            maxTrans = (y >= 0) ? y : -1;
            printPrompt();
        }
        else if(strcmp(cmd, "paths") == 0) {
            string s1, s2;
            int k;
            cin >> s1 >> s2 >> k;
            PathInfo paths[MAX_PATHS];
            int pathCount = getKShortestPaths(s1, s2, k, paths);
            printPaths(paths, pathCount);
            printPrompt();
        }
        else {
            cout << "Invalid command!" << endl;
            printPrompt();
        }
    }


    return 0;
}
