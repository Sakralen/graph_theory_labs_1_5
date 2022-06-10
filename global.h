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
#include <map>
#include <numeric>
#include <fstream>

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
using std::queue;
using std::deque;
using std::ofstream;
//!using

//typedef:
typedef pair<int, int> i2Pair;
typedef vector<vector<int>> iMx;
//!typedef

//define:
#define stop __asm nop

#define UNIFORM_DISTRIB_MAX 1000000
#define FURRY_DISTRIB_PARAM 0.6

#define IS_IN_RANGE(x, min, max) ((x >= min) && (x <= max))

#define OUTPUT_FILE_NAME "output.txt"
//!define

//const:
//!const

//global variables:
//!global variables

//functions:
int FurryRng(double param = FURRY_DISTRIB_PARAM);
void PrintMatrix(const iMx vec);

iMx matrixBoolMult(const iMx mxA, const iMx mxB);
iMx matrixAdd(const iMx mxA, const iMx mxB);
int factorial(int a);
int matrixDet(const iMx mx);
//!functions