// Memorable Code.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
/*
합병정렬 (Merge Sort)

하나의 리스트를 두 개의 균등한 크기로 분할하고 분할된 부분 리스트를 정렬한 다음, 
두 개의 정렬된 부분 리스트를 합하여 전체가 정렬된 리스트가 되게 하는 방법이다.

*/
#include "pch.h"
#include <iostream>

// 기본 알고리즘 (오름차순)

#define MAX_SIZE 8

int sorted[MAX_SIZE]; // 연산 중간에 사용되는 저장소


// [Merge] : 2 개의 인접한 배열 list[left ~ mid] , list[mid+1 ~ right] 의 합병 과정 > 이때 숫자들이 실제로 정렬됨 
// i : 정렬된 왼쪽 리스트에 대한 idx
// j : 정렬된 오른쪽 리스트에 대한 idx
// k : 정렬된 리스트에 대한 idx

void merge(int list[], int left, int mid, int right) {
	int i, j, k, l;
	i = left;   // 분할된 첫째 배열의 첫번째 idx
	j = mid + 1; // 분헐된 두번째 배열의 첫번째 idx
	k = left; // sorted 에 저장될 idx


	/* 분할 정렬된 두 list 의 합병 */
	while (i <= mid && j <= right) { // i > mid 또는 j > right 이상이면 탈출 즉, 두 리스트 중 하나가 다 써지면 탈출
		if (list[i] <= list[j])
			sorted[k++] = list[i++];
		else
			sorted[k++] = list[j++];
	}

	/* 남아 있는 리스트를 일괄 복사 */
	if (i > mid) {
		for (l = j; l <= right; l++) {
			sorted[k++] = list[l];
		}
	} else {
		for (l = i; l <= mid; l++) {
			sorted[k++] = list[l];
		}
	}

	/* sorted 배열을 현재 list 로 재복사*/

	for (l = left; l <= right; l++) {
		list[l] = sorted[l];
	}
}


// [Merge Sort] : 합병 정렬
void merge_sort(int list[], int left, int right) {
	int mid;

	if (left < right) {
		mid = (left + right) / 2; //중간위치를 계산하여 분할
		merge_sort(list, left, mid);
		merge_sort(list, mid + 1, right);
		merge(list, left, mid, right);
	}
}


// 응용 [우선순위를 내가 정해서 정렬]
// priority 를 우선으로하고
// 같다면 time 으로 나래비

struct testNode {
	int pri;
	int time;
}tNode[10];

int sortedApp[10];


int compareTestNode(int node1, int node2) {
	if (tNode[node1].pri > tNode[node2].pri) {
		return node1;
	}
	else if(tNode[node1].pri == tNode[node2].pri) {
		if (tNode[node1].time >= tNode[node2].time) return node1;
		else return node2;
	}
	else {
		return node2;
	}
}

void mergeApp(int list[], int left, int mid, int right) {
	int i, j, k, l;

	i = left;
	j = mid + 1;
	k = left;

	while (i <= mid && j <= right) {
		if (compareTestNode(list[i], list[j]) == list[i]) {
			sortedApp[k++] = list[i++];
		}
		else {
			sortedApp[k++] = list[j++];
		}
	}

	if (i > mid) {
		for (l = j; l <= right; l++) sortedApp[k++] = list[l];
	}
	else {
		for (l = i; l <= mid; l++) sortedApp[k++] = list[l];
	}

	for (l = left; l <= right; l++) {
		list[l] = sortedApp[l];
	}
}

void mergeSortApp(int list[], int left, int right) {
	
	int mid;

	if (left < right) {
		mid = (left + right) / 2;
		mergeSortApp(list, left, mid);
		mergeSortApp(list, mid + 1, right);
		mergeApp(list, left, mid, right);
	}
}


int main()
{
	// 기본
	int list[MAX_SIZE] = { 21, 10, 12, 20, 25, 13, 15, 22 };

	merge_sort(list, 0, MAX_SIZE - 1);

	for (int i = 0; i < MAX_SIZE; i++) {
		printf("%d\r\n", list[i]);
	}

	// 응용
	tNode[0].pri = 14;
	tNode[0].time = 123;

	tNode[1].pri = 34;
	tNode[1].time = 12;

	tNode[2].pri = 12;
	tNode[2].time = 121;

	tNode[3].pri = 90;
	tNode[3].time = 12;

	tNode[4].pri = 1234;
	tNode[4].time = 145;

	tNode[5].pri = 12309;
	tNode[5].time = 1213;

	tNode[6].pri = 1209;
	tNode[6].time = 128;

	tNode[7].pri = 118;
	tNode[7].time = 912;

	tNode[8].pri = 1289;
	tNode[8].time = 129;

	tNode[9].pri = 1234;
	tNode[9].time = 123;

	int list_test[10] = { 0,1,2,3,4,5,6,7,8,9 };

	mergeSortApp(list_test, 0, 9);

	for (int i = 0; i < 10; i++) {
		printf("%d\r\n", list_test[i]);
	}
}

