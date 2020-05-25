#include "Global.h"

void LoadModels(string modelPath, string fileName, map<int, string> & models, osg::Node** & modelNodeList){
	cout<<"[Time "<<(double) clock()/CLOCKS_PER_SEC<<"]\t"<<"Building model(s) will read from "<<modelPath<<endl;

	string modelListFile = modelPath + "modelList.txt";
	//获取文件列表，重定向输出到文件modelList.txt
	string dircmd = "dir /b/d " + modelPath + "*" + modelFileType + " > " + modelListFile;
	system(dircmd.c_str());

	fstream fin(modelListFile);
	string tempStr;
	int i = 0;
	while(getline(fin, tempStr)){
		models[i++] = modelPath + tempStr;
		//cout<<tempStr<<endl;
	}
	fin.close();

	//将读取的模型添加至模型数组中
	modelNodeList = new osg::Node*[models.size()];
	cout<<"[Time "<<(double) clock()/CLOCKS_PER_SEC<<"]\t"<<models.size()<<" model(s) is found."<<endl;
	for(int i=0;i<models.size();i++)
		modelNodeList[i] = osgDB::readNodeFile(models[i]);
}

void LoadModels(string envPath, string fileName, vector<string> & envs, osg::Node** & envNodeList){
	cout<<"[Time "<<(double) clock()/CLOCKS_PER_SEC<<"]\t"<<"Terr model(s) will read from "<<envPath<<endl;

	string envListFile = envPath + fileName;
	string dircmd = "dir /b/d " + envPath + "*" + envFileType + " > " + envListFile;
	system(dircmd.c_str());

	fstream fin(envListFile);
	string tempStr;
	int k = 0;
	while(getline(fin, tempStr)){
		envs.push_back(envPath + tempStr);
	}
	fin.close();

	envNodeList = new osg::Node*[envs.size()];
	cout<<"[Time "<<(double) clock()/CLOCKS_PER_SEC<<"]\t"<<envs.size()<<" env. model(s) is found."<<endl;
	for(int i=0;i<envs.size();i++)
		envNodeList[i] = osgDB::readNodeFile(envs[i]);
}