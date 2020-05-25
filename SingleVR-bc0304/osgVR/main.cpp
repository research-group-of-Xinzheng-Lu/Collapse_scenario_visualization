#ifdef _DEBUG
#pragma comment(lib, "osgViewerd.lib")
#pragma comment(lib, "osgDBd.lib")
#pragma comment(lib, "OpenThreadsd.lib")
#pragma comment(lib, "osgd.lib")
#pragma comment(lib, "osgGAd.lib")
#pragma comment(lib, "osgUtild.lib")
#else
#pragma comment(lib, "osgViewer.lib")
#pragma comment(lib, "osgDB.lib")
#pragma comment(lib, "OpenThreads.lib")
#pragma comment(lib, "osg.lib")
#pragma comment(lib, "osgGA.lib")
#pragma comment(lib, "osgUtil.lib")
#endif

#include "readvtk.h"			//没编完
#include "Global.h"				//全局
#include "AttrDispReader.h"		//读取文件模块
#include "ModelLoader.h"		//模型加载模块
#include "DamageStateLoader.h"

using namespace std;

osg::Node** modelNodeList;
osg::Node** envNodeList;
void LoadModel();
void LoadEnv();

class UseEventHandler:public::osgGA::GUIEventHandler{
public:
	virtual bool handle(const osgGA::GUIEventAdapter &ea,osgGA::GUIActionAdapter &aa){
		osgViewer::Viewer *viewer=dynamic_cast<osgViewer::Viewer*>(&aa);
		if (!viewer)
			return false;		
		switch(ea.getEventType()){
		case osgGA::GUIEventAdapter::KEYDOWN:
			if (ea.getKey()==0xFF09)
				isPause = !isPause;
			break;
		default:
			break;
		}
		return false;
	}
};

class TryUpdate : public osg::Drawable::UpdateCallback{
public:
	int frame;
	int nowTimePoint;
	//int verticeLabel;

	TryUpdate(){
		frame = 0;
		nowTimePoint = 0;
		//verticeLabel = 0;
	}

