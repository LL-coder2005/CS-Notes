#include <iostream>
#include <vector>
using namespace std;
class adjacencyMatrix
{
private:
    int numNode; //图节点数量
    int numEdge; //边数量
    vector<char> graph;
    vector<vector<int>> edge; 
    
public:
    adjacencyMatrix(int numNode,int numEdge);
    ~adjacencyMatrix();
    void addNode();
    void addEdge();
    void dfsGraph();
private:
    int findNode(char node);
    void dfs(char node);
    vector<bool> vis;
};

adjacencyMatrix::adjacencyMatrix(int numNode, int numEdge) 
    : numNode(numNode), numEdge(numEdge)
{
    edge.resize(numNode, vector<int>(numNode, 0));  // 0表示无边
    vis.resize(numNode, false);
}

adjacencyMatrix::~adjacencyMatrix()
{
}

//添加节点
void adjacencyMatrix::addNode(){
    char node;
    cout << "请输入节点(中间以空格隔开)"<<endl;
    for(int i=0;i<numNode;i++){
        cin>>node;
        graph.push_back(node);
    }
}
//添加边
void adjacencyMatrix::addEdge() {
    char from, to;
    int weight;
    cout << "请输入" << numEdge << "条边(格式: 起点 终点 权重):" << endl;
    for(int i = 0; i < numEdge; i++) {
        cin >> from >> to >> weight;
        int fromIdx = findNode(from);
        int toIdx = findNode(to);
        
        if(fromIdx != -1 && toIdx != -1) {
            edge[fromIdx][toIdx] = weight;
            edge[toIdx][fromIdx] = weight;  // 无向图
        } else {
            cout << "节点不存在，请重新输入!" << endl;
            i--;  // 重新输入这条边
        }
    }
}

int adjacencyMatrix::findNode(char node){
    for(int i=0;i<graph.size();i++){
        if(graph[i]==node){
            return i;
        }
    }
    return -1;
}

void adjacencyMatrix::dfsGraph(){
    vis = vector<bool>(numNode,0);
    for(auto node:graph){
        if(!vis[findNode(node)]){
            dfs(node);
        }
    }
}

void adjacencyMatrix::dfs(char node){
    int n = findNode(node);
    cout << node << " ";
    vis[n]=1;
    for(int i = 0;i<numNode;i++){
        if((edge[n][i]<10001)&&vis[i]==0){
            dfs(graph[i]);
        }
    }
}

int main(){
    adjacencyMatrix g(4,5);
    g.addNode();
    g.addEdge();
    g.dfsGraph();

    return 0;
}