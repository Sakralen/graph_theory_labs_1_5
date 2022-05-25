#pragma once

#include "global.h"

#define WEIGHT_MAX 15
#define VERT_CNT_MAX 50
#define VERT_CNT_MIN 2	
#define INF 999
#define FLOW_MAX 20
//#define MIN_EDGES_CNT(p) (p - 1)				
//#define MAX_EDGES_CNT(p) ((p * (p - 1)) / 2)

struct McfRetVals {
	iMx paths;
	vector<int> flows;
	vector<int> costsPerPath;
	iMx modCostMx;
	iMx modFlowMx;
};

enum class ShimbellMode {
	kShortest = 0,
	kLongest
};

enum class WeightsType {
	kPositive,
	kMixed,
	kModifiedPos,
	kModifiedMixed
};

class MyGraph {
private:
	int vertexCnt;
	iMx adjacencyMatrix;
	iMx posWeightsMatrix;
	iMx mixedWeightsMatrix;
	iMx modPosWeightsMx;
	iMx modMixedWeightsMx;
	iMx reachMatrix;
	iMx maxFlowMx;

	vector<int> PrepVertices() const;
	void AssignWeights(/*WeightsType type = WeightsType::kPositive*/);
	void MixWeights(/*WeightsType type = WeightsType::kMixed*/);
	void ModifyWeights(iMx& WeightsMatrix, iMx& modified);

	iMx ShimbellMult(const iMx mxA, const iMx mxB, ShimbellMode mode) const;
public:
	MyGraph(int n);
	int GetVertexCount() const;
	iMx GetAdjMatrix() const;
	iMx GetWeightsMatrix(WeightsType type/* = WeightsType::kPositive*/) const;
	iMx GetReachMatrix() const;

	iMx CalcShimbell(int edgeCnt, ShimbellMode mode) const;
	void GenReachMatrix();
	vector<int> Dijkstra(int inpVert, int& counter) const;
	vector<int> Dijkstra_queue(int inpVert, int& counter) const;
	iMx RestorePaths(int inpVert, const vector<int>& distances, const iMx weightMx) const;
	vector<int> BellmanFord(int inpVert, iMx wieghtsMx, int& counter) const;
	iMx FloydWarshall(int& counter) const;

	iMx GetMaxFlowMatrix() const;
	void GenMaxFlowMx();
	iMx AddFictVert() const;
	bool bfs_FordFulkerson(iMx residual, int source, int sink, vector<int>& path) const;
	int fordFulkerson(int source, int sink) const;
	int ÑalcMinCostFlow(int source, int sink, int flow, McfRetVals& retVals) const;
};

