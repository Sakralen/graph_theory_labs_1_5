#pragma once

//include:
#include <iostream>
#include <cmath>
#include <random>
#include <vector>
#include <algorithm>
#include <iomanip> 
#include <string>
#include <regex>
#include <queue>

#include "MyShuffler.h"
//!include

//enum:
//!enum

//using:
using std::cout;
using std::cin;
using std::vector;
using std::regex;
using std::string;
using std::priority_queue;
using std::pair;
using std::deque;
//!using

//typedef:
typedef pair<int, int> i2Pair;
//!typedef

//define:
#define stop __asm nop

#define UNIFORM_DISTRIB_MAX 1000000
#define FURRY_DISTRIB_PARAM 0.6

#define IS_IN_RANGE(x, min, max) ((x >= min) && (x <= max))
//!define

//const:
//!const

//global variables:
//!global variables

//functions:
int FurryRng(double param = FURRY_DISTRIB_PARAM);
void PrintMatrix(const vector<vector<int>> vec);

vector<vector<int>> matrixBoolMult(const vector<vector<int>> mxA, const vector<vector<int>> mxB);
vector<vector<int>> matrixAdd(const vector<vector<int>> mxA, const vector<vector<int>> mxB);
//!functions