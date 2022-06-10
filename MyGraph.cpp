#include "MyGraph.h"

vector<int> MyGraph::PrepVertices() const {	//генерирует отсортированный вектор размера (n - 2) степеней вершин
	vector<int> vertexDegrees;
	int tmpVertDeg;
	for (int i = 0; i < vertexCnt - 2; i++) {
		do {
			tmpVertDeg = FurryRng();
		} while (tmpVertDeg >= vertexCnt);
		vertexDegrees.push_back(tmpVertDeg);
	}
	std::sort(vertexDegrees.begin(), vertexDegrees.end(), std::greater<int>());
	for (int i = 0; i < vertexCnt - 2; i++) {
		if (vertexDegrees[i] > (vertexCnt - 1 - i)) {
			vertexDegrees[i] = vertexCnt - 1 - i;
		}
	}

	
	/*	std::map<int, int> PrintDistrib;
		for (int i = 0; i < vertexDegrees.size(); i++) {
			PrintDistrib[vertexDegrees[i]]++;
		}
		for (auto i = PrintDistrib.begin(); i != PrintDistrib.end(); ++i) {
			for (int j = 0; j < i->second; j++) {
				cout << ':';
			}
			cout << " <" << i->first << ">\n";
		}*/


	return vertexDegrees;
}

void MyGraph::AssignWeights(/*WeightsType type*/) {
	std::random_device rd;
	std::mt19937 mersenne(rd());
	for (int i = 0; i < vertexCnt; i++) {
		for (int j = i + 1; j < vertexCnt; j++) {
			if (adjacencyMatrix[i][j]/* != 0*/) {
				posWeightsMatrix[i][j] = mersenne() % (WEIGHT_MAX - 1) + 1;
			}
		}
	}
}

void MyGraph::MixWeights(/*WeightsType type*/) {
	mixedWeightsMatrix = posWeightsMatrix;
	int posOrNeg;
	std::random_device rd;
	std::mt19937 mersenne(rd());
	for (int i = 0; i < vertexCnt; i++) {
		for (int j = i + 1; j < vertexCnt; j++) {
			if (mixedWeightsMatrix[i][j]/* != 0*/) {
				posOrNeg = mersenne() % 2;
				if (posOrNeg/* == 1*/) {
					mixedWeightsMatrix[i][j] *= -1;
				}
			}
			/*else {
				mixedWeightsMatrix[i][j] = INF;
			}*/
		}
	}
}

void MyGraph::ModifyWeights(iMx& WeightsMatrix, iMx& modified) {
	modified = WeightsMatrix;
	for (int i = 0; i < vertexCnt; i++) {
		for (int j = 0; j < vertexCnt; j++) {
			if (modified[i][j] == 0) {
				modified[i][j] = INF;
			}
		}
	}
}

MyGraph::MyGraph(int n) :	vertexCnt(n), adjacencyMatrix(n, vector<int>(n, 0)), posWeightsMatrix(n, vector<int>(n, 0)),
							mixedWeightsMatrix(), modPosWeightsMx(), modMixedWeightsMx(), reachMatrix(), maxFlowMx()
						{
	if (IS_IN_RANGE(vertexCnt, VERT_CNT_MIN, VERT_CNT_MAX)) {
		vector<int> vertDegrees = PrepVertices();
		MyShuffler<int> shuffler;
		for (int i = 0; i < vertexCnt - 2; i++) {
			shuffler.Clear();
			for (int j = i + 1; j < vertexCnt; j++) {
				shuffler.Push(j);
			}
			shuffler.Shuffle();
			for (int j = 0; j < vertDegrees[i]; j++) {
				adjacencyMatrix[i][shuffler.Pop()] = 1;
			}
		}
		adjacencyMatrix[vertexCnt - 2][vertexCnt - 1] = 1;

		AssignWeights();
		MixWeights();
		ModifyWeights(posWeightsMatrix, modPosWeightsMx);
		ModifyWeights(mixedWeightsMatrix, modMixedWeightsMx);
		GenReachMatrix();
		GenMaxFlowMx();
	}
	else {
		throw std::exception();
	}
}

int MyGraph::GetVertexCount() const{
	return vertexCnt;
}

iMx MyGraph::GetAdjMatrix() const {
	return adjacencyMatrix;
}

