// 02_Insertion_Sort.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
/*
자료 배열의 모든 요소를 앞에서부터 차례대로 이미 정렬된 배열 부분과 비교 하여, 자신의 위치를 찾아 삽입함으로써 정렬을 완성하는 알고리즘

0, 1 비교 하고 정렬

그다음 2 를 0 , 1차례료 비교해서 정렬

그다음 3을 0 , 1 , 2와 비교해서 정렬



이미 정렬된 list 에 새로운 요소를 넣어줄 때 사용되는 경우가 많다. ( log n )

레코드의 수가 적을 경우 알고리즘 자체가 매우 간단하므로 다른 복잡한 정렬 방법보다 유리할 수 있다.
대부분위 레코드가 이미 정렬되어 있는 경우에 매우 효율적일 수 있다.

*/
//

#include "pch.h"
#include <iostream>

// 기본

void InsertionSort(int list[], int n) {
	int i, j;
	int key;

	for (i = 1; i < n; i++) {

		key = list[i];

		for (j = i - 1; j >= 0; j--) {
			if (list[j] > key) {
				list[j + 1] = list[j];
			}
			else {
				break;
			}
		}

		list[j + 1] = key;
	}
}


// 정렬하면서 삽입하기
#define MAX_SIZE 20

void AddElementWithSort(int element, int list[]) {
	int i;
	int lastIdx;
	for (i = 0; i < MAX_SIZE; i++) {
		if (list[i] == 0) {
			list[i] = element;
			lastIdx = i;
			break;
		}					
	}
	
	InsertionSort(list, lastIdx +1);
}

int list2[MAX_SIZE];


int main()
{
	// 기본 /////////////////////////////////////////
	int i;

	int list[5] = { 8, 5, 6, 2, 4 };

	// 삽입 정렬 수행
	InsertionSort(list, 5);

	// 정렬 결과 출력
	for (i = 0; i < 5; i++) {
		printf("%d\n", list[i]);
	}

	// 응용 /////////////////////////////////////////
	printf("Add with sort \r\n");
	printf("\r\n");
	AddElementWithSort(8, list2);
	AddElementWithSort(5, list2);
	AddElementWithSort(6, list2);
	AddElementWithSort(2, list2);
	AddElementWithSort(4, list2);
	AddElementWithSort(1, list2);
	AddElementWithSort(3, list2);
	AddElementWithSort(10, list2);

	for (i = 0; i < MAX_SIZE; i++) {
		printf("%d\n", list2[i]);
	}

}

