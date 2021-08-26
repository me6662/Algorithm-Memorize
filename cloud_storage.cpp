#include <stack>
#include <vector>
#include <algorithm>
 
using namespace std;
 
const int MAXD = 10001;
const int MAXC = 50001;
 
// Directory Tree 관련
 
struct DIR {
    DIR* par;
    vector<DIR*> childs;
    int ID = -1;
 
    void init()
    {
        ID = -1;
        par = 0;
        childs.clear();      
    }
};
int dirNum;
DIR directories[2][MAXD]; // device 
int dp_cnt[2]; // dir count
 
// CLEAR DIR
void initDirectory(int mDevice) {
 
    // directory
    dp_cnt[mDevice] = 0;
 
    for (int i = 0; i < dirNum; i++) {
        directories[mDevice][i].init();
    }
 
    DIR* root = &directories[mDevice][dp_cnt[mDevice]++];
    root->ID = 0; 
}
 
 
// ADD
void cmd_addDirectory(int mDevice, int mPID, int mUID)
{
    DIR* tmpDir = &directories[mDevice][mUID];
    DIR* parDir = &directories[mDevice][mPID];
 
    tmpDir->par = parDir;
    tmpDir->ID = mUID;
    parDir->childs.push_back(tmpDir);
}
 
// MOVE
void cmd_moveDirectory(int mDevice, int mPID, int mUID)
{
    DIR* tmpDir = &directories[mDevice][mUID];
    DIR* prevParDir = tmpDir->par;
    DIR* newParDir = &directories[mDevice][mPID];
 
    prevParDir->childs.erase(find(prevParDir->childs.begin(), prevParDir->childs.end(), tmpDir));
    newParDir->childs.push_back(tmpDir);
    tmpDir->par = newParDir;
}
 
// REMOVE
 
void removeDFS(DIR* tmpDir) {
    if (tmpDir->childs.empty() == true)
    {
        tmpDir->init();
        return;
    }
    else {
        for (DIR* dir : tmpDir->childs) {
            removeDFS(dir);
        }
        tmpDir->init();
    }
}
 
void cmd_removeDirectory(int mDevice, int mUID)
{
    DIR* tmpDir = &directories[mDevice][mUID];
    DIR* parDir = tmpDir->par;
 
    parDir->childs.erase(find(parDir->childs.begin(), parDir->childs.end(), tmpDir));
 
    removeDFS(tmpDir);
}
 
 
// child counting
int countDirDFS(DIR* tmpDir) {
    int res = 1;
 
    if (tmpDir->childs.size() == 0)
        return res;
    else {
        for (DIR* dir : tmpDir->childs) {
            res += countDirDFS(dir);
        }
    }
 
    return res;
}
 
 
// CMD 관련
 
enum CMD_TYPE { NONE = 0, ADD = 1, REMOVE = 2, MOVE = 3, SKIP = 4 };
 
struct CMD {
    int cmdType;
    int mTime;
    int param1;
    int param2;
         
    void init() {
        cmdType = NONE;
        mTime = -1;
        param1 = -1;
        param2 = -1;        
    }
};
 
CMD cmds[2][MAXC];
 
int cmd_cnt[2];
 
 
 
void init(int N)
{
    // dir init   
    dirNum = N;
 
    // command init
    cmd_cnt[0] = 0;
    cmd_cnt[1] = 0;
 
    for (int i = 0; i < MAXC; i++) {
        cmds[0][i].init();
        cmds[1][i].init();
    }
         
}
 
void addDirectory(int mTime, int mDevice, int mPID, int mUID)
{
    // CMD 추가
    int deviceNum = mDevice;
 
    CMD* addCMD = &cmds[deviceNum][cmd_cnt[deviceNum]++];
 
    addCMD->cmdType = ADD;
    addCMD->mTime = mTime;
    addCMD->param1 = mPID;
    addCMD->param2 = mUID;
    
}
 
void removeDirectory(int mTime, int mDevice, int mUID)
{
    // CMD 추가
    int deviceNum = mDevice;
    CMD* remCMD = &cmds[deviceNum][cmd_cnt[deviceNum]++];
 
    remCMD->cmdType = REMOVE;
    remCMD->mTime = mTime;
    remCMD->param1 = mUID;
 
}
 
void moveDirectory(int mTime, int mDevice, int mPID, int mUID)
{
    // CMD 추가
    int deviceNum = mDevice;
    CMD* moveCMD = &cmds[deviceNum][cmd_cnt[deviceNum]++];
 
    moveCMD->cmdType = MOVE;
    moveCMD->mTime = mTime;
    moveCMD->param1 = mPID;
    moveCMD->param2 = mUID;
 
}
 
void restore(int mTime, int mDevice, int mPrevTime)
{
    // CMD 추가
    int deviceNum = mDevice;
    CMD* skipCMD = &cmds[deviceNum][cmd_cnt[deviceNum]++];
 
    skipCMD->cmdType = SKIP;
    skipCMD->mTime = mTime;
    skipCMD->param1 = deviceNum;
    skipCMD->param2 = mPrevTime;
 
}
 
int rDevice[2] = { 1, 0 };
 
void sync(int mTime, int mDevice, int mPrevTime)
{
    // CMD 추가
    int deviceNum = mDevice;
    CMD* skipCMD = &cmds[deviceNum][cmd_cnt[deviceNum]++];
 
    skipCMD->cmdType = SKIP;
    skipCMD->mTime = mTime;
    skipCMD->param1 = rDevice[deviceNum];
    skipCMD->param2 = mPrevTime;
}
 
 
int findPrevIdx(int mDevice , int mPrevTime) {
    int left = -1;
    int right = cmd_cnt[mDevice];
 
    while (left + 1 < right){
        int mid = (left + right) / 2;
 
        if (cmds[mDevice][mid].mTime <= mPrevTime)
            left = mid;
        else
            right = mid;
    }
 
    return left;
}
 
 
int countDirectory(int mDevice, int mUID)
{  
    // 행렬에 모아둔 명령어를 stack 에저장 (마지막 명령 순서대로)
    int deviceNum = mDevice;
    initDirectory(deviceNum);
 
    int lastCMDidx = cmd_cnt[deviceNum] - 1;
    stack<CMD*> finalCMDs;
 
    while (lastCMDidx >= 0) {
        CMD* tmpCMD = &cmds[deviceNum][lastCMDidx];
 
        if (tmpCMD->cmdType == SKIP) {
            deviceNum = tmpCMD->param1;
             
            lastCMDidx = findPrevIdx(deviceNum, tmpCMD->param2);
             
        }
        else {
            finalCMDs.push(tmpCMD);
            lastCMDidx--;
        }        
         
    }
 
    // stack 에 쌓아둔 순서 (초기 명령) 대로 tree 구조 형성
 
    while (finalCMDs.empty() == false) {
        int param1 = finalCMDs.top()->param1;
        int param2 = finalCMDs.top()->param2;
        int cmd_type = finalCMDs.top()->cmdType;
 
        if (cmd_type == ADD) {
            cmd_addDirectory(mDevice, param1, param2);
        } 
        else if (cmd_type == REMOVE) {
            cmd_removeDirectory(mDevice, param1);
        }
        else if (cmd_type == MOVE) {
            cmd_moveDirectory(mDevice, param1, param2);
        }
 
        finalCMDs.pop();
    }
     
    int result = countDirDFS(&directories[mDevice][mUID]);
 
    return result;
}
