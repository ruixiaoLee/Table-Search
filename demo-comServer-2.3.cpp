//201812@Richelle
//୧(๑•̀⌄•́๑)૭✧
#include "server.h"

using namespace std;

vector<vector <long> > read_table(const string &filename){
  long temp;
	string lineStr;
	vector<vector<long> > table;

  ifstream inFile(filename, ios::in);
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

  long l = num_slots;
  long k = ceil(TABLE_SIZE_N/l);
  cout << "l=" << l << ' '<< "k=" << k << endl;
///////////////////////////////////////////////////////////////////////////////

  //read index and PIR query from file
  cout << "Reading query from DS..." << flush;
  fstream PIRqueryFile("PIR_query", fstream::in);
  Ctxt new_query(publicKey);
  Ctxt new_index1(publicKey);
  Ctxt new_index2(publicKey);
  PIRqueryFile >> new_index1;
  PIRqueryFile >> new_index2;
  PIRqueryFile >> new_query;
  PIRqueryFile.close();
  cout << "OK" << endl;


  //read output table
  vector<vector<long> > LUT_output = read_table("LUT_output");
  vector<Ctxt> res;
  for(int i=0; i<k ; i++){
    Ctxt tep1(publicKey);
    res.push_back(tep1);
  }
  vector<Ctxt> ind;
  for(int i=0; i<l ; i++){
    Ctxt tep2(publicKey);
    ind.push_back(tep2);
  }

////////////////////////////////////////////////////////////////////
  cout <<"Threads index level 1st" << endl;
  auto start1=chrono::high_resolution_clock::now();

  SetNumThreads(NF);

  NTL_EXEC_RANGE(l, first, last)

  for (long i=first ; i<last ; i++){
    Ctxt temp = new_index1;
    ea.rotate(temp,i);
    temp *= new_index2;
    ind[i] = temp;
  }
  NTL_EXEC_RANGE_END

  cout << "OK"<< endl;

////test/////////////////////////
/*  cout << "Saving..." << flush;
  ofstream stindex;
  stindex.open("stindex");
  for(int i=0 ; i<l ; i++){
    stindex << ind[i];
  }
  stindex.close();
  cout << "OK" << endl;
*/
////////////////////////////////////////////////////////////////////

  cout <<"Threads index level 2nd" << endl;

  SetNumThreads(NF);

  NTL_EXEC_RANGE(k, first, last)

  for (long i=first ; i<last ; i++){
    long ss = i/l;
    long kk = i%l;
    //cout << "i="<<i<<" ss="<<ss<<" kk="<<kk<<endl;
    LUT_output[i].resize(num_slots);
    ZZX outputTable_poly;
    ea.encode(outputTable_poly, LUT_output[i]);
    Ctxt temp = ind[ss];
    ea.rotate(temp, kk);
    temp *= new_query;
    temp.multByConstant(outputTable_poly);
    res[i]=temp;
  }
  NTL_EXEC_RANGE_END

  cout << "OK" << endl;


///test//////////////////////////
/*  cout << "Saving..." << flush;
  ofstream ndindex;
  ndindex.open("ndindex");
  for(int i=0 ; i<k ; i++){
    ndindex << res[i];
  }
  ndindex.close();
  cout << "OK" << endl;
  */
//////////////////////////////////////////////////////////////////


  //result累加
  Ctxt fin_res=res[0];
  for(int i=1 ; i<k ; i++){
    fin_res += res[i];
  }

  auto end1=chrono::high_resolution_clock::now();
  chrono::duration<double> diff = end1-start1;
  cout << "Select output time is: " << diff.count() << "s" << endl;

  //write Final_result in a file
  cout << "Saving final result..." << flush;
  ofstream Final_result;
  Final_result.open("Final_result");
  Final_result << fin_res ;
  Final_result.close();
  cout << "OK" << endl;

  return 0;
}
