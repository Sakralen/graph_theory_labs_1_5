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

struct Edge {
	int vert1;
	int vert2;
	int weight;

	Edge(int v1 = INF, int v2 = INF, int w = INF) : vert1(v1), vert2(v2), weight(w) { }

	bool operator > (const Edge& e) const {
		return this->weight > e.weight;
	}

	bool operator < (const Edge& e) const {
		return this->weight < e.weight;
	}
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

enum class IsEulerOrHamilton {
	kFalse2Vert,
	kFalseUnmodifiable,
	kFalseModifiable,
	kTrue
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

	void GenReachMatrix();

	void GenMaxFlowMx();
	iMx AddFictVert() const;
	bool bfs(iMx graphMx, int v1, int v2, vector<int>* path = nullptr, int* counter = nullptr) const;
	bool bfs_FordFulkerson(iMx residual, int source, int sink, vector<int>& path) const;

	priority_queue<Edge, vector<Edge>, std::greater<Edge>> SortEdges(iMx weightsMx) const;
	iMx MakeUnoriented(const iMx weightsMx) const;
	vector<int> CalcDegrees(iMx weightsMx) const;

public:
	MyGraph(int n);
	int GetVertexCount() const;
	iMx GetAdjMatrix() const;
	iMx GetWeightsMatrix(WeightsType type/* = WeightsType::kPositive*/) const;
	iMx GetReachMatrix() const;
	iMx GetMaxFlowMatrix() const;

	iMx CalcShimbell(int edgeCnt, ShimbellMode mode) const;
	//bool IsReachable(int vert1, int vert2) const;
	//bool IsReachableNotOriented(int vert1, int vert2) const;
	
	vector<int> Dijkstra(int inpVert, int& counter) const;
	vector<int> Dijkstra_queue(int inpVert, int& counter) const;
	iMx RestorePaths(int inpVert, const vector<int>& distances, const iMx weightMx) const;
	vector<int> BellmanFord(int inpVert, iMx wieghtsMx, int& counter) const;
	iMx FloydWarshall(int& counter) const;

	int fordFulkerson(int source, int sink) const;
	int ÑalcMinCostFlow(int source, int sink, int flow, McfRetVals& retVals) const;

	iMx Kruskal(iMx weightsMx, int* counter = nullptr, int* sum = nullptr) const;
	iMx Prim(iMx weightsMx, int* counter = nullptr, int* sum = nullptr) const;
	iMx GenKirchhoff() const;
	int CalcSpanTreesCnt() const;
	void PruferEncode(iMx& weightsMx, vector<int>& pruferCode, vector<int>& pruferWeights) const;
	iMx PruferDecode(vector<int>& pruferCode, vector<int>& pruferWeights) const;

	IsEulerOrHamilton IsEuler(iMx weightsMx) const;
	vector<int> EulerCycles(iMx weightsMx, iMx& modWeightsMx, IsEulerOrHamilton& isEulerRes) const;
	vector<int> Hamilton(iMx weightsMx, iMx& modWeightsMx, IsEulerOrHamilton& isHamRes, int& minLen) const;
	void FindHamiltonCycles(ofstream& ofs, iMx& weightsMx, vector<int>& path, vector<int>& minPath, int& len, int& minLen) const;
	//void findHamiltonCycle(ofstream& fout, vector<vector<int>>& graph,
	//	vector<int>& path, int length, vector<int>& minimumPath, int& minimumLength) const;
};

