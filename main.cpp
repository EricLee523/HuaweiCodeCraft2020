#include <bits/stdc++.h>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <thread>
#define HIGH_BOUNDARY  7
#define LOW_BOUNDARY  3

using namespace std;

class Cycle{
public:


    Cycle(int length, const vector<int> &cycle)
            : length(length), cycle(cycle) {
        //construcr
    }
    int         length;
    vector<int> cycle;
    //compare algorithm
    bool operator < (const Cycle&)const;
};


class Method{
public:
    vector<int>            inDegree_;//节点的入度
    vector<int>            outDegree_;//节点的出度

    //构造函数
    Method();

    /**
     * @brief 文件解析
     * @param testFile : 需要解析的文件
     */
    void ParseTestfile(string &testFile);
    /**
     * @brief 创建图
     */
    void mapBuilder();

    /**
     * @brief DFS_SEARCH寻找环
     * @param head 头节点
     * @param cur 当前节点
     * @param depth 当前搜寻深度
     * @param cycle 环
     */
    void DFS_SEARCH(int head,int cur,int depth,vector<int> &cycle);


    /**
     * @brief 寻找图中的所有环
     */
    void GetCycle();
    /**
     * @brief 保存文件
     * @param outputFile 输出文件
     */
    void SaveFile(const string &outputFile);
    /**
     * @brief 删除不成环的节点
     */
    void DelteNode(vector<int> &degs,bool doSoring);

    void DelteNode_in();
    void DelteNode_out();


    void ConstructV2VRecord();

private:
    //std::mutex             _mu;      //yu

    vector<vector<int>>    graph_; //邻接表
    vector<vector<int>>    Rgraph_; //逆邻接表

    unordered_map<int,int> idHash; //图的所有节点映射为数字0-n
    vector<string>         idsComma;//数字0-n映射回原来的节点
    vector<string>         idsLF;
    vector<int>            rawData_; //原始数据

    vector<bool>           visit_;//访问过的节点
    int                    nodeNum_;//图的节点数
    vector<Cycle>          cycleVec_[8];
    vector<int>            reachable;

    //V2VRecord_[end][mid][k]表示结点mid到达结点end，中间经过结点k的路径详情
    vector<unordered_map<int,vector<int> > > V2VRecord_;

    int resultNum_;



};

Method::Method(){

    resultNum_=0;

}



//主要是建立graph_  可优化空间小
void  Method::mapBuilder(){
    auto tmp=rawData_;
    sort(tmp.begin(),tmp.end());
    tmp.erase(unique(tmp.begin(),tmp.end()),tmp.end());
    nodeNum_=tmp.size();
    idsComma.reserve(nodeNum_);
    idsLF.reserve(nodeNum_);
    nodeNum_=0;
    for(int &x:tmp){
        idsComma.push_back(to_string(x)+',');
        idsLF.push_back(to_string(x)+'\n');
        idHash[x]=nodeNum_++;  //节点编号转换至0-n
    }

    //G=new vector<int>[nodeNum_];
    graph_=vector<vector<int>>(nodeNum_);
    Rgraph_=vector<vector<int>>(nodeNum_);//yuyu逆邻接表
    inDegree_=vector<int>(nodeNum_,0);
    outDegree_=vector<int>(nodeNum_,0);

    //根据输入数据建立邻接表
    //优化后(迭代器遍历更快)
    for(auto it = rawData_.cbegin() ; it < rawData_.cend() ; it+=2){
        int u = idHash[*it];
        int v = idHash[*(it+1)];
        graph_[u].push_back(v);
        Rgraph_[v].push_back(u);//yuyu逆邻接表
        //计算每个节点的出度和入度
        ++inDegree_[v];
        ++outDegree_[u];
    }

    //优化前
//    int sz=rawData_.size();
//    for(int i=0;i<sz;i+=2){
//        int u=idHash[rawData_[i]],v=idHash[rawData_[i+1]];
//        graph_[u].push_back(v);
//
//        //计算每个节点的出度和入度
//        ++inDegree_[v];
//        ++outDegree_[u];
//    }
}

