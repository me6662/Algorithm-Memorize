#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;
vector <int> Child[10010];
unordered_map <int, int> IDs;
int par[10010], fcnt;
int size[10010], vsize[10010], sub[10010];
 
int getID(int id) {
    if(IDs.find(id) != IDs.end()) return IDs[id];
    int n = IDs.size();
    IDs[id] = ++n;
    return n;
}
 
void init() {
    IDs.clear();
    for(int i = 0; i < 10010; i++) {
        Child[i].clear();
        par[i] = 0;
        size[i] = vsize[i] = sub[i] = 0;
    }
    getID(10000);
    fcnt = 0;
}
 
int cmdAdd(int newID, int pID, int fileSize) {
    if(fileSize) ++fcnt;
    int ni = getID(newID), pi = getID(pID);
    par[ni] = pi;
    Child[pi].push_back(ni);
    size[ni] = sub[ni] = vsize[ni] = fileSize;
    for(int x = pi; x; x = par[x]) sub[x] += fileSize;
    return sub[pi];
}
 
int cmdMove(int tID, int pID) {
    int ti = getID(tID), pi = getID(pID);
    int qi = par[ti], sz = sub[ti];
    Child[qi].erase(find(Child[qi].begin(), Child[qi].end(), ti));
    par[ti] = pi;
    Child[pi].push_back(ti);
    for(int x = qi; x; x = par[x]) sub[x] -= sz;
    for(int x = pi; x; x = par[x]) sub[x] += sz;
    return sub[pi];
}
 
int dfs_infect(int x, int inc) {
    int res = 0;
    if(size[x] > 0) {
        vsize[x] += inc;
        res += 1;
    }
    for(int e : Child[x]) {
        res += dfs_infect(e, inc);
    }
    sub[x] += inc * res;
    return res;
}
 
int cmdInfect(int tID) {
    if(fcnt == 0) return 0;
    int ti = getID(tID);
    int inc = sub[1] / fcnt, old = sub[ti];
    dfs_infect(ti, inc);
    for(int x = par[ti]; x; x = par[x]) sub[x] += sub[ti] - old;
    return sub[ti];
}
 
int dfs_recov(int x) {
    int res = vsize[x] - size[x];
    vsize[x] = size[x];
    for(int e : Child[x]) {
        res += dfs_recov(e);
    }
    sub[x] -= res;
    return res;
}
 
int cmdRecover(int tID) {
    int ti = getID(tID);
    int old = sub[ti];
    dfs_recov(ti);
    for(int x = par[ti]; x; x = par[x]) sub[x] += sub[ti] - old;
    return sub[ti];
}
 
int dfs_rem(int x) {
    int res = (size[x] > 0);
    for(int e : Child[x]) res += dfs_rem(e);
    return res;
}
 
int cmdRemove(int tID) {
    int ti = getID(tID);
    if(ti == 1) {
        int r = sub[1];
        Child[1].clear();
        vsize[1] = size[1] = sub[1] = 0;
        fcnt = 0;
        return r;
    }
    else {
        fcnt -= dfs_rem(ti);
        int pi = par[ti];
        Child[pi].erase(find(Child[pi].begin(), Child[pi].end(), ti));
        for(int x = pi; x; x = par[x]) sub[x] -= sub[ti];
        return sub[ti];
    }
}