#include <iostream>
//#include <NTL/ZZ.h>
//#include <NTL/BasicThreadPool.h>
//#include "FHE.h"
//#include "timing.h"
//#include "EncryptedArray.h"
//#include <NTL/lzz_pXFactoring.h>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <chrono>
#include <random>
#include <vector>
#include <thread>
#include <future>
#include <ctime>
#include <ratio>
#include <math.h>
#include <string>
#include <sstream>
#define TABLE_SIZE_N 50

using namespace std;

unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::mt19937 generator(seed);


vector<vector <long> > read_table(const string &filename){
  long temp;
	string lineStr;
	vector<vector<long> > table;

  ifstream inFile(filename, ios::in);//read from
	while (getline(inFile, lineStr)){
    vector<long> table_col;
		stringstream ss(lineStr);
		string str;
		while (getline(ss, str, ' ')){
      temp = std::stoi(str);
			table_col.push_back(temp);
    }
		table.push_back(table_col);
	}
  return table;
}

//先产生一个随机数列用于之后的插入操作
vector<long> getRandomVector(long Total){
  vector<long> input=*new vector<long>();
  for(long i=0 ; i<Total ; i++){
    input.push_back(i);
  }
  vector<long> output=*new vector<long>();

  int end=Total;
  for(long i=0;i<Total;i++){
    vector<long>::iterator iter=input.begin();
    long num=generator()%end;
    iter+=num;
    output.push_back(*iter);
    input.erase(iter);
    end--;
  }
  return output;
}

//此处生成新的LUT
void Create_LUT(vector<vector<long> > LUT, vector<long> randomVector,vector<vector<long> > &LUT_input, vector<vector<long> > &LUT_output){
  //vector <vector<long> > LUT_input;
  long k=5;
  long l=10;
  cout<<k<<' '<<l<<endl;

  vector<long> sub_input;
  //vector <vector<long> > LUT_output;
  vector<long> sub_output;

  long Total=randomVector.size();

  cout<<Total<<endl;

  long index=0;
    for(long i=0 ; i<k ; i++){
      for(long j=0 ; j<l ; j++){
        //cout<<"s="<<s<<"i="<<i<<"j="<<j<<endl;
        long s=randomVector[index];
        cout<<"s="<<s<<"i="<<i<<"j="<<j<<endl;
        long temp_in=LUT[0][s];
        cout <<LUT[0][s]<<endl;
        sub_input.push_back(temp_in);
        long temp_out=LUT[1][s];
        cout<<LUT[1][s]<<endl;
        sub_output.push_back(temp_out);
        index++;
      }
      LUT_input.push_back(sub_input);
      sub_input.clear();
      LUT_output.push_back(sub_output);
      sub_output.clear();
    }

    cout<<"end"<<endl;
}

void output_plaintext(const vector<vector <long>> &a){
  long row=a.size();
  long col=a[0].size();

    for(int i=0 ; i<row ; i++){
        for(int j=0 ; j<col ; j++)
            cout<<a[i][j]<<' ';
        cout<<endl;
    }
}

int main(){
  long T=50;
  vector<long> vi = getRandomVector(T);
  for(int i=0 ; i<T ; i++){
    cout << vi[i] <<' '<< i<<endl;
  }
  cout<<endl;

  //Read InputTable from file
  cout << "Reading InputTable from file..." << flush;
  vector<vector<long> > InputTable = read_table("inputTable");
  //vector<vector<long> > OutputTable = read_table("outputTable");
  output_plaintext(InputTable);
  cout << endl;
  //output_plaintext(OutputTable);
  //cout << endl;
  cout << "OK" << endl;

  //change all position, make new LUT
  cout << "Reconstructe index..." << flush;
  vector<vector<long> > LUT_input;
  vector<vector<long> > LUT_output;
  Create_LUT(InputTable, vi, LUT_input, LUT_output);
  cout<<"ininin"<<endl;

  output_plaintext(LUT_input);
  cout << endl;
  output_plaintext(LUT_output);
  cout << endl;
  cout << "OK" << endl;

  return 0;

}