iMx MyGraph::GetWeightsMatrix(WeightsType type) const {
	switch (type) {
	case WeightsType::kPositive:
		return posWeightsMatrix;
		break;
	case WeightsType::kMixed:
		return mixedWeightsMatrix;
		break;
	case WeightsType::kModifiedPos:
		return modPosWeightsMx;
		break;
	case WeightsType::kModifiedMixed:
		return modMixedWeightsMx;
		break;
	default:
		break;
	}
}

iMx MyGraph::ShimbellMult(const iMx mxA, const iMx mxB, ShimbellMode mode) const {
	iMx resMatrix(vertexCnt, vector<int>(vertexCnt, 0));
	vector<int> buf;
	bool isNotZero;
	for (int i = 0; i < vertexCnt; i++) {
		for (int j = 0; j < vertexCnt; j++) {
			buf.clear();
			isNotZero = false;
			for (int k = 0; k < vertexCnt; k++) {
				if ((mxA[i][k] != 0) && (mxB[k][j] != 0)) {
					buf.push_back(mxA[i][k] + mxB[k][j]);
					isNotZero = true;
				}
			}
			if (isNotZero) {
				if (mode == ShimbellMode::kShortest) {
					resMatrix[i][j] = *std::min_element(buf.begin(), buf.end());
				}
				else {	//mode == ShimbellMode::kLongest
					resMatrix[i][j] = *std::max_element(buf.begin(), buf.end());
				}
			}
			else {
				resMatrix[i][j] = 0;
			}
		}
	}
	return resMatrix;
}

iMx MyGraph::CalcShimbell(int edgeCnt, ShimbellMode mode) const {
	//AssignWeights();
	iMx resMatrix = posWeightsMatrix;
	for (int i = 0; i < edgeCnt - 1; i++) {
		resMatrix = ShimbellMult(resMatrix, posWeightsMatrix, mode);
	}
	return resMatrix;
}

iMx MyGraph::GetReachMatrix() const {
	return reachMatrix;
}

void MyGraph::GenReachMatrix() {
	iMx boolExpMatrix = adjacencyMatrix;
	reachMatrix = iMx(vertexCnt, vector<int>(vertexCnt, 0));
	for (int i = 0; i < vertexCnt - 1; i++) {
		boolExpMatrix = matrixBoolMult(boolExpMatrix, adjacencyMatrix);
		reachMatrix = matrixAdd(reachMatrix, boolExpMatrix);
	}
	for (int i = 0; i < vertexCnt; i++) {
		reachMatrix[i][i] = 1;
	}
}

vector<int> MyGraph::Dijkstra(int inpVert, int& counter) const {
	vector<int> distances(modPosWeightsMx[inpVert]); 
	distances[inpVert] = 0;

	vector<bool> isVisitedVector(vertexCnt, false);
	isVisitedVector[inpVert] = true;

	int min, curVert = vertexCnt - 1;

	for (int i = 0; i < vertexCnt; i++) {
		min = INF;
		for (int j = 0; j < vertexCnt; j++, counter++) {
			if (!isVisitedVector[j] && (distances[j] < min)) {
				min = distances[j];
				curVert = j;
			}
		}
		isVisitedVector[curVert] = true;
		for (int j = 0; j < vertexCnt; j++, counter++) {
			if (!isVisitedVector[j] && (modPosWeightsMx[curVert][j] != INF) && (distances[curVert] != INF) && ((distances[curVert] + modPosWeightsMx[curVert][j]) < distances[j])) {
				distances[j] = distances[curVert] + modPosWeightsMx[curVert][j];
			}
		}
	}
	return distances;
}

#define DISTANCE first
#define VERTEX second

//https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-using-priority_queue-stl/amp/
vector<int> MyGraph::Dijkstra_queue(int inpVert, int& counter) const {
	counter = 0;
	vector<int> distances(vertexCnt, INF);
	distances[inpVert] = 0;

	i2Pair cur;
	int newDistance;

	priority_queue<i2Pair, vector<i2Pair>, std::greater<i2Pair>> pq;
	pq.push(std::make_pair(0, inpVert));

	while (!pq.empty()) {
		cur = pq.top();
		pq.pop();
		for (int i = cur.VERTEX + 1; i < vertexCnt; i++, counter++) {
			if (modPosWeightsMx[cur.VERTEX][i] != INF) {
				newDistance = cur.DISTANCE + modPosWeightsMx[cur.VERTEX][i];
				if (newDistance < distances[i]) {
					distances[i] = newDistance;
					pq.push(std::make_pair(newDistance, i));	//По-хорошему, нужно не добавлять новые пары в очередь,
																//а редактировать уже существующие. Но pq не предоставляет произвольный доступ.
				}
			}
		}
	}
	return distances;
}

