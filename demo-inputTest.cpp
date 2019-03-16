//201811@Richelle
//୧(๑•̀⌄•́๑)૭✧

#include "server.h"

using namespace std;
void out(vector<long> ss){
  for(int i=0 ; i<ss.size() ; i++){
    cout << ss[i] << " ";
  }cout << endl;
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
  cout << "end" << endl;

  long l = num_slots;
  long k = ceil(TABLE_SIZE_N/l);
  //make query
  //Get query
  cout<<"Get query! (0-63):";
  long LUT_query;
  cin >> LUT_query;

  //encrypt the LUT query
  cout << "Encrypt and save your query..." << flush;
  vector<long> query;
  for(int i=0 ; i<l ; i++){
    query.push_back(LUT_query);
  }
  query.resize(l);
  out(query);

  Ctxt ct_query(publicKey);                      //new ciphertext for query_poly
  ea.encrypt(ct_query, publicKey, query);        //encrypt the query_poly to ct_query

  //save in a file
  ofstream queryFile;
  queryFile.open("query");
  queryFile << ct_query << endl;
  queryFile.close();
  cout << "end" << endl;

  return 0;
}