void Method::DelteNode_in(){
    queue<int> que;
    for(int i=0;i<nodeNum_;i++){
        if(0==inDegree_[i])
            que.push(i);
    }
    while(!que.empty()){
        int u=que.front();
        que.pop();
        for(int &v:graph_[u]) {
            if(0==--inDegree_[v])   //与u相关的所有可以删除的点(入度为1)
                que.push(v);
        }
    }

    for(int i=0;i<nodeNum_;i++){
        if(inDegree_[i]==0){
            graph_[i].clear();    //对于出度或入度为0的边直接删除
            Rgraph_[i].clear();
        }
    }

}

void Method::DelteNode_out(){

    queue<int> que;
    for(int i=0;i<nodeNum_;i++){
        if(0==outDegree_[i])
            que.push(i);
    }
    while(!que.empty()){
        int u=que.front();
        que.pop();
        for(int &v:Rgraph_[u]) {
            if(0==--outDegree_[v])   //与u相接的
                que.push(v);
        }
    }



    for(int i=0;i<nodeNum_;i++){
        if(outDegree_[i]==0){
            graph_[i].clear();    //对于出度或入度为0的边直接删除
            Rgraph_[i].clear();
        }else{
            sort(graph_[i].begin(),graph_[i].end());
        }
    }

}
void Method::DelteNode(vector<int> &degs,bool doSoring){
    queue<int> que;
    for(int i=0;i<nodeNum_;i++){
        if(0==degs[i])
            que.push(i);
    }
    while(!que.empty()){
        int u=que.front();
        que.pop();
        for(int &v:graph_[u]) {
            if(0==--degs[v])   //与u相接的
                que.push(v);
        }
    }
    int cnt=0;

    for(int i=0;i<nodeNum_;i++){
        if(degs[i]==0){
            graph_[i].clear();    //对于出度或入度为0的边直接删除
            cnt++;
        }else if(doSoring){
            sort(graph_[i].begin(),graph_[i].end());
        }
    }


}

//比较耗时
//认真考虑如何优化
//若数据量较小考虑不执行？？？
void Method::ConstructV2VRecord(){


    V2VRecord_ = vector<unordered_map<int,vector<int> > >      //类型
                 (nodeNum_,unordered_map<int,vector<int> >()); //空间分配

    //构建V2VRecord_
    //已优化，通过访问逆邻接表，减少却页中断，性能提升3倍左右
    for(int end = 0; end < nodeNum_; end++){
        //auto &gi=Rgraph_[i];
        for(int &mem:Rgraph_[end]){      //Rgraph_[end]是end的入口 //mem是i的某一个入口
            for(int &cur:Rgraph_[mem]){ //Rgraph_[mem]是end的入口的入口
                if(end == cur ) continue;
                V2VRecord_[end][cur].push_back(mem);  //从cur到end可能经过哪些mem

            }
        }

    }

    //对V2VRecord_元素排序
    //已优化
    for(int i = 0; i < nodeNum_; i++ ){
        auto itB = V2VRecord_[i].begin();
        auto itE = V2VRecord_[i].end();

        for(auto it =  itB ; it!= itE ; it++){
            if(it->second.size()>1){
                sort(it->second.begin(),it->second.end());
            }
        }
    }


}