#undef DISTANCE
#undef VERTEX

iMx MyGraph::RestorePaths(int inpVert, const vector<int>& distances, const iMx weightMx) const {
	iMx paths(vertexCnt, vector<int>());
	int tmp, curVert;
	for (int i = 0; i < vertexCnt; i++) {
		if (distances[i]/* != 0*/) {		//== 0 -- исходная вершина
			if (distances[i] != INF) {	//Если есть путь
				curVert = i;
				paths[i].push_back(curVert);
				while (curVert != inpVert) {	//Пока не дошли до исходной вершины
					for (int j = 0; j < vertexCnt; j++) {	//Ищем такую смежную вершину с нынешней, чтобы
															//значение ее метки было равно разности значений
															//метки нынешней вершины и веса ребра между ними
						if (weightMx[j][curVert] != INF) {
							if ((distances[curVert] - weightMx[j][curVert]) == distances[j]) {
								curVert = j;
								paths[i].push_back(j);
								break;
							}
						}
					}
				}
			}
			else {
				paths[i].push_back(INF);
			}
		}
		else {
			paths[i].push_back(INF);
		}
	}
	return paths;
}

vector<int> MyGraph::BellmanFord(int inpVert, iMx wieghtsMx, int& counter) const {
	counter = 0;
	vector<int> distances(vertexCnt, INF);
	distances[inpVert] = 0;

	int curVert, newDistance;

	deque<int> dq;
	dq.push_back(inpVert);

	while (!dq.empty()) {
		curVert = dq.front();
		dq.pop_front();
		for (int i = curVert + 1; i < vertexCnt; i++, counter++) {
			if (wieghtsMx[curVert][i] != INF) {
				newDistance = distances[curVert] + wieghtsMx[curVert][i];
				if (newDistance < distances[i]) {
					distances[i] = newDistance;
					if (std::find(dq.begin(), dq.end(), i) == dq.end()) {		//этой вершины в очереди нет
						dq.push_back(i);										//добавили в конец очереди
					}
					else {														//вершина есть в очереди
						std::remove(dq.begin(), dq.end(), i);					//удалили ее из очереди
						dq.push_front(i);										//добавили в начало очереди
					}
				}
			}
		}
	}

	return distances;
}

iMx MyGraph::FloydWarshall(int& counter) const {
	iMx distancesMx = modMixedWeightsMx;
	for (int i = 0; i < vertexCnt; i++) {
		for (int j = 0; j < vertexCnt; j++) {
			for (int k = 0; k < vertexCnt; k++, counter++) {
				if (distancesMx[i][k] != INF && distancesMx[k][j] != INF) {
					distancesMx[i][j] = std::min(distancesMx[i][j], (distancesMx[i][k] + distancesMx[k][j]));
				}
			}
		}
	}

	for (int i = 0; i < vertexCnt; i++) {
		distancesMx[i][i] = 0;
	}

	return distancesMx;
}


void MyGraph::GenMaxFlowMx() {
	std::random_device rd;
	std::mt19937 mersenne(rd());
	maxFlowMx = adjacencyMatrix;
	for (int i = 0; i < vertexCnt; i++) {
		for (int j = i + 1; j < vertexCnt; j++) {
			if (adjacencyMatrix[i][j]/* != 0*/) {
				maxFlowMx[i][j] = mersenne() % (FLOW_MAX - 1) + 1;
			}
		}
	}
}

iMx MyGraph::GetMaxFlowMatrix() const{
	return maxFlowMx;
}

iMx MyGraph::AddFictVert() const{
	std::stack<int> tmp;
	bool isSource;
	int tmpVert;
	for (int i = 0; i < vertexCnt; i++) {
		isSource = true;
		for (int j = 0; j < vertexCnt; j++) {
			if (adjacencyMatrix[j][i]) {
				isSource = false;
				break;
			}
		}
		if (isSource) {
			tmp.push(i);
		}
	}
	if (tmp.size() > 1) { 
		iMx resMaxFlowMx(vertexCnt + 1, vector<int>(vertexCnt + 1, 0));
		while (!tmp.empty()) {
			resMaxFlowMx[0][tmp.top() + 1] = std::accumulate(maxFlowMx[tmp.top()].begin(), maxFlowMx[tmp.top()].end(), 0);
			tmp.pop();
		}
		for (int i = 0; i < vertexCnt; i++) {
			for (int j = 0; j < vertexCnt; j++) {
				resMaxFlowMx[i + 1][j + 1] = maxFlowMx[i][j];
			}
		}
		/*PrintMatrix(resMaxFlowMx);
		cout << '\n';*/
		return resMaxFlowMx;
	}
	else {
		return maxFlowMx;
	}
}

