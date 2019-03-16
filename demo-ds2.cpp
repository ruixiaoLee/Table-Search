//201902@Richelle
//୧(๑•̀⌄•́๑)૭✧
#include "server.h"

//when the number of column >= row

using namespace std;

vector<long> shift_work(const vector<long> &query, const long &index, const long &num_slots){
  vector<long> new_index;
  long size = query.size();
  long temp;

  for(long i=0 ; i<num_slots ; i++){
    if((i+index) >= size) temp = query[i+index-size];
    else temp=query[i+index];
    new_index.push_back(temp);
  }

  return new_index;
}

void out(vector<long> ss){
  for(int i=0 ; i<ss.size() ; i++){
    cout << ss[i] << " ";
  }cout << endl;
}

void output_plaintext(const vector<vector <long>> &a){
  long row=a.size();
  long col=a[0].size();

    for(int i=0 ; i<row ; i++){
        for(int j=0 ; j<col ; j++)
            cout<<a[i][j]<<" ";
        cout<<endl;
    }
}

int main(){
  //resetting FHE
  cout << "Setting FHE..." << flush;
  unsigned long m,p,r;
  vector<long> gens,ords;
  ifstream sk("sk.txt");
  readContextBase(sk,m,p,r,gens,ords);
  FHEcontext context(m,p,r,gens,ords);
  sk >> context;
  FHESecKey secretKey(context);
  const FHEPubKey& publicKey = secretKey;
  sk >> secretKey;
  sk.close();

  ZZX G = context.alMod.getFactorsOverZZ()[0];
  EncryptedArray ea(context, G);
  long num_slots = ea.size();
  cout << "OK" << endl;

  long l = num_slots;
  long k = ceil((double)TABLE_SIZE_N/l);
  cout << "l=" << l << ' '<< "k=" << k << endl;
///////////////////////////////////////////////////////////////////
  vector<Ctxt> ct_result;
  vector<vector<long> > dec_result(k);
  cout << "Reading intermediate result..." << flush;

  auto start2=chrono::high_resolution_clock::now();

  ifstream mid_result("mid_result");
  for(int i = 0; i < k ; i++) {
    Ctxt temp(publicKey);
    mid_result >> temp;
    ct_result.push_back(temp);
  }
  mid_result.close();
  cout << "OK" << endl;

  auto end2=chrono::high_resolution_clock::now();
  chrono::duration<double> diff2 = end2-start2;
  cout << "Reading time is: " << diff2.count() << "s" << endl;

  cout << "Decrypting..."<< flush;

  auto start1=chrono::high_resolution_clock::now();

  SetNumThreads(NF);

  NTL_EXEC_RANGE(k, first, last)

  for (int i=first ; i<last ; i++){
    ea.decrypt(ct_result[i], secretKey, dec_result[i]);
  }

  NTL_EXEC_RANGE_END

  cout << "OK" << endl;
  auto end1=chrono::high_resolution_clock::now();
  chrono::duration<double> diff = end1-start1;
  cout << "Decryption time is: " << diff.count() << "s" << endl;

  //find the position of 0.
  cout << "Making PIR-query..." << flush;
  auto start3=chrono::high_resolution_clock::now();

  long index;
  vector<vector<long> > query;//new_query is a matrix
  vector<long> query_zero;

  for(long i=0 ; i<k ; i++){
    for(long j=0 ; j<num_slots ; j++){
      if(dec_result[i][j] == 0){
        index = i;
        for(int kk=0 ; kk<num_slots ; kk++){
          if(kk==j) query_zero.push_back(1);
          else query_zero.push_back(0);
        }
      }
    }
  }
  cout << "OK" << endl;

//new_index is new_query left_shift the value of index
  long dime;//# of dimension
  dime = ceil((double)log(TABLE_SIZE_N)/(double)log(l));
  cout << dime << endl;

  for(int i=0 ; i<dime ; i++){
    query.push_back(query_zero);
  }

  for(int w=1; w<dime ; w++){
    long index_t=((double)index/pow(l,w-1));
    index_t %= num_slots;
    cout << index_t << endl;
    query[w]=shift_work(query[w-1], index_t, num_slots);
    //print
    out(query[w]);
    //ea.encrypt(Ctxt_query[w], publicKey, query[w]);
  }
  vector <Ctxt> Ctxt_query;
  for(int i=0 ; i<dime ; i++){
    Ctxt que(publicKey);
    ea.encrypt(que, publicKey, query[i]);
    Ctxt_query.push_back(que);
  }

  cout << "OK" << endl;
  auto end3=chrono::high_resolution_clock::now();
  chrono::duration<double> diff3 = end3-start3;
  cout << "Make PIR query time is: " << diff3.count() << "s" << endl;

  //write in a file
  cout << "Saving query..." << flush;
  ofstream PIRqueryFile;
  PIRqueryFile.open("PIR_query");
  for(int i=0 ; i<dime ; i++){
    PIRqueryFile << Ctxt_query[i];
  }
  PIRqueryFile.close();
  cout << "OK" << endl;

  return 0;

}
