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
	tmpSink++;
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

	return minCostFlow;
}