	virtual void update(osg::NodeVisitor* nv, osg::Drawable* drawable){
		//动画开始前设置一定时长的idle，用于（从容地）调整视角
		if(frame < idleFrame){
			frame++;
			return ;
		}

		if(isPause)
			return;

		if(isFrameSkip && frame % frameSkip != 0){
			frame++;
			return ;
		}

		if(nowTimePoint == nTimePoint){
			nowTimePoint = 0;
			frame = 0;
		}

		//cout<<"test"<<endl;

		//osg::Geometry* geomtest = dynamic_cast<osg::Geometry*>(drawable);

		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(drawable);

		if (!geom)
			return;

		osg::BoundingBox bx = geom->getBoundingBox();
		//cout<<geom->getName()<<endl;
		string tempID;
		stringstream ss(geom->getName());
		getline(ss, tempID, ' ');

		//如果是环境模型，跳过
		if(tempID[0]=='e')
			return;

		int bdIndex = idMap[atoi(tempID.c_str())];
		float z1 = bx.zMin();
		float z2 = bx.zMax();
		float x1 = bx.xMin();
		float x2 = bx.xMax();
		float y1 = bx.yMin();
		float y2 = bx.yMax();

		//cout<<geom->getName()<<" "<<bdIndex<<endl;
		float l= (z2-z1)/(atoi(attrArray[bdIndex][storeyIndex].c_str()));		//每层高度
		osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		if(verticeLabel < 1){
			//cout<<verticeLabel<<endl;
			for (int i=0;i<1;i++){
				cout<<(*vertices)[1].z()<<endl;
			}
			verticeLabel++;
			//cout<<verticeLabel<<"	"<<bdIndex<<endl;
		}

		//cout<<(*vertices)[1].x();
		//if(verticeLabel < 1){
		//	verticeLabel++;
		//	for (int i=0;i<vertices->getNumElements();i++){
		//		VerticeXcoordsArray[bdIndex].push_back((*vertices)[i].x());		//获取建筑模型几何点X坐标
		//		VerticeYcoordsArray[bdIndex].push_back((*vertices)[i].y());		//获取建筑模型几何点Y坐标
		//		VerticeZcoordsArray[bdIndex].push_back((*vertices)[i].z());		//获取建筑模型几何点Z坐标
		//  }
		//}

		if(setDamage){
			//cout<<"Set Damage"<<endl;
			//cout<<"Frame	"<<frame<<endl;
			if(nowTimePoint == 0)
				;		//跳过第0帧，避免数组越界		
			else if(damageState[bdIndex]!=-1 && (nowTimePoint > damageState[bdIndex] && nowTimePoint < damageState[bdIndex]+200)){		//如果到了破坏时刻
				//cout<<"damage occured."<<endl;
				if (damageType[bdIndex] == 0){
					if (nowTimePoint < damageState[bdIndex]+41){
						for (int i=0;i<vertices->getNumElements();i++){
							float zDisp = 0.0;
							int collapseTimePoint = 0;
							collapseTimePoint = nowTimePoint - damageState[bdIndex];
							zDisp = -((*vertices)[i].z()-z1)/(41-collapseTimePoint);		//倒塌时间20帧
							(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z()+zDisp);
						}						
					}
					else{
						for (int i=0;i<vertices->getNumElements();i++){
							(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z());
						}		
					}						
						//(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z()*collapsSpeed+(1-collapsSpeed)*z1);
				}
				else if (damageType[bdIndex] == 1){
					if (nowTimePoint < damageState[bdIndex]+21){
						for (int i=0;i<vertices->getNumElements();i++){
							float z = (*vertices)[i].z()-z1;							
							float zDisp = 0.0;
							int collapseTimePoint = 0;							
							collapseTimePoint = nowTimePoint - damageState[bdIndex];
							float revHeight = (z+(collapseTimePoint-1)*l*0.05)/l;
							int storeyZ = int(revHeight);				//判断顶点所在层
							float disToFloor = revHeight-storeyZ;		//判断顶点所在层中位置，为插值准备
							//插值
							if(storeyZ == 0){
								zDisp = -((*vertices)[i].z()-z1)/(21-collapseTimePoint);		//倒塌时间20帧
							}
							else {
								zDisp = -l*0.05;
							}
							(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z()+zDisp);
							//float zDisp = 0.0;
							//zDisp = -l*0.05;
							//(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z()+zDisp);			
						}
					}
					else{
						for (int i=0;i<vertices->getNumElements();i++){
							(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z());
						}
					}
				}
				else if (damageType[bdIndex] == 2){
					if (nowTimePoint < damageState[bdIndex]+21){
						for (int i=0;i<vertices->getNumElements();i++){
							float z = (*vertices)[i].z()-z1;
							float zDisp = 0.0;
							int collapseTimePoint = 0;
							collapseTimePoint = nowTimePoint - damageState[bdIndex];
							float revHeight = (z+(collapseTimePoint-1)*3*l*0.05)/l;	//底部3层倒塌
							int storeyZ = int(revHeight);							//判断顶点所在层
							float disToFloor = revHeight-storeyZ;					//判断顶点所在层中位置，为插值准备							
							//插值
							if(storeyZ == 0 || storeyZ == 1 || storeyZ == 2){
								zDisp = -((*vertices)[i].z()-z1)/(21-collapseTimePoint);		//倒塌时间20帧
							}
							else {
								zDisp = -3*l*0.05;
							}
							(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z()+zDisp);		
						}
					}
					else{
						for (int i=0;i<vertices->getNumElements();i++){
							(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z());
						}
					}
				}
				else if (damageType[bdIndex] == 3){
					if (nowTimePoint < damageState[bdIndex]+21){
						for (int i=0;i<vertices->getNumElements();i++){
							float z = (*vertices)[i].z()-z1;
							float zDisp = 0.0;
							//int collapseTimePoint = 0;
							//collapseTimePoint = nowTimePoint - damageState[bdIndex];
							//float revHeight = (z+(collapseTimePoint-1)*l*0.05)/l;	//中间层倒塌
							float revHeight = z/l;									//中间层倒塌
							int storeyZ = int(revHeight);							//判断顶点所在层												
							//插值
							if(storeyZ >= collapseStorey[bdIndex]-1){
								zDisp = -l*0.05;		//倒塌时间20帧
							}
							(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z()+zDisp);		
						}
					}
					else{
						for (int i=0;i<vertices->getNumElements();i++){
							(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z());
						}
					}
				}
				else if (damageType[bdIndex] == 4){
					if (nowTimePoint < damageState[bdIndex]+29){
						if (rotateDirctn[bdIndex] == 1){
							for (int i=0;i<vertices->getNumElements();i++){
								float yLocalDisp = 0.0;		//局部坐标系y坐标变化
								float zLocalDisp = 0.0;		//局部坐标系z坐标变化
								float xDisp = 0.0;			//整体坐标系x坐标变化
								float yDisp = 0.0;			//整体坐标系y坐标变化
								float zDisp = 0.0;			//整体坐标系z坐标变化
								float dToRotAxs =0.0;		//几何点到旋转轴的距离
								float dToApt = 0.0;			//几何点在旋转轴垂足到上A'点的距离
								float sinAlpha = 0.0;
								float cosAlpha = 0.0;
								float sinHalfTheta = 0.0;
								float cosHalfTheta = 0.0;
								//float sinGamma = 0.4679;
								//float cosGamma = 0.8838;
								float sinGamma = sin(PI_F/4);
								float cosGamma = cos(PI_F/4);
								dToApt = ((rotateAxis[1][0]-rotateAxis[0][0])*((*vertices)[i].x()-rotateAxis[0][0])+(rotateAxis[1][1]-rotateAxis[0][1])*((*vertices)[i].y()-rotateAxis[0][1]))/sqrt(pow(rotateAxis[1][0]-rotateAxis[0][0],2)+pow(rotateAxis[1][1]-rotateAxis[0][1],2));
								dToRotAxs = sqrt(pow((*vertices)[i].x()-rotateAxis[0][0],2)+pow((*vertices)[i].y()-rotateAxis[0][1],2)+pow((*vertices)[i].z()-rotateAxis[0][2],2)-pow(dToApt,2));
								cosAlpha = abs(((*vertices)[i].z()-rotateAxis[0][2])/dToRotAxs);
								sinAlpha = sqrt(1-pow(cosAlpha,2));
								sinHalfTheta = sin((3.0f/90.0f)/2*PI_F/2);
								cosHalfTheta = cos((3.0f/90.0f)/2*PI_F/2);
								if (((*vertices)[i].y()-rotateAxis[0][1])*cosGamma-((*vertices)[i].x()-rotateAxis[0][0])*sinGamma < 0){
									yLocalDisp = 2*dToRotAxs*sinHalfTheta*(cosHalfTheta*cosAlpha+sinHalfTheta*sinAlpha);
									zLocalDisp = -2*dToRotAxs*sinHalfTheta*(sinHalfTheta*cosAlpha-cosHalfTheta*sinAlpha);
									xDisp = -yLocalDisp*sinGamma;
									yDisp = yLocalDisp*cosGamma;
									zDisp = zLocalDisp;
									(*vertices)[i].set((*vertices)[i].x()+xDisp, (*vertices)[i].y()+yDisp, (*vertices)[i].z()+zDisp);
								}
								else{
									yLocalDisp = 2*dToRotAxs*sinHalfTheta*(cosHalfTheta*cosAlpha-sinHalfTheta*sinAlpha);
									zLocalDisp = -2*dToRotAxs*sinHalfTheta*(sinHalfTheta*cosAlpha+cosHalfTheta*sinAlpha);
									xDisp = -yLocalDisp*sinGamma;
									yDisp = yLocalDisp*cosGamma;
									zDisp = zLocalDisp;
									(*vertices)[i].set((*vertices)[i].x()+xDisp, (*vertices)[i].y()+yDisp, (*vertices)[i].z()+zDisp);
								}								
							}
						}
						else if (rotateDirctn[bdIndex] == -1){
							for (int i=0;i<vertices->getNumElements();i++){
								float yLocalDisp = 0.0;		//局部坐标系y坐标变化
								float zLocalDisp = 0.0;		//局部坐标系z坐标变化
								float xDisp = 0.0;			//整体坐标系x坐标变化
								float yDisp = 0.0;			//整体坐标系y坐标变化
								float zDisp = 0.0;			//整体坐标系z坐标变化
								float dToRotAxs =0.0;		//几何点到旋转轴的距离
								float dToApt = 0.0;			//几何点在旋转轴垂足到上A'点的距离
								float sinAlpha = 0.0;
								float cosAlpha = 0.0;
								float sinHalfTheta = 0.0;
								float cosHalfTheta = 0.0;
								//float sinGamma = 0.4679;
								//float cosGamma = 0.8838;
								float sinGamma = sin(PI_F/4);
								float cosGamma = cos(PI_F/4);
								dToApt = ((rotateAxis[1][0]-rotateAxis[0][0])*((*vertices)[i].x()-rotateAxis[0][0])+(rotateAxis[1][1]-rotateAxis[0][1])*((*vertices)[i].y()-rotateAxis[0][1]))/sqrt(pow(rotateAxis[1][0]-rotateAxis[0][0],2)+pow(rotateAxis[1][1]-rotateAxis[0][1],2));
								dToRotAxs = sqrt(pow((*vertices)[i].x()-rotateAxis[0][0],2)+pow((*vertices)[i].y()-rotateAxis[0][1],2)+pow((*vertices)[i].z()-rotateAxis[0][2],2)-pow(dToApt,2));
								cosAlpha = abs(((*vertices)[i].z()-rotateAxis[0][2])/dToRotAxs);
								sinAlpha = sqrt(1-pow(cosAlpha,2));
								sinHalfTheta = sin((3.0f/90.0f)/2*PI_F/2);
								cosHalfTheta = cos((3.0f/90.0f)/2*PI_F/2);
								if (((*vertices)[i].y()-rotateAxis[0][1])*cosGamma-((*vertices)[i].x()-rotateAxis[0][0])*sinGamma < 0){
									if ((*vertices)[i].z()-rotateAxis[0][2] > 0){
										yLocalDisp = -2*dToRotAxs*sinHalfTheta*(cosHalfTheta*cosAlpha-sinHalfTheta*sinAlpha);
										zLocalDisp = -2*dToRotAxs*sinHalfTheta*(sinHalfTheta*cosAlpha+cosHalfTheta*sinAlpha);
										xDisp = -yLocalDisp*sinGamma;
										yDisp = yLocalDisp*cosGamma;
										zDisp = zLocalDisp;
										(*vertices)[i].set((*vertices)[i].x()+xDisp, (*vertices)[i].y()+yDisp, (*vertices)[i].z()+zDisp);
									}
									else{
										(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z());
									}
								}
								else{
									if ((*vertices)[i].z()-rotateAxis[0][2] > 0){
										yLocalDisp = -2*dToRotAxs*sinHalfTheta*(cosHalfTheta*cosAlpha+sinHalfTheta*sinAlpha);
										zLocalDisp = -2*dToRotAxs*sinHalfTheta*(sinHalfTheta*cosAlpha-cosHalfTheta*sinAlpha);
										xDisp = -yLocalDisp*sinGamma;
										yDisp = yLocalDisp*cosGamma;
										zDisp = zLocalDisp;
										(*vertices)[i].set((*vertices)[i].x()+xDisp, (*vertices)[i].y()+yDisp, (*vertices)[i].z()+zDisp);
									}
									else{
										(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z());
									}
								}
							}
						}
					}
					else{
						for (int i=0;i<vertices->getNumElements();i++){
							(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z());	
						}
					}		
				}
				//else if (damageType[bdIndex] == 4){
				//	if (nowTimePoint < damageState[bdIndex]+29){
				//		if (rotateDirctn[bdIndex] == 1){
				//			for (int i=0;i<vertices->getNumElements();i++){
				//				float yDisp = 0.0;
				//				float zDisp = 0.0;
				//				float dToRotAxs =0.0;	//几何点到旋转轴的距离
				//				float dToApt = 0.0;		//几何点到旋转轴上A'点的距离
				//				float sinAlpha = 0.0;
				//				float cosAlpha = 0.0;
				//				float sinHalfTheta = 0.0;
				//				float cosHalfTheta = 0.0;
				//				dToApt = fabs((*vertices)[i].x()-rotateAxis[0][0]);
				//				dToRotAxs = sqrt(pow((*vertices)[i].x()-rotateAxis[0][0],2)+pow((*vertices)[i].y()-rotateAxis[0][1],2)+pow((*vertices)[i].z()-rotateAxis[0][2],2)-pow(dToApt,2));
				//				sinAlpha = abs(((*vertices)[i].y()-rotateAxis[0][1])/dToRotAxs);
				//				cosAlpha = abs(((*vertices)[i].z()-rotateAxis[0][2])/dToRotAxs);
				//				sinHalfTheta = sin((3.0f/90.0f)/2*PI_F/2);
				//				cosHalfTheta = cos((3.0f/90.0f)/2*PI_F/2);
				//				if ((*vertices)[i].y()-rotateAxis[0][1] < 0){
				//					yDisp = 2*dToRotAxs*sinHalfTheta*(cosHalfTheta*cosAlpha+sinHalfTheta*sinAlpha);
				//					zDisp = -2*dToRotAxs*sinHalfTheta*(sinHalfTheta*cosAlpha-cosHalfTheta*sinAlpha);
				//					(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y()+yDisp, (*vertices)[i].z()+zDisp);
				//				}
				//				else{
				//					yDisp = 2*dToRotAxs*sinHalfTheta*(cosHalfTheta*cosAlpha-sinHalfTheta*sinAlpha);
				//					zDisp = -2*dToRotAxs*sinHalfTheta*(sinHalfTheta*cosAlpha+cosHalfTheta*sinAlpha);
				//					(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y()+yDisp, (*vertices)[i].z()+zDisp);
				//				}								
				//			}
				//		}
				//		else if (rotateDirctn[bdIndex] == -1){
				//			for (int i=0;i<vertices->getNumElements();i++){
				//				float yDisp = 0.0;
				//				float zDisp = 0.0;
				//				float dToRotAxs =0.0;	//几何点到旋转轴的距离
				//				float dToApt = 0.0;		//几何点到旋转轴上A'点的距离
				//				float sinAlpha = 0.0;
				//				float cosAlpha = 0.0;
				//				float sinHalfTheta = 0.0;
				//				float cosHalfTheta = 0.0;
				//				dToApt = fabs((*vertices)[i].x()-rotateAxis[0][0]);
				//				dToRotAxs = sqrt(pow((*vertices)[i].x()-rotateAxis[0][0],2)+pow((*vertices)[i].y()-rotateAxis[0][1],2)+pow((*vertices)[i].z()-rotateAxis[0][2],2)-pow(dToApt,2));
				//				sinAlpha = abs(((*vertices)[i].y()-rotateAxis[0][1])/dToRotAxs);
				//				cosAlpha = abs(((*vertices)[i].z()-rotateAxis[0][2])/dToRotAxs);
				//				sinHalfTheta = sin((3.0f/90.0f)/2*PI_F/2);
				//				cosHalfTheta = cos((3.0f/90.0f)/2*PI_F/2);
				//				if ((*vertices)[i].y()-rotateAxis[0][1] < 0){
				//					if ((*vertices)[i].z()-rotateAxis[0][2] > 0){
				//						yDisp = -2*dToRotAxs*sinHalfTheta*(cosHalfTheta*cosAlpha-sinHalfTheta*sinAlpha);
				//						zDisp = -2*dToRotAxs*sinHalfTheta*(sinHalfTheta*cosAlpha+cosHalfTheta*sinAlpha);
				//						(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y()+yDisp, (*vertices)[i].z()+zDisp);
				//					}
				//					else{
				//						(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z());
				//					}
				//				}
				//				else{
				//					if ((*vertices)[i].z()-rotateAxis[0][2] > 0){
				//						yDisp = -2*dToRotAxs*sinHalfTheta*(cosHalfTheta*cosAlpha+sinHalfTheta*sinAlpha);
				//						zDisp = -2*dToRotAxs*sinHalfTheta*(sinHalfTheta*cosAlpha-cosHalfTheta*sinAlpha);
				//						(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y()+yDisp, (*vertices)[i].z()+zDisp);
				//					}
				//					else{
				//						(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z());
				//					}
				//				}
				//			}
				//		}
				//	}
				//	else{
				//		for (int i=0;i<vertices->getNumElements();i++){
				//			(*vertices)[i].set((*vertices)[i].x(), (*vertices)[i].y(), (*vertices)[i].z());	
				//		}
				//	}		
				//}
			}

			else if(damageState[bdIndex]!=-1 && nowTimePoint > damageState[bdIndex]+200)
				return ;

			else{
				for (int i=0;i<vertices->getNumElements();i++){
					float z = (*vertices)[i].z()-z1;
					float revHeight = z/l;
					int storeyZ = int(revHeight);				//判断顶点所在层
					float disToFloor = revHeight-storeyZ;		//判断顶点所在层中位置，为插值准备
					float xDisp = 0.0;
					float yDisp = 0.0;
					//插值
					if(storeyZ == 0)
						xDisp = (disToFloor*XdispArray[bdIndex][nowTimePoint][storeyZ]-disToFloor*XdispArray[bdIndex][nowTimePoint-1][storeyZ])*globalDispScale;
					else
						xDisp = ((disToFloor*XdispArray[bdIndex][nowTimePoint][storeyZ]-(disToFloor-1)*XdispArray[bdIndex][nowTimePoint][storeyZ-1])-(disToFloor*XdispArray[bdIndex][nowTimePoint-1][storeyZ]-(disToFloor-1)*XdispArray[bdIndex][nowTimePoint-1][storeyZ-1]))*globalDispScale;
				
					if(storeyZ == 0)
						yDisp = (disToFloor*YdispArray[bdIndex][nowTimePoint][storeyZ]-disToFloor*YdispArray[bdIndex][nowTimePoint-1][storeyZ])*globalDispScale;
					else
						yDisp = ((disToFloor*YdispArray[bdIndex][nowTimePoint][storeyZ]-(disToFloor-1)*YdispArray[bdIndex][nowTimePoint][storeyZ-1])-(disToFloor*YdispArray[bdIndex][nowTimePoint-1][storeyZ]-(disToFloor-1)*YdispArray[bdIndex][nowTimePoint-1][storeyZ-1]))*globalDispScale;
					//cout<<xDisp<<" "<<yDisp<<endl;
					(*vertices)[i].set((*vertices)[i].x()+xDisp, (*vertices)[i].y()+yDisp, (*vertices)[i].z());
				}
			}
		}

		else{
			//cout<<"No Damage"<<endl;

			if(nowTimePoint == 0)
				;
			else{
				for (int i=0;i<vertices->getNumElements();i++){
					float z = (*vertices)[i].z()-z1;
					float revHeight = z/l;
					int storeyZ = int(revHeight);				//判断顶点所在层
					float disToFloor = revHeight-storeyZ;		//判断顶点所在层中位置，为插值准备
					float xDisp = 0.0;
					float yDisp = 0.0;
					//插值
					if(storeyZ == 0)
						xDisp = (disToFloor*XdispArray[bdIndex][nowTimePoint][storeyZ]-disToFloor*XdispArray[bdIndex][nowTimePoint-1][storeyZ])*globalDispScale;
					else
						xDisp = ((disToFloor*XdispArray[bdIndex][nowTimePoint][storeyZ]-(disToFloor-1)*XdispArray[bdIndex][nowTimePoint][storeyZ-1])-(disToFloor*XdispArray[bdIndex][nowTimePoint-1][storeyZ]-(disToFloor-1)*XdispArray[bdIndex][nowTimePoint-1][storeyZ-1]))*globalDispScale;
				
					if(storeyZ == 0)
						yDisp = (disToFloor*YdispArray[bdIndex][nowTimePoint][storeyZ]-disToFloor*YdispArray[bdIndex][nowTimePoint-1][storeyZ])*globalDispScale;
					else
						yDisp = ((disToFloor*YdispArray[bdIndex][nowTimePoint][storeyZ]-(disToFloor-1)*YdispArray[bdIndex][nowTimePoint][storeyZ-1])-(disToFloor*YdispArray[bdIndex][nowTimePoint-1][storeyZ]-(disToFloor-1)*YdispArray[bdIndex][nowTimePoint-1][storeyZ-1]))*globalDispScale;
					//cout<<xDisp<<" "<<yDisp<<endl;
					(*vertices)[i].set((*vertices)[i].x()+xDisp, (*vertices)[i].y()+yDisp, (*vertices)[i].z());			

				}
			}
			//TODO: 这里只是临时随便找一帧输出，这个逻辑应该放到破坏组
			if(useDamageSnapshot){
				if(nowTimePoint == 143){
					string filePath = "G:\\osg_test\\SingleVR2\\_data\\JiaXingGuangChang\\JX2\\"+tempID+".fbx";
					osgDB::Options *op2 = new osgDB::Options();
					op2->setOptionString("Embedded");
					//cout<<op2->getOptionString()<<endl;
					osgDB::writeNodeFile(*(modelNodeList[bdIndex]), filePath, op2);
				}
			}
		}

		if(useVBO)
			vertices->dirty();	
		else
			geom->dirtyDisplayList();

		frame++;
		nowTimePoint++;
	}
};

