#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <queue>

using namespace std;

const int MAXD = 13010;

int add_cnt;
int add_succ_cnt;
int merge_cnt;
int merge_succ_cnt;

struct DEPT {
	int time;
	int mNum;
	int class_;

	DEPT* upperDept;
	vector<DEPT*> lowerDepts;
	int lowerCnt;

	void init() {
		time = 0;
		mNum = 0;
		class_ = 0;
		upperDept = 0;
		lowerDepts.clear();
		lowerCnt = 0;
	}
};

DEPT depts[MAXD];
int nd_cnt;


unordered_map<string, DEPT*>deptTable;

bool isDeptExist(string deptName) {
	if (deptTable.find(deptName) != deptTable.end()) {
		return true;
	}
	else return false;
}


void init(int mNum)
{
	add_cnt = 0;
	add_succ_cnt = 0;
	merge_cnt = 0;
	merge_succ_cnt = 0;

	// value 초기화
	nd_cnt = 0;
	deptTable.clear();

	for (int i = 0; i < MAXD; i++) {
		depts[i].init();
	}


	// root 추가
	DEPT* root = &depts[nd_cnt++];
	root->init();
	root->time = nd_cnt;
	root->mNum = mNum;
	root->class_ = 0;
	const char* rootName = "root";
	string rootStr = rootName;
	deptTable[rootStr] = root;

}

void destroy()
{

}

int addDept(char mUpperDept[], char mNewDept[], int mNum)
{
	add_cnt++;

	string upperName = mUpperDept;
	string newName = mNewDept;

	// 예외조건 필터링
	if (isDeptExist(upperName) == false) return -1;

	DEPT* upperDept = deptTable[upperName];

	if (upperDept->class_ == 5) return -1;
	if (upperDept->lowerCnt == 10) return -1;
	if (upperDept->mNum <= mNum) return -1;
	if (isDeptExist(newName) == true) return -1;

	// add 하기
	DEPT* newDept = &depts[nd_cnt++];
	deptTable[newName] = newDept; // 테이블추가
	newDept->init();

	newDept->class_ = upperDept->class_ + 1; // 클래스업데이트
	newDept->time = nd_cnt; // 생성시간 업뎃

	newDept->upperDept = upperDept; // 직속상위
	upperDept->lowerDepts.push_back(newDept); //직속하위 
	upperDept->lowerCnt++;

	newDept->mNum = mNum; // 하위조직 직원수 +
	upperDept->mNum -= mNum; // 상위조직 직원수 -

	add_succ_cnt++;
	return upperDept->mNum;
}


// 하위부서 찾기 DFS

bool findLowerDFS(DEPT* dept1, DEPT* dept2) {

	if (dept2->lowerCnt == 0) return false;

	for (DEPT* dept : dept2->lowerDepts) {
		if (dept == dept1) {
			return true;
		}
		else {
			bool res = findLowerDFS(dept1, dept);

			if (res == true) return true;
		}
	}

	return false;
}

void classChangeDFS(DEPT* dept) {
	dept->class_ = dept->upperDept->class_ + 1;

	if (dept->lowerCnt == 0) return;
	else {
		for (DEPT* e : dept->lowerDepts) {
			classChangeDFS(e);
		}
	}
} 


int returnChildDepth(DEPT* dept) {
	int depth = 0;

	if (dept->lowerDepts.size() == 0)
		return depth;
	else {
		depth++;

		int maxDepth = 0;

		for (DEPT* e : dept->lowerDepts) {
			
			int childDepth = returnChildDepth(e);

			if (childDepth > maxDepth)
				maxDepth = childDepth;			
		}
		depth += maxDepth;
	}
	
	return depth;
}


int mergeDept(char mDept1[], char mDept2[])
{
	merge_cnt++;

	string dept1Name = mDept1;
	string dept2Name = mDept2;

	// 예외조건 필터링
	if (isDeptExist(dept1Name) == false || isDeptExist(dept2Name) == false) return -1;

	DEPT* dept1 = deptTable[dept1Name];
	DEPT* dept2 = deptTable[dept2Name];

	if (findLowerDFS(dept1, dept2) == true) return -1;
	if (dept1->class_ + returnChildDepth(dept2) >= 6) return -1;

	if (dept2->upperDept == dept1) { // dept2 가 dept1 의 직속하위
		if (dept1->lowerCnt - 1 + dept2->lowerCnt > 10) return -1;
	}
	else { // 직속하위 아닐때
		if (dept1->lowerCnt + dept2->lowerCnt > 10) return -1;
	}

	// Merge 수행

	DEPT* prevUpper = dept2->upperDept;   // dpet2 직속관계 끊기
	prevUpper->lowerDepts.erase(find(prevUpper->lowerDepts.begin(), prevUpper->lowerDepts.end(), dept2));
	prevUpper->lowerCnt--;

	dept1->mNum += dept2->mNum; // 직원수 추가
	for (DEPT* lowerDept : dept2->lowerDepts) {
		dept1->lowerDepts.push_back(lowerDept); // 하위부서 이동
		lowerDept->upperDept = dept1; // 하위부서 직속 변경
		classChangeDFS(lowerDept); // 클래스변경 DFS 함수 추가
		dept1->lowerCnt++;
	}

	deptTable.erase(dept2Name); // table 에서 지움
	//dept2->init();

	merge_succ_cnt++;
	return dept1->mNum;
}

void moveDFS(DEPT* top, DEPT* dept, int mDepth, int mNum) {
	if (mDepth == 0) return;

	for (DEPT* lower : dept->lowerDepts) {
		if (lower->mNum > mNum) {
			lower->mNum -= mNum;
			top->mNum += mNum;
		}
		else {
			top->mNum += lower->mNum - 1;
			lower->mNum = 1;
		}
		moveDFS(top, lower, mDepth - 1, mNum);
	}
}


int moveEmployee(char mDept[], int mDepth, int mNum)
{
	string deptName = mDept;

	// 예외조건 필터링
	if (isDeptExist(deptName) == false) return -1;

	DEPT* dept = deptTable[deptName];

	// 하위부서 없으면 걍 리턴
	if (dept->lowerCnt == 0) return dept->mNum;

	// 직원 징집
	moveDFS(dept, dept, mDepth, mNum);
	return dept->mNum;
}

struct cmp {
	bool operator() (DEPT* left, DEPT* right) {
		if (left->mNum > right->mNum) return true;
		else if (left->mNum == right->mNum) {
			if (left->time < right->time) return true;
			else return false;
		}
		else return false;
	}
};


void recruit(int mDeptNum, int mNum)
{	
	priority_queue<DEPT*, vector<DEPT*>, cmp> recruitHeap;

	for (auto iter = deptTable.begin(); iter != deptTable.end(); iter++) {
		recruitHeap.push(iter->second);
	}


	for (int i = 0; i < mDeptNum; i++) {
		DEPT* tmpDept = recruitHeap.top();
		tmpDept->mNum += mNum;
		recruitHeap.pop();
	}

}