bool MyGraph::bfs(iMx graphMx, int v1, int v2, vector<int>* path, int* counter) const{
	vector<bool> isVisitedArr(graphMx.size(), false);
	std::queue<int> q;
	int curVert;

	q.push(v1);
	isVisitedArr[v1] = true;
	if (path) {
		(*path)[v1] = -1;
	}
	
	while (!q.empty()) {
		curVert = q.front();
		q.pop();

		for (int i = 0; i < graphMx.size(); i++) {
			if (counter) {
				(*counter)++;
			}
			if ((isVisitedArr[i] == false) && (graphMx[curVert][i] != 0)) {
				if (path) {
					(*path)[i] = curVert;
				}
				if (i == v2) {
					return true;
				}
				q.push(i);
				isVisitedArr[i] = true;
			}
		}
	}
	return false;
}

//https://www.geeksforgeeks.org/ford-fulkerson-algorithm-for-maximum-flow-problem/
bool MyGraph::bfs_FordFulkerson(iMx residualG, int source, int sink, vector<int>& path) const{
	vector<bool> isVisitedArr(residualG.size(), false);
	std::queue<int> q;
	int curVert;

	q.push(source);
	isVisitedArr[source] = true;
	path[source] = -1;

	while (!q.empty()) {
		curVert = q.front();
		q.pop();

		for (int i = 0; i < residualG.size(); i++) {
			if ((isVisitedArr[i] == false) && (residualG[curVert][i] > 0)) {
				path[i] = curVert;
				if (i == sink) {
					return true;
				}
				q.push(i);
				isVisitedArr[i] = true;
			}
		}
	}
	return false;
}

int MyGraph::fordFulkerson(int source, int sink) const {
	int tmpSink = sink;
	iMx residualGraph = AddFictVert();
	if (residualGraph.size() != vertexCnt) {
		source++;
		tmpSink++;
	}

	vector<int> path(residualGraph.size(), 0);
	int maxFlow = 0;
	int curFlow;
	while (bfs_FordFulkerson(residualGraph, source, tmpSink, path)) {
		curFlow = INF;
		for (int i = tmpSink; i != source; i = path[i]) {
			curFlow = std::min(curFlow, residualGraph[path[i]][i]);
		}
		for (int i = tmpSink; i != source; i = path[i]) {
			residualGraph[path[i]][i] -= curFlow;
			residualGraph[i][path[i]] += curFlow;
		}
		maxFlow += curFlow;
	}
	return maxFlow;
}

int MyGraph::СalcMinCostFlow(int source, int sink, int flow, McfRetVals& retVals) const {
	int curCost = 0, bottleNeck = INF, minCostFlow = 0;
	int counter; //Заглушка
	iMx costMx = modMixedWeightsMx, flowMx = maxFlowMx;
	vector<int> path;
	vector<i2Pair> edgesToRemove;

	while (flow) {
		curCost = 0;
		bottleNeck = INF;
		edgesToRemove.clear();

		path = RestorePaths(source, BellmanFord(source, costMx, counter), costMx)[sink];
		retVals.paths.push_back(path);
		
		for (int i = path.size() - 1; i > 0; i--) {
			bottleNeck = std::min(bottleNeck, flowMx[path[i]][path[i - 1]]);
		}
		bottleNeck = std::min(bottleNeck, flow);
		retVals.flows.push_back(bottleNeck);

		for (int i = path.size() - 1; i > 0; i--) {
			flowMx[path[i]][path[i - 1]] -= bottleNeck;
			curCost += costMx[path[i]][path[i - 1]];
			if (flowMx[path[i]][path[i - 1]] == 0) {
				edgesToRemove.push_back(std::make_pair(path[i], path[i - 1]));
			}
		}
		retVals.costsPerPath.push_back(curCost);

		minCostFlow += bottleNeck * curCost;

		for (auto it = edgesToRemove.begin(); it != edgesToRemove.end(); ++it) {
			costMx[it->first][it->second] = INF;
		}

		flow -= bottleNeck;
	}

	retVals.modCostMx = costMx;
	retVals.modFlowMx = flowMx;

	return minCostFlow;
}

