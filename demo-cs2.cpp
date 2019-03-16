//201902@Richelle
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
  long k = ceil((double)TABLE_SIZE_N/l);
  cout << "l=" << l << ' '<< "k=" << k << endl;
///////////////////////////////////////////////////////////////////////////////

  //read index and PIR query from file
  long dime;//# of dimension
  dime = ceil((double)log(TABLE_SIZE_N)/(double)log(l));
  cout << dime << endl;

  cout << "Reading query from DS..." << flush;
  fstream PIRqueryFile("PIR_query", fstream::in);
  vector <Ctxt> new_query;
  for (int i=0 ; i<dime ; i++){
    Ctxt query(publicKey);
    PIRqueryFile >> query;
    new_query.push_back(query);
  }
  PIRqueryFile.close();
  cout << "OK" << endl;


  //read output table
  vector<vector<long> > LUT_output = read_table("LUT_output");
  vector<Ctxt> res;
  for(int i=0; i<k ; i++){
    Ctxt tep(publicKey);
    res.push_back(tep);
  }
////////////////////////////////////////////////////////////////////
  cout << "threads work" << endl;

  auto start1=chrono::high_resolution_clock::now();
//////////////////////////////////////////////////////////////////
  vector<vector <Ctxt> > rec_query;//save new query
  vector<Ctxt> exv;
  for(int i=0 ; i<dime-1 ; i++){
    for(int j=0 ; j<pow(num_slots,i+1); j++){
      Ctxt exn(publicKey);
      exv.push_back(exn);
    }
    rec_query.push_back(exv);
  }

  Ctxt temp(publicKey);
  long s = dime-1;
  cout << s << endl;
  //when the number of dimension>2


  if(dime>2){
    for(int i=0 ; i<dime-2 ; i++){
      //SetNumThreads(NF);
      //NTL_EXEC_RANGE(pow(num_slots,i+1), first, last)
      for(int j=0 ; j<pow(num_slots,i+1) ; j++){
        if(i == 0){
          temp = new_query[s];
        }
        else{
          temp = rec_query[i-1][j/num_slots];
          //cout<<i-1<<"j="<<j<<"j/l="<<j/num_slots<<endl;
        }
        ea.rotate(temp,j%num_slots);
        temp *= new_query[s-1];
        rec_query[i][j]=temp;
        //cout << "i="<<i<<"j="<<j<<"s="<<s<<endl;
        //s-=1;
      }s-=1;
    }
  }
  cout<<"1st end"<<endl;

//  SetNumThreads(NF);
//  NTL_EXEC_RANGE(k, first, last)
  Ctxt fin_res(publicKey);
  for(int i=0 ; i<k ; i++){
    LUT_output[i].resize(num_slots);
    ZZX outputTable_poly;
    ea.encode(outputTable_poly, LUT_output[i]);
    if(dime==2){
      temp = new_query[1];
    }
    else{
      temp = rec_query[dime-3][i/num_slots];
    }
    ea.rotate(temp, i%num_slots);
    cout<<"here"<<endl;
    cout<<temp.findBaseLevel()<<endl;
    temp*=new_query[0];//bug
    //temp.multiplyBy(new_query[0]);
    //cout<<"hhh"<<endl;
    temp.multByConstant(outputTable_poly);
    //totalSums(ea, temp);
    res[i]=temp;
  }

//////////////////////////////////////////////////////////////////
  //result累加

  fin_res = res[0];
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
