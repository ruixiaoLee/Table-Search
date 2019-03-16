//201811@Richelle
//୧(๑•̀⌄•́๑)૭✧
#include "server.h"

using namespace std;

int main(void){
  cout<<"FHE setting..."<< flush;
  //FHE setting, generate PK and SK////////////////////////////////
  long m = 10243;    // Specific modulus
  long p = 2;    // Plaintext base [default=2], should be a prime number
  long r = 14;    // Lifting [default=1]
  long L = 10;   // Number of levels in the modulus chain [default=heuristic]
  long c = 3;    // Number of columns in key-switching matrix [default=2]
  long w = 64;   // Hamming weight of secret key
  long d = 0;    // Degree of the field extension [default=1]
  long security = 128;  // Security parameter [default=80]
  long s = 0;    // Minimum number of slots [default=0]
  //m = FindM(security, L, c, p, d, s, 0);
  long plain_mod = pow(p, r);
  ZZX G;
  FHEcontext context(m, p, r);//Rpr
  buildModChain(context, L, c);//levels

  G = context.alMod.getFactorsOverZZ()[0];

  FHESecKey secretKey(context);
  const FHEPubKey& publicKey = secretKey;
  secretKey.GenSecKey(w);
  addSome1DMatrices(secretKey);

  //initialize FHEcontext
  EncryptedArray ea(context, G);
  assert(context.securityLevel()>=128);

  cout << "end" << endl;

  long num_slots = context.zMStar.getNSlots();
  cout << "Number of slots: " << num_slots << endl;
  double act_security = context.securityLevel(); //obrain real security level
  cout << "Security: " << act_security << endl;
  cout << "Plaintext modulus: " << plain_mod << endl;
  cout << "m=" << m << endl;

  cout << "Save pk and sk..." << flush;
  //Write Public Key in a file
  ofstream pkFile("pk.txt");
  writeContextBase(pkFile, context);
  pkFile << context << endl;
  pkFile << publicKey << endl;
  pkFile.close();

  //Write Secret Key in a file
  ofstream skFile("sk.txt");
  writeContextBase(skFile, context);
  skFile << context << endl;
  skFile << secretKey << endl;
  skFile.close();

  cout << "end" << endl;

  return 0;
}