//bool MyGraph::IsReachable(int vert1, int vert2) const {
//	return reachMatrix[vert1][vert2];
//}
//
//bool MyGraph::IsReachableNotOriented(int vert1, int vert2) const {
//	return (reachMatrix[vert1][vert2] || reachMatrix[vert2][vert1]);
//}

priority_queue<Edge, vector<Edge>, std::greater<Edge>> MyGraph::SortEdges(iMx weightsMx) const {
	priority_queue<Edge, vector<Edge>, std::greater<Edge>> pq;

	for (int i = 0; i < vertexCnt; i++) {
		for (int j = 0; j < vertexCnt; j++) {
			if (weightsMx[i][j] != 0) {
				pq.push(Edge(i, j, weightsMx[i][j]));
			}
		}
	}

	return pq;
}

iMx MyGraph::Kruskal(iMx weightsMx, int* counter, int* sum) const {
	iMx minSpanTree = iMx(vertexCnt, vector<int>(vertexCnt, 0));
	priority_queue<Edge, vector<Edge>, std::greater<Edge>> pq = SortEdges(weightsMx);
	Edge edge;
	if (counter) {
		(*counter) = pow(vertexCnt, 2); //итераций на сортировку
	}

	while (!pq.empty()) {
		edge = pq.top();
		pq.pop();
		if (!bfs(minSpanTree, edge.vert1, edge.vert2, nullptr, counter)) {
			minSpanTree[edge.vert1][edge.vert2] = weightsMx[edge.vert1][edge.vert2];
			minSpanTree[edge.vert2][edge.vert1] = weightsMx[edge.vert1][edge.vert2];
		}
		if (counter) {
			(*counter)++;
		}
	}

	if (sum) {
		(*sum) = 0;
		for (int i = 0; i < vertexCnt; i++) {
			for (int j = i + 1; j < vertexCnt; j++) {
				if (minSpanTree[i][j] != 0) {
					(*sum) += minSpanTree[i][j];
				}
			}
		}
	}

	return minSpanTree;
}

//https://www.geeksforgeeks.org/prims-minimum-spanning-tree-mst-greedy-algo-5/
iMx MyGraph::Prim(iMx weightsMx, int* counter, int* sum) const {
	iMx minSpanTree = iMx(vertexCnt, vector<int>(vertexCnt, 0));
	vector<int> mstPath(vertexCnt, 0);
	mstPath[0] = -1;
	vector<int> mstKeys(vertexCnt, INF); //ключи -- минимальный вес ребра, инцидентного вершине
	mstKeys[0] = 0;
	vector<bool> isInMst(vertexCnt, false);
	int minKey, minIndex;
	if (counter) {
		(*counter) = 0;
	}

	for (int i = 0; i < vertexCnt; i++) {
		for (int j = 0; j < i; j++) {
			weightsMx[i][j] = weightsMx[j][i];
		}
	}

	for (int i = 0; i < vertexCnt - 1; i++) {
		minKey = INF;
		for (int j = 0; j < vertexCnt; j++) {
			if ((isInMst[j] == false) && (mstKeys[j] < minKey)) {
				minIndex = j;
				minKey = mstKeys[j];
			}
			if (counter) {
				(*counter)++;
			}
		}
		isInMst[minIndex] = true;

		for (int j = 0; j < vertexCnt; j++) {
			if ((weightsMx[minIndex][j] != 0) && (isInMst[j] == false) && (weightsMx[minIndex][j] < mstKeys[j])) {
				mstKeys[j] = weightsMx[minIndex][j];
				mstPath[j] = minIndex;
			}
			if (counter) {
				(*counter)++;
			}
		}
		stop
	}

	for (int i = 1; i < vertexCnt; i++) {
		minSpanTree[i][mstPath[i]] = weightsMx[i][mstPath[i]];
		minSpanTree[mstPath[i]][i] = weightsMx[i][mstPath[i]];
	}

	if (sum) {
		(*sum) = 0;
		for (int i = 0; i < vertexCnt; i++) {
			for (int j = i + 1; j < vertexCnt; j++) {
				if (minSpanTree[i][j] != 0) {
					(*sum) += minSpanTree[i][j];
				}
			}
		}
	}

	return minSpanTree;
}

