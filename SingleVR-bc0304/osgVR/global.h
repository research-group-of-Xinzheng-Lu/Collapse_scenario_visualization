#ifndef GLOBAL_H
#define GLOBAL_H

#include <osg/NodeVisitor>
#include <osg/Geometry>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Image>
#include <osg/TexGen>
#include <osg/Texture1D>
#include <osg/TexEnv>
#include <osg/StateSet>
#include <osg/Switch>
#include <osg/Billboard>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include <osg/Vec3>
#include <osg/LightModel>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgUtil/Optimizer>
#include <osgUtil/Export>
#include <osgUtil/SmoothingVisitor>
#include <osgUtil/Simplifier>
#include <osgUtil/MeshOptimizers>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/Registry>
#include <osgDB/ReaderWriter>

#include <osg\NodeVisitor>
#include <osg\MatrixTransform>
#include <osg\PagedLOD>
#include <osgDB\FileNameUtils>
#include <osg\Geode>
#include <strstream>

#include <osgParticle/ExplosionDebrisEffect>
#include <osgParticle/ExplosionEffect>
#include <osgParticle/SmokeEffect>
#include <osgParticle/FireEffect>
#include <osgParticle/BoxPlacer>
#include <osgParticle/ModularProgram>
#include <osgParticle/AccelOperator>
#include <osgParticle/FluidFrictionOperator>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/MultiSegmentPlacer>
#include <osg/Fog>

#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include <math.h>
#include <cstdlib>

using namespace std;

extern const string attrFilePath;		//属性表文件目录
extern const string XdispFilePath;
extern const string YdispFilePath;

extern const string modelPath;			//模型文件目录
extern const string envPath;			//环境文件目录
extern const string damageStatePath;	//破坏状态文件目录
extern const string smokeDataPath;		//烟气文件目录
extern const string modelFileType;		//模型格式
extern const string envFileType;		//环境格式
extern int nTimePoint;			
extern int nBuildings;					//建筑物总数量
extern float globalDispScale;			//建筑物位移系数，默认为1.0

extern int objIDIndex;					//属性表文件中ID的索引下标
extern int storeyIndex;					//属性表文件中层数的索引下标

extern bool isFrameSkip;				//是否开启跳帧
extern int frameSkip;					//每x帧读取一次（设置为大于1则效果为跳帧播放）
extern float collapsSpeed;
extern bool setDamage;					//是否显示破碎后状态动画
extern bool useVBO;						//是否使用VBO，同时也将更改顶点更新方式
extern int idleFrame;					//动画开始前的等待帧，60帧idle一秒，同时不建议与VBO模式同用
extern bool useDamageSnapshot;

extern int verticeLabel;				//读取模型几何点计数

//ArcGIS ID映射
//idMap[i] = j
//i: ArcGIS objectID	j: indexInArray
extern int* idMap;			
//模型文件目录
//modesl[i] = str
//i: index    str: 模型文件目录(X:\\...\\a.fbx)
extern map<int, string> models;
//环境模型目录
extern vector<string> envs;
//属性表向量: attrArry[a][b]
//a：objectIndex    b:属性项目
//[0]OBJECTID	[1]名称		[2]建造时间		[3]建筑面积		[4]层数; 
//[5]结构类型		[6]层高		[7]特殊情况		[8]结构功能		[9]占地面积   
extern vector<vector<string>> attrArray;

//时程位移向量：dispArray[a][b][c]
//a: objectIndex     b (0~nTimePoint): 相应时刻位移    c（0~attrArray[a][4]): 楼层  
extern vector<vector<vector<double>>> XdispArray;
extern vector<vector<vector<double>>> YdispArray;

//模型坐标点：coordsArray[a][b]
//a：建筑模型编号	b：模型第b个几何点的坐标
extern vector<vector<float>> VerticeXcoordsArray;
extern vector<vector<float>> VerticeYcoordsArray;
extern vector<vector<float>> VerticeZcoordsArray;

//结构破坏状态数组，倒塌发生的时刻
extern vector<int> damageState;
//结构破坏状态数组，根据震害结果确定的倒塌发生的时刻
extern vector<int> damageStateOrigin;
//倒塌类型：-1代表未倒塌，0代表完全竖向垮塌，1代表底部一层倒塌，2代表底部数层倒塌，3代表中间层倒塌，4代表整体倾倒
extern vector<int> damageType;
//倒塌楼层：-1代表底部，其他正整数代表中间某层倒塌
extern vector<int> collapseStorey;
//建筑完全倾倒围绕的旋转轴
extern float rotateAxis[2][3];
//建筑倾倒的方向
//1代表坐标轴正方向，-1代表坐标轴负方向，0代表不倾倒
extern int rotateDirctn[3];

//烟气粒子坐标：smokeCoordsArray[a][b]
//a：烟气编号	b：烟气粒子坐标
//[0]X坐标	[1]Y坐标
extern float smokeCoordsArray[1000][2];

//烟气时间：smokeTime[a][b]
//a：烟气编号	b：烟气粒子时刻
//[0]起始时刻		[1]结束时刻
extern int smokeTime[1000][2];

//烟气结束时间：smokeStartTime[a]
//a：烟气编号
//[0]30帧		[1]50帧		[2]60帧		[3]100帧
extern int smokeStartTime[6];

//烟气结束时间：smokeEndTime[a]
//a：烟气编号
//[0]200帧		[1]150帧		[2]100帧		[3]30帧
extern int smokeEndTime[6];

//是否有对应的建筑fbx模型
extern vector<int> inModelList;

extern bool isPause;

extern float PI_F;

#endif
