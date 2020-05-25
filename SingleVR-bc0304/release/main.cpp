#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/LightModel>

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

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgUtil/Optimizer>
#include <osgUtil/Export>
#include <osgUtil/SmoothingVisitor>

#include <osg/Billboard>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>
#include<osg/Vec3>

#include "readvtk.h"//没编完
using namespace std;


class TryUpdate : public osg::Drawable::UpdateCallback
{
public:
	TryUpdate() 
	{
		frame = 0;
	}

	int frame;

	virtual void update( osg::NodeVisitor* nv, osg::Drawable* drawable )

	{
		if(frame>40)
		{ frame = 0;}
				
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>( drawable );
		if ( !geom ) return;
		osg::BoundingBox bx = geom->getBoundingBox();
		float z1,z2;
		z1= bx.zMin();
		z2= bx.zMax();
		float l= (z2-z1)/7.0;
		//std::cout<<l;
		float dis = 1.0;

		osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
		if(frame == 0)
		{    for (int i=0;i<vertices->getNumElements();i++)//i means vertices array，判断在哪层，加位移
			{
				float z = (*vertices)[i].z()-z1;
				if(z>=l&&z<2*l)
				(*vertices)[i].set((*vertices)[i].x()+0.1*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=2*l&&z<3*l)
				(*vertices)[i].set((*vertices)[i].x()+0.2*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=3*l&&z<4*l)
				(*vertices)[i].set((*vertices)[i].x()+0.3*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=4*l&&z<5*l)
				(*vertices)[i].set((*vertices)[i].x()+0.4*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=5*l&&z<6*l)
				(*vertices)[i].set((*vertices)[i].x()+0.5*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=6*l)
				(*vertices)[i].set((*vertices)[i].x()+0.6*dis,(*vertices)[i].y(),(*vertices)[i].z());
				
				vertices->dirty();	
			  }
		}

		if(frame ==10 )
		{    for (int i=0;i<vertices->getNumElements();i++)//i means vertices array，判断在哪层，加位移
			{
				float z = (*vertices)[i].z()-z1;
				if(z>=l&&z<2*l)
				(*vertices)[i].set((*vertices)[i].x()-0.1*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=2*l&&z<3*l)
				(*vertices)[i].set((*vertices)[i].x()-0.2*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=3*l&&z<4*l)
				(*vertices)[i].set((*vertices)[i].x()-0.3*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=4*l&&z<5*l)
				(*vertices)[i].set((*vertices)[i].x()-0.4*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=5*l&&z<6*l)
				(*vertices)[i].set((*vertices)[i].x()-0.5*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=6*l)
				(*vertices)[i].set((*vertices)[i].x()-0.6*dis,(*vertices)[i].y(),(*vertices)[i].z());
				
				vertices->dirty();	
			  }
		}
				
		if(frame ==20 )
		{    for (int i=0;i<vertices->getNumElements();i++)//i means vertices array，判断在哪层，加位移
			{
				float z = (*vertices)[i].z()-z1;
				if(z>=l&&z<2*l)
				(*vertices)[i].set((*vertices)[i].x()-0.1*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=2*l&&z<3*l)
				(*vertices)[i].set((*vertices)[i].x()-0.2*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=3*l&&z<4*l)
				(*vertices)[i].set((*vertices)[i].x()-0.3*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=4*l&&z<5*l)
				(*vertices)[i].set((*vertices)[i].x()-0.4*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=5*l&&z<6*l)
				(*vertices)[i].set((*vertices)[i].x()-0.5*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=6*l)
				(*vertices)[i].set((*vertices)[i].x()-0.6*dis,(*vertices)[i].y(),(*vertices)[i].z());
				
				vertices->dirty();	
			  }
		}

		if(frame ==30 )
		{    for (int i=0;i<vertices->getNumElements();i++)//i means vertices array，判断在哪层，加位移
			{
				float z = (*vertices)[i].z()-z1;
				if(z>=l&&z<2*l)
				(*vertices)[i].set((*vertices)[i].x()+0.1*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=2*l&&z<3*l)
				(*vertices)[i].set((*vertices)[i].x()+0.2*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=3*l&&z<4*l)
				(*vertices)[i].set((*vertices)[i].x()+0.3*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=4*l&&z<5*l)
				(*vertices)[i].set((*vertices)[i].x()+0.4*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=5*l&&z<6*l)
				(*vertices)[i].set((*vertices)[i].x()+0.5*dis,(*vertices)[i].y(),(*vertices)[i].z());
				else if (z>=6*l)
				(*vertices)[i].set((*vertices)[i].x()+0.6*dis,(*vertices)[i].y(),(*vertices)[i].z());
				
				vertices->dirty();
			  }
		}		
			
		frame++;
	}

//protected:
//	int step;
//	int Tstep;
};




