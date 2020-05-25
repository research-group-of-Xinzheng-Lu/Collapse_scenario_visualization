#include <osg/Geometry>
#include <osgViewer/Viewer>
#include <osg/LightModel>

#include <osg/NodeVisitor>
#include <osg/Geode>
#include <osg/Geometry>
#include <osgUtil/Export>
#include<osgUtil/SmoothingVisitor>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgUtil/Optimizer>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <osg/Texture2D>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Billboard>
#include <osg/Image>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>

#include <osg/TexEnv>
#include <osg/StateSet>
#include<osg/Vec3>
using namespace std;

void ReadPositionVTK(char* name,ifstream& ifile1,double m[][3])
{
	int Nodenum;
	string name1,name2,name3,name4,name5,name6,name7,name8,name9,name10,name11;
	//读顶点
	if(!ifile1) 	{ 	cout << "Cannot open file1.\n"; 	getchar();}
	ifile1>>name1>>name2>>name3>>name4>>name5>>name6>>name7>>name8>>name9>>name10>>Nodenum>>name11;//得到Nodenum,其余name无用
	//cout<<"顶点Nodenum:"<<Nodenum<<"\n";
		
	const int ROW = Nodenum;
	double (*d)[3];
	d=new double[ROW][3];
		
	for(int i = 0; i < ROW; ++i)
	{
		for(int j = 0; j <3; ++j)
			ifile1 >> d[i][j];
	}
}