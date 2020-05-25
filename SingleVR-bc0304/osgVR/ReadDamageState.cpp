#include "Global.h"
#include "DamageStateLoader.h"

void ReadDamageState(){
	string temp, tempType, tempString;
	int tempNum;
	int count = 1;	//发生倒塌建筑的序号
	int damageStateFloor[1000][50];	//1000栋建筑，建筑层数不超过50
	//int seqBldDamage = 0;
	fstream readType(damageStatePath+"DamageStateFloor.txt");
	getline(readType, tempType);
	if(nBuildings > 1000){
		cout<<"nBuildings exceeds length of array."<<endl;
		return;
	}
	for(int i=0; i<nBuildings; i++){
		readType>>tempNum>>tempString;
		for(int j=0; j<atoi(attrArray[i][4].c_str()); j++){
			readType>>damageStateFloor[i][j];	
		}
	}
	//for(int i=0; i<nBuildings; i++){
	//	for(int j=0; j<atoi(attrArray[i][4].c_str()); j++){
	//		cout<<damageStateFloor[i][j]<<"	";	
	//	}
	//	cout<<endl;
	//}
	readType.close();
	
	damageType.resize(nBuildings);
	fill(damageType.begin(),damageType.end(),-1);

	fstream fin(damageStatePath+"DamageState.txt");
	getline(fin, temp);			//略去表头
	//damageState.resize(atoi(attrArray[attrArray.size()-1][objIDIndex].c_str())+1);  //wy之前编的
	//cout<<"damage state size: "<<atoi(attrArray[attrArray.size()-1][objIDIndex].c_str())<<endl;
	//cout<<"attrArray size: "<<attrArray.size()<<endl;
	damageState.resize(nBuildings);		//可视化对应的倒塌时刻
	fill(damageState.begin(),damageState.end(),nTimePoint);
	damageStateOrigin.resize(nBuildings);		//发生倒塌的时刻
	fill(damageStateOrigin.begin(),damageStateOrigin.end(),nTimePoint);		//若未在BlgXX_DamageStates.txt找到倒塌状态4，默认不发生倒塌
	collapseStorey.resize(nBuildings);		//中间发生倒塌的楼层
	fill(collapseStorey.begin(),collapseStorey.end(),-1);
	while(getline(fin, temp)){
		//cout<<temp<<endl;
		int highestCollapseFloor = 0;	//发生倒塌的楼层
		int numStorey;		//楼层数量
		int tempDamage = atoi(&temp[temp.length()-1]);
		//cout<<tempDamage<<endl;
		if(tempDamage == 4){		//4代表出现破坏
			//判断建筑倒塌类型
			if(atoi(attrArray[count-1][4].c_str())>3){
				if(damageStateFloor[count-1][0] != 4){
					damageType[count-1]=3;			//判断中间层倒塌
					for(int i=0; i<atoi(attrArray[count-1][4].c_str()); i++){
						if(damageStateFloor[count-1][i] == 4){
								collapseStorey[count-1] = i+1;	//中间发生倒塌的楼层
								break;
						}
					}
				}
				else if(damageStateFloor[count-1][1] != 4){
					damageType[count-1]=1;			//判断底部一层倒塌
				}
				else if(damageStateFloor[count-1][2] == 4){
					cout<<attrArray[count-1][5].c_str()<<endl;
					if(atoi(attrArray[count-1][5].c_str()) == 1){	//判断是否为框剪结构，对应属性文件“1”
						damageType[count-1]=4;			//判断剪力墙整体倾斜倒塌
					}
					else{
						for(int i=2; i<atoi(attrArray[count-1][4].c_str()); i++){
							if(damageStateFloor[count-1][i] == 4){
								highestCollapseFloor = i+1;	//发生倒塌的最高楼层
							}
						}
						//cout<<highestCollapseFloor<<endl;
						if(highestCollapseFloor > atoi(attrArray[count-1][4].c_str())*0.6){
							damageType[count-1]=0;			//判断竖向垮塌倒塌
						}
						else{
							damageType[count-1]=2;			//判断底部数层倒塌，默认底部3层倒塌
						}
					}
				}
				else{
					damageType[count-1]=2;			//判断底部数层倒塌，默认底部3层倒塌
				}
			}
			else{
				numStorey = atoi(attrArray[count-1][4].c_str());
				switch (numStorey){
					case 1:{
						damageType[count-1]=0;			//判断竖向垮塌倒塌
						break;
					}
					case 2:{
						if(damageStateFloor[count-1][1] == 4){
							damageType[count-1]=0;			//判断竖向垮塌倒塌
						}
						else{
							damageType[count-1]=1;			//判断底部一层倒塌
						}
						break;
					}
					case 3:{
						if(damageStateFloor[count-1][0] == 4){
							if(damageStateFloor[count-1][1] == 4){
								damageType[count-1]=0;			//判断竖向垮塌倒塌
							}
							else{
								damageType[count-1]=1;			//判断底部一层倒塌
							}
						}
						else{
							damageType[count-1]=3;			//判断中间层倒塌
							collapseStorey[count-1] = 2;	//中间发生倒塌的楼层
						}
						break;
					}
					default:{
						cout<<"Error! numStorey = "<<numStorey<<endl;
						break;
					}
				}
			}
			//cout<<attrArray[count-1][objIDIndex]<<"	"<<damageType[count-1]<<endl;

			stringstream s;
			//s<<(count);
			s<<attrArray[count-1][objIDIndex].c_str();
			//cout<<s.str();
			fstream readState(damageStatePath+"Blg"+s.str()+"_DamageStates.txt");
			//cout<<damageStatePath+"Blg"+s.str()+"_DamageStates.txt"<<endl;
			string curState;
			bool hasDamage = false;
			int damageTP = 0;
			while(getline(readState, curState)){
				if(hasDamage){
					readState.close();
					break;
				}
				damageTP++;
				for(int i=0;i<curState.length();i++){
					if(curState[i]!='4')
						continue;
					else{
						damageStateOrigin[count-1]=damageTP;
						damageState[count-1]=damageTP + (rand()%10-10);
						hasDamage = true;
						break;
					}
				}
			}
		}
		count++;
	}

	fin.close();
	
	//旋转轴上的A点
	rotateAxis[0][0] = -792.0f;	//X坐标
	rotateAxis[0][1] = 716.0f;	//Y坐标
	rotateAxis[0][2] = 518.0f;	//Z坐标

	//旋转轴上的B点
	rotateAxis[1][0] = -786.0f;	//X坐标
	rotateAxis[1][1] = 722.0f;	//Y坐标
	rotateAxis[1][2] = 518.0f;	//Z坐标

	//旋转轴上的A点
	//rotateAxis[0][0] = -873.0f;	//X坐标
	//rotateAxis[0][1] = 650.0f;	//Y坐标
	//rotateAxis[0][2] = 518.0f;	//Z坐标

	//旋转轴上的B点
	//rotateAxis[1][0] = -805.0f;	//X坐标
	//rotateAxis[1][1] = 686.0f;	//Y坐标
	//rotateAxis[1][2] = 518.0f;	//Z坐标

	//倾倒的方向
	rotateDirctn[0] = -1;
	rotateDirctn[1] = 0;
	rotateDirctn[2] = 0;
	//for(int i=0;i<atoi(attrArray[attrArray.size()-1][objIDIndex].c_str())+1;i++)
	//	cout<<"ID:"<<i<<"\tID Map:"<<idMap[i]<<"\tDamageState:"<<damageState[idMap[i]]<<endl;

	//读取破坏后的烟气数据
	float smokeCoord = 0.0f;
	fstream readSmoke(smokeDataPath+"SmokeData.txt");
	for(int i=0; i<attrArray.size(); i++){
		readSmoke>>smokeCoordsArray[i][0]>>smokeCoordsArray[i][1]>>smokeTime[i][0]>>smokeTime[i][1];
	}
	
	for(int i=0; i<attrArray.size(); i++){
		cout<<smokeCoordsArray[i][0]<<"	"<<smokeCoordsArray[i][1]<<"	"<<smokeTime[i][0]<<"	"<<smokeTime[i][1]<<endl;
	}
	readSmoke.close();

	smokeStartTime[0] = 50;
	smokeStartTime[1] = 30;
	smokeStartTime[2] = 10;
	smokeStartTime[3] = -150;
	smokeStartTime[4] = -250;
	smokeStartTime[5] = -350;

	smokeEndTime[0] = 70;
	smokeEndTime[1] = 50;
	smokeEndTime[2] = 30;
	smokeEndTime[3] = 10;
	smokeEndTime[4] = 10;
	smokeEndTime[5] = 10;

	//读取模型列表编号
	int numModel[707];
	fstream readNumModel(smokeDataPath+"NumModelList.txt");
	for(int i=0; i<707; i++){
		readNumModel>>numModel[i];
	}
	readNumModel.close();

	inModelList.resize(nBuildings);		//是否有对应的建筑fbx模型
	fill(inModelList.begin(),inModelList.end(),0);		//默认不存在对应模型
	for(int i=0; i<attrArray.size(); i++){
		for(int j=0; j<707; j++){
			if(atoi(attrArray[i][objIDIndex].c_str()) == numModel[j]){
			inModelList[i] = 1;
			break;
			}
		}
	}

	for(int i=0; i<attrArray.size(); i++){
		cout<<inModelList[i]<<endl;
	}
}