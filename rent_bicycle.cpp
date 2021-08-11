#include <list>
#include <queue>
#include <unordered_map>
#include <string>
using namespace std;

#define MAXN 100 // 대여소 수
#define MAXL 8 // 고객 이름 길이 (null 포함)
#define MAXU 10001 // add 시 추가되는 자전거 수
#define MAXB 100001 // 전체 케이스의 자전거 수 ??

int mdur; // 자전거의 최대 안전 운행 시간 (모든 자전거가 동일함)

// 자전거
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
		return l->dur < r->dur; // 오른쪽이 더큰게 less -> max heap (큰놈나열)
	}
};

// 대여소
struct SHOP { 
	priority_queue<BIKE*, vector<BIKE*>, cmpless> bq; // MAX HP , dur 이 큰 순서대로 나열됨. (dur 이 큰놈부터 나열됨)
	priority_queue<int, vector<int>, greater<int>> dq; // MIN HP , 숫자가 작은 순서대로 나열됨 (자식이 부모보다 큼)

	int nbikes; // 바이크 수
	int ctime; // ??
	int dtime; // 배달시간
};


// 고객
struct USER {
	BIKE * bike;
	int valid;
	void init() {
		bike = 0;
		valid = 0;
	}
};

unordered_map<string, USER*> idx;

SHOP sp[MAXN]; // SHOP 구조체 미리 할당해놓고 (100개) --> 접근시 포인터로 접근...
int sp_cnt; // 대여소 수

USER up[MAXU];
int up_cnt; // 고객 수

BIKE bp[MAXB];
int bp_cnt; // 자전거 수


void init(int N, int durableTime, int deliveryTimes[MAXN])
{
	sp_cnt = 0; up_cnt = 1; bp_cnt = 0; 
	mdur = durableTime; // 최대안전운행시간 고정

	for (int i = 0; i < N; i++) {
		SHOP* s = &sp[sp_cnt++];
		priority_queue<BIKE*, vector<BIKE*>, cmpless> ebq; // 새 테스트 케이스 교체용 
		priority_queue<int, vector<int>, greater<int>> edq; // 새 테스트 케이스 교체용 
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

void update(int sid, int time) {// 폐기소에 보냈던 자전거 배치
	SHOP* s = &sp[sid];
	
	while (!s->dq.empty() && s->dq.top() <= time) { // 폐기소에 보냈던 자전거 배치
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

	if (u->bike != 0 || u->valid < cTimestamp || (s->nbikes == 0 && s->bq.empty())) // 대여 불가 
		return -1;
	else {
		if (s->nbikes > 0) { // nbike : 현재 대여소에 남은 새 자전거 수
			b = &bp[bp_cnt++];
			b->init();
			s->nbikes--;
		}
		else { // 대여소에 새자전거는 없고, 사용경험이 있는 자전거
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

	if (b->dur < 0) // 재주문
		s->dq.push(cTimestamp + s->dtime);
	else
		s->bq.push(b);

	int res = cTimestamp - u->valid;

	if (res < 0) // 제 때 반납
		return 0;

	
	return res; // 연체
}