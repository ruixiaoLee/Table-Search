//201811@Richelle
//୧(๑•̀⌄•́๑)૭✧
#include "server.h"

using namespace std;
//random seed
unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::mt19937 generator(seed);

//read table from file
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

//create a random vector for permutation
vector<long> getRandomVector(long Total){
  vector<long> input=*new vector<long>();
  for(long i=0 ; i<Total ; i++){
    input.push_back(i);
  }
  cout<<"LONG TIME WAIT..."<<endl;
  vector<long> output=*new vector<long>();

  int end=Total;
  for(long i=0;i<Total;i++){
    vector<long>::iterator iter=input.begin();
    long num=generator()%end;
    iter+=num;
    output.push_back(*iter);
    input.erase(iter);
    end--;
    cout<<i<<endl;
  }
  return output;
}

//creat new LUT
void Create_LUT(vector<vector<long> > LUT, vector<long> randomVector,vector<vector<long> > &LUT_input, vector<vector<long> > &LUT_output, long &l, long &k){
  //vector <vector<long> > LUT_input;
  cout<<k<<' '<<l<<endl;

  vector<long> sub_input;
  //vector <vector<long> > LUT_output;
  vector<long> sub_output;

  long Total=randomVector.size();

  cout<<Total<<endl;

  long index=0;
    for(long i=0 ; i<k ; i++){
      for(long j=0 ; j<l ; j++){
        long s=randomVector[index];
        //cout<<"s="<<s<<"i="<<i<<"j="<<j<<endl;
        long temp_in=LUT[0][s];
        cout <<LUT[0][s]<<endl;
        sub_input.push_back(temp_in);
        long temp_out=LUT[1][s];
        //cout<<LUT[1][s]<<endl;
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

//output a plaintext table
void output_plaintext(const vector<vector <long>> &a){
  long row=a.size();
  long col=a[0].size();

    for(int i=0 ; i<row ; i++){
        for(int j=0 ; j<col ; j++)
            cout<<a[i][j]<<" ";
        cout<<endl;
    }
}

int main(void){
  //resetting FHE
  cout << "Setting FHE..." << flush;
  unsigned long m,p,r;
  vector<long> gens,ords;
  ifstream ct("pk.txt");
  readContextBase(ct,m,p,r,gens,ords);
  FHEcontext context(m,p,r,gens,ords);
  ct >> context;
  FHEPubKey publicKey(context);
  ct >> publicKey;
  ct.close();

  ZZX G = context.alMod.getFactorsOverZZ()[0];
  EncryptedArray ea(context, G);
  long num_slots = ea.size();
  cout << "OK" << endl;
//////////////////////////////////////////////////////

  long l = num_slots;
  long k = ceil((double)TABLE_SIZE_N/l);
  cout << "l=" << l << ' '<< "k=" << k << endl;

  //Read query from file
  cout << "Reading query ..." << flush;
  ifstream readQuery;
  readQuery.open("query");
  Ctxt ct_query(publicKey);
  readQuery >> ct_query;
  readQuery.close();
  cout << "OK" <<endl;


  //Read InputTable from file
  cout << "Reading original LUT from file..." << flush;
  vector<vector<long> > ori_LUT = read_table("ori_LUT");
  //output_plaintext(ori_LUT);
//  cout << endl;
  cout << "OK" << endl;

  vector<long> vi = getRandomVector(TABLE_SIZE_N);
//  for(int i=0 ; i<TABLE_SIZE_N ; i++){
//    cout << vi[i] <<' '<< i<<endl;
//  }
//  cout<<endl;

  auto start2=chrono::high_resolution_clock::now();
  //change all position, make new LUT
  cout << "Make new LUT ..." << flush;
  vector<vector<long> > LUT_input;
  vector<vector<long> > LUT_output;
  Create_LUT(ori_LUT, vi, LUT_input, LUT_output, l, k);
  cout<<"end"<<endl;

  auto end2=chrono::high_resolution_clock::now();
  chrono::duration<double> diff2 = end2-start2;
  cout << "Construct LUTs time is: " << diff2.count() << "s" << endl;

  //write shifted_output_table in a file
  ofstream OutputTable;
  OutputTable.open("LUT_output");
  for(int i=0 ; i<k ; i++){
    for(int j=0 ; j<l ; j++){
      OutputTable << LUT_output[i][j] <<' ';
    }
    OutputTable << endl;
  }
  OutputTable.close();

  //Make Threads to compute every row of table
  cout << "Make threads for each row and compute" << endl;
  vector<Ctxt> Result;

  for(int i=0; i<k ; i++){
    Ctxt tep(publicKey);
    Result.push_back(tep);

  }
  //thread work
  auto start1=chrono::high_resolution_clock::now();

  SetNumThreads(NF);

  NTL_EXEC_RANGE(k, first, last)

    for(long i=first ; i<last ; i++){
      LUT_input[i].resize(num_slots);
      Ctxt res = ct_query;
      ZZX tt_poly;
      ea.encode(tt_poly, LUT_input[i]);
      res.addConstant(-tt_poly);
      res.multByConstant(to_ZZX(generator()%5+1));
      Result[i]=res;
    }

  NTL_EXEC_RANGE_END

  auto end1=chrono::high_resolution_clock::now();
  chrono::duration<double> diff = end1-start1;
  cout << "Search input time is: " << diff.count() << "s" << endl;

  //Get vector<Ctxt> Result
  cout << "Saving..." << flush;
  //save in a file
  ofstream outResult;
  outResult.open("mid_result");
  for(int i=0 ; i<k ; i++){
    outResult << Result[i];
  }
  outResult.close();
  cout << "OK" << endl;

  return 0;
}
