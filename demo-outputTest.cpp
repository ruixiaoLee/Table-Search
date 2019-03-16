//201811@Richelle
//୧(๑•̀⌄•́๑)૭✧
#include "server.h"
using namespace std;
void output_plaintext(const vector<vector <long>> &a){
  long row=a.size();
  long col=a[0].size();

    for(int i=0 ; i<row ; i++){
        for(int j=0 ; j<col ; j++)
            cout<<a[i][j]<<" ";
        cout<<endl;
	cout<<i<<endl;
    }
}

void out(vector<long> ss){
  for(int i=0 ; i<ss.size() ; i++){
    cout << ss[i] << " ";
  }cout << endl;
}

int main(){
  //resetting FHE
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
/////////////////////////////////////////////////////////

  Ctxt ct_result(publicKey);
  ifstream result("Final_result");
  result >> ct_result;

  cout << "The result from CS"<< endl;
  vector<long> final_result;
  ea.decrypt(ct_result, secretKey, final_result);
  out(final_result);
  //long fs=final_result[0];
  //cout << "final result is: " << fs << endl;
  //cout << mid_result[0]<<endl;

  return 0;
}
