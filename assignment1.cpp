//first.cpp Adding numbers using two nodes C++ version
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <climits>
#include <iostream>

#include "mpi.h"

typedef unsigned long ULONG;

//==================================
// function declarations
//==================================
ULONG createRandInt(ULONG i);
bool inCircle(double x, double y);
double findPI(ULONG n, ULONG first);

using namespace std;

const int k = 31; 
const ULONG a = 2523738949;
const ULONG m = ULONG_MAX;
const ULONG c = 1473591045;

int main(int argc,char* argv[])
{

  MPI_Status Stat;

  MPI::Init(argc, argv);

  int myid = MPI::COMM_WORLD.Get_rank();
  int numproc = MPI::COMM_WORLD.Get_size();

  if(myid == 0){ 
    ULONG masterRand = createRandInt(0);
    double totalPi;

    for(int i = 1; i < numproc; i++){
      ULONG rand = createRandInt(i);
      cout << "sent rand " << rand << endl;
      MPI_Send(&rand, 1, MPI::UNSIGNED_LONG, i, 0, MPI_COMM_WORLD);
    }

    double pi = findPI(129032258, masterRand);
    totalPi += pi;

    for(int i = 1; i < numproc; i++){
      double slavepi; 
      MPI_Recv(&slavepi, 1, MPI::DOUBLE, i, 0, MPI_COMM_WORLD, &Stat);
      cout << "recieved pi " << slavepi << " from " << i << endl;
      totalPi += slavepi;
    }

    double result = totalPi/numproc;
    cout << "result pi is: " << result << endl;

  }else if(myid > 0){ 
      ULONG rand;
      MPI_Recv(&rand, 1, MPI::UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, &Stat);
      cout << "received rand " << rand << endl;
      double pi = findPI(129032258, rand);  
      cout << "slave pi: " << pi << " for id " << myid << endl;
      MPI_Send(&pi, 1, MPI::DOUBLE, 0, 0, MPI_COMM_WORLD);
  }

  MPI::Finalize();
}

ULONG createRandInt(ULONG i){
  return ((a * i) + c) % m;
}

bool inCircle(double x, double y){ 
  double d = sqrt(pow(x - 0.5, 2) + pow(y - 0.5, 2));
  if(abs(d) >= 0.5){
    return false;
  }
  return true;
}

double findPI(ULONG n, ULONG first){
  int numTotal = 0;
  int numInside = 0; 

  ULONG prev = first, next = 0;

  for(int i = 0; i < n; i++){
    next = createRandInt(prev); 
    double x = abs((double)next/m); 
    double y = abs((double)prev/m); 

    if(inCircle(x, y)){
      numInside++; 
    }
    numTotal ++; 
    prev = next;
  }
  double num = (double)numInside/numTotal;
  double pi = abs(num * 4);

  return pi;
}
