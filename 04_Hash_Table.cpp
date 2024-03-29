// 04_Hash_Table.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>
typedef long long int lld;


#define HM 1048577

struct Hash {
	lld name;
	int aid;
	int next;
} hnode[90009];
int hl;
int hs[HM];
int hashf(lld name) {
	return name % HM;
}
int getaid(lld name) {
	int hh = hashf(name);
	for (int it = hs[hh]; it; it = hnode[it].next)
	{
		if (hnode[it].name == name)
		{
			return hnode[it].aid;
		}
	}
	return 0;
}
void setaid(lld name, int aid) {
	int hh = hashf(name);
	int hid = hl++;
	hnode[hid].aid = aid;
	hnode[hid].name = name;
	hnode[hid].next = hs[hh];
	hs[hh] = hid;
}



int main()
{
    std::cout << "Hello World!\n"; 
}
