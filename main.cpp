#include "global.h"
#include "MyGraph.h"
#include "menu.h"

void main() {
	setlocale(LC_ALL, "Russian");
	int menuItem;
	MenuItems item;
	bool overrideFlag = false;

	MyGraph graph = GenGraph();

	//ExecPrufer(graph);

	do {
		PrintMenu();
		cout << '\n';

		do {
			cout << "Выберете пункт меню:\n";
		} while (!GetInputInt(menuItem, MENU_ITEMS_MIN, MENU_ITEMS_MAX));
		cout << '\n';

		item = static_cast<MenuItems>(menuItem);
		switch (item) {
			case MenuItems::kGenGraph:
				graph = GenGraph();
				break;
			case MenuItems::kShimbell:
				ExecShimbell(graph);
				break;
			case MenuItems::kReachability:
				ExecReachability(graph);
				break;
			case MenuItems::kDijkstra:
				ExecDijkstra(graph);
				break;
			case MenuItems::kBellmanFord:
				ExecBellmanFord(graph);
				break;
			case MenuItems::kFloydWarshall:
				ExecFloydWarshall(graph);
				break;
			case MenuItems::kMinCostFlow:
				ExecMinCostFlow(graph);
				break;
			case MenuItems::kKruskal:
				ExecKruskal(graph);
				break;
			case MenuItems::kPrim:
				ExecPrim(graph);
				break;
			case MenuItems::kSpanTreesCnt:
				ExecSpanTreesCnt(graph);
				break;
			case MenuItems::kPrufer:
				ExecPrufer(graph);
				break;
			case MenuItems::kEulerCycle:
				ExecEulerCycle(graph);
				break;
			case MenuItems::kHamiltonCycle:
				ExecHamiltonCycle(graph);
				break;
			case MenuItems::kExit:
				overrideFlag = true;
				break;
			default:
				break;
		}
	} while (IsContinue(overrideFlag));
	stop
}
