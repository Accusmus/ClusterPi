//first.cpp Adding numbers using two nodes C++ version
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <climits>
#include <iostream>
#include <fstream>
#include <sstream>

#include "mpi.h"

typedef unsigned long ULONG;

//==================================
// function declarations
//==================================
ULONG createRandInt(ULONG i, ULONG k);
bool inCircle(double x, double y);
ULONG findPI(ULONG n, ULONG first, ULONG k);

using namespace std;

ULONG A[200]= {1664525, 389569705, 2940799637,158984081,2862450781,3211393721, 1851289957,3934847009, 218491486,246739401,1948736821, 2941245873,4195587069,4088025561,980655621,2001863745,657792333,65284841,1282409429, 3808694225,2968195997,2417331449,2878627493,307989601,504219373, 1897564169,2574089845,3294562801, 3478292285,2651335705,2523738949, 666245249,4137395341, 2604435753,1706708245, 3963176977,3678957277,3530469177, 3858799589,629287073,3146069549,3820924489, 2403397557, 2390444593, 2593868413, 4291139161, 1705056389, 3186638017,1279327181,1263785871, 1520793449};

ULONG K[200] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50};

ULONG m = ULONG_MAX; 

ULONG C[200] = {1013904223, 1196435762, 3519870697, 2868466484, 1649599747, 2670642822, 1476291629, 2748932008, 2180890343,2498801434, 3421909937 , 3167820124 , 2636375307, 3801544430, 28987765, 2210837584, 3039689583, 1338634754, 1649346937, 2768872580 , 2254235155, 2326606934, 1719328701, 1061592568, 53332215, 1140036074, 4224358465, 2629538988, 1946028059, 573775550, 1473591045, 95141024,1592739711, 1618554578, 4257218569, 2685635028, 2617994019, 740185638, 4194465613, 2426187848, 967350023, 366635194, 2557108433, 3503432700, 353185579, 706247310, 408928405, 1855199472, 1263785871, 2223693730};

int main(int argc,char* argv[])
{
  MPI_Status Stat;

  MPI::Init(argc, argv);

  int myid = MPI::COMM_WORLD.Get_rank();
  int numproc = MPI::COMM_WORLD.Get_size(); 

  ULONG totalIter, iterPerProc;
  totalIter = strtoul(argv[1], NULL, 0);
  iterPerProc = totalIter/numproc;

  double t1 = MPI_Wtime();
  double timerunning;
  if(myid == 0){
    ULONG masterRand = createRandInt(0, numproc-1);
    double totalPi;
    
    double comt1, comt2, d1 = 0, d2 = 0;
    for(int i = 1; i < numproc; i++){
      ULONG rand = createRandInt(i, numproc-1);
      comt1 = MPI_Wtime();
      MPI_Send(&rand, 1, MPI::UNSIGNED_LONG, i, 0, MPI_COMM_WORLD);
      d1 += MPI_Wtime() - comt1;
    }

    ULONG pi = findPI(iterPerProc, masterRand, numproc);
    totalPi += pi;

    for(int i = 1; i < numproc; i++){
      ULONG slavepi;
      comt2 = MPI_Wtime();
      MPI_Recv(&slavepi, 1, MPI::UNSIGNED_LONG, i, 0, MPI_COMM_WORLD, &Stat);
      d2 += MPI_Wtime() - comt2;
      totalPi += slavepi;
    }
    double dt = MPI_Wtime() - t1;
    double result = totalPi/(iterPerProc*numproc);
    cout << "result pi is: " << result*4 << endl;
    cout << "total time taken: " << dt << endl;
    dt = d1 + d2;
    cout << "communication time: " << dt << endl;

  }else if(myid > 0){ 
      ULONG rand;
      MPI_Recv(&rand, 1, MPI::UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, &Stat);
      ULONG pi = findPI(iterPerProc, rand, numproc-1);
      MPI_Send(&pi, 1, MPI::UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD);
  }

  MPI::Finalize();
}

ULONG createRandInt(ULONG i, ULONG k){
  return ((A[k] * i) + C[k]) % m;
}

bool inCircle(double x, double y){ 
  double d = sqrt(pow(x - 0.5, 2) + pow(y - 0.5, 2));
  if(abs(d) >= 0.5){
    return false;
  }
  return true;
}

ULONG findPI(ULONG n, ULONG first, ULONG k){
  ULONG numTotal = 0;
  ULONG numInside = 0; 

  ULONG prev = first, next = 0;

  for(int i = 0; i < n; i++){
    next = createRandInt(prev, k); 
    double x = abs((double)next/m); 
    double y = abs((double)prev/m); 

    if(inCircle(x, y)){
      numInside++; 
    }
    numTotal ++; 
    prev = next;
  }

  return numInside;
}
