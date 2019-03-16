#include <iostream>
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
#define TABLE_SIZE_N 448000

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

int main(){

}
