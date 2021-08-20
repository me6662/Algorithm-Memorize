
#include <string>
#include <unordered_map>
#include <algorithm>
  
  
#define MAX_N 10001
#define MAX_BUCKET 70001
  
  
using namespace std;
  
  
struct SNode
{
    int num;
    int bottomDepth; 
    int par;
    int childCount;
    int children[10];
    int isDel;
};
  
  
SNode zero = {};
  
  
int DeptIndexCount = 0;
SNode Depts[MAX_N];
  
  
unordered_map<string, size_t> DeptName;
  
  
int GetDepth(int index)
{
    int depth = 0;
    int cur = index;
    while (Depts[cur].par != -1) {
        depth++;
        cur = Depts[cur].par;
    }
    return depth;
}
  
  
int GetBottomDepth(int index)
{
    return Depts[index].bottomDepth;
}
  
  
int GoDepth(int index, int curDepth)
{
    SNode& node = Depts[index];
    int max = curDepth;
    int cur;
    for (int i = 0; i < node.childCount; i++) {
        cur = GoDepth(node.children[i], curDepth + 1);
        if (max < cur) max = cur;
    }
    return max;
}
  
  
void init(int mNum)
{
    DeptName.clear();
    DeptName.rehash(MAX_BUCKET);
    DeptIndexCount = 0;
    SNode& node = Depts[DeptIndexCount];
  
  
    node = zero;
    node.num = mNum;
    node.par = -1;
  
  
    DeptName["root"] = DeptIndexCount++;
}
  
  
void destroy()
{
    DeptName.clear();
}
  
  
int IsValidDept(char strDept[])
{
    int count = DeptName.count(strDept);
    if (count < 1) return 0;
    int index = DeptName[strDept];
    if (Depts[index].isDel == 1) return 0;
    return 1;
}
  
  
void CalcBottomDepth(int index)
{
    if (index < 0) return;
    SNode& node = Depts[index];
    int maxDepth = -1;
  
  
    for (int i = 0; i < node.childCount; i++) {
        SNode& childNode = Depts[node.children[i]];
        if (maxDepth < childNode.bottomDepth) {
            maxDepth = childNode.bottomDepth;
        }
    }
    node.bottomDepth = maxDepth + 1;
}
  
  
void CalcBottomDepthReq(int index)
{
    if (index < 0) return;
    CalcBottomDepth(index);
    CalcBottomDepthReq(Depts[index].par);
}
  
  
void UpdateBottomDepth(int index, int maxBottomDepth)
{
    if (index < 0) return;
    SNode& node = Depts[index];
    if (node.bottomDepth >= maxBottomDepth) return;
    node.bottomDepth = maxBottomDepth;
    UpdateBottomDepth(node.par, node.bottomDepth + 1);
}
  
  
int addDept(char mUpperDept[], char mNewDept[], int mNum)
{
    if (IsValidDept(mUpperDept) == 0) return -1;
    int count1 = DeptName.count(mUpperDept);
    int index1 = DeptName[mUpperDept];
    SNode& nodeUpper = Depts[index1];
  
  
    if (GetDepth(index1) >= 5) return -1;
  
  
    if (nodeUpper.childCount >= 10) return -1;
    if (nodeUpper.num <= mNum) return -1;
  
  
    int count2 = DeptName.count(mNewDept);
    int index2 = -1;
  
  
    if (count2 > 0) {
        index2 = DeptName[mNewDept];
        if (Depts[index2].isDel == 0) {
            return -1;
        }
    }
  
  
    index2 = DeptIndexCount;
    DeptName[mNewDept] = DeptIndexCount++;
  
  
    SNode& nodeNew = Depts[index2];
    nodeNew = zero;
  
  
    nodeNew.num = mNum;
    nodeUpper.num -= mNum;
  
  
    nodeUpper.children[nodeUpper.childCount++] = index2;
    nodeNew.par = index1;
  
  
    UpdateBottomDepth(index1, 1);
  
  
    return nodeUpper.num;
}
  
  
int IsAncestor(int par, int index)
{
    while (index >= 0) {
        if (par == index) return 1;
        index = Depts[index].par;
    }
    return 0;
}
  
  
void Remove(int parIndex, int delIndex)
{
    SNode& node = Depts[parIndex];
    for (int i = 0; i < node.childCount; i++) {
        if (delIndex == node.children[i]) {
            node.children[i] = node.children[--node.childCount];
            break;
        }
    }
}
  
  
void MoveChildren(int index1, int index2)
{
    SNode& nodeDept1 = Depts[index1];
    SNode& nodeDept2 = Depts[index2];
    int& count = nodeDept1.childCount;
    for (int i = 0; i < nodeDept2.childCount; i++) {
        int childIndex = nodeDept2.children[i];
        nodeDept1.children[count++] = childIndex;
        Depts[childIndex].par = index1;
    }
    nodeDept2.childCount = 0;
}
  
  
int mergeDept(char mDept1[], char mDept2[])
{
    if (IsValidDept(mDept1) == 0 || IsValidDept(mDept2) == 0) return -1;
  
  
    int index1 = DeptName[mDept1];
    int index2 = DeptName[mDept2];
  
  
    SNode& nodeDept1 = Depts[index1];
    SNode& nodeDept2 = Depts[index2];
  
  
    if (IsAncestor(index2, index1) == 1) return -1;
  
  
    int isDirectSon = 0;
    if (nodeDept2.par == index1) isDirectSon = 1;
    if (nodeDept1.childCount + nodeDept2.childCount - isDirectSon > 10) return -1;
  
  
    int depth = GetDepth(index1);
    int bottom = GetBottomDepth(index2);
  
  
    if (depth + bottom >= 6) return -1;
    Remove(nodeDept2.par, index2);
    nodeDept2.isDel = 1;
  
  
    DeptName.erase(mDept2);
  
  
    CalcBottomDepthReq(nodeDept2.par);
  
  
    nodeDept1.num += nodeDept2.num;
    nodeDept2.num = 0;
  
  
    MoveChildren(index1, index2);
  
  
    CalcBottomDepth(index1);
    UpdateBottomDepth(nodeDept1.par, nodeDept1.bottomDepth + 1);
  
  
    return nodeDept1.num;
}
  
  
int move(int index, int curDepth, int limitDepth, int num)
{
    if (curDepth > limitDepth) return 0;
  
  
    SNode& nodeDept = Depts[index];
  
  
    int count = 0;
    for (int i = 0; i < nodeDept.childCount; i++) {
        count += move(nodeDept.children[i], curDepth + 1, limitDepth, num);
    }
  
  
    if (nodeDept.num > num) {
        count += num;
        nodeDept.num -= num;
    }
    else {
        count += nodeDept.num - 1;
        nodeDept.num = 1;
    }
  
  
    return count;
}
  
  
int moveEmployee(char mDept[], int mDepth, int mNum)
{
    if (IsValidDept(mDept) == 0) return -1;
    int index = DeptName[mDept];
    SNode& nodeDept = Depts[index];
  
  
    int count = 0;
    for (int i = 0; i < nodeDept.childCount; i++) {
        count += move(nodeDept.children[i], 1, mDepth, mNum);
    }
  
  
    nodeDept.num += count;
  
  
    return nodeDept.num;
}
  
  
int temp[10010];
  
  
void recruit(int mDeptNum, int mNum) {
    int count = 0;
    for (int i = 0; i < DeptIndexCount; i++) {
        if (Depts[i].isDel == 1) continue;
        temp[count++] = i;
    }
  
  
    nth_element(temp, temp + mDeptNum - 1, temp + count, [&](int a, int b) {
        if (Depts[a].num != Depts[b].num) return Depts[a].num < Depts[b].num;
        return a > b;
        }
    );
  
  
    for (int i = 0; i < mDeptNum; i++) {
        Depts[temp[i]].num += mNum;
    }
}