//加光照
osg::ref_ptr<osg::Group> createLightModel(osg::ref_ptr<osg::Node> node){
	osg::ref_ptr<osg::Group> lightRoot= new osg::Group();
	lightRoot->addChild(node);

	//开启光照
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	stateset = lightRoot->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
	//stateset->setMode(GL_LIGHT_MODEL_TWO_SIDE,osg::StateAttribute::ON);	//此行持续报错，慎用

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
};

//创建自定义粒子系统
osg::ref_ptr<osg::Group> CreateMyParticleScene_0(int num_smokeCoord)
{
	osg::ref_ptr<osg::Group> root = new osg::Group();

	osgParticle::Particle ptemplate;
	ptemplate.setLifeTime(30);
	ptemplate.setSizeRange(osgParticle::rangef(5.0f,10.0f));
	ptemplate.setAlphaRange(osgParticle::rangef(0.0f,1.0f));
	ptemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.4f,0.4f,0.4f,1.0f),osg::Vec4(0.5f,0.5f,0.5f,1.0f)));
	ptemplate.setRadius(1.0f);
	ptemplate.setMass(0.05f);

	osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
	ps->setDefaultAttributes("C://OSG//OpenSceneGraph-Data-3.4.0//Images//Smoke.rgb",false,false);
	ps->setDefaultParticleTemplate(ptemplate);

	osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter();
	emitter->setParticleSystem(ps.get());

	osg::ref_ptr<osgParticle::RandomRateCounter> counter = new osgParticle::RandomRateCounter();
	counter->setRateRange(2.0f,5.0f);
	emitter->setCounter(counter.get());

	osgParticle::MultiSegmentPlacer* lineSegment = new osgParticle::MultiSegmentPlacer();
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0],smokeCoordsArray[num_smokeCoord][1]+13.0f,517.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+12.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,517.0f);	
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-12.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,517.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0],smokeCoordsArray[num_smokeCoord][1]-13.0f,517.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+12.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,517.0f);	
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-12.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,517.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+25.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,517.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-25.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,517.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+25.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,517.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-25.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,517.0f);
	emitter->setPlacer(lineSegment); 

	osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter();
	shooter->setInitialSpeedRange(20.0f,25.0f);
	//shooter->setPhiRange((osg::PI)/4.0,(osg::PI)/4.0);
	emitter->setShooter(shooter.get());

	root->addChild(emitter.get());

	osg::ref_ptr<osgParticle::ModularProgram> program = new osgParticle::ModularProgram();
	program->setParticleSystem(ps.get());

	osg::ref_ptr<osgParticle::AccelOperator> ap = new osgParticle::AccelOperator();
	ap->setToGravity(-0.5f);
	program->addOperator(ap.get());

	osg::ref_ptr<osgParticle::FluidFrictionOperator> ffo = new osgParticle::FluidFrictionOperator();
	ffo->setFluidToAir();
	program->addOperator(ffo.get());

	root->addChild(program.get());

	osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu = new osgParticle::ParticleSystemUpdater();
	psu->addParticleSystem(ps.get());

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(ps.get());

	root->addChild(geode.get());
	root->addChild(psu.get());

	return root.get();
}

osg::ref_ptr<osg::Group> CreateMyParticleScene_1(int num_smokeCoord)
{
	osg::ref_ptr<osg::Group> root = new osg::Group();

	osgParticle::Particle ptemplate;
	ptemplate.setLifeTime(30);
	ptemplate.setSizeRange(osgParticle::rangef(5.0f,10.0f));
	ptemplate.setAlphaRange(osgParticle::rangef(0.0f,1.0f));
	ptemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.4f,0.4f,0.4f,1.0f),osg::Vec4(0.5f,0.5f,0.5f,1.0f)));
	ptemplate.setRadius(1.0f);
	ptemplate.setMass(0.05f);

	osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
	ps->setDefaultAttributes("C://OSG//OpenSceneGraph-Data-3.4.0//Images//Smoke.rgb",false,false);
	ps->setDefaultParticleTemplate(ptemplate);

	osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter();
	emitter->setParticleSystem(ps.get());

	osg::ref_ptr<osgParticle::RandomRateCounter> counter = new osgParticle::RandomRateCounter();
	counter->setRateRange(2.0f,5.0f);
	emitter->setCounter(counter.get());

	osgParticle::MultiSegmentPlacer* lineSegment = new osgParticle::MultiSegmentPlacer();
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0],smokeCoordsArray[num_smokeCoord][1]+13.0f,526.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+13.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,526.0f);	
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-13.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,526.0f);	
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0],smokeCoordsArray[num_smokeCoord][1]-13.0f,526.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+13.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,526.0f);	
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-13.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,526.0f);	
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+27.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,526.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-27.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,526.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+27.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,526.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-27.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,526.0f);
	emitter->setPlacer(lineSegment); 

	osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter();
	shooter->setInitialSpeedRange(20.0f,25.0f);
	//shooter->setPhiRange((osg::PI)/4.0,(osg::PI)/4.0);
	emitter->setShooter(shooter.get());

	root->addChild(emitter.get());

	osg::ref_ptr<osgParticle::ModularProgram> program = new osgParticle::ModularProgram();
	program->setParticleSystem(ps.get());

	osg::ref_ptr<osgParticle::AccelOperator> ap = new osgParticle::AccelOperator();
	ap->setToGravity(-0.5f);
	program->addOperator(ap.get());

	osg::ref_ptr<osgParticle::FluidFrictionOperator> ffo = new osgParticle::FluidFrictionOperator();
	ffo->setFluidToAir();
	program->addOperator(ffo.get());

	root->addChild(program.get());

	osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu = new osgParticle::ParticleSystemUpdater();
	psu->addParticleSystem(ps.get());

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(ps.get());

	root->addChild(geode.get());
	root->addChild(psu.get());

	return root.get();
}

osg::ref_ptr<osg::Group> CreateMyParticleScene_2(int num_smokeCoord)
{
	osg::ref_ptr<osg::Group> root = new osg::Group();

	osgParticle::Particle ptemplate;
	ptemplate.setLifeTime(30);
	ptemplate.setSizeRange(osgParticle::rangef(5.0f,10.0f));
	ptemplate.setAlphaRange(osgParticle::rangef(0.0f,1.0f));
	ptemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.4f,0.4f,0.4f,1.0f),osg::Vec4(0.5f,0.5f,0.5f,1.0f)));
	ptemplate.setRadius(1.0f);
	ptemplate.setMass(0.05f);

	osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
	ps->setDefaultAttributes("C://OSG//OpenSceneGraph-Data-3.4.0//Images//Smoke.rgb",false,false);
	ps->setDefaultParticleTemplate(ptemplate);

	osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter();
	emitter->setParticleSystem(ps.get());

	osg::ref_ptr<osgParticle::RandomRateCounter> counter = new osgParticle::RandomRateCounter();
	counter->setRateRange(2.0f,5.0f);
	emitter->setCounter(counter.get());

	osgParticle::MultiSegmentPlacer* lineSegment = new osgParticle::MultiSegmentPlacer();
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0],smokeCoordsArray[num_smokeCoord][1]+13.0f,535.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+15.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,535.0f);	
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-15.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,535.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+28.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,535.0f);	
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-28.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,535.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0],smokeCoordsArray[num_smokeCoord][1]-13.0f,535.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+15.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,535.0f);	
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-15.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,535.0f);	
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+28.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,535.0f);	
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-28.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,535.0f);	
	emitter->setPlacer(lineSegment);

	osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter();
	shooter->setInitialSpeedRange(20.0f,25.0f);
	//shooter->setPhiRange((osg::PI)/4.0,(osg::PI)/4.0);
	emitter->setShooter(shooter.get());

	root->addChild(emitter.get());

	osg::ref_ptr<osgParticle::ModularProgram> program = new osgParticle::ModularProgram();
	program->setParticleSystem(ps.get());

	osg::ref_ptr<osgParticle::AccelOperator> ap = new osgParticle::AccelOperator();
	ap->setToGravity(-0.5f);
	program->addOperator(ap.get());

	osg::ref_ptr<osgParticle::FluidFrictionOperator> ffo = new osgParticle::FluidFrictionOperator();
	ffo->setFluidToAir();
	program->addOperator(ffo.get());

	root->addChild(program.get());

	osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu = new osgParticle::ParticleSystemUpdater();
	psu->addParticleSystem(ps.get());

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(ps.get());

	root->addChild(geode.get());
	root->addChild(psu.get());

	return root.get();
}

osg::ref_ptr<osg::Group> CreateMyParticleScene_3(int num_smokeCoord)
{
	osg::ref_ptr<osg::Group> root = new osg::Group();

	osgParticle::Particle ptemplate;
	ptemplate.setLifeTime(30);
	ptemplate.setSizeRange(osgParticle::rangef(5.0f,10.0f));
	ptemplate.setAlphaRange(osgParticle::rangef(0.0f,1.0f));
	ptemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.4f,0.4f,0.4f,1.0f),osg::Vec4(0.5f,0.5f,0.5f,1.0f)));
	ptemplate.setRadius(1.0f);
	ptemplate.setMass(0.05f);

	osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
	ps->setDefaultAttributes("C://OSG//OpenSceneGraph-Data-3.4.0//Images//Smoke.rgb",false,false);
	ps->setDefaultParticleTemplate(ptemplate);

	osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter();
	emitter->setParticleSystem(ps.get());

	osg::ref_ptr<osgParticle::RandomRateCounter> counter = new osgParticle::RandomRateCounter();
	counter->setRateRange(2.0f,5.0f);
	emitter->setCounter(counter.get());

	osgParticle::MultiSegmentPlacer* lineSegment = new osgParticle::MultiSegmentPlacer();
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+40.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,517.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-40.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,517.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+40.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,517.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-40.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,517.0f);
	emitter->setPlacer(lineSegment); 

	osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter();
	shooter->setInitialSpeedRange(20.0f,25.0f);
	//shooter->setPhiRange((osg::PI)/4.0,(osg::PI)/4.0);
	emitter->setShooter(shooter.get());

	root->addChild(emitter.get());

	osg::ref_ptr<osgParticle::ModularProgram> program = new osgParticle::ModularProgram();
	program->setParticleSystem(ps.get());

	osg::ref_ptr<osgParticle::AccelOperator> ap = new osgParticle::AccelOperator();
	ap->setToGravity(-0.5f);
	program->addOperator(ap.get());

	osg::ref_ptr<osgParticle::FluidFrictionOperator> ffo = new osgParticle::FluidFrictionOperator();
	ffo->setFluidToAir();
	program->addOperator(ffo.get());

	root->addChild(program.get());

	osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu = new osgParticle::ParticleSystemUpdater();
	psu->addParticleSystem(ps.get());

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(ps.get());

	root->addChild(geode.get());
	root->addChild(psu.get());

	return root.get();
}

osg::ref_ptr<osg::Group> CreateMyParticleScene_4(int num_smokeCoord)
{
	osg::ref_ptr<osg::Group> root = new osg::Group();

	osgParticle::Particle ptemplate;
	ptemplate.setLifeTime(30);
	ptemplate.setSizeRange(osgParticle::rangef(5.0f,10.0f));
	ptemplate.setAlphaRange(osgParticle::rangef(0.0f,1.0f));
	ptemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.4f,0.4f,0.4f,1.0f),osg::Vec4(0.5f,0.5f,0.5f,1.0f)));
	ptemplate.setRadius(1.0f);
	ptemplate.setMass(0.05f);

	osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
	ps->setDefaultAttributes("C://OSG//OpenSceneGraph-Data-3.4.0//Images//Smoke.rgb",false,false);
	ps->setDefaultParticleTemplate(ptemplate);

	osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter();
	emitter->setParticleSystem(ps.get());

	osg::ref_ptr<osgParticle::RandomRateCounter> counter = new osgParticle::RandomRateCounter();
	counter->setRateRange(2.0f,5.0f);
	emitter->setCounter(counter.get());

	osgParticle::MultiSegmentPlacer* lineSegment = new osgParticle::MultiSegmentPlacer();
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+45.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,526.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-45.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,526.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+45.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,526.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-45.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,526.0f);
	emitter->setPlacer(lineSegment); 

	osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter();
	shooter->setInitialSpeedRange(20.0f,25.0f);
	//shooter->setPhiRange((osg::PI)/4.0,(osg::PI)/4.0);
	emitter->setShooter(shooter.get());

	root->addChild(emitter.get());

	osg::ref_ptr<osgParticle::ModularProgram> program = new osgParticle::ModularProgram();
	program->setParticleSystem(ps.get());

	osg::ref_ptr<osgParticle::AccelOperator> ap = new osgParticle::AccelOperator();
	ap->setToGravity(-0.5f);
	program->addOperator(ap.get());

	osg::ref_ptr<osgParticle::FluidFrictionOperator> ffo = new osgParticle::FluidFrictionOperator();
	ffo->setFluidToAir();
	program->addOperator(ffo.get());

	root->addChild(program.get());

	osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu = new osgParticle::ParticleSystemUpdater();
	psu->addParticleSystem(ps.get());

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(ps.get());

	root->addChild(geode.get());
	root->addChild(psu.get());

	return root.get();
}

