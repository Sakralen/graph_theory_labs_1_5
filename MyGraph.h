#pragma once

#include "global.h"

#define WEIGHT_MAX 15
#define VERT_CNT_MAX 50
#define VERT_CNT_MIN 2	
#define INF 999
//#define MIN_EDGES_CNT(p) (p - 1)				
//#define MAX_EDGES_CNT(p) ((p * (p - 1)) / 2)

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
	vector<vector<int>> adjacencyMatrix;
	vector<vector<int>> posWeightsMatrix;
	vector<vector<int>> mixedWeightsMatrix;
	vector<vector<int>> modPosWeightsMx;
	vector<vector<int>> modMixedWeightsMx;
	vector<vector<int>> reachMatrix;

	vector<int> PrepVertices() const;
	void AssignWeights(/*WeightsType type = WeightsType::kPositive*/);
	void MixWeights(/*WeightsType type = WeightsType::kMixed*/);
	void ModifyWeights(vector<vector<int>>& WeightsMatrix, vector<vector<int>>& modified);

	vector<vector<int>> ShimbellMult(const vector<vector<int>> mxA, const vector<vector<int>> mxB, ShimbellMode mode) const;
public:
	MyGraph(int n);
	int GetVertexCount() const;
	vector<vector<int>> GetAdjMatrix() const;
	vector<vector<int>> GetWeightsMatrix(WeightsType type/* = WeightsType::kPositive*/) const;
	vector<vector<int>> GetReachMatrix() const;

	vector<vector<int>> CalcShimbell(int edgeCnt, ShimbellMode mode) const;
	void GenReachMatrix();
	vector<int> Dijkstra(int inpVert, int& counter) const;
	vector<int> Dijkstra_queue(int inpVert, int& counter) const;
	vector<vector<int>> RestorePaths(int inpVert, const vector<int>& labels) const;
	vector<int> BellmanFord(int inpVert, int& counter) const;
};

