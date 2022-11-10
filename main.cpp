#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#define INF 1000000000
#define sz(a) (int)a.size()
#define pb(a) push_back(a)
#define mset(a,val) memset(a,val,sizeof(a))
#define REP(i,n) for(__typeof(n) i=0;i<(n);++i)
#define FORE(it,c) for(__typeof((c).begin()) it=(c).begin();it!=(c).end();++it)
 
#define pii pair<int,int>
#define mp(a,b) make_pair(a,b)
#define fi first
#define se second

typedef long long ll;
using namespace std;
struct node//segtree node
{
	int val;
};
 
node MND(int a)
{
	node t={a};
	return t;
}
 
node getNode(node A,node B)
{
	A.val=max(A.val,B.val);
	return A;
}
 
// ---------- LOWEST COMMON ANCESTOR + HEAVY LIGHT DECOMPOSITION-----------
#define MAXN 10005
#define LOGN 16
int N,PATHC;
vector<pii> G[MAXN];//GRAPH
 
int T[MAXN];//PARENT
int D[MAXN];//DIST TO PARENT
int L[MAXN];//LEVEL
int ANC[MAXN][LOGN];//MAXN,log2(MAXN)  ANC[i][j] = 2^j-th ancestor of i
 
int getPATH[MAXN];//what heavy path i-th node belongs to
int getPPOS[MAXN];//position of the i-th node in its heavy path
vector<int> PATHS[MAXN];//nodes of i-th heavy path
vector<node> SEGTREE[MAXN];//segment tree for i-th path
pii EDGE[MAXN];
 
void initStuff()
{
	REP(i,N)G[i].clear(),PATHS[i].clear(),SEGTREE[i].clear();	
}
 
int makeT(int i,int par,int lev)
{
	int maxi=-1,ind=0,cur=0,size=1;
	T[i]=par;
	L[i]=lev;	
 
	//find the biggest subtree
	FORE(it,G[i])
		if (it->first!=par)
		{
			cur=makeT(it->first,i,lev+1);
			D[it->first]=it->second;
			size+=cur;
			if (cur>maxi) maxi=cur,ind=it->first;
		}
 
	//extend the right path
	if(maxi==-1)
	{//we are in the leaf
		getPATH[i]=PATHC++;
	}else	
		getPATH[i]=getPATH[ind];
 
	getPPOS[i]=sz(PATHS[getPATH[i]]);
	PATHS[getPATH[i]].pb(i);
 
	return size;
}
 
void initLCA()
{
	mset(ANC,-1);	
	REP(i,N)
		ANC[i][0]=T[i];	
	for(int j=1;(1<<j)<N;j++)
		REP(i,N)
			if (ANC[i][j-1]!=-1)
				ANC[i][j]=ANC[ANC[i][j-1]][j-1];
}
 
inline int getAncestor(int ind,int k)//gets k-th ancestor of node ind
{
	for(int i=LOGN-1;ind!=-1&&i>=0;i--)
		if (k>=(1<<i))
		{
			k-=(1<<i);
			ind=ANC[ind][i];
		}
	return ind;
}
inline int getLCA(int a,int b)//returns lowest common ancestor of a and b
{
	if (L[a]<L[b]) swap(a,b);
	if (L[a]!=L[b])
		a=getAncestor(a,L[a]-L[b]);
	if (a==b) return a;
	for(int i=LOGN-1;i>=0;i--)
		if (ANC[a][i]!=-1&&ANC[a][i]!=ANC[b][i])
			a=ANC[a][i],
			b=ANC[b][i];
	return T[a];
}
//---------------------------------------------

