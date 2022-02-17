#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

long long *array;
int cmp(const void *a, const void *b){
    long long ia = *(long long *)a;
    long long ib = *(long long *)b;
    return array[ia] < array[ib] ? -1 : array[ia] > array[ib];
}
int isValid(int mbikes, int nmen, int k, int z, long long index[]);

int main() {
    //find the k shortes edges "in the bipartite graph" between men & bikes
    //performance metric is the max distance among the k pairs
    //this is a min max problem, minimizing the max distance
    int nmen,mbikes,kspots;
    scanf("%d %d %d",&nmen,&mbikes,&kspots);
    long men[nmen][2], bikes[mbikes][2];
    for(int i=0;i<nmen;i++){scanf("%ld %ld",&men[i][0],&men[i][1]);}
    for(int i=0;i<mbikes;i++){scanf("%ld %ld",&bikes[i][0],&bikes[i][1]);}
    long long d,dists[mbikes*nmen];
    for(int i=0;i<mbikes;i++){
        for(int j=0;j<nmen;j++){
            d=(bikes[i][0]-men[j][0]);
            dists[i*nmen+j]=d*d;
            d=(bikes[i][1]-men[j][1]);
            dists[i*nmen+j]+=d*d;          
        }
    }
    //sort distances, only really need k smallest from each bike
    //discard those that are larger (but not those that are equal)
    long long index[mbikes*nmen];//use malloc to large size array
    for(long i=0;i<mbikes*nmen;i++){index[i] = i;}
    array = dists;
    qsort(index, mbikes*nmen, sizeof(*index), cmp);
    //for(long i=0;i<mbikes*nmen;i++){printf("%lld ",dists[index[i]]);} printf("\n");
    
    int last=kspots;   
    //do binary search to find out minimum dist that allows a valid assignment
    int left=0, right=mbikes*nmen, width=mbikes*nmen, mid;
    while(width>4){
        width/=2; mid=(left+right)/2; 
        //printf("Check %d\n",mid);
        if(!isValid(mbikes,nmen,kspots,mid,index)){ left=mid; }
        else{right=mid;}
    }
    last=right;
    for (int j=left;j<right;j++){
        //printf("Check %d\n",j);
        if(isValid(mbikes,nmen,kspots,j,index)) {last=j;break;}
    } 
    printf("%lld",dists[index[last-1]]);
    return 0;
}

#define WHITE 0
#define GRAY 1
#define BLACK 2
#define MAX_NODES 1000
#define oo 1000000000
int n;  // number of nodes
int e;  // number of edges
int capacity[MAX_NODES][MAX_NODES]; // capacity matrix
int flow[MAX_NODES][MAX_NODES];     // flow matrix
int color[MAX_NODES]; // needed for breadth-first search               
int pred[MAX_NODES];  // array to store augmenting path
int max_flow (int source, int sink);
int isValid(int mbikes, int nmen, int k, int z, long long index[]){
    //check if we can pick k unique row/col pairs among the first z
    //this is a matching of cardinality k in the bipartite ii-jj graph
    if(z<k) return 0;
    //capacity rows 0-249, cols 250-499, source as 500, sink as 501
    for(int i=0;i<500;i++){
        for(int j=0;j<500;j++){capacity[i][j]=0;}
    }   
    for(int i=0;i<250;i++){capacity[500][i]=1;}
    for(int i=0;i<250;i++){capacity[250+i][501]=1;}
    for(int i=0;i<z;i++){
        int ii=index[i]/nmen;
        int jj=index[i]%nmen;
        capacity[ii][250+jj]=1;
    }
    n=502; e=z+2; 
    int maxflow=max_flow(500,501);
    //printf("Max flow for z= %d\n",maxflow);
    if(maxflow>=k) return 1;
    else return 0;
}
// below follows Ford-Fulkerson algorithm for max matching via max flow

int min (int x, int y) {
    return x<y ? x : y;  // returns minimum of x and y
}
int head,tail;
int q[MAX_NODES+2];
void enqueue(int x){q[tail] = x; tail++; color[x] = GRAY;}
int dequeue(){int x = q[head]; head++; color[x] = BLACK; return x;}
int bfs (int start, int target) {
    int u,v;
    for (u=0; u<n; u++) { color[u] = WHITE; }   
    head = tail = 0;
    enqueue(start);
    pred[start] = -1;
    while (head!=tail) {
        u = dequeue();
        // Search all adjacent white nodes v. If the capacity
        // from u to v in the residual network is positive, enqueue v.
        for (v=0; v<n; v++) {
            if (color[v]==WHITE && capacity[u][v]-flow[u][v]>0) {
                enqueue(v); pred[v] = u;
            }
        }
    }
    // If the color of the target node is black now, it means that we reached it.
    return color[target]==BLACK;
}
int max_flow (int source, int sink) {
    int i,j,u;
    // Initialize empty flow.
    int max_flow = 0;
    for (i=0; i<n; i++) {
        for (j=0; j<n; j++) {
            flow[i][j] = 0;
        }
    }
    // While there exists an augmenting path, increment the flow along this path.
    while (bfs(source,sink)) {
        // Determine the amount by which we can increment the flow.
        int increment = oo;
        for (u=n-1; pred[u]>=0; u=pred[u]) {
            increment = min(increment,capacity[pred[u]][u]-flow[pred[u]][u]);
        }
        // Now increment the flow.
        for (u=n-1; pred[u]>=0; u=pred[u]) {
            flow[pred[u]][u] += increment; flow[u][pred[u]] -= increment;
        }
        max_flow += increment;
    }
    // No augmenting path anymore. We are done.
    return max_flow;
}
                    