iMx MyGraph::GenKirchhoff() const {
	iMx kirchhoffMx = iMx(vertexCnt, vector<int>(vertexCnt, 0));
	int vertDeg;
	iMx modAdjMx = adjacencyMatrix;

	for (int i = 0; i < vertexCnt; i++) {
		for (int j = 0; j < i; j++) {
			modAdjMx[i][j] = adjacencyMatrix[j][i];
		}
	}

	for (int i = 0; i < vertexCnt; i++) {
		vertDeg = 0;
		for (int j = 0; j < vertexCnt; j++) {
			if (modAdjMx[i][j]) {
				kirchhoffMx[i][j] = -1;
				vertDeg++;
			}
		}
		kirchhoffMx[i][i] = vertDeg;
	}
	return kirchhoffMx;
}

int MyGraph::CalcSpanTreesCnt() const {
	iMx kirchhoff = GenKirchhoff();
	iMx kirchhoffMinor = iMx(vertexCnt - 1, vector<int>(vertexCnt - 1, 0));
	for (int i = 0; i < vertexCnt - 1; i++) {
		for (int j = 0; j < vertexCnt - 1; j++) {
			kirchhoffMinor[i][j] = kirchhoff[i][j];
		}
	}
	return matrixDet(kirchhoffMinor);
}

void MyGraph::PruferEncode(iMx& weightsMx, vector<int>& pruferCode, vector<int>& pruferWeights) const {
	if (pruferCode.size() == weightsMx.size() - 2) {
		for (int i = 0; i < weightsMx.size(); i++) {
			for (int j = 0; j < weightsMx.size(); j++) {
				if (weightsMx[i][j] != 0) {
					pruferCode.push_back(i + 1);
					pruferWeights.push_back(weightsMx[i][j]);
					return;
				}
			}
		}
	}

	Edge edge;
	iMx tmpWeightsMx = weightsMx;
	int cnt;
	for (int i = 0; i < weightsMx.size(); i++) {
		cnt = 0;
		for (int j = 0; j < weightsMx.size(); j++) {
			if (weightsMx[i][j] != 0) {
				if (cnt > 1) {
					break;
				}
				else {
					edge.vert2 = j;
					cnt++;
				}
			}		
		}
		if (cnt == 1) {
			edge.vert1 = i;
			edge.weight = weightsMx[edge.vert1][edge.vert2];
			break;
		}
	}

	pruferCode.push_back(edge.vert2 + 1);
	pruferWeights.push_back(edge.weight);
    tmpWeightsMx[edge.vert1][edge.vert2] = tmpWeightsMx[edge.vert2][edge.vert1] = 0;

	//cout << "Removed: " << edge.vert1 << "\n\n";
	//PrintMatrix(tmpWeightsMx);

	PruferEncode(tmpWeightsMx, pruferCode, pruferWeights);
}

iMx MyGraph::PruferDecode(vector<int>& pruferCode, vector<int>& pruferWeights) const {
	iMx weightMx = iMx(pruferCode.size() + 1, vector<int>(pruferCode.size() + 1, 0));
	vector<bool> isUsed(weightMx.size(), false);
	queue<int> qPruferCode, qPruferWeights;
	for (int i = 0; i < pruferCode.size(); i++) {
		qPruferCode.push(pruferCode[i] - 1);
		qPruferWeights.push(pruferWeights[i]);
	}
	int tmpVert, tmpWeight;

	while (!qPruferCode.empty()) {
		for (int i = 0; i < weightMx.size(); i++) {
			if (qPruferCode._Get_container().end() == std::find(qPruferCode._Get_container().begin(), qPruferCode._Get_container().end(), i)) {
				if (isUsed[i] == false) {
					isUsed[i] = true;
					tmpVert = qPruferCode.front(); tmpWeight = qPruferWeights.front();
					weightMx[i][tmpVert] = weightMx[tmpVert][i] = tmpWeight;
					qPruferCode.pop(); qPruferWeights.pop();
					break;
				}
			}
		}
	}

	return weightMx;
}