osg::ref_ptr<osg::Group> CreateMyParticleScene_5(int num_smokeCoord)
{
	osg::ref_ptr<osg::Group> root = new osg::Group();

	osgParticle::Particle ptemplate;
	ptemplate.setLifeTime(30);
	ptemplate.setSizeRange(osgParticle::rangef(5.0f,10.0f));
	ptemplate.setAlphaRange(osgParticle::rangef(0.0f,1.0f));
	ptemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.4f,0.4f,0.4f,1.0f),osg::Vec4(0.5f,0.5f,0.5f,1.0f)));
	ptemplate.setRadius(1.0f);
	ptemplate.setMass(0.05f);

	osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
	ps->setDefaultAttributes("C://OSG//OpenSceneGraph-Data-3.4.0//Images//Smoke.rgb",false,false);
	ps->setDefaultParticleTemplate(ptemplate);

	osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter();
	emitter->setParticleSystem(ps.get());

	osg::ref_ptr<osgParticle::RandomRateCounter> counter = new osgParticle::RandomRateCounter();
	counter->setRateRange(2.0f,5.0f);
	emitter->setCounter(counter.get());

	osgParticle::MultiSegmentPlacer* lineSegment = new osgParticle::MultiSegmentPlacer();
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+45.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,535.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-45.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,535.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+45.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,535.0f);
	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-45.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,535.0f);
	emitter->setPlacer(lineSegment); 

	osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter();
	shooter->setInitialSpeedRange(20.0f,25.0f);
	//shooter->setPhiRange((osg::PI)/4.0,(osg::PI)/4.0);
	emitter->setShooter(shooter.get());

	root->addChild(emitter.get());

	osg::ref_ptr<osgParticle::ModularProgram> program = new osgParticle::ModularProgram();
	program->setParticleSystem(ps.get());

	osg::ref_ptr<osgParticle::AccelOperator> ap = new osgParticle::AccelOperator();
	ap->setToGravity(-0.5f);
	program->addOperator(ap.get());

	osg::ref_ptr<osgParticle::FluidFrictionOperator> ffo = new osgParticle::FluidFrictionOperator();
	ffo->setFluidToAir();
	program->addOperator(ffo.get());

	root->addChild(program.get());

	osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu = new osgParticle::ParticleSystemUpdater();
	psu->addParticleSystem(ps.get());

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(ps.get());

	root->addChild(geode.get());
	root->addChild(psu.get());

	return root.get();
}

//osg::ref_ptr<osg::Group> CreateMyParticleScene_4(int num_smokeCoord)
//{
//	osg::ref_ptr<osg::Group> root = new osg::Group();
//
//	osgParticle::Particle ptemplate;
//	ptemplate.setLifeTime(30);
//	ptemplate.setSizeRange(osgParticle::rangef(5.0f,10.0f));
//	ptemplate.setAlphaRange(osgParticle::rangef(0.0f,1.0f));
//	ptemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.4f,0.4f,0.4f,1.0f),osg::Vec4(0.5f,0.5f,0.5f,1.0f)));
//	ptemplate.setRadius(2.0f);
//	ptemplate.setMass(0.05f);
//
//	osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
//	ps->setDefaultAttributes("C://OSG//OpenSceneGraph-Data-3.4.0//Images//Smoke.rgb",false,false);
//	ps->setDefaultParticleTemplate(ptemplate);
//
//	osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter();
//	emitter->setParticleSystem(ps.get());
//
//	osg::ref_ptr<osgParticle::RandomRateCounter> counter = new osgParticle::RandomRateCounter();
//	counter->setRateRange(1.0f,2.0f);
//	emitter->setCounter(counter.get());
//
//	//osg::ref_ptr<osgParticle::PointPlacer> placer = new osgParticle::PointPlacer();
//	//placer->setCenter(osg::Vec3(smokeCoordsArray[num_smokeCoord][0]+35.0f,smokeCoordsArray[num_smokeCoord][1],537.0f));
//	//emitter->setPlacer(placer.get());
//	osgParticle::MultiSegmentPlacer* lineSegment = new osgParticle::MultiSegmentPlacer();
//	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+30.0f,smokeCoordsArray[num_smokeCoord][1],537.0f);
//	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+35.0f,smokeCoordsArray[num_smokeCoord][1],537.0f);
//	//lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+25.0f,smokeCoordsArray[num_smokeCoord][1]+26.0f,533.0f);
//	//lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-25.0f,smokeCoordsArray[num_smokeCoord][1]+26.0f,533.0f);
//	//lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0],smokeCoordsArray[num_smokeCoord][1]-26.0f,533.0f);
//	//lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+25.0f,smokeCoordsArray[num_smokeCoord][1]-26.0f,533.0f);
//	//lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-25.0f,smokeCoordsArray[num_smokeCoord][1]-26.0f,533.0f);
//	emitter->setPlacer(lineSegment); 
//
//	osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter();
//	shooter->setInitialSpeedRange(20.0f,25.0f);
//	//shooter->setThetaRange(osg::PI_4,osg::PI_4);
//	//shooter->setPhiRange(osg::PI_4,osg::PI_4);
//	emitter->setShooter(shooter.get());
//
//	root->addChild(emitter.get());
//
//	osg::ref_ptr<osgParticle::ModularProgram> program = new osgParticle::ModularProgram();
//	program->setParticleSystem(ps.get());
//
//	osg::ref_ptr<osgParticle::AccelOperator> ap = new osgParticle::AccelOperator();
//	ap->setToGravity(-0.5f);
//	program->addOperator(ap.get());
//
//	osg::ref_ptr<osgParticle::FluidFrictionOperator> ffo = new osgParticle::FluidFrictionOperator();
//	ffo->setFluidToAir();
//	program->addOperator(ffo.get());
//
//	root->addChild(program.get());
//
//	osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu = new osgParticle::ParticleSystemUpdater();
//	psu->addParticleSystem(ps.get());
//
//	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
//	geode->addDrawable(ps.get());
//
//	root->addChild(geode.get());
//	root->addChild(psu.get());
//
//	return root.get();
//}


////创建自定义粒子系统
//osg::ref_ptr<osg::Group> CreateMyParticleScene(int num_smokeCoord)
//{
//	osg::ref_ptr<osg::Group> root = new osg::Group();
//
//	osgParticle::Particle ptemplate;
//	ptemplate.setLifeTime(4);
//	ptemplate.setSizeRange(osgParticle::rangef(5.0f,10.0f));
//	ptemplate.setAlphaRange(osgParticle::rangef(0.0f,1.0f));
//	ptemplate.setColorRange(osgParticle::rangev4(osg::Vec4(0.3f,0.3f,0.3f,1.0f),osg::Vec4(0.4f,0.4f,0.4f,1.0f)));
//	ptemplate.setRadius(1.0f);
//	ptemplate.setMass(0.05f);
//
//	osg::ref_ptr<osgParticle::ParticleSystem> ps = new osgParticle::ParticleSystem();
//	ps->setDefaultAttributes("C://OSG//OpenSceneGraph-Data-3.4.0//Images//Smoke.rgb",false,false);
//	ps->setDefaultParticleTemplate(ptemplate);
//
//	osg::ref_ptr<osgParticle::ModularEmitter> emitter = new osgParticle::ModularEmitter();
//	emitter->setParticleSystem(ps.get());
//
//	osg::ref_ptr<osgParticle::RandomRateCounter> counter = new osgParticle::RandomRateCounter();
//	counter->setRateRange(30.0f,40.0f);
//	emitter->setCounter(counter.get());
//
//	osgParticle::MultiSegmentPlacer* lineSegment = new osgParticle::MultiSegmentPlacer();
//	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0],smokeCoordsArray[num_smokeCoord][1]+13.0f,525.0f);
//	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+10.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,525.0f);
//	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+20.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,525.0f);
//	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-10.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,525.0f);
//	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-20.0f,smokeCoordsArray[num_smokeCoord][1]+13.0f,525.0f);
//	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0],smokeCoordsArray[num_smokeCoord][1]-13.0f,525.0f);
//	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+10.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,525.0f);
//	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]+20.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,525.0f);
//	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-10.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,525.0f);
//	lineSegment->addVertex(smokeCoordsArray[num_smokeCoord][0]-20.0f,smokeCoordsArray[num_smokeCoord][1]-13.0f,525.0f);
//	emitter->setPlacer(lineSegment); 
//
//	osg::ref_ptr<osgParticle::RadialShooter> shooter = new osgParticle::RadialShooter();
//	shooter->setInitialSpeedRange(0.1,0);
//	emitter->setShooter(shooter.get());
//
//	root->addChild(emitter.get());
//
//	osg::ref_ptr<osgParticle::ModularProgram> program = new osgParticle::ModularProgram();
//	program->setParticleSystem(ps.get());
//
//	osg::ref_ptr<osgParticle::AccelOperator> ap = new osgParticle::AccelOperator();
//	ap->setToGravity(0.01f);
//	program->addOperator(ap.get());
//
//	osg::ref_ptr<osgParticle::FluidFrictionOperator> ffo = new osgParticle::FluidFrictionOperator();
//	ffo->setFluidToAir();
//	program->addOperator(ffo.get());
//
//	root->addChild(program.get());
//
//	osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu = new osgParticle::ParticleSystemUpdater();
//	psu->addParticleSystem(ps.get());
//
//	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
//	geode->addDrawable(ps.get());
//
//	root->addChild(geode.get());
//	root->addChild(psu.get());
//
//	return root.get();
//}

//创建节点回调
class SpaceShipCallback:public osg::NodeCallback
{
public:
	int frame;
	int num_smoke;
	int num_smokeLoc;
	int nowTimePoint;
	double angle;	//旋转角度

public:
	SpaceShipCallback(int num_smokeTime, int num_smokeLocation):angle(0)
	{
		frame = 0;
		nowTimePoint = 0;
		num_smoke = num_smokeTime;
		num_smokeLoc = num_smokeLocation;
	}

	virtual void operator()(osg::Node* node,osg::NodeVisitor* nv)
	{
		if(nowTimePoint < (idleFrame+damageState[num_smoke]-smokeTime[num_smoke][0]-smokeStartTime[num_smokeLoc]) || nowTimePoint > (idleFrame+damageState[num_smoke]+smokeTime[num_smoke][1]+4000+smokeEndTime[num_smokeLoc]))	//if(nowTimePoint < (idleFrame+damageState[num_smoke]-smokeTime[num_smoke][0]) || nowTimePoint > (idleFrame+damageState[num_smoke]+smokeTime[num_smoke][1]+smokeEndTime[num_smokeLoc]))		//if(nowTimePoint < 2115 || nowTimePoint > 2126)
		{
			osg::ref_ptr<osg::Switch> sw = dynamic_cast<osg::Switch*>(node);
			sw->setAllChildrenOff();
			//继续传递参数,执行其他设置回调的节点
			traverse(node,nv);
		}
		else{
			osg::ref_ptr<osg::Switch> swon = dynamic_cast<osg::Switch*>(node);
			swon->setAllChildrenOn();
			//继续传递参数,执行其他设置回调的节点
			traverse(node,nv);
		}
		frame++;
		nowTimePoint++;
	}
};

//纹理共享
class TextureVisitor : public osg::NodeVisitor{
public:
//构造函数，遍历所有子节点
TextureVisitor( ):
    osg::NodeVisitor( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN )   { }

