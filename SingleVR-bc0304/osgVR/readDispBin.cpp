#include "AttrDispReader.h"

using namespace std;



//读取属性表文件。文件编码格式需为asci。
//ID	attrArry[x][0]
//层数	attrArry[x][4]
//层高	attrArry[x][6]
void readAttrFile(string filePath){
	ifstream inFile(filePath);
	string lineStr;

	while(getline(inFile, lineStr)){
		//cout<< lineStr <<endl;
		string str;
		stringstream ss(lineStr);
		vector<string> lineArray;
		while(getline(ss, str, ',')){
			lineArray.push_back(str);
		}
		attrArray.push_back(lineArray);
	}

	nBuildings = attrArray.size()-1;
	cout<<"attr file is read in "<<(double) clock()/CLOCKS_PER_SEC<<" sec."<<endl;
	inFile.close();
}

//读取属性表文件。文件编码格式需为asci。
//读取disp.bin文件
void readDispFile(string filePath, int nTimePoint){
	ifstream dispFile(filePath, ios::binary);

	int nBdsInDisp;
	int tempInt;
	double disp = 0.0;
	string tempStr;
	double tempDouble;


	dispFile.read((char*)(&nBdsInDisp), sizeof(nBdsInDisp));

	if(nBdsInDisp != nBuildings){
		cout<<"nBuildings has discrepancy between disp.bin and attr.txt"<<endl;
		return;
	}

	for(int i=0;i<nBuildings;i++){
		dispFile.read((char*)(&tempInt), sizeof(tempInt));
		// cout<<i<<endl;	//输出，相当于进度
		//cout<<"Object ID: "<<tempInt+1<<endl;
		int nos = atoi(attrArray[i+1][4].c_str());
		//cout<<"Nubmer of storeies: "<<nos<<endl;

		vector<vector<double>> objDisp;
		for(int j=0;j<=nTimePoint;j++){
			dispFile.read((char*)(&tempDouble),sizeof(tempDouble));
			//cout<<"    Time: "<<tempDouble<<endl;
			vector<double> storeyDisp;
			for(int k=0;k<nos;k++){
				dispFile.read((char*)(&disp),sizeof(disp));
				//cout<<"        Stroey "<<k+1<<": "<<disp<<"\t";
				storeyDisp.push_back(disp);
			}
			//cout<<endl;
			//cout<<storeyDisp.size()<<endl;
			objDisp.push_back(storeyDisp);
			storeyDisp.clear();
		}
		dispArray.push_back(objDisp);
		objDisp.clear();
	}
	cout<<"disp file is read in "<<(double) clock()/CLOCKS_PER_SEC<<" sec."<<endl;
	//cout<<dispArray[1][1506][1]<<endl;

	dispFile.close();
}