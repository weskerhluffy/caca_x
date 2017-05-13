#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <bitset>
#include <cctype>
#include <cstdio>
#include <string>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <algorithm>
typedef long long LL;
typedef std::pair<int, int> PII;
#define PB push_back
#define MP std::make_pair
#define AA first
#define BB second
#define OP begin()
#define ED end()
#define SZ size()
#define SQ(x) ((x)*(x))
#define SSP system("pause")
#define cmin(x,y) x=std::min(x,y)
#define cmax(x,y) x=std::max(x,y)
const double eps = 1e-8;
const double INF = 1e20;
const double PI = acos(-1.);
const int MXN = 50;
const LL MOD = 1000000007;

#define Left pz,mid
#define Right mid+1,py
const int Maxn = 50005;
const int Maxq = 100005;
const int Maxm = 4000005;

struct node {
	int l, r;
	LL v;
} pl[Maxm];
int pcnt, n;

void newnode(int &p) {
	pl[p = ++pcnt].v = 0;
	assert(pcnt <= 4000000);
	pl[p].l = -1;
	pl[p].r = -1;
}
void refresh(int& p, int pz, int py, int pos, LL vv) {
	int mid = (pz + py) >> 1;
	if (p < 0)
		newnode(p);
	if (pz == py) {
		pl[p].v += vv;
		return;
	}
	if (pos <= mid)
		refresh(pl[p].l, Left, pos, vv);
	else
		refresh(pl[p].r, Right, pos, vv);
	pl[p].v = 0;
	if (pl[p].l != -1) {
		pl[p].v = pl[pl[p].l].v;
	}
	if (pl[p].r != -1) {
		pl[p].v += pl[pl[p].r].v;
	}
}
LL query(int p, int pz, int py, int zz, int yy) {
	printf("dentro de query p %d\n", p);
	int mid = (pz + py) >> 1;
	LL caca = 0;
	if (p < 0) {
		caca = 0;
	} else {
		if (pz == zz && py == yy) {
			caca = pl[p].v;
		} else {
			if (yy <= mid) {
				caca = query(pl[p].l, Left, zz, yy);
			} else {
				if (mid < zz) {
					caca = query(pl[p].r, Right, zz, yy);
				} else {
					LL t1 = query(pl[p].l, Left, zz, mid);
					LL t2 = query(pl[p].r, Right, mid + 1, yy);
					caca = t1 + t2;
				}
			}
		}
	}
	printf("p %u pz %u py %u zz %u yy %u regreso %d \n", p, pz, py, zz, yy,
			caca);
	return caca;
}
struct BIT {
	int n, r[Maxn];
	void init(int m) {
		n = m + 5;
		for (int i = 0; i <= n; i++)
			r[i] = -1;
	}
	void add(int x, int y, LL vv) {
		for (x += 3; x <= n; x += x & -x) {
			printf("refrescando x:%u r x:%d y:%u vv:%d\n", x - 3, r[x], y, vv);
			refresh(r[x], 1, n + 1, y, vv);
			printf("refrescado  x:%u r x:%d y:%u vv:%d\n", x - 3, r[x], y, vv);
		}
	}
	void add(int x1, int y1, int x2, int y2, LL vv) {
		printf("refrescando arriba x1:%u y1:%u x2:%u y2:%u vv:%d\n", x1, y1, x2,
				y2, vv);
		printf("refre 1 \n");
		add(x1, y1, vv);
		printf("refre 2 \n");
		add(x1, y2 + 1, -vv);
		printf("refre 3 \n");
		add(x2 + 1, y1, -vv);
		printf("refre 4 \n");
		add(x2 + 1, y2 + 1, vv);
	}
	LL sum(int x, int y1, int y2) {
		LL ret = 0;
		for (x += 3; x > 0; x -= x & -x) {
			printf("queriando x:%u r x:%d y1:%u y2:%u\n", x - 3, r[x], y1, y2);
			ret += query(r[x], 1, n + 1, y1, y2);
		}
		return ret;
	}
} B;
struct Query {
	char tp;
	int x, y, id;
	LL ans;
} q[Maxq];
int a[Maxn];
std::map<int, std::set<int> > pos;
bool cmp1(Query A, Query B) {
	if (A.tp != B.tp)
		return A.tp == 'U';
	if (A.y != B.y)
		return A.y < B.y;
	return A.x < B.x;
}
bool cmp2(Query A, Query B) {
	return A.id < B.id;
}
void getRange(int x, int y, PII &range) {
	std::set<int>::iterator it;
	std::set<int>&th = pos[y];
	it = th.lower_bound(x);
	printf("obteniando rango x %u y %d\n", x, y);
	if (it == th.begin())
		range.AA = 0;
	else
		range.AA = *(--it);
	it = th.upper_bound(x);
	if (it == th.end())
		range.BB = n + 1;
	else
		range.BB = *it;

	printf("rango resultante AA %u BB %u\n", range.AA, range.BB);
}
int main() {
//    freopen("9066in.txt","r",stdin);
//    freopen("9066out.txt","w",stdout);
	int i, j, nq, root;
	memset(pl, 0, sizeof(pl));
	memset(B.r, 0, sizeof(B.r));
	memset(q, 0, sizeof(q));
	scanf("%d", &n);
	printf("leido n %d\n", n);
	for (i = 1; i <= n; i++)
		scanf("%d", &a[i]);
	scanf("%d", &nq);
	for (i = 1; i <= nq; i++) {
		char buff[2];
		scanf("%s%d%d", buff, &q[i].x, &q[i].y);
		q[i].tp = buff[0];
		q[i].id = i;
	}
	std::sort(q + 1, q + nq + 1, cmp1);
	pos.clear();
	pcnt = 0;
	newnode(root);
	for (j = 1; j <= nq && q[j].tp == 'U'; j++)
		;
	for (i = 1; i <= n; i++) {
		std::set<int>::iterator it;
		std::set<int>&th = pos[a[i]];
		if (!th.empty()) {
			it = th.end();
			--it;
			refresh(root, 1, n, *it, -a[i]);
			refresh(root, 1, n, i, a[i]);
			th.insert(i);
		} else {
			refresh(root, 1, n, i, a[i]);
			th.insert(i);
		}
		while (j <= nq && q[j].y <= i) {
			q[j].ans = query(root, 1, n, q[j].x, q[j].y);
			j++;
		}
	}
	std::sort(q + 1, q + nq + 1, cmp2);
	pcnt = 0;
	printf("aora n es %d\n", n);
	B.init(n);
	for (i = 1; i <= nq; i++) {
		if (q[i].tp == 'Q') {
			LL del = B.sum(q[i].x, 1, q[i].y);
			printf("%lld\n", del + q[i].ans);
		} else {
			PII range;
			getRange(q[i].x, a[q[i].x], range);
			B.add(range.AA + 1, q[i].x, q[i].x, range.BB - 1, -a[q[i].x]);
			getRange(q[i].x, q[i].y, range);
			B.add(range.AA + 1, q[i].x, q[i].x, range.BB - 1, q[i].y);
			pos[a[q[i].x]].erase(q[i].x);
			pos[q[i].y].insert(q[i].x);
			a[q[i].x] = q[i].y;
		}
	}
	return 0;
}
