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

void MyGraph::ModifyWeights(vector<vector<int>>& WeightsMatrix, vector<vector<int>>& modified) {
	modified = WeightsMatrix;
	for (int i = 0; i < vertexCnt; i++) {
		for (int j = 0; j < vertexCnt; j++) {
			if (modified[i][j] == 0) {
				modified[i][j] = INF;
			}
		}
	}
}

MyGraph::MyGraph(int n)	 :	vertexCnt(n), adjacencyMatrix(n, vector<int>(n, 0)), posWeightsMatrix(n, vector<int>(n, 0)), 
							mixedWeightsMatrix(), modPosWeightsMx(), modMixedWeightsMx(), reachMatrix() 
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
	}
	else {
		throw std::exception();
	}
}

int MyGraph::GetVertexCount() const{
	return vertexCnt;
}

vector<vector<int>> MyGraph::GetAdjMatrix() const {
	return adjacencyMatrix;
}

vector<vector<int>> MyGraph::GetWeightsMatrix(WeightsType type) const {
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

vector<vector<int>> MyGraph::ShimbellMult(const vector<vector<int>> mxA, const vector<vector<int>> mxB, ShimbellMode mode) const {
	vector<vector<int>> resMatrix(vertexCnt, vector<int>(vertexCnt, 0));
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

vector<vector<int>> MyGraph::CalcShimbell(int edgeCnt, ShimbellMode mode) const {
	//AssignWeights();
	vector<vector<int>> resMatrix = posWeightsMatrix;
	for (int i = 0; i < edgeCnt - 1; i++) {
		resMatrix = ShimbellMult(resMatrix, posWeightsMatrix, mode);
	}
	return resMatrix;
}

vector<vector<int>> MyGraph::GetReachMatrix() const {
	return reachMatrix;
}

void MyGraph::GenReachMatrix() {
	vector<vector<int>> boolExpMatrix = adjacencyMatrix;
	reachMatrix = vector<vector<int>>(vertexCnt, vector<int>(vertexCnt, 0));
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
#define VERTICE second

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
		for (int i = cur.VERTICE + 1; i < vertexCnt; i++, counter++) {
			if (modPosWeightsMx[cur.VERTICE][i] != INF) {
				newDistance = cur.DISTANCE + modPosWeightsMx[cur.VERTICE][i];
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
#undef VERTICE

vector<vector<int>> MyGraph::RestorePaths(int inpVert, const vector<int>& distances, const vector<vector<int>> weightMx) const {
	vector<vector<int>> paths(vertexCnt, vector<int>());
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

vector<int> MyGraph::BellmanFord(int inpVert, int& counter) const {
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
			if (modMixedWeightsMx[curVert][i] != INF) {
				newDistance = distances[curVert] + modMixedWeightsMx[curVert][i];
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