#ifndef ATTRDISPTREADR_H
#define ATTRDISPTREADR_H

#include "global.h"

using namespace std;

void ReadResult();
void ReadAttrFile(string filePath);
void ReadDispFile(string filePath, vector<vector<vector<double>>>& dispArr, int nTimePoint);

#endif