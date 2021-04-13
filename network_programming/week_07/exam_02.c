#include <stdio.h>
#include <limits.h>
#define TRUE 1
#define FALSE 0
#define MAX_NODES 10

int distance[MAX_NODES];	//시작 정점으로 부터 각 정점까지의 거리비용
int found[MAX_NODES] = {FALSE};		// 최소 거리 계산이 완료된 정점을 표시
typedef struct GraphType {
	int n;
	int cost[MAX_NODES][MAX_NODES];
}GraphType;
void print_status(GraphType* g,int step)
{
	int i;
	printf("STEP %d\n",step);
	printf("\tdistance: ");		// distance 출력
	for(i = 0;i< g->n;i++)
	{
		if(distance[i] != INT_MAX)	// 인접한 노드인 경우
			printf("\t%d",distance[i]);	
		else				// 인접한 노드가 아닌경우
			printf("\t*");		// *로 출력
	}printf("\n");
	
	printf("\tfound:");			// found 출력
	for(i = 0;i < g->n; i++)
	{
		printf("\t%d",found[i]);
	}

}

int choose(int distance[], int n, int found[])
{
	int i, min_dis,min_index;
	min_dis = INT_MAX;
	for(i = 0; i < n; i++)
	{
		if(!found[i])		// 최단경로 계산이 완료되지 않은 정점(FALSE인 부분)
		{	
			if(distance[i] < min_dis)
			{
				min_dis = distance[i];		// 가장 작을때의 값저장
				min_index = i;			// 가장 작을떄의 인덱스 값저장
			}
		}
	}
	return min_index;
}

int test_found(int n)		// 전역변수 found에서 0~ n까지의  데이터가 전부 1인지 확인한다.
{
	int i,sf;
	for (sf = 1,i=0;i < n ;i++)			// found[1] ~ found[n]까지 모든 데이터가 1인지 테스트
	{
		sf = sf & found[i];			// and연산을 하여 전부 1이면 sf는 1이다.
		if(!sf)
			return 0;			// found데이터의 하나라도 0이면 0이다.
	}
	return 1;
	
}

void shortest_path(GraphType* g, int start_node)
{
	int i,sf,z;
	found[start_node] = TRUE;			// 자기 자신의 거리는 알고있다.
	for (i = 0;i < g->n;i++)			// Step 0번과같은 초기와 작업
	{
		distance[i] = g->cost[start_node][i];	// 현재 start_node의 인접 노트들의 거리 저장
	}
	while(!test_found(g->n))			// found배열에 전부 1이 들어갈 떄 까지 반복
	{
		print_status(g,start_node);
		sf = choose(distance,MAX_NODES,found);	// 다음 최소 거리를 가지는 인덱스 반환
		found[sf] = TRUE;			// 해당 노드는 최소 거리를 찾음
		for(i = 0;g->n;i++)			// 노드 갯수만큼 반복
		{
			if(!found[i]){			// 최소 거리 노드를 가지지 않는 정점만 수행
				if((distance[sf] + g->cost[sf][i])  < distance[i])
					distance[i] = (distance[sf] + g->cost[sf][i]);
			}
		}
	}

}
int main(int argc,char *argv[])
{
	int start_node_index = 1;
	GraphType G = { 4,
				   {{0,5,3,7,INT_MAX,INT_MAX},
					{5,0,4,INT_MAX,7,9},
					{3,4,0,3,8,INT_MAX},
					{7,INT_MAX,3,4,INT_MAX},
					{INT_MAX,7,8,4,2},
					{INT_MAX,9,INT_MAX,INT_MAX,2}
				   }};
	shortest_path(&G, start_node_index);

	return 0;
}
