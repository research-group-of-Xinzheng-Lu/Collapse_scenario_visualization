#ifndef MODEL_LOADER
#define MODEL_LOADER

#include "Global.h"

void LoadModels(string modelPath, string fileName, map<int, string> & models, osg::Node** & list);
void LoadModels(string modelPath, string fileName, vector<string> & envs, osg::Node** & list);

#endif