    //重载apply方法
    virtual void apply( osg::PagedLOD& pagedLOD ){
        if(pagedLOD.getStateSet())
			apply( pagedLOD.getStateSet() );
       
        //实现继续遍历节点
        traverse(pagedLOD);
    }

    //重载apply方法
    virtual void apply( osg::Node& node ){

        if(node.getStateSet())
                apply( node.getStateSet() );

        //实现继续遍历节点
        traverse(node);
    }

    //重载apply方法
    virtual void apply( osg::Geode& geode ){
        if(geode.getStateSet())
            apply( geode.getStateSet() );

		//cout<<geode.getName()<<endl;

		if(geode.getName()[0]<'0' || geode.getName()[0]>'9')
			return ;

        unsigned int cnt = geode.getNumDrawables();

        for( unsigned int i = 0; i < cnt; i++ )
			apply(geode.getDrawable(i)->getStateSet());
		if(cnt>1)
			traverse(geode);
    }

    //得到贴图列表
	void apply( osg::StateSet* state ){
        osg::Texture2D* tex2D = NULL;
        if(tex2D = dynamic_cast<osg::Texture2D*>(state->getTextureAttribute(0, osg::StateAttribute::TEXTURE))){
			if( tex2D->getImage() ){
                                 
				//对比映射表里的tex2D是否有一致的，有的话，用映射表里的tex2D取代
				std::map<std::string,osg::Texture2D*>::iterator iter=_texList.find(tex2D->getImage()->getFileName());
				if (iter !=_texList.end()){//如果有一致的
					state->removeAttribute(osg::StateAttribute::TEXTURE,0U);
					state->setTextureAttribute(0,iter->second,0U);//共享纹理
					//std::cout<<"share"<<std::endl;  
				}
				else //否则，写入映射表                                
					_texList.insert(std::make_pair(tex2D->getImage()->getFileName(), tex2D)); //写入映射表
			}
		}
	}

	//得到贴图
	std::map<std::string, osg::Image*>& getImages( void ){
		return _imageList;
	}

protected:
	//贴图映射表，用来保存贴图名和贴图
	std::map<std::string, osg::Image*> _imageList;
	std::map<std::string,osg::Texture2D*> _texList;//状态映射表
};

class SetAnimation:public osg::NodeVisitor{
public:
	SetAnimation():NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN){}
	void apply(osg::Geode& geode){
		//osg::ComputeBoundsVisitor bnv;
		//geode.accept(bnv);
		//osg::BoundingBox bbox = bnv.getBoundingBox();
		for (int i=0;i<geode.getNumDrawables();i++){
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));	
			//cout<<"geo->getName(): "<<geom->getName()<<endl;
			geom->setUseVertexBufferObjects(useVBO);
			geom->setUpdateCallback(new TryUpdate());			
		}
		NodeVisitor::apply(geode);
	}
};

