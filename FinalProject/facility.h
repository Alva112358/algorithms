#ifndef _FACILITY_
#define _FACILITY_

#include <iostream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <ctime>

using namespace std;

struct facility {
    facility(int _volumn, int _cost) {
        Max = _volumn;
        volumn = _volumn;
        cost = _cost;
    }
    int Max;
    int volumn;
    int cost;
};

struct costomer {
    costomer(int _demand) : demand(_demand) {}
    int demand;
    vector<int> assignment_cost;
};

class CFLP {
public:
    CFLP();
    ~CFLP();
    void init();
    void write();
    int greedyAlgorithm();
    int simulateAnnealing();
private:
    int facNum;
    int cosNum;
    int currentBestSolution;
    vector<facility> fct;
    vector<costomer> csm;
    vector<int> isOpen;
    vector<int> serveList;
    int getCurrentCost(vector<int> serve, vector<int> open);
    int initSolution();
    vector<int> createNewSolution(vector<facility>& fctTemp);
};

#endif //_FACILITY_