//depth从1开始
void Method::DFS_SEARCH(int head,int cur,int depth,vector<int> &cycle){

    cycle.push_back(cur);
    visit_[cur]  =  true;


    auto &nextNodes = graph_[cur];

    auto it=lower_bound(nextNodes.begin(),nextNodes.end(),head); //二分法查找第一个大于或等于head的节点

    if(it != nextNodes.end()          &&  *it==head
       && depth>=LOW_BOUNDARY  &&  depth<HIGH_BOUNDARY)
    {

        cycleVec_[depth].emplace_back(Cycle(depth,cycle));  //深度为depth的环
        ++resultNum_;
    }

    if(depth < HIGH_BOUNDARY-1){
        while( it != nextNodes.end() ){
            if( !visit_[*it] ){
                DFS_SEARCH(head,*it,depth+1,cycle);
            }
            ++it;
        }
    }
    else if(reachable[cur] > -1 && depth == HIGH_BOUNDARY-1 ){ //第六层
        auto paths  =  V2VRecord_[head][cur];//从cur到head的所有路径
        int path_num = paths.size();

        for(int pos = reachable[cur];pos < path_num; ++pos){

            int ndoe = paths[pos];
            if(visit_[ndoe]) continue;

            auto tmp_store = cycle;
            tmp_store.push_back(ndoe);
            cycleVec_[depth+1].emplace_back(Cycle( depth+1,tmp_store ) ) ;
            ++resultNum_;
        }
    }
    visit_[cur] = false;
    cycle.pop_back();
}


void Method::GetCycle(){

    ConstructV2VRecord();      //建立路径

    visit_       =  vector<bool>(nodeNum_,false);
    reachable    =  vector<int>(nodeNum_,-1);


    vector<int> cycle;
    vector<int> Js_reachable(nodeNum_);

    for(int i = 0;i < nodeNum_; ++i){


        if(!graph_[i].empty()){
            //V2VRecord_[i] 各节点到i的路径的map

            for(auto &paths : V2VRecord_[i]){
                int from = paths.first;
                if(from > i){
                    auto &path = paths.second;

                    int pos = lower_bound(path.begin(),path.end(),i) - path.begin(); //
                    if(pos < path.size())
                        reachable[from] = pos ;  //表示从j到i经过的路径中最小者的编号  表示j为第6层时,可直接判断终点
                    Js_reachable.push_back(from);
                }
            }
            DFS_SEARCH(i, i, 1, cycle);
            for(int &x : Js_reachable)
                reachable[x] = -1;
            Js_reachable.clear();
        }
    }


}

void Method::ParseTestfile(string &testFile){
    FILE* file=fopen(testFile.c_str(),"r");
    int u,v,c;
    int num = 0;
    while(fscanf(file,"%u,%u,%u",&u,&v,&c)!=EOF){
        rawData_.push_back(u);
        rawData_.push_back(v);
        ++num;
    }
}

void Method::SaveFile(const string &outputFile){
//    auto t=clock();
    FILE *fp = fopen(outputFile.c_str(), "wb");
    char buf[1024];


    int idx=sprintf(buf,"%d\n",resultNum_);
    buf[idx]='\0';
    fwrite(buf, idx , sizeof(char), fp );
    for(int i=LOW_BOUNDARY;i<=HIGH_BOUNDARY;i++) {
        for (auto &x:cycleVec_[i]) {
            auto cycle = x.cycle;
            int sz = cycle.size();
            for(int j=0;j<sz-1;j++){
                auto res=idsComma[cycle[j]];
                fwrite(res.c_str(), res.size() , sizeof(char), fp );
            }
            auto res=idsLF[cycle[sz-1]];
            fwrite(res.c_str(), res.size() , sizeof(char), fp );
        }
    }
    fclose(fp);

}

bool Cycle::operator < (const Cycle& cyc)const{
    if(length != cyc.length)
        return length<cyc.length;
    for(int i=0;i<length;i++){
        if(cycle[i] != cyc.cycle[i])
            return cycle[i] < cyc.cycle[i];
    }
}


int main()
{

    string test_file =  "/data/test_data.txt";
    string result_file = "/projects/student/result.txt";

    Method* m = new Method;
    m->ParseTestfile(test_file);


    m->mapBuilder();



    m->DelteNode_in();
    m->DelteNode_out();



    m->GetCycle();



    m->SaveFile(result_file);


    exit(0);

    return 0;
}