//添加光照
osg::ref_ptr<osg::Group> createLightModel(osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<osg::Group> lightRoot= new osg::Group();
	lightRoot->addChild(node);

	//开启光照
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	stateset = lightRoot->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
//	stateset->setMode(GL_LIGHT_MODEL_TWO_SIDE,osg::StateAttribute::ON);

	//计算包围盒
    osg::BoundingSphere bs ;
    node->computeBound() ;
    bs=node->getBound() ;

	//创建一个Light对象
	osg::ref_ptr<osg::Light> light = new osg::Light();
	light->setLightNum(0);
	//设置方向
	light->setDirection(osg::Vec3(0.0f,0.0f,-1.0f));
	//设置位置
	light->setPosition(osg::Vec4(bs.center().x(),bs.center().y(),bs.center().z()+bs.radius(),1.0f));
	//设置环境光的颜色
	light->setAmbient(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
	//设置散射光的颜色
	light->setDiffuse(osg::Vec4(1.0f,1.0f,1.0f,1.0f));

	//设置恒衰减指数
	light->setConstantAttenuation(1.0f);
	//设置线形衰减指数
	light->setLinearAttenuation(0.0f);
	//设置二次方衰减指数
	light->setQuadraticAttenuation(0.0f);

	//创建光源
	osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
	lightSource->setLight(light.get());

	lightRoot->addChild(lightSource.get());

	return lightRoot.get() ;
}

class SetAnimation:public osg::NodeVisitor
{
public:
	SetAnimation():NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN){}
	void apply(osg::Geode& geode)
	{
		//osg::ComputeBoundsVisitor bnv;
		//geode.accept(bnv);
		//osg::BoundingBox bbox = bnv.getBoundingBox();
		for (int i=0;i<geode.getNumDrawables();i++)
		{
			
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));				
					 
			geom->setUseVertexBufferObjects( true );
			geom->setUpdateCallback( new TryUpdate() );			

		}

		NodeVisitor::apply(geode);

	}
};

