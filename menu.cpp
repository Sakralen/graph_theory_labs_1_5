#include "menu.h"

bool IsContinue(bool overrideFlag) {
	if (overrideFlag) {
		return false;
	}
	else {
		char d;
		string strInp;
		bool isOk = false;
		do {
			cout << "Продолжить? (Y/N): ";
			getline(cin, strInp);
			if (regex_match(strInp, kRxYn)) {
				isOk = true;
				d = strInp[0];
				switch (d) {
				case 'Y':
				case 'y':
					return true;
				case 'N':
				case 'n':
					return false;
				}
			}
			else {
				isOk = false;
				cout << "Некорректный ввод!\n";
			}
		} while (!isOk);
	}
}

void PrintMenu() {
	cout << kDelimeter << '\n';
	for (int i = 0; i <= MENU_ITEMS_MAX; i++) {
		cout << std::setw(3) << i << ": " << kMenuItemsArr[i] << '\n';
	}
	cout << kDelimeter << '\n';
}

bool GetInputInt(int& iInp, int min, int max, int forbidden) {
	string strInp;
	bool res;
	getline(cin, strInp);
	if (regex_match(strInp, kRxNumber)) {
		iInp = stoi(strInp);
		if (IS_IN_RANGE(iInp, min, max)) {
			if (forbidden == INT_MAX) {
				res = true;
			}
			else {
				if (iInp != forbidden) {
					res = true;
				}
				else {
					res = false;
				}
			}
			
		}
		else {
			res = false;
		}
	}
	else {
		res = false;
	}
	if (res) {
		return true;
	}
	else {
		cout << "Некорректный ввод!\n";
		return false;
	}
}


MyGraph GenGraph() {
	int vertCnt;
	do {
		cout << "Введите количество вершин графа n (" << VERT_CNT_MIN << " <= n <= " << VERT_CNT_MAX << "):\n";
	} while (!GetInputInt(vertCnt, VERT_CNT_MIN, VERT_CNT_MAX));
	cout << '\n';

	MyGraph graph(vertCnt);

	cout << "Матрица смежности вершин:\n";
	PrintMatrix(graph.GetAdjMatrix());
	cout << '\n';

	return graph;
}

void ExecShimbell(const MyGraph& graph) {
	int edgesCnt, mType;
	do {
		cout << "Введите количество ребер:\n";
	} while (!GetInputInt(edgesCnt, 1, graph.GetVertexCount() - 1));
	cout << '\n';
	do {
		cout << "Выберете требование к матрице:\nматрица кратчайших маршрутов (0), матрица длиннейших маршрутов (1)\n";
	} while (!GetInputInt(mType, 0, 1));
	cout << '\n';
	cout << "Матрица весов:\n";
	PrintMatrix(graph.GetWeightsMatrix(WeightsType::kPositive));
	cout << '\n';
	cout << "Матрица Шимбелла:\n";
	PrintMatrix(graph.CalcShimbell(edgesCnt, static_cast<ShimbellMode>(mType)));
	cout << '\n';
}

void ExecReachability(const MyGraph& graph) {
	int vert1, vert2;
	iMx reachMatrix = graph.GetReachMatrix();
	do {
		cout << "Введите номер первой вершины:\n";
	} while (!GetInputInt(vert1, 1, graph.GetVertexCount()));
	cout << '\n';
	do {
		cout << "Введите номер второй вершины:\n";
	} while (!GetInputInt(vert2, 1, graph.GetVertexCount()));
	cout << '\n';
	vert1--;
	vert2--;
	cout << "Матрица достижомости:\n";
	PrintMatrix(reachMatrix);
	cout << '\n';
	if (reachMatrix[vert1][vert2]/* != 0*/) {
		cout << "Cуществует " << reachMatrix[vert1][vert2] << " маршрутов между данными вершинами!\n";
	}
	else {
		cout << "Не существует маршрутов между данными вершинами!\n";
	}
	cout << '\n';
}

void ExecDijkstra(const MyGraph& graph) {
	int inpVert, counter = 0;

	do {
		cout << "Введите исходную вершину:\n";;
	} while (!GetInputInt(inpVert, 1, graph.GetVertexCount()));
	cout << '\n';
	inpVert--;

	cout << "Матрица весов:\n";
	PrintMatrix(graph.GetWeightsMatrix(WeightsType::kPositive));
	cout << '\n';

	cout << "П Е Р Е Б О Р:\n";

	vector<int> distances = graph.Dijkstra(inpVert, counter);
	iMx paths = graph.RestorePaths(inpVert, distances, graph.GetWeightsMatrix(WeightsType::kModifiedPos));

	for (int i = 0; i < graph.GetVertexCount(); i++) {
		if (i != inpVert) {
			if (paths[i][0] != INF) {
				cout << "Кратчайший путь длиной " << std::setw(2) << distances[i] << " до вершины " << std::setw(2) << i + 1 << ": ";
				for (int j = paths[i].size() - 1; j > 0; j--) {
					cout << std::setw(2) << paths[i][j] + 1 << " -> ";
				}
				cout << std::setw(2) << paths[i][0] + 1 << '\n';
			}
			else {
				cout << "До вершины " << std::setw(2) << i + 1 << " пути нет!\n";
			}
		}
	}
	cout << '\n';
	cout << "Количество итераций: " << counter << '\n';
	cout << '\n';

	cout << "О Ч Е Р Е Д Ь:\n";

	distances = graph.Dijkstra_queue(inpVert, counter);
	paths = graph.RestorePaths(inpVert, distances, graph.GetWeightsMatrix(WeightsType::kModifiedPos));

	for (int i = 0; i < graph.GetVertexCount(); i++) {
		if (i != inpVert) {
			if (paths[i][0] != INF) {
				cout << "Кратчайший путь длиной " << std::setw(2) << distances[i] << " до вершины " << std::setw(2) << i + 1 << ": ";
				for (int j = paths[i].size() - 1; j > 0; j--) {
					cout << std::setw(2) << paths[i][j] + 1 << " -> ";
				}
				cout << std::setw(2) << paths[i][0] + 1 << '\n';
			}
			else {
				cout << "До вершины " << std::setw(2) << i + 1 << " пути нет!\n";
			}
		}
	}
	cout << '\n';
	cout << "Количество итераций: " << counter << '\n';
	cout << '\n';

	//cout << "Б Ф:\n";

	//distances = graph.BellmanFord(inpVert, counter);
	//paths = graph.RestorePaths(inpVert, distances);

	//for (int i = 0; i < graph.GetVertexCount(); i++) {
	//	if (i != inpVert) {
	//		if (paths[i][0] != INF) {
	//			cout << "Кратчайший путь длиной " << std::setw(2) << distances[i] << " до вершины " << std::setw(2) << i + 1 << ": ";
	//			for (int j = paths[i].size() - 1; j > 0; j--) {
	//				cout << std::setw(2) << paths[i][j] + 1 << " -> ";
	//			}
	//			cout << std::setw(2) << paths[i][0] + 1 << '\n';
	//		}
	//		else {
	//			cout << "До вершины " << std::setw(2) << i + 1 << " пути нет!\n";
	//		}
	//	}
	//}
	//cout << '\n';
	//cout << "Количество итераций:" << std::setw(4) << counter << '\n';
	//cout<< '\n';
}

