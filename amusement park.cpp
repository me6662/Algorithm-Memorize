#include <list>
#include <queue>
#include <unordered_map>
using namespace std;

#define MAXN 100 
#define MAXA 10001

// ����ο�
struct WAIT {
	int peopleCnt;
	int priority;
};

WAIT wp[MAXA];
int wp_cnt;


struct cmpless {
	bool operator() (WAIT* left, WAIT* right) {
		return left->priority < right->priority;
	}
};

// ���̱ⱸ
struct MACHINE {
	int mid;
	int duration;
	int capacity;

	int cTime; // ���̱ⱸ update �ð�
	int mWait; // ����ڼ�

	void init() {
		mid = 0;
		duration = 0;
		capacity = 0;
		cTime = 0;
		mWait = 0;

	}

	priority_queue<WAIT*, vector<WAIT*>, cmpless> waitq;
};

MACHINE mp[MAXN]; // ���̱ⱸ ���
int mp_cnt; // ���̱ⱸ ��� idx

unordered_map<int, MACHINE*> midTable;

void init(int N, int mId[], int mDuration[], int mCapacity[]) {
	mp_cnt = 0; wp_cnt = 0;
	midTable.clear();

	for (int i = 0; i < N; i++) {
		MACHINE* tmpMachine = &mp[mp_cnt++];
		tmpMachine->init();
		tmpMachine->duration = mDuration[i];
		tmpMachine->capacity = mCapacity[i];
		priority_queue<WAIT*, vector<WAIT*>, cmpless> newWaitq;
		swap(tmpMachine->waitq, newWaitq);

		tmpMachine->mid = mId[i];
		midTable[mId[i]] = tmpMachine;
	}

}

void update(MACHINE* machine, int timeStamp) {
	int dur = machine->duration;

	while (machine->cTime != 0 && machine->cTime <= timeStamp - dur) {
		if (machine->waitq.empty() == true) {
			machine->cTime = 0;
			break;
		}

		int capa = machine->capacity;
		int topCnt = machine->waitq.top()->peopleCnt;

		while (true) {
			if (topCnt > capa) {
				machine->waitq.top()->peopleCnt = topCnt - capa;
				machine->mWait -= capa;
				break;
			}
			else if (topCnt == capa) {
				machine->waitq.pop();
				machine->mWait -= capa;
				break;
			}
			else {
				capa = capa - topCnt;
				machine->mWait -= topCnt;
				machine->waitq.pop();
				if (machine->waitq.size() == 0)
					break;
				else
					topCnt = machine->waitq.top()->peopleCnt;
			}
		}

		machine->cTime += dur;
	}

}

int add(int tStamp, int mId, int mNum, int mPriority) {
	MACHINE* tmpMachine = midTable[mId];

	// add �ð��� 0���� ū ��� tmpMachine �� ����ڼ� update (��������� ����ڼ� ������Ʈ �ʿ�)
	if (tStamp > 0)
		update(tmpMachine, tStamp - 1);

	// update �� ���� 

	WAIT* tmpWait = &wp[wp_cnt++];
	tmpWait->peopleCnt = mNum;
	tmpWait->priority = mPriority;
	tmpMachine->waitq.push(tmpWait);
	tmpMachine->mWait += mNum;


	if (tmpMachine->cTime == 0 || tmpMachine->cTime + tmpMachine->duration == tStamp) {
		// 1���̱ⱸ 1ȸ �̿�� capacity ��ŭ waitq���� ���ִ� �ڵ�
		int capa = tmpMachine->capacity;
		int topCnt = tmpMachine->waitq.top()->peopleCnt;

		while (true) {
			if (topCnt > capa) {
				tmpMachine->waitq.top()->peopleCnt = topCnt - capa;
				tmpMachine->mWait -= capa;
				break;
			}
			else if (topCnt == capa) {
				tmpMachine->waitq.pop();
				tmpMachine->mWait -= capa;
				break;
			}
			else {
				capa = capa - topCnt;
				tmpMachine->mWait -= topCnt;
				tmpMachine->waitq.pop();

				if (tmpMachine->waitq.size() == 0)
					break;
				else
					topCnt = tmpMachine->waitq.top()->peopleCnt;
			}
		}
		tmpMachine->cTime = tStamp;
	}

	// return
	if (tmpMachine->mWait == 0) {
		return 0;
	}
	else {

		return tmpMachine->waitq.top()->priority;
	}

}

struct cmplessMachine {
	bool operator() (MACHINE* left, MACHINE* right) {

		if (left->mWait < right->mWait) return true;
		else if (left->mWait == right->mWait) {
			return left->mid < right->mid;
		}
		else return false;
	}
};

void search(int tStamp, int mCount, int mId[], int mWait[]) {
	priority_queue<MACHINE*, vector<MACHINE*>, cmplessMachine> resultq;

	for (int i = 0; i < mp_cnt; i++) {
		MACHINE* tmpMachine = &mp[i];

		if (tmpMachine->cTime != 0)
			update(tmpMachine, tStamp);
		resultq.push(tmpMachine);
	}

	for (int j = 0; j < mCount; j++) {
		MACHINE* tmpMachine = resultq.top();
		mId[j] = tmpMachine->mid;
		mWait[j] = tmpMachine->mWait;
		resultq.pop();
	}
}