int main()
{
	//创建Viewer对象，场景浏览器
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
	osg::ref_ptr<osg::Group> root = new osg::Group();
	//osg::ref_ptr<osg::Node> node1 = createPolygon();
	//osg::MatrixTransform* rotateMT= new osg::MatrixTransform;
	//rotateMT->setMatrix(osg::Matrix::rotate(osg::inDegrees(-90.0f),0.0f,0.0f,1.0f));
	//rotateMT->addChild(node1);
	//添加到场景
	//osg::Node* model = osgDB::readNodeFile("../Model_L22_0000t1.osgb");
	osg::Node* model = osgDB::readNodeFile("../bd.fbx");
	//osg::Node* model = osgDB::readNodeFile("../Model.osgb");
	//osg::Node* model = osgDB::readNodeFile("../Model_L19_0.osgb");
	//osg::Node* model = osgDB::readNodeFile("../Model_L22_0000t1.osgb");
	//osg::Node* model = osgDB::readNodeFile("../Model_L25_0000000.osgb");
	//osg::Node* model1 = osgDB::readNodeFile("../Model_L25_0000100.osgb");
	//osg::Node* model2= osgDB::readNodeFile("../Model_L25_0000200.osgb");	
	//osg::Node* model3 = osgDB::readNodeFile("../Model_L25_0000300.osgb");
	//osg::Node* model4 = osgDB::readNodeFile("../Model_L25_0001000.osgb");
	//osg::Node* model5 = osgDB::readNodeFile("../Model_L25_0001100.osgb");
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
	pat->addChild(model);
	root->addChild(pat.get());
	//root->addChild(model);
	//root->addChild(model1);
	//	root->addChild(model2);
	//		root->addChild(model3);
	//			root->addChild(model4);
	//	root->addChild(model5);
	
	//向场景中添加光源
	//root->addChild(createLightModel(model));
	
	//优化场景数据
	osgUtil::Optimizer optimizer ;
	optimizer.optimize(root.get()) ;

	//for(int i=0;i<root->getNumChildren();i++)
	//{
	//	osg::Node* node = root->getChild(i);
	//	osg::Group* group = dynamic_cast<osg::Group*>(node);
	//	//int k = lod->getNumChildren();
	//	//std::cout<<k<<endl;
	//	for(int j=0;j<group->getNumChildren();j++)
	//	{ osg::LOD* lod = dynamic_cast<osg::LOD*>(group->getChild(j));
	//	
	//	for(int k=0; k<lod->getNumChildren();k++)
	//		{
	//			osg::Geode* geode = dynamic_cast<osg::Geode*>(lod->getChild(k));

	//			for(int l=0;l<geode->getNumDrawables();l++)
	//			{
	//				osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode->getDrawable(l));				
	//				//	NodeNum = geom->getVertexArray()->getNumElements(); 
	//			    geom->setUseVertexBufferObjects( true );
	//				geom->setUpdateCallback( new TryUpdate() );
	//			}

	//		}
	//	}
	//}

	//对于Model.osgb有效。//
	//for(int i=0;i<root->getNumChildren();i++)
	//{
	//	osg::Node* node = root->getChild(i);
	//	osg::LOD* lod = dynamic_cast<osg::LOD*>(node);
	//	//int k = lod->getNumChildren();
	//	//std::cout<<k<<endl;

	//	for(int j=0;j<lod->getNumChildren();j++)
	//	{
	//		osg::Geode* geode = dynamic_cast<osg::Geode*>(lod->getChild(j));

	//		for(int k=0; k<geode->getNumDrawables();k++)
	//		{
	//				osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode->getDrawable(k));				
	//				//	NodeNum = geom->getVertexArray()->getNumElements(); 
	//			    geom->setUseVertexBufferObjects( true );
	//				geom->setUpdateCallback( new TryUpdate() );
	//		}
	//	}		
	//}

		SetAnimation ani;
		root->accept(ani);

			//向场景中添加光源
	//root->addChild(createLightModel(model));
	//root->addChild(createLightModel(model));

	viewer->setSceneData(root.get());
	//viewer->addEventHandler(new osgViewer::StatsHandler);


	viewer->realize();
	viewer->run();
	//float i=0.0;
	//while (!viewer->done())  
 //   {  
	//	//pat->setAttitude(osg::Quat(0,0,1,i));
	//	//i=i+1.0;
	//	viewer->frame();  
 //   }  

	return 0 ;
}

