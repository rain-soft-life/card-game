#ifndef GRAPH_H
#define GRAPH_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//无穷值，表示两点之间无边
#define INF 0x7fffffff
//图类型
typedef enum {
    DIGRAPH,
    UNDIGRAPH
} GraphType;
//图结构体/邻接矩阵存储
typedef struct {
    GraphType type;//图类型
    int vertexNum;//顶点总数
    int edgeNum;//边总数
    int* vertexData;//顶点数据数组
    int** matrix;//二维邻接矩阵
} Graph;
Graph* CreateGraph(GraphType type, int vNum);
void SetVertexData(Graph* g, int idx, int data);
void AddEdge(Graph* g, int start, int end, int weight);
void PrintMatrix(Graph* g);
void DFS(Graph* g, int start);
void BFS(Graph* g, int start);
void DestroyGraph(Graph* g);

#endif