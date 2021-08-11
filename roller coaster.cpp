#include <unordered_map>
using namespace std;

const int MAX_NODE = 51000;
const int MAX_N = 5001;

enum Movement {
	LEFT = 0, UP = 1, RIGHT = 2, DOWN = 3
};

int opposite[4] = { RIGHT, DOWN, LEFT, UP };

int moment[4][3] = {
	{DOWN, LEFT, UP}, // from : LEFT 일때, 0(좌회전) : DOWN, 1(직진) : LEFT , 2(우회전) : UP 
	 {LEFT, UP, RIGHT},
	 {UP, RIGHT, DOWN},
	 {RIGHT, DOWN , LEFT} };

// 이전 NODE 의 to 에 따라 다음 NODE row, column 값이 바뀜 (delta)
int offset[4][2] = { {0,-1}, // LEFT
	  {-1, 0}, // UP
	  {0, 1}, // RIGHT
	  {1,0} }; // DOWN

// 최대 row column 갯수 10000 x 10000 이면 1byte 라 해도 1 GB 가 되므로 (공간제약)
// 해시테이블 써서 5000개로 줄여야됨.
int getID(int row, int col) {
	return row * 2 * MAX_N + col;
}


struct NODE {	
	int mfront;
	int row, column;
	int from, to;
	
	NODE* prev;
	NODE* next;
	
	bool removed;

	void init() {
		mfront = 0;
		prev = 0;
		next = 0;
		row = 0;
		column = 0;
		from = 0;
		to = 0;
		removed = false;
	}
};

NODE np[MAX_NODE];
NODE* rear;
NODE* front;

unordered_map<int, NODE*> mapper;


struct LINKEDLIST{
	NODE head;
	NODE tail;

	void init() {
		head.next = &tail;
		tail.prev = &head;
	};

	bool empty() { return head.next == &tail; }

	NODE* push_back(NODE* tmpNode) {
		tail.prev->next = tmpNode;		
		tmpNode->prev = tail.prev;
		tail.prev = tmpNode;
		tmpNode->next = &tail;
		return tmpNode;
	}
	
	// front - rear 가 연결되는지 확인
	bool connected(LINKEDLIST * other) {
		if (other->empty()) return false;

		NODE *last = tail.prev;
		int* delta = offset[last->to];
		int row2 = last->row + delta[0];
		int column2 = last->column + delta[1];

		NODE *other_last = other->tail.prev;
		delta = offset[other_last->to];
		int row1 = other_last->row + delta[0];
		int column1 = other_last->column + delta[1];

		if (last->row == row1 && last->column == column1 && other_last->row == row2 && other_last->column == column2) {
			return true;
		}

		return false;
	}

	void move(NODE *iter, LINKEDLIST* other) {
		if (!iter) return;
		if (empty()) return;

		for (NODE* back = tail.prev; back != iter; ) {
			NODE *marker = back->prev;
			int tmp = opposite[back->from];
			back->from = opposite[back->to];
			back->to = tmp;
			back->mfront = back->mfront == 0 ? 1 : 0;
			other->push_back(back);
			back = marker;
		}

		iter->prev->next = &tail;
		tail.prev = iter->prev;
	}

	int erase_all(NODE *data) {
		if (empty()) return 0;

		int result = 0;

		for (NODE* iter = data; iter != &tail; iter = iter->next) {
			iter->removed = true;
			mapper[getID(iter->row, iter->column)] = 0;
			result++;
		}
		data->prev->next = &tail;
		tail.prev = data->prev;
		return result;
	}
};

LINKEDLIST frontList;
LINKEDLIST rearList;

int nidx;
int myN;



void init(int N)
{
	frontList.init();
	rearList.init();
	mapper.clear();

	myN = N;
	nidx = 0;

	front = 0;
	rear = 0;		
}



void addRail(int mFront, int mDirection)
{
	NODE* tmpNode = &np[nidx++];
	tmpNode->init();
	tmpNode->mfront = mFront;
	

	// rear
	if (mFront == 0) { 
		if (rearList.empty()) {// 승강장 뒤쪽에 NODE 0
			tmpNode->row = myN;
			tmpNode->column = myN - 1;
			
			tmpNode->from = LEFT;
			tmpNode->to = moment[LEFT][mDirection];
		}
		else { // 승강장 뒤쪽에 있음
			int *delta = offset[rear->to];
			tmpNode->row = rear->row + delta[0];
			tmpNode->column = rear->column + delta[1];

			tmpNode->from = rear->to;
			tmpNode->to = moment[rear->to][mDirection];
		}
		// rear 리스트 삽입
		rear = rearList.push_back(tmpNode);
		mapper[getID(rear->row, rear->column)] = rear;
	}
	// front
	else {
		if (frontList.empty()) { // 승강장 앞쪽 에 NODE 0
			tmpNode->row = myN;
			tmpNode->column = myN + 1;

			tmpNode->from = RIGHT;
			tmpNode->to = moment[RIGHT][mDirection];
		}
		else { // 승강장 앞쪽에 있음
			int *delta = offset[front->to];
			tmpNode->row = front->row + delta[0];
			tmpNode->column = front->column + delta[1];

			tmpNode->from = front->to;
			tmpNode->to = moment[front->to][mDirection];
		}
		// front 리스트 삽입
		front = frontList.push_back(tmpNode);
		mapper[getID(front->row, front->column)] = front;
	}
}

int delRail(int mRow, int mCol)
{
	int result = 0;

	NODE * tmpNode = mapper[getID(mRow, mCol)];

	if (tmpNode) {
		bool connected = rearList.connected(&frontList); // 두 리스트의 폐루프 유무 확인

		// 승강장 후방 노드
		if (tmpNode->mfront == 0) { 
			if (connected) { // 폐루프면 1개만 지움
				rearList.move(tmpNode, &frontList);
				result = 1;
			}
			else { // 폐루프 아니면 밑에 연결된 애들 다죽임.
				result = rearList.erase_all(tmpNode);
			}
		}
		// 승강장 전방 노드
		else { 
			if (connected) {
				frontList.move(tmpNode, &rearList);
				result = 1;
			}
			else {
				result = frontList.erase_all(tmpNode);
			}
		}
		rear = rearList.tail.prev;
		front = frontList.tail.prev;		
	}
	
	// mRow, mCol 에 해당되는 칸에 NODE 가 없으면 0 리턴

	return result; 
}