// ------------------ SEGMENT TREE ------------------------------
void makeSEGTREE(int lo,int hi,int indT,int indP)//lo,hi - interval, indT - tree index, indP - path index
{
	assert(indT<sz(SEGTREE[indP]));	
	if (lo+1==hi)
		SEGTREE[indP][indT]=MND(D[PATHS[indP][lo]]);
	else
	{
		int mid=(lo+hi)/2;
		makeSEGTREE(lo,mid,indT*2,indP);	
		makeSEGTREE(mid,hi,indT*2+1,indP);
		SEGTREE[indP][indT]=getNode(SEGTREE[indP][indT*2],SEGTREE[indP][indT*2+1]);
	}			
}
void updateSEGTREE(int lo,int hi,int indT,int indP,int updatePos)//lo,hi - interval, indT - tree index, indP - path index
{
	assert(indT<sz(SEGTREE[indP]));	
	if (lo+1==hi)
		SEGTREE[indP][indT]=MND(D[PATHS[indP][lo]]);
	else
	{
		int mid=(lo+hi)/2;
		if (updatePos<mid)
			updateSEGTREE(lo,mid,indT*2,indP,updatePos);	
		else
			updateSEGTREE(mid,hi,indT*2+1,indP,updatePos);
		SEGTREE[indP][indT]=getNode(SEGTREE[indP][indT*2],SEGTREE[indP][indT*2+1]);
	}			
}
node querySEGTREE(int lo,int hi,int indT,int indP,int qX,int qY)
{	
	assert(indT<sz(SEGTREE[indP]));
  if (qX<=lo&&hi<=qY) return SEGTREE[indP][indT];
  if (lo+1==hi) return SEGTREE[indP][indT];
 
  int mid=(lo+hi)/2;
 
  node L=MND(-INF),R=MND(-INF);
 
  if (max(lo,qX)<min(qY,mid))
    L=querySEGTREE(lo,mid,indT*2,indP,qX,qY);
  if (max(mid,qX)<min(qY,hi))
    R=querySEGTREE(mid,hi,indT*2+1,indP,qX,qY);
 
  if (L.val==-INF) return R;
  if (R.val==-INF) return L;
 
  return getNode(L,R);
}
// ---------------------------------------------------------------
 
int queryTree(int ind,int tar)//computes the maximum edge on the path ind->tar where L[ind]>=L[tar]
{
	if (ind==tar) return 0;
 
	if (getPATH[ind]==getPATH[T[ind]])
	{//we can jump forward - HEAVY EDGE
		int cPath=getPATH[ind];
		int end=PATHS[cPath].back();
		end=L[end]>=L[tar]?end:tar;
 
		int val1=queryTree(end,tar);
		int val2=querySEGTREE(0,sz(PATHS[cPath])-1,1,cPath,getPPOS[ind],getPPOS[end]).val;
		return max(val1,val2);
	}else
	{//we have to move - LIGHT EDGE		
		return max(queryTree(T[ind],tar),D[ind]);	
	}	
}
 
 
int cases,a,b,c;
char buf[16];
int main( )
{
	scanf("%d",&cases);
	REP(ii,cases)
	{
		scanf("%d",&N);
		initStuff();
		REP(i,N-1)
		{
			scanf("%d %d %d",&a,&b,&c),a--,b--;
			EDGE[i]=mp(a,b);
			G[a].pb(mp(b,c));
			G[b].pb(mp(a,c));			
		}
		PATHC=0;
		makeT(0,-1,0);
		initLCA();
		REP(i,PATHC)
		if (sz(PATHS[i])>1)		
		{//init segment tree
			SEGTREE[i].resize(sz(PATHS[i])*5);
			makeSEGTREE(0,sz(PATHS[i])-1,1,i);
		}
 
		while(scanf("%s",buf),buf[0]!='D')
		{
			scanf("%d %d",&a,&b);
			if (buf[0]=='C')
			{//CHANGE i ti : change the cost of the i-th edge to ti
				a--;
				int A=EDGE[a].fi;
				int B=EDGE[a].se;
				if (L[A]<L[B])swap(A,B);//A is child
				D[A]=b;
				int curPath=getPATH[A];
				if (PATHS[curPath].back()!=A)
					updateSEGTREE(0,sz(PATHS[curPath])-1,1,curPath,getPPOS[A]);
			}else
			{//QUERY a b : ask for the maximum edge cost on the path from node a to node b 
				a--,b--;
				int LCA=getLCA(a,b);
				int res=max(queryTree(a,LCA),queryTree(b,LCA));
				printf("%d\n",res);				
			}
		}
		if (ii<cases-1)printf("\n");
	}
 
  return 0;
} 