int main(){
	//格式化输出
	cout<<setprecision(3)<<fixed;

	//读取全局参数：时间步数,位移放大倍数,空闲帧数量
	fstream readGlobalParameter(smokeDataPath+"GlobalParameter.txt");
	readGlobalParameter>>nTimePoint>>globalDispScale>>idleFrame;
	readGlobalParameter.close();
	cout<<nTimePoint<<"	"<<globalDispScale<<"	"<<idleFrame<<endl;

	TextureVisitor textureTV;		//纹理映射
	SetAnimation ani;				//动画管理

	ReadResult();			//加载时程结果

	if(setDamage)	ReadDamageState();		//加载建筑物损失状态

	LoadModels(modelPath, "modelList.txt", models, modelNodeList);
	LoadModels(envPath, "envModelList.txt", envs, envNodeList);

	if(!isFrameSkip)
		frameSkip = 1;

	//创建Viewer对象，场景浏览器
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
	//物体组，分为sceneRoot（全部模型）、root（建筑模型）和envRoot（环境模型）、smokeRoot（烟气模型）
	osg::ref_ptr<osg::Group> sceneRoot = new osg::Group();
	osg::ref_ptr<osg::Group> root = new osg::Group();
	osg::ref_ptr<osg::Group> envRoot = new osg::Group();

	//处理坐标转换（仅适用于3ds Max处理过的模型fbx文件）
	osg::MatrixTransform* rotateMT= new osg::MatrixTransform;
	rotateMT->setMatrix(osg::Matrix::rotate(osg::inDegrees(90.0f),1.0f,0.0f,0.0f));
	//为所有模型添加光照。
	for(int i=0;i<models.size();i++){
		//rotateMT->addChild((modelNodeList[i]));
		rotateMT->addChild(createLightModel(modelNodeList[i]));
	}
	root->addChild(rotateMT);
	root->accept(textureTV);

	//set animation
	root->accept(ani);

	osg::MatrixTransform* rotateMTenv= new osg::MatrixTransform;
	rotateMTenv->setMatrix(osg::Matrix::rotate(osg::inDegrees(90.0f),1.0f,0.0f,0.0f));
	for(int i=0;i<envs.size();i++){
		//envRoot->addChild(envNodeList[i]);
		rotateMTenv->addChild((envNodeList[i]));
		//rotateMTenv->addChild(createLightModel(envNodeList[i]));
	}
	envRoot->addChild(rotateMTenv);

	sceneRoot->addChild(root);
	sceneRoot->addChild(envRoot);

	//添加烟尘效果
	//osg::ref_ptr<osg::Switch> sw0 = new osg::Switch();
	//sw0->addChild(CreateMyParticleScene(0));
	//sw0->setUpdateCallback(new SpaceShipCallback(0));
	//sceneRoot->addChild(sw0.get());
	//
	//osg::ref_ptr<osg::Switch> sw1 = new osg::Switch();
	//sw1->addChild(CreateMyParticleScene(1));
	//sw1->setUpdateCallback(new SpaceShipCallback(1));
	//sceneRoot->addChild(sw1.get());
	//
	//osg::ref_ptr<osg::Switch> sw2 = new osg::Switch();
	//sw2->addChild(CreateMyParticleScene(2));
	//sw2->setUpdateCallback(new SpaceShipCallback(2));
	//sceneRoot->addChild(sw2.get());
	//
	//osg::ref_ptr<osg::Switch> sw3 = new osg::Switch();
	//sw3->addChild(CreateMyParticleScene(3));
	//sw3->setUpdateCallback(new SpaceShipCallback(3));
	//sceneRoot->addChild(sw3.get());
	//
	//osg::ref_ptr<osg::Switch> sw4 = new osg::Switch();
	//sw4->addChild(CreateMyParticleScene(4));
	//sw4->setUpdateCallback(new SpaceShipCallback(4));
	//sceneRoot->addChild(sw4.get());
	//
	//osg::ref_ptr<osg::Switch> sw5 = new osg::Switch();
	//sw5->addChild(CreateMyParticleScene(5));
	//sw5->setUpdateCallback(new SpaceShipCallback(5));
	//sceneRoot->addChild(sw5.get());

	if((damageState[318]<nTimePoint) && (inModelList[318] ==1)){
		osg::ref_ptr<osg::Switch> sw318_0 = new osg::Switch();
		sw318_0->addChild(CreateMyParticleScene_0(318));
		sw318_0->setUpdateCallback(new SpaceShipCallback(318,0));
		sceneRoot->addChild(sw318_0.get());
		osg::ref_ptr<osg::Switch> sw318_1 = new osg::Switch();
		sw318_1->addChild(CreateMyParticleScene_1(318));
		sw318_1->setUpdateCallback(new SpaceShipCallback(318,1));
		sceneRoot->addChild(sw318_1.get());
		osg::ref_ptr<osg::Switch> sw318_2 = new osg::Switch();
		sw318_2->addChild(CreateMyParticleScene_2(318));
		sw318_2->setUpdateCallback(new SpaceShipCallback(318,2));
		sceneRoot->addChild(sw318_2.get());
		osg::ref_ptr<osg::Switch> sw318_3 = new osg::Switch();
		sw318_3->addChild(CreateMyParticleScene_3(318));
		sw318_3->setUpdateCallback(new SpaceShipCallback(318,3));
		sceneRoot->addChild(sw318_3.get());
		osg::ref_ptr<osg::Switch> sw318_4 = new osg::Switch();
		sw318_4->addChild(CreateMyParticleScene_4(318));
		sw318_4->setUpdateCallback(new SpaceShipCallback(318,4));
		sceneRoot->addChild(sw318_4.get());
		osg::ref_ptr<osg::Switch> sw318_5 = new osg::Switch();
		sw318_5->addChild(CreateMyParticleScene_5(318));
		sw318_5->setUpdateCallback(new SpaceShipCallback(318,5));
		sceneRoot->addChild(sw318_5.get());
	}

	if((damageState[331]<nTimePoint) && (inModelList[331] ==1)){
		osg::ref_ptr<osg::Switch> sw331_0 = new osg::Switch();
		sw331_0->addChild(CreateMyParticleScene_0(331));
		sw331_0->setUpdateCallback(new SpaceShipCallback(331,0));
		sceneRoot->addChild(sw331_0.get());
		osg::ref_ptr<osg::Switch> sw331_1 = new osg::Switch();
		sw331_1->addChild(CreateMyParticleScene_1(331));
		sw331_1->setUpdateCallback(new SpaceShipCallback(331,1));
		sceneRoot->addChild(sw331_1.get());
		osg::ref_ptr<osg::Switch> sw331_2 = new osg::Switch();
		sw331_2->addChild(CreateMyParticleScene_2(331));
		sw331_2->setUpdateCallback(new SpaceShipCallback(331,2));
		sceneRoot->addChild(sw331_2.get());
		osg::ref_ptr<osg::Switch> sw331_3 = new osg::Switch();
		sw331_3->addChild(CreateMyParticleScene_3(331));
		sw331_3->setUpdateCallback(new SpaceShipCallback(331,3));
		sceneRoot->addChild(sw331_3.get());
		osg::ref_ptr<osg::Switch> sw331_4 = new osg::Switch();
		sw331_4->addChild(CreateMyParticleScene_4(331));
		sw331_4->setUpdateCallback(new SpaceShipCallback(331,4));
		sceneRoot->addChild(sw331_4.get());
		osg::ref_ptr<osg::Switch> sw331_5 = new osg::Switch();
		sw331_5->addChild(CreateMyParticleScene_5(331));
		sw331_5->setUpdateCallback(new SpaceShipCallback(331,5));
		sceneRoot->addChild(sw331_5.get());
	}

	if((damageState[339]<nTimePoint) && (inModelList[339] ==1)){
		osg::ref_ptr<osg::Switch> sw339_0 = new osg::Switch();
		sw339_0->addChild(CreateMyParticleScene_0(339));
		sw339_0->setUpdateCallback(new SpaceShipCallback(339,0));
		sceneRoot->addChild(sw339_0.get());
		osg::ref_ptr<osg::Switch> sw339_1 = new osg::Switch();
		sw339_1->addChild(CreateMyParticleScene_1(339));
		sw339_1->setUpdateCallback(new SpaceShipCallback(339,1));
		sceneRoot->addChild(sw339_1.get());
		osg::ref_ptr<osg::Switch> sw339_2 = new osg::Switch();
		sw339_2->addChild(CreateMyParticleScene_2(339));
		sw339_2->setUpdateCallback(new SpaceShipCallback(339,2));
		sceneRoot->addChild(sw339_2.get());
		osg::ref_ptr<osg::Switch> sw339_3 = new osg::Switch();
		sw339_3->addChild(CreateMyParticleScene_3(339));
		sw339_3->setUpdateCallback(new SpaceShipCallback(339,3));
		sceneRoot->addChild(sw339_3.get());
		osg::ref_ptr<osg::Switch> sw339_4 = new osg::Switch();
		sw339_4->addChild(CreateMyParticleScene_4(339));
		sw339_4->setUpdateCallback(new SpaceShipCallback(339,4));
		sceneRoot->addChild(sw339_4.get());
		osg::ref_ptr<osg::Switch> sw339_5 = new osg::Switch();
		sw339_5->addChild(CreateMyParticleScene_5(339));
		sw339_5->setUpdateCallback(new SpaceShipCallback(339,5));
		sceneRoot->addChild(sw339_5.get());
	}

	if((damageState[359]<nTimePoint) && (inModelList[359] ==1)){
		osg::ref_ptr<osg::Switch> sw359_0 = new osg::Switch();
		sw359_0->addChild(CreateMyParticleScene_0(359));
		sw359_0->setUpdateCallback(new SpaceShipCallback(359,0));
		sceneRoot->addChild(sw359_0.get());
		osg::ref_ptr<osg::Switch> sw359_1 = new osg::Switch();
		sw359_1->addChild(CreateMyParticleScene_1(359));
		sw359_1->setUpdateCallback(new SpaceShipCallback(359,1));
		sceneRoot->addChild(sw359_1.get());
		osg::ref_ptr<osg::Switch> sw359_2 = new osg::Switch();
		sw359_2->addChild(CreateMyParticleScene_2(359));
		sw359_2->setUpdateCallback(new SpaceShipCallback(359,2));
		sceneRoot->addChild(sw359_2.get());
		osg::ref_ptr<osg::Switch> sw359_3 = new osg::Switch();
		sw359_3->addChild(CreateMyParticleScene_3(359));
		sw359_3->setUpdateCallback(new SpaceShipCallback(359,3));
		sceneRoot->addChild(sw359_3.get());
		osg::ref_ptr<osg::Switch> sw359_4 = new osg::Switch();
		sw359_4->addChild(CreateMyParticleScene_4(359));
		sw359_4->setUpdateCallback(new SpaceShipCallback(359,4));
		sceneRoot->addChild(sw359_4.get());
		osg::ref_ptr<osg::Switch> sw359_5 = new osg::Switch();
		sw359_5->addChild(CreateMyParticleScene_5(359));
		sw359_5->setUpdateCallback(new SpaceShipCallback(359,5));
		sceneRoot->addChild(sw359_5.get());
	}

	if((damageState[362]<nTimePoint) && (inModelList[362] ==1)){
		osg::ref_ptr<osg::Switch> sw362_0 = new osg::Switch();
		sw362_0->addChild(CreateMyParticleScene_0(362));
		sw362_0->setUpdateCallback(new SpaceShipCallback(362,0));
		sceneRoot->addChild(sw362_0.get());
		osg::ref_ptr<osg::Switch> sw362_1 = new osg::Switch();
		sw362_1->addChild(CreateMyParticleScene_1(362));
		sw362_1->setUpdateCallback(new SpaceShipCallback(362,1));
		sceneRoot->addChild(sw362_1.get());
		osg::ref_ptr<osg::Switch> sw362_2 = new osg::Switch();
		sw362_2->addChild(CreateMyParticleScene_2(362));
		sw362_2->setUpdateCallback(new SpaceShipCallback(362,2));
		sceneRoot->addChild(sw362_2.get());
		osg::ref_ptr<osg::Switch> sw362_3 = new osg::Switch();
		sw362_3->addChild(CreateMyParticleScene_3(362));
		sw362_3->setUpdateCallback(new SpaceShipCallback(362,3));
		sceneRoot->addChild(sw362_3.get());
		osg::ref_ptr<osg::Switch> sw362_4 = new osg::Switch();
		sw362_4->addChild(CreateMyParticleScene_4(362));
		sw362_4->setUpdateCallback(new SpaceShipCallback(362,4));
		sceneRoot->addChild(sw362_4.get());
		osg::ref_ptr<osg::Switch> sw362_5 = new osg::Switch();
		sw362_5->addChild(CreateMyParticleScene_5(362));
		sw362_5->setUpdateCallback(new SpaceShipCallback(362,5));
		sceneRoot->addChild(sw362_5.get());
	}
	
	if((damageState[377]<nTimePoint) && (inModelList[377] ==1)){
		osg::ref_ptr<osg::Switch> sw377_0 = new osg::Switch();
		sw377_0->addChild(CreateMyParticleScene_0(377));
		sw377_0->setUpdateCallback(new SpaceShipCallback(377,0));
		sceneRoot->addChild(sw377_0.get());
		osg::ref_ptr<osg::Switch> sw377_1 = new osg::Switch();
		sw377_1->addChild(CreateMyParticleScene_1(377));
		sw377_1->setUpdateCallback(new SpaceShipCallback(377,1));
		sceneRoot->addChild(sw377_1.get());
		osg::ref_ptr<osg::Switch> sw377_2 = new osg::Switch();
		sw377_2->addChild(CreateMyParticleScene_2(377));
		sw377_2->setUpdateCallback(new SpaceShipCallback(377,2));
		sceneRoot->addChild(sw377_2.get());
		osg::ref_ptr<osg::Switch> sw377_3 = new osg::Switch();
		sw377_3->addChild(CreateMyParticleScene_3(377));
		sw377_3->setUpdateCallback(new SpaceShipCallback(377,3));
		sceneRoot->addChild(sw377_3.get());
		osg::ref_ptr<osg::Switch> sw377_4 = new osg::Switch();
		sw377_4->addChild(CreateMyParticleScene_4(377));
		sw377_4->setUpdateCallback(new SpaceShipCallback(377,4));
		sceneRoot->addChild(sw377_4.get());
		osg::ref_ptr<osg::Switch> sw377_5 = new osg::Switch();
		sw377_5->addChild(CreateMyParticleScene_5(377));
		sw377_5->setUpdateCallback(new SpaceShipCallback(377,5));
		sceneRoot->addChild(sw377_5.get());
	}

	if((damageState[390]<nTimePoint) && (inModelList[390] ==1)){
		osg::ref_ptr<osg::Switch> sw390_0 = new osg::Switch();
		sw390_0->addChild(CreateMyParticleScene_0(390));
		sw390_0->setUpdateCallback(new SpaceShipCallback(390,0));
		sceneRoot->addChild(sw390_0.get());
		osg::ref_ptr<osg::Switch> sw390_1 = new osg::Switch();
		sw390_1->addChild(CreateMyParticleScene_1(390));
		sw390_1->setUpdateCallback(new SpaceShipCallback(390,1));
		sceneRoot->addChild(sw390_1.get());
		osg::ref_ptr<osg::Switch> sw390_2 = new osg::Switch();
		sw390_2->addChild(CreateMyParticleScene_2(390));
		sw390_2->setUpdateCallback(new SpaceShipCallback(390,2));
		sceneRoot->addChild(sw390_2.get());
		osg::ref_ptr<osg::Switch> sw390_3 = new osg::Switch();
		sw390_3->addChild(CreateMyParticleScene_3(390));
		sw390_3->setUpdateCallback(new SpaceShipCallback(390,3));
		sceneRoot->addChild(sw390_3.get());
		osg::ref_ptr<osg::Switch> sw390_4 = new osg::Switch();
		sw390_4->addChild(CreateMyParticleScene_4(390));
		sw390_4->setUpdateCallback(new SpaceShipCallback(390,4));
		sceneRoot->addChild(sw390_4.get());
		osg::ref_ptr<osg::Switch> sw390_5 = new osg::Switch();
		sw390_5->addChild(CreateMyParticleScene_5(390));
		sw390_5->setUpdateCallback(new SpaceShipCallback(390,5));
		sceneRoot->addChild(sw390_5.get());
	}

	if((damageState[402]<nTimePoint) && (inModelList[402] ==1)){
		osg::ref_ptr<osg::Switch> sw402_0 = new osg::Switch();
		sw402_0->addChild(CreateMyParticleScene_0(402));
		sw402_0->setUpdateCallback(new SpaceShipCallback(402,0));
		sceneRoot->addChild(sw402_0.get());
		osg::ref_ptr<osg::Switch> sw402_1 = new osg::Switch();
		sw402_1->addChild(CreateMyParticleScene_1(402));
		sw402_1->setUpdateCallback(new SpaceShipCallback(402,1));
		sceneRoot->addChild(sw402_1.get());
		osg::ref_ptr<osg::Switch> sw402_2 = new osg::Switch();
		sw402_2->addChild(CreateMyParticleScene_2(402));
		sw402_2->setUpdateCallback(new SpaceShipCallback(402,2));
		sceneRoot->addChild(sw402_2.get());
		osg::ref_ptr<osg::Switch> sw402_3 = new osg::Switch();
		sw402_3->addChild(CreateMyParticleScene_3(402));
		sw402_3->setUpdateCallback(new SpaceShipCallback(402,3));
		sceneRoot->addChild(sw402_3.get());
		osg::ref_ptr<osg::Switch> sw402_4 = new osg::Switch();
		sw402_4->addChild(CreateMyParticleScene_4(402));
		sw402_4->setUpdateCallback(new SpaceShipCallback(402,4));
		sceneRoot->addChild(sw402_4.get());
		osg::ref_ptr<osg::Switch> sw402_5 = new osg::Switch();
		sw402_5->addChild(CreateMyParticleScene_5(402));
		sw402_5->setUpdateCallback(new SpaceShipCallback(402,5));
		sceneRoot->addChild(sw402_5.get());
	}

	if((damageState[411]<nTimePoint) && (inModelList[411] ==1)){
		osg::ref_ptr<osg::Switch> sw411_0 = new osg::Switch();
		sw411_0->addChild(CreateMyParticleScene_0(411));
		sw411_0->setUpdateCallback(new SpaceShipCallback(411,0));
		sceneRoot->addChild(sw411_0.get());
		osg::ref_ptr<osg::Switch> sw411_1 = new osg::Switch();
		sw411_1->addChild(CreateMyParticleScene_1(411));
		sw411_1->setUpdateCallback(new SpaceShipCallback(411,1));
		sceneRoot->addChild(sw411_1.get());
		osg::ref_ptr<osg::Switch> sw411_2 = new osg::Switch();
		sw411_2->addChild(CreateMyParticleScene_2(411));
		sw411_2->setUpdateCallback(new SpaceShipCallback(411,2));
		sceneRoot->addChild(sw411_2.get());
		osg::ref_ptr<osg::Switch> sw411_3 = new osg::Switch();
		sw411_3->addChild(CreateMyParticleScene_3(411));
		sw411_3->setUpdateCallback(new SpaceShipCallback(411,3));
		sceneRoot->addChild(sw411_3.get());
		osg::ref_ptr<osg::Switch> sw411_4 = new osg::Switch();
		sw411_4->addChild(CreateMyParticleScene_4(411));
		sw411_4->setUpdateCallback(new SpaceShipCallback(411,4));
		sceneRoot->addChild(sw411_4.get());
		osg::ref_ptr<osg::Switch> sw411_5 = new osg::Switch();
		sw411_5->addChild(CreateMyParticleScene_5(411));
		sw411_5->setUpdateCallback(new SpaceShipCallback(411,5));
		sceneRoot->addChild(sw411_5.get());
	}

	if((damageState[416]<nTimePoint) && (inModelList[416] ==1)){
		osg::ref_ptr<osg::Switch> sw416_0 = new osg::Switch();
		sw416_0->addChild(CreateMyParticleScene_0(416));
		sw416_0->setUpdateCallback(new SpaceShipCallback(416,0));
		sceneRoot->addChild(sw416_0.get());
		osg::ref_ptr<osg::Switch> sw416_1 = new osg::Switch();
		sw416_1->addChild(CreateMyParticleScene_1(416));
		sw416_1->setUpdateCallback(new SpaceShipCallback(416,1));
		sceneRoot->addChild(sw416_1.get());
		osg::ref_ptr<osg::Switch> sw416_2 = new osg::Switch();
		sw416_2->addChild(CreateMyParticleScene_2(416));
		sw416_2->setUpdateCallback(new SpaceShipCallback(416,2));
		sceneRoot->addChild(sw416_2.get());
		osg::ref_ptr<osg::Switch> sw416_3 = new osg::Switch();
		sw416_3->addChild(CreateMyParticleScene_3(416));
		sw416_3->setUpdateCallback(new SpaceShipCallback(416,3));
		sceneRoot->addChild(sw416_3.get());
		osg::ref_ptr<osg::Switch> sw416_4 = new osg::Switch();
		sw416_4->addChild(CreateMyParticleScene_4(416));
		sw416_4->setUpdateCallback(new SpaceShipCallback(416,4));
		sceneRoot->addChild(sw416_4.get());
		osg::ref_ptr<osg::Switch> sw416_5 = new osg::Switch();
		sw416_5->addChild(CreateMyParticleScene_5(416));
		sw416_5->setUpdateCallback(new SpaceShipCallback(416,5));
		sceneRoot->addChild(sw416_5.get());
	}

	if((damageState[432]<nTimePoint) && (inModelList[432] ==1)){
		osg::ref_ptr<osg::Switch> sw432_0 = new osg::Switch();
		sw432_0->addChild(CreateMyParticleScene_0(432));
		sw432_0->setUpdateCallback(new SpaceShipCallback(432,0));
		sceneRoot->addChild(sw432_0.get());
		osg::ref_ptr<osg::Switch> sw432_1 = new osg::Switch();
		sw432_1->addChild(CreateMyParticleScene_1(432));
		sw432_1->setUpdateCallback(new SpaceShipCallback(432,1));
		sceneRoot->addChild(sw432_1.get());
		osg::ref_ptr<osg::Switch> sw432_2 = new osg::Switch();
		sw432_2->addChild(CreateMyParticleScene_2(432));
		sw432_2->setUpdateCallback(new SpaceShipCallback(432,2));
		sceneRoot->addChild(sw432_2.get());
		osg::ref_ptr<osg::Switch> sw432_3 = new osg::Switch();
		sw432_3->addChild(CreateMyParticleScene_3(432));
		sw432_3->setUpdateCallback(new SpaceShipCallback(432,3));
		sceneRoot->addChild(sw432_3.get());
		osg::ref_ptr<osg::Switch> sw432_4 = new osg::Switch();
		sw432_4->addChild(CreateMyParticleScene_4(432));
		sw432_4->setUpdateCallback(new SpaceShipCallback(432,4));
		sceneRoot->addChild(sw432_4.get());
		osg::ref_ptr<osg::Switch> sw432_5 = new osg::Switch();
		sw432_5->addChild(CreateMyParticleScene_5(432));
		sw432_5->setUpdateCallback(new SpaceShipCallback(432,5));
		sceneRoot->addChild(sw432_5.get());
	}

	if((damageState[433]<nTimePoint) && (inModelList[433] ==1)){
		osg::ref_ptr<osg::Switch> sw433_0 = new osg::Switch();
		sw433_0->addChild(CreateMyParticleScene_0(433));
		sw433_0->setUpdateCallback(new SpaceShipCallback(433,0));
		sceneRoot->addChild(sw433_0.get());
		osg::ref_ptr<osg::Switch> sw433_1 = new osg::Switch();
		sw433_1->addChild(CreateMyParticleScene_1(433));
		sw433_1->setUpdateCallback(new SpaceShipCallback(433,1));
		sceneRoot->addChild(sw433_1.get());
		osg::ref_ptr<osg::Switch> sw433_2 = new osg::Switch();
		sw433_2->addChild(CreateMyParticleScene_2(433));
		sw433_2->setUpdateCallback(new SpaceShipCallback(433,2));
		sceneRoot->addChild(sw433_2.get());
		osg::ref_ptr<osg::Switch> sw433_3 = new osg::Switch();
		sw433_3->addChild(CreateMyParticleScene_3(433));
		sw433_3->setUpdateCallback(new SpaceShipCallback(433,3));
		sceneRoot->addChild(sw433_3.get());
		osg::ref_ptr<osg::Switch> sw433_4 = new osg::Switch();
		sw433_4->addChild(CreateMyParticleScene_4(433));
		sw433_4->setUpdateCallback(new SpaceShipCallback(433,4));
		sceneRoot->addChild(sw433_4.get());
		osg::ref_ptr<osg::Switch> sw433_5 = new osg::Switch();
		sw433_5->addChild(CreateMyParticleScene_5(433));
		sw433_5->setUpdateCallback(new SpaceShipCallback(433,5));
		sceneRoot->addChild(sw433_5.get());
	}

	if((damageState[436]<nTimePoint) && (inModelList[436] ==1)){
		osg::ref_ptr<osg::Switch> sw436_0 = new osg::Switch();
		sw436_0->addChild(CreateMyParticleScene_0(436));
		sw436_0->setUpdateCallback(new SpaceShipCallback(436,0));
		sceneRoot->addChild(sw436_0.get());
		osg::ref_ptr<osg::Switch> sw436_1 = new osg::Switch();
		sw436_1->addChild(CreateMyParticleScene_1(436));
		sw436_1->setUpdateCallback(new SpaceShipCallback(436,1));
		sceneRoot->addChild(sw436_1.get());
		osg::ref_ptr<osg::Switch> sw436_2 = new osg::Switch();
		sw436_2->addChild(CreateMyParticleScene_2(436));
		sw436_2->setUpdateCallback(new SpaceShipCallback(436,2));
		sceneRoot->addChild(sw436_2.get());
		osg::ref_ptr<osg::Switch> sw436_3 = new osg::Switch();
		sw436_3->addChild(CreateMyParticleScene_3(436));
		sw436_3->setUpdateCallback(new SpaceShipCallback(436,3));
		sceneRoot->addChild(sw436_3.get());
		osg::ref_ptr<osg::Switch> sw436_4 = new osg::Switch();
		sw436_4->addChild(CreateMyParticleScene_4(436));
		sw436_4->setUpdateCallback(new SpaceShipCallback(436,4));
		sceneRoot->addChild(sw436_4.get());
		osg::ref_ptr<osg::Switch> sw436_5 = new osg::Switch();
		sw436_5->addChild(CreateMyParticleScene_5(436));
		sw436_5->setUpdateCallback(new SpaceShipCallback(436,5));
		sceneRoot->addChild(sw436_5.get());
	}

	if((damageState[442]<nTimePoint) && (inModelList[442] ==1)){
		osg::ref_ptr<osg::Switch> sw442_0 = new osg::Switch();
		sw442_0->addChild(CreateMyParticleScene_0(442));
		sw442_0->setUpdateCallback(new SpaceShipCallback(442,0));
		sceneRoot->addChild(sw442_0.get());
		osg::ref_ptr<osg::Switch> sw442_1 = new osg::Switch();
		sw442_1->addChild(CreateMyParticleScene_1(442));
		sw442_1->setUpdateCallback(new SpaceShipCallback(442,1));
		sceneRoot->addChild(sw442_1.get());
		osg::ref_ptr<osg::Switch> sw442_2 = new osg::Switch();
		sw442_2->addChild(CreateMyParticleScene_2(442));
		sw442_2->setUpdateCallback(new SpaceShipCallback(442,2));
		sceneRoot->addChild(sw442_2.get());
		osg::ref_ptr<osg::Switch> sw442_3 = new osg::Switch();
		sw442_3->addChild(CreateMyParticleScene_3(442));
		sw442_3->setUpdateCallback(new SpaceShipCallback(442,3));
		sceneRoot->addChild(sw442_3.get());
		osg::ref_ptr<osg::Switch> sw442_4 = new osg::Switch();
		sw442_4->addChild(CreateMyParticleScene_4(442));
		sw442_4->setUpdateCallback(new SpaceShipCallback(442,4));
		sceneRoot->addChild(sw442_4.get());
		osg::ref_ptr<osg::Switch> sw442_5 = new osg::Switch();
		sw442_5->addChild(CreateMyParticleScene_5(442));
		sw442_5->setUpdateCallback(new SpaceShipCallback(442,5));
		sceneRoot->addChild(sw442_5.get());
	}

	if((damageState[443]<nTimePoint) && (inModelList[443] ==1)){
		osg::ref_ptr<osg::Switch> sw443_0 = new osg::Switch();
		sw443_0->addChild(CreateMyParticleScene_0(443));
		sw443_0->setUpdateCallback(new SpaceShipCallback(443,0));
		sceneRoot->addChild(sw443_0.get());
		osg::ref_ptr<osg::Switch> sw443_1 = new osg::Switch();
		sw443_1->addChild(CreateMyParticleScene_1(443));
		sw443_1->setUpdateCallback(new SpaceShipCallback(443,1));
		sceneRoot->addChild(sw443_1.get());
		osg::ref_ptr<osg::Switch> sw443_2 = new osg::Switch();
		sw443_2->addChild(CreateMyParticleScene_2(443));
		sw443_2->setUpdateCallback(new SpaceShipCallback(443,2));
		sceneRoot->addChild(sw443_2.get());
		osg::ref_ptr<osg::Switch> sw443_3 = new osg::Switch();
		sw443_3->addChild(CreateMyParticleScene_3(443));
		sw443_3->setUpdateCallback(new SpaceShipCallback(443,3));
		sceneRoot->addChild(sw443_3.get());
		osg::ref_ptr<osg::Switch> sw443_4 = new osg::Switch();
		sw443_4->addChild(CreateMyParticleScene_4(443));
		sw443_4->setUpdateCallback(new SpaceShipCallback(443,4));
		sceneRoot->addChild(sw443_4.get());
		osg::ref_ptr<osg::Switch> sw443_5 = new osg::Switch();
		sw443_5->addChild(CreateMyParticleScene_5(443));
		sw443_5->setUpdateCallback(new SpaceShipCallback(443,5));
		sceneRoot->addChild(sw443_5.get());
	}

	if((damageState[459]<nTimePoint) && (inModelList[459] ==1)){
		osg::ref_ptr<osg::Switch> sw459_0 = new osg::Switch();
		sw459_0->addChild(CreateMyParticleScene_0(459));
		sw459_0->setUpdateCallback(new SpaceShipCallback(459,0));
		sceneRoot->addChild(sw459_0.get());
		osg::ref_ptr<osg::Switch> sw459_1 = new osg::Switch();
		sw459_1->addChild(CreateMyParticleScene_1(459));
		sw459_1->setUpdateCallback(new SpaceShipCallback(459,1));
		sceneRoot->addChild(sw459_1.get());
		osg::ref_ptr<osg::Switch> sw459_2 = new osg::Switch();
		sw459_2->addChild(CreateMyParticleScene_2(459));
		sw459_2->setUpdateCallback(new SpaceShipCallback(459,2));
		sceneRoot->addChild(sw459_2.get());
		osg::ref_ptr<osg::Switch> sw459_3 = new osg::Switch();
		sw459_3->addChild(CreateMyParticleScene_3(459));
		sw459_3->setUpdateCallback(new SpaceShipCallback(459,3));
		sceneRoot->addChild(sw459_3.get());
		osg::ref_ptr<osg::Switch> sw459_4 = new osg::Switch();
		sw459_4->addChild(CreateMyParticleScene_4(459));
		sw459_4->setUpdateCallback(new SpaceShipCallback(459,4));
		sceneRoot->addChild(sw459_4.get());
		osg::ref_ptr<osg::Switch> sw459_5 = new osg::Switch();
		sw459_5->addChild(CreateMyParticleScene_5(459));
		sw459_5->setUpdateCallback(new SpaceShipCallback(459,5));
		sceneRoot->addChild(sw459_5.get());
	}

	if((damageState[460]<nTimePoint) && (inModelList[460] ==1)){
		osg::ref_ptr<osg::Switch> sw460_0 = new osg::Switch();
		sw460_0->addChild(CreateMyParticleScene_0(460));
		sw460_0->setUpdateCallback(new SpaceShipCallback(460,0));
		sceneRoot->addChild(sw460_0.get());
		osg::ref_ptr<osg::Switch> sw460_1 = new osg::Switch();
		sw460_1->addChild(CreateMyParticleScene_1(460));
		sw460_1->setUpdateCallback(new SpaceShipCallback(460,1));
		sceneRoot->addChild(sw460_1.get());
		osg::ref_ptr<osg::Switch> sw460_2 = new osg::Switch();
		sw460_2->addChild(CreateMyParticleScene_2(460));
		sw460_2->setUpdateCallback(new SpaceShipCallback(460,2));
		sceneRoot->addChild(sw460_2.get());
		osg::ref_ptr<osg::Switch> sw460_3 = new osg::Switch();
		sw460_3->addChild(CreateMyParticleScene_3(460));
		sw460_3->setUpdateCallback(new SpaceShipCallback(460,3));
		sceneRoot->addChild(sw460_3.get());
		osg::ref_ptr<osg::Switch> sw460_4 = new osg::Switch();
		sw460_4->addChild(CreateMyParticleScene_4(460));
		sw460_4->setUpdateCallback(new SpaceShipCallback(460,4));
		sceneRoot->addChild(sw460_4.get());
		osg::ref_ptr<osg::Switch> sw460_5 = new osg::Switch();
		sw460_5->addChild(CreateMyParticleScene_5(460));
		sw460_5->setUpdateCallback(new SpaceShipCallback(460,5));
		sceneRoot->addChild(sw460_5.get());
	}

	if((damageState[545]<nTimePoint) && (inModelList[545] ==1)){
		osg::ref_ptr<osg::Switch> sw545_0 = new osg::Switch();
		sw545_0->addChild(CreateMyParticleScene_0(545));
		sw545_0->setUpdateCallback(new SpaceShipCallback(545,0));
		sceneRoot->addChild(sw545_0.get());
		osg::ref_ptr<osg::Switch> sw545_1 = new osg::Switch();
		sw545_1->addChild(CreateMyParticleScene_1(545));
		sw545_1->setUpdateCallback(new SpaceShipCallback(545,1));
		sceneRoot->addChild(sw545_1.get());
		osg::ref_ptr<osg::Switch> sw545_2 = new osg::Switch();
		sw545_2->addChild(CreateMyParticleScene_2(545));
		sw545_2->setUpdateCallback(new SpaceShipCallback(545,2));
		sceneRoot->addChild(sw545_2.get());
		osg::ref_ptr<osg::Switch> sw545_3 = new osg::Switch();
		sw545_3->addChild(CreateMyParticleScene_3(545));
		sw545_3->setUpdateCallback(new SpaceShipCallback(545,3));
		sceneRoot->addChild(sw545_3.get());
		osg::ref_ptr<osg::Switch> sw545_4 = new osg::Switch();
		sw545_4->addChild(CreateMyParticleScene_4(545));
		sw545_4->setUpdateCallback(new SpaceShipCallback(545,4));
		sceneRoot->addChild(sw545_4.get());
		osg::ref_ptr<osg::Switch> sw545_5 = new osg::Switch();
		sw545_5->addChild(CreateMyParticleScene_5(545));
		sw545_5->setUpdateCallback(new SpaceShipCallback(545,5));
		sceneRoot->addChild(sw545_5.get());
	}

	if((damageState[560]<nTimePoint) && (inModelList[560] ==1)){
		osg::ref_ptr<osg::Switch> sw560_0 = new osg::Switch();
		sw560_0->addChild(CreateMyParticleScene_0(560));
		sw560_0->setUpdateCallback(new SpaceShipCallback(560,0));
		sceneRoot->addChild(sw560_0.get());
		osg::ref_ptr<osg::Switch> sw560_1 = new osg::Switch();
		sw560_1->addChild(CreateMyParticleScene_1(560));
		sw560_1->setUpdateCallback(new SpaceShipCallback(560,1));
		sceneRoot->addChild(sw560_1.get());
		osg::ref_ptr<osg::Switch> sw560_2 = new osg::Switch();
		sw560_2->addChild(CreateMyParticleScene_2(560));
		sw560_2->setUpdateCallback(new SpaceShipCallback(560,2));
		sceneRoot->addChild(sw560_2.get());
		osg::ref_ptr<osg::Switch> sw560_3 = new osg::Switch();
		sw560_3->addChild(CreateMyParticleScene_3(560));
		sw560_3->setUpdateCallback(new SpaceShipCallback(560,3));
		sceneRoot->addChild(sw560_3.get());
		osg::ref_ptr<osg::Switch> sw560_4 = new osg::Switch();
		sw560_4->addChild(CreateMyParticleScene_4(560));
		sw560_4->setUpdateCallback(new SpaceShipCallback(560,4));
		sceneRoot->addChild(sw560_4.get());
		osg::ref_ptr<osg::Switch> sw560_5 = new osg::Switch();
		sw560_5->addChild(CreateMyParticleScene_5(560));
		sw560_5->setUpdateCallback(new SpaceShipCallback(560,5));
		sceneRoot->addChild(sw560_5.get());
	}

	if((damageState[583]<nTimePoint) && (inModelList[583] ==1)){
		osg::ref_ptr<osg::Switch> sw583_0 = new osg::Switch();
		sw583_0->addChild(CreateMyParticleScene_0(583));
		sw583_0->setUpdateCallback(new SpaceShipCallback(583,0));
		sceneRoot->addChild(sw583_0.get());
		osg::ref_ptr<osg::Switch> sw583_1 = new osg::Switch();
		sw583_1->addChild(CreateMyParticleScene_1(583));
		sw583_1->setUpdateCallback(new SpaceShipCallback(583,1));
		sceneRoot->addChild(sw583_1.get());
		osg::ref_ptr<osg::Switch> sw583_2 = new osg::Switch();
		sw583_2->addChild(CreateMyParticleScene_2(583));
		sw583_2->setUpdateCallback(new SpaceShipCallback(583,2));
		sceneRoot->addChild(sw583_2.get());
		osg::ref_ptr<osg::Switch> sw583_3 = new osg::Switch();
		sw583_3->addChild(CreateMyParticleScene_3(583));
		sw583_3->setUpdateCallback(new SpaceShipCallback(583,3));
		sceneRoot->addChild(sw583_3.get());
		osg::ref_ptr<osg::Switch> sw583_4 = new osg::Switch();
		sw583_4->addChild(CreateMyParticleScene_4(583));
		sw583_4->setUpdateCallback(new SpaceShipCallback(583,4));
		sceneRoot->addChild(sw583_4.get());
		osg::ref_ptr<osg::Switch> sw583_5 = new osg::Switch();
		sw583_5->addChild(CreateMyParticleScene_5(583));
		sw583_5->setUpdateCallback(new SpaceShipCallback(583,5));
		sceneRoot->addChild(sw583_5.get());
	}

	if((damageState[588]<nTimePoint) && (inModelList[588] ==1)){
		osg::ref_ptr<osg::Switch> sw588_0 = new osg::Switch();
		sw588_0->addChild(CreateMyParticleScene_0(588));
		sw588_0->setUpdateCallback(new SpaceShipCallback(588,0));
		sceneRoot->addChild(sw588_0.get());
		osg::ref_ptr<osg::Switch> sw588_1 = new osg::Switch();
		sw588_1->addChild(CreateMyParticleScene_1(588));
		sw588_1->setUpdateCallback(new SpaceShipCallback(588,1));
		sceneRoot->addChild(sw588_1.get());
		osg::ref_ptr<osg::Switch> sw588_2 = new osg::Switch();
		sw588_2->addChild(CreateMyParticleScene_2(588));
		sw588_2->setUpdateCallback(new SpaceShipCallback(588,2));
		sceneRoot->addChild(sw588_2.get());
		osg::ref_ptr<osg::Switch> sw588_3 = new osg::Switch();
		sw588_3->addChild(CreateMyParticleScene_3(588));
		sw588_3->setUpdateCallback(new SpaceShipCallback(588,3));
		sceneRoot->addChild(sw588_3.get());
		osg::ref_ptr<osg::Switch> sw588_4 = new osg::Switch();
		sw588_4->addChild(CreateMyParticleScene_4(588));
		sw588_4->setUpdateCallback(new SpaceShipCallback(588,4));
		sceneRoot->addChild(sw588_4.get());
		osg::ref_ptr<osg::Switch> sw588_5 = new osg::Switch();
		sw588_5->addChild(CreateMyParticleScene_5(588));
		sw588_5->setUpdateCallback(new SpaceShipCallback(588,5));
		sceneRoot->addChild(sw588_5.get());
	}

	if((damageState[594]<nTimePoint) && (inModelList[594] ==1)){
		osg::ref_ptr<osg::Switch> sw594_0 = new osg::Switch();
		sw594_0->addChild(CreateMyParticleScene_0(594));
		sw594_0->setUpdateCallback(new SpaceShipCallback(594,0));
		sceneRoot->addChild(sw594_0.get());
		osg::ref_ptr<osg::Switch> sw594_1 = new osg::Switch();
		sw594_1->addChild(CreateMyParticleScene_1(594));
		sw594_1->setUpdateCallback(new SpaceShipCallback(594,1));
		sceneRoot->addChild(sw594_1.get());
		osg::ref_ptr<osg::Switch> sw594_2 = new osg::Switch();
		sw594_2->addChild(CreateMyParticleScene_2(594));
		sw594_2->setUpdateCallback(new SpaceShipCallback(594,2));
		sceneRoot->addChild(sw594_2.get());
		osg::ref_ptr<osg::Switch> sw594_3 = new osg::Switch();
		sw594_3->addChild(CreateMyParticleScene_3(594));
		sw594_3->setUpdateCallback(new SpaceShipCallback(594,3));
		sceneRoot->addChild(sw594_3.get());
		osg::ref_ptr<osg::Switch> sw594_4 = new osg::Switch();
		sw594_4->addChild(CreateMyParticleScene_4(594));
		sw594_4->setUpdateCallback(new SpaceShipCallback(594,4));
		sceneRoot->addChild(sw594_4.get());
		osg::ref_ptr<osg::Switch> sw594_5 = new osg::Switch();
		sw594_5->addChild(CreateMyParticleScene_5(594));
		sw594_5->setUpdateCallback(new SpaceShipCallback(594,5));
		sceneRoot->addChild(sw594_5.get());
	}

	if((damageState[614]<nTimePoint) && (inModelList[614] ==1)){
		osg::ref_ptr<osg::Switch> sw614_0 = new osg::Switch();
		sw614_0->addChild(CreateMyParticleScene_0(614));
		sw614_0->setUpdateCallback(new SpaceShipCallback(614,0));
		sceneRoot->addChild(sw614_0.get());
		osg::ref_ptr<osg::Switch> sw614_1 = new osg::Switch();
		sw614_1->addChild(CreateMyParticleScene_1(614));
		sw614_1->setUpdateCallback(new SpaceShipCallback(614,1));
		sceneRoot->addChild(sw614_1.get());
		osg::ref_ptr<osg::Switch> sw614_2 = new osg::Switch();
		sw614_2->addChild(CreateMyParticleScene_2(614));
		sw614_2->setUpdateCallback(new SpaceShipCallback(614,2));
		sceneRoot->addChild(sw614_2.get());
		osg::ref_ptr<osg::Switch> sw614_3 = new osg::Switch();
		sw614_3->addChild(CreateMyParticleScene_3(614));
		sw614_3->setUpdateCallback(new SpaceShipCallback(614,3));
		sceneRoot->addChild(sw614_3.get());
		osg::ref_ptr<osg::Switch> sw614_4 = new osg::Switch();
		sw614_4->addChild(CreateMyParticleScene_4(614));
		sw614_4->setUpdateCallback(new SpaceShipCallback(614,4));
		sceneRoot->addChild(sw614_4.get());
		osg::ref_ptr<osg::Switch> sw614_5 = new osg::Switch();
		sw614_5->addChild(CreateMyParticleScene_5(614));
		sw614_5->setUpdateCallback(new SpaceShipCallback(614,5));
		sceneRoot->addChild(sw614_5.get());
	}

	if((damageState[617]<nTimePoint) && (inModelList[617] ==1)){
		osg::ref_ptr<osg::Switch> sw617_0 = new osg::Switch();
		sw617_0->addChild(CreateMyParticleScene_0(617));
		sw617_0->setUpdateCallback(new SpaceShipCallback(617,0));
		sceneRoot->addChild(sw617_0.get());
		osg::ref_ptr<osg::Switch> sw617_1 = new osg::Switch();
		sw617_1->addChild(CreateMyParticleScene_1(617));
		sw617_1->setUpdateCallback(new SpaceShipCallback(617,1));
		sceneRoot->addChild(sw617_1.get());
		osg::ref_ptr<osg::Switch> sw617_2 = new osg::Switch();
		sw617_2->addChild(CreateMyParticleScene_2(617));
		sw617_2->setUpdateCallback(new SpaceShipCallback(617,2));
		sceneRoot->addChild(sw617_2.get());
		osg::ref_ptr<osg::Switch> sw617_3 = new osg::Switch();
		sw617_3->addChild(CreateMyParticleScene_3(617));
		sw617_3->setUpdateCallback(new SpaceShipCallback(617,3));
		sceneRoot->addChild(sw617_3.get());
		osg::ref_ptr<osg::Switch> sw617_4 = new osg::Switch();
		sw617_4->addChild(CreateMyParticleScene_4(617));
		sw617_4->setUpdateCallback(new SpaceShipCallback(617,4));
		sceneRoot->addChild(sw617_4.get());
		osg::ref_ptr<osg::Switch> sw617_5 = new osg::Switch();
		sw617_5->addChild(CreateMyParticleScene_5(617));
		sw617_5->setUpdateCallback(new SpaceShipCallback(617,5));
		sceneRoot->addChild(sw617_5.get());
	}

	//优化场景数据
	osgUtil::Optimizer optimizer;
	optimizer.optimize(sceneRoot.get(), osgUtil::Optimizer::ALL_OPTIMIZATIONS ^ osgUtil::Optimizer::OPTIMIZE_TEXTURE_SETTINGS);
	//<magic>
	osgUtil::IndexMeshVisitor imv;
	sceneRoot->accept(imv);
	imv.makeMesh();
	//</magic>

	viewer->addEventHandler(new UseEventHandler());
	viewer->addEventHandler(new osgViewer::StatsHandler);

	viewer->setSceneData(sceneRoot.get());
	viewer->realize();
	return viewer->run();
}