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

	// value �ʱ�ȭ
	nd_cnt = 0;
	deptTable.clear();

	for (int i = 0; i < MAXD; i++) {
		depts[i].init();
	}


	// root �߰�
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

	// �������� ���͸�
	if (isDeptExist(upperName) == false) return -1;

	DEPT* upperDept = deptTable[upperName];

	if (upperDept->class_ == 5) return -1;
	if (upperDept->lowerCnt == 10) return -1;
	if (upperDept->mNum <= mNum) return -1;
	if (isDeptExist(newName) == true) return -1;

	// add �ϱ�
	DEPT* newDept = &depts[nd_cnt++];
	deptTable[newName] = newDept; // ���̺��߰�
	newDept->init();

	newDept->class_ = upperDept->class_ + 1; // Ŭ����������Ʈ
	newDept->time = nd_cnt; // �����ð� ����

	newDept->upperDept = upperDept; // ���ӻ���
	upperDept->lowerDepts.push_back(newDept); //�������� 
	upperDept->lowerCnt++;

	newDept->mNum = mNum; // �������� ������ +
	upperDept->mNum -= mNum; // �������� ������ -

	add_succ_cnt++;
	return upperDept->mNum;
}


// �����μ� ã�� DFS

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

	// �������� ���͸�
	if (isDeptExist(dept1Name) == false || isDeptExist(dept2Name) == false) return -1;

	DEPT* dept1 = deptTable[dept1Name];
	DEPT* dept2 = deptTable[dept2Name];

	if (findLowerDFS(dept1, dept2) == true) return -1;
	if (dept1->class_ + returnChildDepth(dept2) >= 6) return -1;

	if (dept2->upperDept == dept1) { // dept2 �� dept1 �� ��������
		if (dept1->lowerCnt - 1 + dept2->lowerCnt > 10) return -1;
	}
	else { // �������� �ƴҶ�
		if (dept1->lowerCnt + dept2->lowerCnt > 10) return -1;
	}

	// Merge ����

	DEPT* prevUpper = dept2->upperDept;   // dpet2 ���Ӱ��� ����
	prevUpper->lowerDepts.erase(find(prevUpper->lowerDepts.begin(), prevUpper->lowerDepts.end(), dept2));
	prevUpper->lowerCnt--;

	dept1->mNum += dept2->mNum; // ������ �߰�
	for (DEPT* lowerDept : dept2->lowerDepts) {
		dept1->lowerDepts.push_back(lowerDept); // �����μ� �̵�
		lowerDept->upperDept = dept1; // �����μ� ���� ����
		classChangeDFS(lowerDept); // Ŭ�������� DFS �Լ� �߰�
		dept1->lowerCnt++;
	}

	deptTable.erase(dept2Name); // table ���� ����
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

	// �������� ���͸�
	if (isDeptExist(deptName) == false) return -1;

	DEPT* dept = deptTable[deptName];

	// �����μ� ������ �� ����
	if (dept->lowerCnt == 0) return dept->mNum;

	// ���� ¡��
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