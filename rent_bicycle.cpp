#include <list>
#include <queue>
#include <unordered_map>
#include <string>
using namespace std;

#define MAXN 100 // �뿩�� ��
#define MAXL 8 // �� �̸� ���� (null ����)
#define MAXU 10001 // add �� �߰��Ǵ� ������ ��
#define MAXB 100001 // ��ü ���̽��� ������ �� ??

int mdur; // �������� �ִ� ���� ���� �ð� (��� �����Ű� ������)

// ������
struct BIKE {
	int dur;
	int stime;
	void init() {
		dur = mdur;
		stime = 0;
	}
};


struct cmpless {
	bool operator() (BIKE* l, BIKE* r) {
		return l->dur < r->dur; // �������� ��ū�� less -> max heap (ū�𳪿�)
	}
};

// �뿩��
struct SHOP { 
	priority_queue<BIKE*, vector<BIKE*>, cmpless> bq; // MAX HP , dur �� ū ������� ������. (dur �� ū����� ������)
	priority_queue<int, vector<int>, greater<int>> dq; // MIN HP , ���ڰ� ���� ������� ������ (�ڽ��� �θ𺸴� ŭ)

	int nbikes; // ����ũ ��
	int ctime; // ??
	int dtime; // ��޽ð�
};


// ��
struct USER {
	BIKE * bike;
	int valid;
	void init() {
		bike = 0;
		valid = 0;
	}
};

unordered_map<string, USER*> idx;

SHOP sp[MAXN]; // SHOP ����ü �̸� �Ҵ��س��� (100��) --> ���ٽ� �����ͷ� ����...
int sp_cnt; // �뿩�� ��

USER up[MAXU];
int up_cnt; // �� ��

BIKE bp[MAXB];
int bp_cnt; // ������ ��


void init(int N, int durableTime, int deliveryTimes[MAXN])
{
	sp_cnt = 0; up_cnt = 1; bp_cnt = 0; 
	mdur = durableTime; // �ִ��������ð� ����

	for (int i = 0; i < N; i++) {
		SHOP* s = &sp[sp_cnt++];
		priority_queue<BIKE*, vector<BIKE*>, cmpless> ebq; // �� �׽�Ʈ ���̽� ��ü�� 
		priority_queue<int, vector<int>, greater<int>> edq; // �� �׽�Ʈ ���̽� ��ü�� 
		swap(s->bq, ebq);
		swap(s->dq, edq);

		s->nbikes = 0;
		s->dtime = deliveryTimes[i];
		s->ctime = 0;		
	}
	idx.clear();	
}

void addBicycle(int cTimestamp, int pID, int bicycleNum)
{
	SHOP* s = &sp[pID];
	s->nbikes += bicycleNum;
}

USER* findu(char* uName) {
	USER * res;
	string name = uName;

	if (idx[name] == 0) {
		res = &up[up_cnt++];
		res->init();
		idx[name] = res;
		return res;
	}
	return idx[name];
}

void buyTicket(int cTimestamp, char uName[MAXL], int validTime)
{
	USER* u = findu(uName);

	if (u->valid < cTimestamp)
		u->valid = cTimestamp + validTime - 1;
	else
		u->valid += validTime;
}

void update(int sid, int time) {// ���ҿ� ���´� ������ ��ġ
	SHOP* s = &sp[sid];
	
	while (!s->dq.empty() && s->dq.top() <= time) { // ���ҿ� ���´� ������ ��ġ
		s->ctime = s->dq.top();
		s->nbikes++;
		s->dq.pop();
	}
}



int rentBicycle(int cTimestamp, char uName[MAXL], int pID)
{
	USER* u = findu(uName);
	SHOP* s = &sp[pID];
	BIKE* b;

	update(pID, cTimestamp);

	if (u->bike != 0 || u->valid < cTimestamp || (s->nbikes == 0 && s->bq.empty())) // �뿩 �Ұ� 
		return -1;
	else {
		if (s->nbikes > 0) { // nbike : ���� �뿩�ҿ� ���� �� ������ ��
			b = &bp[bp_cnt++];
			b->init();
			s->nbikes--;
		}
		else { // �뿩�ҿ� �������Ŵ� ����, �������� �ִ� ������
			b = s->bq.top();
			s -> bq.pop();
		}
		b->stime = cTimestamp;
		u->bike = b;
	}

	return mdur - b->dur;
}

int returnBicycle(int cTimestamp, char uName[MAXL], int pID)
{
	USER* u = findu(uName);
	if (u->bike == 0)
		return -1;

	BIKE* b = u->bike;
	SHOP* s = &sp[pID];

	b->dur -= cTimestamp - b->stime;
	u->bike = 0;

	if (b->dur < 0) // ���ֹ�
		s->dq.push(cTimestamp + s->dtime);
	else
		s->bq.push(b);

	int res = cTimestamp - u->valid;

	if (res < 0) // �� �� �ݳ�
		return 0;

	
	return res; // ��ü
}