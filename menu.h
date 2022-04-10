#pragma once

//include:
#include "global.h"
#include "MyGraph.h"
//!include

//define:
#define MENU_ITEMS_MAX 4
#define MENU_ITEMS_MIN 0
//!define

//enum:
enum class MenuItems {
	kGenGraph,
	kShimbell,
	kReachability,
	kDijkstra,
	kBellmanFord
};
//!enum

//const:
const regex kRxYn("[YNyn]$");
const regex kRxNumber("^(([1-9])([0-9])*)|(0)$");

static const char* kDelimeter = "--------------------------------------------------";

static const char* kMenuItemsArr[] = {
"Сгенерировать новый граф",
"Метод Шимбелла",
"Определение возможности построения маршрута",
"Алгоритм Дейкстры",
"Алгоритм Беллмана-Форда"
};
//!const

//functions:
bool IsContinue();
bool GetInputInt(int& iInp, int min, int max);
void PrintMenu();

MyGraph GenGraph();
void ExecShimbell(const MyGraph& graph);
void ExecReachability(const MyGraph& graph);
void ExecDijkstra(const MyGraph& graph);
void ExecBellmanFord(const MyGraph& graph);
//!functions

