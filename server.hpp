#include <iostream>
#include <NTL/ZZ.h>
#include <NTL/BasicThreadPool.h>
#include "FHE.h"
#include "timing.h"
#include "EncryptedArray.h"
#include <NTL/lzz_pXFactoring.h>
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
#include <FHEContext.h>

#define TABLE_SIZE_N 7290

#define NF 6