iMx MyGraph::MakeUnoriented(const iMx weightsMx) const {
	iMx tmpWeightMx = weightsMx;
	for (int i = 0; i < weightsMx.size(); i++) {
		for (int j = 0; j < i; j++) {
			tmpWeightMx[i][j] = weightsMx[j][i];
		}
	}
	return tmpWeightMx;
}

vector<int> MyGraph::CalcDegrees(iMx weightsMx) const {
	vector<int> vertDeg(weightsMx.size(), 0);
	for (int i = 0; i < weightsMx.size(); i++) {
		for (int j = 0; j < weightsMx.size(); j++) {
			if (weightsMx[i][j] != 0) {
				vertDeg[i]++;
			}
		}
	}
	return vertDeg;
}

IsEulerOrHamilton MyGraph::IsEuler(iMx weightsMx) const {
	if (vertexCnt == 2) {
		return IsEulerOrHamilton::kFalse2Vert;
	}

	iMx unorWeightsMx = MakeUnoriented(weightsMx);
	vector<int> vertDeg = CalcDegrees(unorWeightsMx);
	IsEulerOrHamilton result;

	int counter = 0;
	bool isNotEuler = false;
	for (int i = 0; i < vertDeg.size(); i++) {
		if ((vertDeg[i] % 2) == 1) {
			isNotEuler = true;
			if (vertDeg[i] == (weightsMx.size() - 1)) {
				return IsEulerOrHamilton::kFalseUnmodifiable;
			}		
		}	
	}

	if (counter == 1) {
		return IsEulerOrHamilton::kFalseModifiable;
	}

	return IsEulerOrHamilton::kTrue;
}

vector<int> MyGraph::EulerCycles(iMx weightsMx, iMx& modWeightsMx, IsEulerOrHamilton& isEulerRes) const {
	isEulerRes = IsEuler(weightsMx);
	if (isEulerRes == IsEulerOrHamilton::kFalse2Vert || isEulerRes == IsEulerOrHamilton::kFalseUnmodifiable) {
		return vector<int>();
	}

	modWeightsMx = MakeUnoriented(weightsMx);
	vector<int> vertDeg = CalcDegrees(modWeightsMx);

	if (isEulerRes == IsEulerOrHamilton::kFalseModifiable) {
		bool isEuler = false, isChanged = false;
		int vertToConnect = -1;
		std::random_device rd;
		std::mt19937 mersenne(rd());

		while (!isEuler) {
			isChanged = false;
			for (int i = 0; i < modWeightsMx.size(); i++) {
				if ((vertDeg[i] % 2) == 1) {							//если степень вершины i нечетная
					vertToConnect = -1;
					for (int j = 0; j < modWeightsMx.size(); j++) {
						if ((modWeightsMx[i][j] == 0) && (i != j)) {	//если можно добавить ребро
							if (vertToConnect == -1) {					//если все степени вершин j окажутся четные, добавить ребро подходящей вершине				
								if (modWeightsMx.size() % 2 == 0) {
									if (vertDeg[j] != modWeightsMx.size() - 1) {
										vertToConnect = j;
									}
								}
								else {
									vertToConnect = j;
								}
							}
							if ((vertDeg[j] % 2) == 1) { //если степень вершины j нечетная
								isChanged = true;
								vertDeg[i]++;
								vertDeg[j]++;
								modWeightsMx[i][j] = modWeightsMx[j][i] = mersenne() % (WEIGHT_MAX - 1) + 1; //TODO: добавить смеш. веса
								break;
							}
						}
					}

					if (!isChanged && (vertToConnect != -1)) {
						isChanged = true;
						vertDeg[i]++;
						vertDeg[vertToConnect]++;
						modWeightsMx[i][vertToConnect] = modWeightsMx[vertToConnect][i] = mersenne() % (WEIGHT_MAX - 1) + 1; //TODO: добавить смеш. веса
					}

					if (vertToConnect == -1) {
						isEulerRes = IsEulerOrHamilton::kFalseUnmodifiable;
						return vector<int>();
					}
				}
			}

			if (!isChanged) {
				isEuler = true;
				//isEulerRes = IsEulerRes::kTrue;
			}
		}
	}

	//if (isEulerRes == IsEulerRes::kTrue)
	vector<int> eulerPath;
	int curVert;
	std::stack<int> vertices;
	vertices.push(0);
	iMx weightsForDec = modWeightsMx;

	while (!vertices.empty()) {
		curVert = vertices.top();
		if (vertDeg[curVert] == 0) {
			vertices.pop();
			eulerPath.push_back(curVert);
		}
		else {
			for (int i = 0; i < vertexCnt; i++) {
				if (weightsForDec[curVert][i] != 0) {
					vertices.push(i);
					vertDeg[i]--;
					vertDeg[curVert]--;
					weightsForDec[curVert][i] = 0;
					weightsForDec[i][curVert] = 0;
					break;
				}
			}
		}
	}

	return eulerPath;
}

