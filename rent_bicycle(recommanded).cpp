#include <list>
#include <queue>
#include <unordered_map>
#include <string>
using namespace std;
 
#define MAXN        100 //  대여소 수
#define MAXL        8 // 고객 이름 (널문자 포함 8글자)
#define MAXU        10001 // 한번에 추가할 수 있는 자전거 대수 
#define MAXB        120001 // 자전거 대수?

int mdur;
 
struct BIKE {
    int dur;
    int stime;
    void init() {
        dur = mdur;
        stime = 0;
    }
};
 
struct cmp_less {
    bool operator()(BIKE*l, BIKE*r) {
        return l->dur < r->dur;
    }
};
 
struct SHOP {
    priority_queue<BIKE*, vector<BIKE*>, cmp_less> bq; //MAX_HP
    priority_queue<int, vector<int>, greater<int>> dq; //MIN HP
    int nbikes;
    int ctime;
    int dtime;
};
 
struct USER {
    BIKE* bike;
    int valid;
    void init() {
        bike = 0;
        valid = 0;
    }
};
 
unordered_map<string, USER*> idx;
 
SHOP sp[MAXN];
int sp_cnt;
USER up[MAXU];
int up_cnt;
BIKE bp[MAXB];
int bp_cnt;
 
void init(int N, int durableTime, int deliveryTimes[MAXN])
{
    sp_cnt = 0; up_cnt = 1; bp_cnt = 0;
    mdur = durableTime;
    for (int i = 0; i < N; i++) {
        SHOP* s = &sp[sp_cnt++];
        priority_queue<BIKE*, vector<BIKE*>, cmp_less> ebq; //MAX_HP
        priority_queue<int, vector<int>, greater<int>> edq; //MIN HP
        swap(s->bq, ebq);
        swap(s->dq, edq);
        s->nbikes = 0;
        s->dtime = deliveryTimes[i];
        s->ctime = 0;
    }
    idx.clear();
}
 
void update(int sid, int time) {
    BIKE* b;
    SHOP* s = &sp[sid];
    while (!s->dq.empty() && s->dq.top() <= time) {//자전거  배치
        s->ctime = s->dq.top();
        s->nbikes++;
        s->dq.pop();
    }
}
void addBicycle(int cTimestamp, int pID, int bicycleNum)
{
    SHOP* s = &sp[pID];
    s->nbikes += bicycleNum;
    //printf ("  pID:%d, Num:%d\n", pID, s->bq.size());
}
 
USER* findu(char* uName) {
    USER* res;
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
    //printf ("VALID %d,%d\n", cTimestamp, u->valid);
}
 
int rentBicycle(int cTimestamp, char uName[MAXL], int pID)
{
    USER* u = findu(uName);
    SHOP* s = &sp[pID];
    BIKE* b;
    update(pID, cTimestamp);
    //printf ("  pID:%d, Num:%d\n", pID, s->bq.size());
    if (u->bike != 0 || u->valid < cTimestamp || (s->nbikes == 0 && s->bq.empty()))//대여불가
        return -1;
    else {
        if (s->nbikes > 0) {
            b = &bp[bp_cnt++];
            b->init();
            s->nbikes--;
        }
        else {
            b = s->bq.top();
            s->bq.pop();
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
    //update(pID, cTimestamp);
    b->dur -= cTimestamp - b->stime;
    u->bike = 0;
    if (b->dur < 0)//재주문
        s->dq.push(cTimestamp + s->dtime);
    else
        s->bq.push(b);
 
    int res = cTimestamp - u->valid;
    if (res < 0)
        return 0;
    return res;
}