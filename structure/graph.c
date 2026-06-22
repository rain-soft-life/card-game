#include "graph.h"

//内部递归DFS辅助函数
static void DFS_Recursion(Graph* g, int cur, bool* visited)
{
    //输出当前顶点
    printf("%d ", g->vertexData[cur]);
    visited[cur] = true;

    //遍历所有邻接点
    for (int j = 0; j < g->vertexNum; j++)
    {
        if (g->matrix[cur][j] != INF && !visited[j])
        {
            DFS_Recursion(g, j, visited);
        }
    }
}

Graph* CreateGraph(GraphType type, int vNum)
{
    if (vNum <= 0)
    {
        printf("顶点数量非法\n");
        return NULL;
    }

    Graph* g = (Graph*)malloc(sizeof(Graph));
    g->type = type;
    g->vertexNum = vNum;
    g->edgeNum = 0;

    //分配顶点数组
    g->vertexData = (int*)malloc(sizeof(int) * vNum);

    //分配二维邻接矩阵
    g->matrix = (int**)malloc(sizeof(int*) * vNum);
    for (int i = 0; i < vNum; i++)
    {
        g->matrix[i] = (int*)malloc(sizeof(int) * vNum);
        //矩阵初始化为无穷（无边）
        for (int j = 0; j < vNum; j++)
        {
            g->matrix[i][j] = INF;
        }
    }
    return g;
}

void SetVertexData(Graph* g, int idx, int data)
{
    if (g == NULL || idx < 0 || idx >= g->vertexNum)
        return;
    g->vertexData[idx] = data;
}

void AddEdge(Graph* g, int start, int end, int weight)
{
    if (g == NULL) return;
    int vCnt = g->vertexNum;
    if (start < 0 || start >= vCnt || end < 0 || end >= vCnt)
    {
        printf("顶点下标越界，添加边失败\n");
        return;
    }
    //赋值起点->终点
    g->matrix[start][end] = weight;
    g->edgeNum++;
    //无向图双向赋值
    if (g->type == UNDIGRAPH)
    {
        g->matrix[end][start] = weight;
    }
}

void PrintMatrix(Graph* g)
{
    if (g == NULL) return;
    int v = g->vertexNum;
    printf("===== 邻接矩阵 =====\n");
    for (int i = 0; i < v; i++)
    {
        for (int j = 0; j < v; j++)
        {
            if (g->matrix[i][j] == INF)
                printf("∞\t");
            else
                printf("%d\t", g->matrix[i][j]);
        }
        printf("\n");
    }
    printf("====================\n");
}

void DFS(Graph* g, int start)
{
    if (g == NULL || start < 0 || start >= g->vertexNum)
        return;
    int v = g->vertexNum;
    bool* visited = (bool*)calloc(v, sizeof(bool));

    printf("DFS深度遍历：");
    DFS_Recursion(g, start, visited);
    printf("\n");

    free(visited);
}

void BFS(Graph* g, int start)
{
    if (g == NULL || start < 0 || start >= g->vertexNum)
        return;
    int v = g->vertexNum;
    bool* visited = (bool*)calloc(v, sizeof(bool));
    //简易循环队列
    int* queue = (int*)malloc(sizeof(int) * v);
    int front = 0, rear = 0;

    printf("BFS广度遍历：");
    queue[rear++] = start;
    visited[start] = true;

    while (front < rear)
    {
        int cur = queue[front++];
        printf("%d ", g->vertexData[cur]);
        //查找所有邻接点入队
        for (int j = 0; j < v; j++)
        {
            if (g->matrix[cur][j] != INF && !visited[j])
            {
                visited[j] = true;
                queue[rear++] = j;
            }
        }
    }
    printf("\n");

    free(visited);
    free(queue);
}

void DestroyGraph(Graph* g)
{
    if (g == NULL) return;
    //释放二维矩阵每一行
    for (int i = 0; i < g->vertexNum; i++)
    {
        free(g->matrix[i]);
    }
    free(g->matrix);
    free(g->vertexData);
    free(g);
}