void ExecBellmanFord(const MyGraph& graph) {
	int inpVert, counter = 0;

	do {
		cout << "Введите исходную вершину:\n";;
	} while (!GetInputInt(inpVert, 1, graph.GetVertexCount()));
	cout << '\n';
	inpVert--;

	cout << "Матрица весов:\n";
	PrintMatrix(graph.GetWeightsMatrix(WeightsType::kMixed));
	cout << '\n';

	vector<int> distances = graph.BellmanFord(inpVert, graph.GetWeightsMatrix(WeightsType::kMixed), counter);
	iMx paths = graph.RestorePaths(inpVert, distances, graph.GetWeightsMatrix(WeightsType::kModifiedMixed));

	for (int i = 0; i < graph.GetVertexCount(); i++) {
		if (i != inpVert) {
			if (paths[i][0] != INF) {
				cout << "Кратчайший путь длиной " << std::setw(3) << distances[i] << " до вершины " << std::setw(2) << i + 1 << ": ";
				for (int j = paths[i].size() - 1; j > 0; j--) {
					cout << std::setw(2) << paths[i][j] + 1 << " -> ";
				}
				cout << std::setw(2) << paths[i][0] + 1 << '\n';
			}
			else {
				cout << "До вершины " << std::setw(2) << i + 1 << " пути нет!\n";
			}
		}
	}
	cout << '\n';
	cout << "Количество итераций: " << counter << '\n';
	cout << '\n';
}

void ExecFloydWarshall(const MyGraph& graph) {
	int counter = 0;

	cout << "Матрица весов:\n";
	PrintMatrix(graph.GetWeightsMatrix(WeightsType::kMixed));
	cout << '\n';

	iMx distancesMx = graph.FloydWarshall(counter);

	cout << "Матрица расстояний:\n";
	for (int i = 0; i < graph.GetVertexCount(); i++) {
		for (int j = 0; j < graph.GetVertexCount(); j++) {
			if (distancesMx[i][j] != INF) {
				cout << std::setw(3) << distancesMx[i][j] << " ";
			}
			else {
				cout << std::setw(3) << "INF" << " ";
			}
		}
		cout << '\n';
	}
	cout << '\n';

	cout << "Количество итераций: " << counter << '\n';
	cout << '\n';
}

void ExecMinCostFlow(const MyGraph& graph) {
	int vert1, vert2;
	do {
		cout << "Введите номер первой вершины:\n";
	} while (!GetInputInt(vert1, 1, graph.GetVertexCount()));
	cout << '\n';
	do {
		cout << "Введите номер второй вершины:\n";
	} while (!GetInputInt(vert2, 1, graph.GetVertexCount(), vert1));
	cout << '\n';
	vert1--;
	vert2--;

	cout << "Матрица пропускных способностей:\n";
	PrintMatrix(graph.GetMaxFlowMatrix());
	cout << '\n';

	int maxFlow = graph.fordFulkerson(vert1, vert2);
	cout << "Максимальный поток: " << maxFlow << '\n';
	cout << "Используемый поток: " << maxFlow * 2 / 3 << '\n';
	cout << '\n';

	cout << "Матрица стоимостей:\n";
	PrintMatrix(graph.GetWeightsMatrix(WeightsType::kMixed));
	cout << '\n';

	McfRetVals retVals;
	int mcf = graph.СalcMinCostFlow(vert1, vert2, (maxFlow * 2 / 3), retVals);

	int resCnt = retVals.flows.size();
	for (int k = 0; k < resCnt; k++) {
		cout << "Поток величины " << retVals.flows[k] << " со стоимостью " << retVals.costsPerPath[k] << " за единицу потока\n";
		cout << "по пути: ";
		for (int i = retVals.paths[k].size() - 1; i > 0; i--) {
			cout << retVals.paths[k][i] + 1 << " -> ";
		}
		cout << retVals.paths[k][0] + 1;
		cout << '\n';
		cout << "Итоговая стоимость: " << retVals.flows[k] * retVals.costsPerPath[k] << '\n';
		cout << '\n';
	}

	cout << "Величина потока минимальной стоимости: " << mcf << '\n';
	cout << '\n';
}