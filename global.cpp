#include "global.h"

int FurryRng(double param) {
	std::random_device rd;
	std::mt19937 mersenne(rd());
	int result;
	int iUniRand = mersenne() % (UNIFORM_DISTRIB_MAX - 1) + 1;
	double dUniRand = static_cast<double>(iUniRand) / UNIFORM_DISTRIB_MAX;
	result = static_cast<int>(log(dUniRand) / log(1 - param)) + 1;
	return result;
}

void PrintMatrix(const iMx vec) {
	int n = vec.size();
	int m = vec[0].size();
	cout << "   ";
	for (int i = 0; i < vec[0].size() - 1; i++) {
		cout << std::setw(3) << i + 1 << " ";
	}
	cout << std::setw(3) << vec[0].size() << '\n';

	for (int i = 0; i < n; i++) {
		cout << std::setw(3) << i + 1;
		for (int j = 0; j < m; j++) {
			cout << std::setw(3) << vec[i][j] << " ";
		}
		cout << '\n';
	}
}

iMx matrixBoolMult(const iMx mxA, const iMx mxB) {
	iMx resMatrix(mxA.size(), vector<int>(mxA.size(), 0));
	vector<int> buf;
	for (int i = 0; i < mxA.size(); i++) {
		for (int j = 0; j < mxA.size(); j++) {
			buf.clear();
			for (int k = 0; k < mxA.size(); k++) {
				buf.push_back(mxA[i][k] && mxB[k][j]);
				//resMatrix[i][j] |= (mxA[i][k] & mxB[k][j]);
			}
			resMatrix[i][j] = *std::max_element(buf.begin(), buf.end());
		}
	}
	return resMatrix;
}

iMx matrixAdd(const iMx mxA, const iMx mxB) {
	iMx resMatrix(mxA.size(), vector<int>(mxA.size(), 0));
	for (int i = 0; i < mxA.size(); i++) {
		for (int j = 0; j < mxA.size(); j++) {
			resMatrix[i][j] = mxA[i][j] + mxB[i][j];
		}
	}
	return resMatrix;
}

int factorial(int a) {
	if (a == 1) {
		return 1;
	}
	return a * factorial(a - 1);
}

int matrixDet(const iMx mx) {
	if (mx.size() == 1) {
		return mx[0][0];
	}
	if (mx.size() == 2) {
		return mx[0][0] * mx[1][1] - mx[0][1] * mx[1][0];
	}
	int det = 0;
	iMx tmpMx;
	for (int k = 0; k < mx.size(); k++) {
		tmpMx = iMx(mx.size() - 1, vector<int>(mx.size() - 1, 0));
		for (int i = 0; i < mx.size() - 1; i++) {
			for (int j = 0; j < mx.size() - 1; j++) {
				if (i >= k) {
					tmpMx[i][j] = mx[i + 1][j + 1];
				}
				else {
					tmpMx[i][j] = mx[i][j + 1];
				}
			}
		}
		if (k % 2 == 0) {
			det += mx[0][k] * matrixDet(tmpMx);
		}
		else {
			det -= mx[0][k] * matrixDet(tmpMx);
		}
	}
	return det;
}