////识别节点，并更新回调
//class DisplaceUpdate : public osg::Drawable::UpdateCallback
//{
//public:
//	DisplaceUpdate() 
//	{
//		step = 0;
//		Tstep = 40;
//		frame = 0;
//	}
//	DisplaceUpdate(float t,double m)
//	{
//		step = 0;
//		Tstep = t;
//		frame = 0;
//	}
//	int frame;
//
//	virtual void update( osg::NodeVisitor* nv, osg::Drawable* drawable )
//
//	{
//		frame++;
//		if(frame>40)
//		{
//			frame=0; 
//		}
//		
//		int Nodenum;
//		double BlgAtr;
//		double bottom;//最小z坐标
//		string name1,name2,name3,name4,name5,name6,name7,name8,name9,name10,name11;
//		float a;
//		const int n=1;//建筑vtk数量
//		
//		//读入层数
//		ifstream ifile0("../BlgAttributessmarthouse2.txt");
//		if(!ifile0) 	{ 	cout << "Cannot open file0.\n"; 	getchar();}
//		ifile0>>name1>>name2>>name3>>name4>>BlgAtr>>bottom;
//		ifile0.close();
//		
//		/*char name[n][32]={"2.vtk"};*/
//		//while(strcmp(name,"#")!=0)//循环输入直到输入“#”结束
//		//{
//		//	ReadPositionVTK(name);
//		//}
//
//
//
//		////调用头文件readvtk.h，但没编完
//		//char * VTKname="2.vtk";
//		//double d[][3]={0};
//		//ifstream ifile1(VTKname);
//		//ReadPositionVTK(VTKname,ifile1,d);
//
//
//
//        //读顶点
//		ifstream ifile1("../house2.vtk");
//		if(!ifile1) 	{ 	cout << "Cannot open file1.\n"; 	getchar();}
//		ifile1>>name1>>name2>>name3>>name4>>name5>>name6>>name7>>name8>>name9>>name10>>Nodenum>>name11;//得到Nodenum,其余name无用
//		//cout<<"顶点Nodenum:"<<Nodenum<<"\n";
//		
//		const int ROW = Nodenum;
//		const int VOL =3;
//		double (*d)[VOL];
//		d=new double[ROW][VOL];
//		
//		for(int i = 0; i < ROW; ++i)
//		{
//			for(int j = 0; j < VOL; ++j)
//				ifile1 >> d[i][j];
//		}
//		ifile1.close();
//
//		//读入位移数据
//		ifstream ifile2("../Blg1_ CBD_IDrevised_2_FloorsDisp.txt");
//		if(!ifile2) 	{ 	cout << "Cannot open file2.\n"; getchar();}
//		ifile2>>name1>>name2>>name3>>name4>>name5;
//		
//		const int ROWd2 =400;//时间
//		const int VOLd2 =100;//楼层数
//		double (*d2)[VOLd2];
//		d2=new double[ROWd2][VOLd2];		
//		for(int m = 0; m < ROWd2; ++m)
//		{		
//			ifile2>>a;
//			for(int n = 0; n < BlgAtr; ++n)
//			{
//				ifile2>>d2[m][n];
//				d2[m][n]=d2[m][n]*100;//位移放大10倍		
//			}
//		}
//		ifile2.close();
//		
//		osg::Geometry* geom = dynamic_cast<osg::Geometry*>( drawable );
//		if ( !geom ) return;
//		osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>( geom->getVertexArray() );
//		
//		for (int i=0;i<Nodenum;i++)//i means vertices array，判断在哪层，加位移
//		{
//			if(((int)((d[i][2]-bottom)/0.08))>(BlgAtr-1))
//				(*vertices)[i].set(d[i][0]+d2[frame][(int)((d[i][2]-bottom)/0.08)-1],d[i][1],d[i][2]);
//				//(*vertices)[i].set(d[i][0]+d2[frame][(int)(d[i][2]/0.1016)],d[i][1],d[i][2]);
//			else (*vertices)[i].set(d[i][0]+d2[frame][(int)((d[i][2]-bottom)/0.08)],d[i][1],d[i][2]);
//			vertices->dirty();	
//          }
//			
//		    delete []d;
//			delete []d2;
//	}
//
//protected:
//	int step;
//	int Tstep;
//};
//
////创建二维纹理状态对象
//osg::ref_ptr<osg::StateSet> createTexture2DState(osg::ref_ptr<osg::Image> image)
//{
//	//创建状态集对象
//	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
//
//	//创建二维纹理对象
//	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
//	texture->setDataVariance(osg::Object::DYNAMIC);
//	//设置贴图
//	texture->setImage(image.get());
//
//	stateset->setTextureAttributeAndModes(0,texture.get(),osg::StateAttribute::ON);
//
//	return stateset.get() ;
//}
//
////创建建筑物节点
//osg::ref_ptr<osg::Node> createPolygon()
//{
//	//创建一个叶节点对象
//	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
//
//	//创建一个几何体对象
//	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
//
//	//创建顶点数组
//	osg::ref_ptr<osg::Vec3Array> v= new osg::Vec3Array();
//
//	//创建纹理坐标数组
//	osg::ref_ptr<osg::Vec2Array> v_Texture= new osg::Vec2Array();
//
//	int Nodenum,Ploynum,meaningless;
//	string name1,name2,name3,name4,name5,name6,name7,name8,name9,name10,name11;
//
//	//int n=1;
//	//for(n=1;n<173;n++)
//
//	//读纹理坐标
//	ifstream ifile0("../101Texture.vtk");
//	if(!ifile0) 	{ 	cout << "Cannot open texture file.\n"; 		return 0; 	}
//	//ifile1.getline(s,30); 
//	ifile0>>name1>>name2>>name3>>name4>>name5>>name6>>name7>>name8>>name9>>name10>>Nodenum>>name11;//得到Nodenum,其余name无用
//	const int ROW_Texture = Nodenum;//顶点数组
//	double (*c)[2];
//	c=new double[ROW_Texture][2];
//	for(int i = 0; i < ROW_Texture; ++i)
//	 {
//		for(int j = 0; j <2; ++j)
//		{
//			ifile0 >> c[i][j];
//		}
//		v_Texture->push_back(osg::Vec2(c[i][0],c[i][1]));
//	 }
//	ifile0.close();
//
//    //导入顶点坐标+顶点索引
//	//ifstream ifile1("../remeshedCBD.vtk");
//	ifstream ifile1("../101.vtk");
//	if(!ifile1) 	{ 	cout << "Cannot open vtk file.\n"; 		return 0; 	}
//	//ifile1.getline(s,30); 
//	ifile1>>name1>>name2>>name3>>name4>>name5>>name6>>name7>>name8>>name9>>name10>>Nodenum>>name11;//得到Nodenum,其余name无用
//	const int ROW = Nodenum;//顶点数组
//	double (*d)[3];
//	d=new double[ROW][3];
//	for(int i = 0; i < ROW; ++i)
//	 {
//		for(int j = 0; j <3; ++j)
//		{
//			ifile1 >> d[i][j];
//		}
//		v->push_back(osg::Vec3(d[i][0],d[i][1],d[i][2]));
//	 }
//	ifile1>>name1>>Ploynum>>meaningless;
//	//cout<<"Ploynum:"<<Ploynum<<endl;
//
//	const int ROWb = Ploynum;//索引数组
//	//const int ROWb = 50000;
//    const int VOLb = 6;
//	int ElemID;//多边形数量
//	string Material;//接受“Material”
//	string MaterialName;
//	double (*f)[VOLb];
//	f=new double [ROWb][VOLb];
//	for (int i=0;i<ROWb;i++)
//	{
//		string MaterialName;//材质名称
//		ifile1.ignore(10,'\n');//在"<<"和getline混用时必须用此项
//		getline(ifile1,MaterialName);
//		ifile1>>ElemID;
//
//		osg::ref_ptr<osg::DrawElementsUInt> polygon= new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON,0);
//		//osg::ref_ptr<osg::UByteArray> Coord_array=new osg::UByteArray;
//
//		for(int j=0;j<ElemID;j++)
//		{
//			ifile1>>f[i][j];
//			polygon->push_back(f[i][j]);
//			//Coord_array->push_back(f[i][j]);
//		}
//		geom->addPrimitiveSet(polygon.get());
//	}	
//	geom->setUseDisplayList( false );
//	string MaterialPath="C:\\CBD(62).jpg";
//	//string MaterialPath="C:\\Users\\Lenovo\\Desktop\\CBD_Texture\\CBD(50).jpg";
//	osg::ref_ptr<osg::Image> image = osgDB::readImageFile(MaterialPath );
//	//osg::ref_ptr<osg::Image> image = osgDB::readImageFile("C:\\Users\\Lenovo\\Desktop\\new\\cement.jpg");
//	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
//	stateset = createTexture2DState(image.get());
//	 //使用二维纹理
//	geom->setStateSet(stateset.get());
//	geom->setUseVertexBufferObjects( true );
//	//geom->setUpdateCallback( new DisplaceUpdate() );
//	ifile1.close();
//
//	//设置顶点数据
//	geom->setVertexArray(v.get());
//	//设置纹理坐标数据
//	geom->setTexCoordArray(0,v_Texture);
//	//geom->setVertexIndices(Coord_array);
//	//geom->setTexCoordIndices(0,Coord_array);
//	//geom->setTexCoordIndices(0,Coord_array);
//
//	osgUtil::SmoothingVisitor::smooth(*(geom.get()));
//	
//	geode->addDrawable(geom.get());
//	return geode.get();
//	delete []d;
//	delete []f;
//}