vector<int> MyGraph::Hamilton(iMx weightsMx, iMx& modWeightsMx, IsEulerOrHamilton& isHamRes, int& minLen) const {
	if (weightsMx.size() == 2) {
		isHamRes = IsEulerOrHamilton::kFalse2Vert;
		return vector<int>();
	}


	modWeightsMx = MakeUnoriented(weightsMx);
	vector<int> vertDeg;
	bool isHamilton = true, isChanged = false;
	isHamRes = IsEulerOrHamilton::kTrue;
	int vertToConnect = -1;
	std::random_device rd;
	std::mt19937 mersenne(rd());

	//for (int i = 0; i < weightsMx.size(); i++) {
	//	for (int j = 0; j < weightsMx.size(); j++) {
	//		if ((i != j) && (modWeightsMx[i][j] == 0)) {
	//			isHamilton = false;
	//			modWeightsMx[i][j] = modWeightsMx[j][i] = mersenne() % (WEIGHT_MAX - 1) + 1;
	//		}
	//	}
	//}

	vertDeg = CalcDegrees(modWeightsMx);
	for (int i = 0; i < weightsMx.size(); i++) {
		if (vertDeg[i] < (vertexCnt / 2)) {
			isHamilton = false;
			isHamRes = IsEulerOrHamilton::kFalseModifiable;
			break;
		}
	}

	while (!isHamilton) {
		isChanged = false;
		for (int i = 0; i < weightsMx.size(); i++) {
			vertToConnect = -1;
			if (vertDeg[i] < (vertexCnt / 2)) {
				for (int j = 0; j < weightsMx.size(); j++) {
					if ((modWeightsMx[i][j] == 0) && (i != j)) {
						vertToConnect = j;
						if (vertDeg[j] < (vertexCnt / 2)) {
							isChanged = true;
							vertDeg[i]++;
							vertDeg[j]++;
							modWeightsMx[i][j] = modWeightsMx[j][i] = mersenne() % (WEIGHT_MAX - 1) + 1;
							break;
						}
					}
				}
				if (!isChanged && (vertToConnect != -1)) {
					isChanged = true;
					vertDeg[i]++;
					modWeightsMx[i][vertToConnect] = modWeightsMx[vertToConnect][i] = mersenne() % (WEIGHT_MAX - 1) + 1;
				}
			}
		}
		if (!isChanged) {
			isHamilton = true;
		}
	}

	ofstream ofs(OUTPUT_FILE_NAME);
	if (ofs.is_open()) {
		stop;
	}
	vector<int> path;
	path.push_back(0);
	vector<int> minPath;
	int len = 0;
	minLen = INT_MAX;

	FindHamiltonCycles(ofs, modWeightsMx, path, minPath, len, minLen);

	ofs.close();
	return minPath;
}

void MyGraph::FindHamiltonCycles(ofstream& ofs, iMx& weightsMx, vector<int>& path, vector<int>& minPath, int& len, int& minLen) const {
	if (path.size() == weightsMx.size()) {
		if (weightsMx[path.size() - 1][0] != 0) {
			len += weightsMx[path.size() - 1][0];
			path.push_back(0);
			if (len < minLen) {
				minLen = len;
				minPath = path;
			}

			for (int i = 0; i < path.size() - 1; i++) {
				ofs << path[i] + 1 << " -> ";
			}
			ofs << path[path.size() - 1] + 1 << '\t';
			ofs << "Weight: " << len << '\n';

			path.pop_back();
		}
		return;
	}

	bool isInPath = false;
	int tmpLen;
	for (int i = 0; i < weightsMx.size(); i++) {
		if (weightsMx[path.size() - 1][i] != 0) {
			if (path.end() == find(path.begin(), path.end(), i)) {
				tmpLen = len + weightsMx[path.size() - 1][i];
				path.push_back(i);
				FindHamiltonCycles(ofs, weightsMx, path, minPath, tmpLen, minLen);
				path.pop_back();
			}
		}
	}
}