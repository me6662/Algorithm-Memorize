#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

const int MAXA = 10011;

struct NODE {
    int fSize;
    int originSize;
    int subSizeSum;
	int subFileCnt;

    NODE* parent;
    vector<NODE*> childs;

    void init() {       
        parent = 0;
        fSize = 0;
        originSize = 0;
        subSizeSum = 0;
		subFileCnt = 0;
        childs.clear();
    }    
};

NODE nodes[MAXA];
int np_cnt;

unordered_map<int, NODE*> idTable;

void init() {

    for (int i = 0; i < MAXA; i++) {
        nodes[i].init();
    }

    idTable.clear();    
    np_cnt = 0;    
    idTable[10000] = &nodes[np_cnt++];
}

int cmdAdd(int newID, int pID, int fileSize) {
    NODE* tNode = &nodes[np_cnt++];
    tNode->init();
    idTable[newID] = tNode;

    NODE* pNode = idTable[pID];
    tNode->parent = pNode; // 부모 등록
    pNode->childs.push_back(tNode); // 부모> 자식등록
	
    tNode->fSize = fileSize;
    tNode->originSize = fileSize;
        
    if (fileSize != 0) { // file 인 경우  
       
		for (NODE* iter = tNode; iter->parent; iter = iter->parent) {
			iter->parent->subSizeSum += fileSize;
			iter->parent->subFileCnt++;
		}
                    
    }

    return pNode->subSizeSum;
}

int cmdMove(int tID, int pID) {
    NODE* tNode = idTable[tID];
    
    // 기존 부모로부터 erase 
   
    // 기존 부모로부터 erase + filsize 전체 바꿈 (subSumSize)
    int delta = 0;
    if (tNode->fSize != 0) delta = tNode->fSize;
    else delta = tNode->subSizeSum;

	int deltaf = 0;
	if (tNode->fSize != 0) deltaf = 1;
	else deltaf = tNode->subFileCnt;


	for (NODE* iter = tNode; iter->parent; iter = iter->parent) {
		iter->parent->subSizeSum -= delta;
		iter->parent->subFileCnt -= deltaf;
	}
        

    NODE* ppNode = tNode->parent;
    ppNode->childs.erase(find(ppNode->childs.begin(), ppNode->childs.end(), tNode));
	

    // 새부모 add
    NODE* npNode = idTable[pID];
    npNode->childs.push_back(tNode);
    tNode->parent = npNode;

    // 새부모 add + filesize 전체 update
	for (NODE* iter = tNode; iter->parent; iter = iter->parent) {
		iter->parent->subSizeSum += delta;
		iter->parent->subFileCnt += deltaf;
	}
        

    return npNode->subSizeSum;
}


void infectDFS(NODE* tNode, int inc) {   
    // dir
    if (tNode->fSize == 0) {

        if (tNode->childs.empty() == false) {            
            for (NODE* child : tNode->childs) {
                infectDFS(child, inc);
            }
        }
    }
    // file
    else {
        
        tNode->fSize += inc;      

        for (NODE* iter = tNode; iter->parent; iter = iter->parent) {
            iter->parent->subSizeSum += inc;
        }
    }
}


int cmdInfect(int tID) {
    if (idTable[10000]->subFileCnt == 0) return 0;

    NODE* tNode = idTable[tID];
        
    int inc = idTable[10000]->subSizeSum / idTable[10000]->subFileCnt;

    // directory
    if (tNode->fSize == 0) {
        if (tNode->childs.empty() == true) return 0;
        infectDFS(tNode, inc);
        return tNode->subSizeSum;
    }
    // file
    else { 
        
        tNode->fSize += inc;

        for (NODE* iter = tNode; iter->parent; iter = iter->parent) {
            iter->parent->subSizeSum += inc;
        }
        return tNode->fSize;
    }
}

void recoverDFS(NODE* tNode) {
    // dir
    if (tNode->fSize == 0) {

        if (tNode->childs.empty() == false) {        
            for (NODE* child : tNode->childs) {
                recoverDFS(child);
            }
        }
    }
    // file
    else {
        int delta = tNode->fSize - tNode->originSize;

        if (delta > 0) {
            
            tNode->fSize = tNode->originSize;

            for (NODE* iter = tNode; iter->parent; iter = iter->parent) {
                iter->parent->subSizeSum -= delta;
            }
        }        
    }
}


int cmdRecover(int tID) {
    NODE* tNode = idTable[tID];

    // directory
    if (tNode->fSize == 0) {
        if (tNode->childs.empty() == true) return 0;
        recoverDFS(tNode);

        return tNode->subSizeSum;
    }
    // file
    else {
        int delta = tNode->fSize - tNode->originSize;

        if (delta > 0) {

            tNode->fSize = tNode->originSize;

            for (NODE* iter = tNode; iter->parent; iter = iter->parent) {
                iter->parent->subSizeSum -= delta;
            }
        }

        return tNode->fSize;
    }

}




int cmdRemove(int tID) {
    NODE* tNode = idTable[tID];

	// if rooot

	if (tID == 10000) {
		int res = idTable[10000]->subSizeSum;
		init();
		return res;
	}

    // directory
    if (tNode->fSize == 0) {
        NODE* pNode = tNode->parent;

        pNode->childs.erase(find(pNode->childs.begin(), pNode->childs.end(), tNode));

        for (NODE* iter = tNode; iter->parent; iter = iter->parent) {
			iter->parent->subSizeSum -= tNode->subSizeSum;
			iter->parent->subFileCnt -= tNode->subFileCnt;
        }

        return tNode->subSizeSum;
    }
    // file
    else {
        NODE* pNode = tNode->parent;

        pNode->childs.erase(find(pNode->childs.begin(), pNode->childs.end(), tNode));		

        for (NODE* iter = tNode; iter->parent; iter = iter->parent) {
			iter->parent->subSizeSum -= tNode->fSize;
			iter->parent->subFileCnt--;

        }
		
        return tNode->fSize;
    }

}
