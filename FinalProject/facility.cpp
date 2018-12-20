#include "facility.h"

// Produce a random number between [min, max)
int random(int min, int max) {
    return rand() % max + min;
}

CFLP::CFLP() {
    init();
    clock_t start,finish;
    start = clock();
    // currentBestSolution = greedyAlgorithm();
    currentBestSolution = simulateAnnealing();
    finish = clock();
    cout << "Running time: " << (double)(finish-start) / CLOCKS_PER_SEC << "s" << endl;
    
    // Print the best cost.
    cout << "Result: " << currentBestSolution << endl;

    // Print the facility's open states.
    cout << "States: ";
    for (int i = 0; i < facNum; i++) {
        cout << isOpen[i] << " ";
    }
    cout << endl;

    // Print the serveList.
    cout << "Assignment: ";
    for (int i = 0; i < cosNum; i++) {
        cout << serveList[i] << " ";
    }
    cout << endl;   
}

CFLP::~CFLP() {
    // cout << "CFLP Finish! Thank you!" << endl;
}

void CFLP::init() {
    // cout << "Initialize the facility and costomers" << endl;

    cin >> facNum >> cosNum;

    // initial the facility and costomers.
    fct = vector<facility>(facNum, facility(0, 0));
    csm = vector<costomer>(cosNum, costomer(0));

    for (int i = 0; i < facNum; i++) {
        cin >> fct[i].volumn >> fct[i].cost;
        fct[i].Max = fct[i].volumn;
    }

    for (int i = 0; i < cosNum; i++) {
        csm[i].assignment_cost = vector<int>(facNum, 0);
        float temp;
        cin >> temp;
        // cout << temp << endl;
        csm[i].demand = (int)temp;
        // cout << csm[i].demand << endl;
    }
    
    for (int j = 0; j < facNum; j++) {
        for (int i = 0; i < cosNum; i++) {
            float temp;
            cin >> temp;
            csm[i].assignment_cost[j] = temp;
        }
    }

    // cout << "Initialize the facility and costomers finish!" << endl;
}

void CFLP::write() {
    cout << "FacNum: " << facNum << endl;
    cout << "CosNum: " << cosNum << endl;
    cout << endl;

    cout << "Facility: " << endl;
    for (int i = 0; i < facNum; i++) {
        printf("Facility %d: Volumn=%d Cost=%d\n", i, fct[i].volumn, fct[i].cost);
    }
    cout << endl;

    for (int i = 0; i < cosNum; i++) {
        printf("Costomer %d: Demand=%d\n", i, csm[i].demand);
        printf("Assigment_cost:\n");
        for (int j = 0; j < facNum; j++) {
            cout << csm[i].assignment_cost[j] << " ";
            if (j % 11 == 10) cout << endl;
        }
        cout << endl;
    }
}


int CFLP::greedyAlgorithm() {
    isOpen = vector<int>(facNum, 0);
    serveList = vector<int>(cosNum, -1);
    // Current cost.
    int G = 0;

    // Greedy Algorithm.
    // Costomer t.
    for (int t = 0; t < cosNum; t++) {
        int minCos = INT_MAX;
        int bestServe = -1;
        // Costomers t choose facility j.
        // Assume all the facility are open at first.
        for (int j = 0; j < facNum; j++) {
            // fct[j].volumn-csm[t].demand >= 0 means the fct[j] have enough space to satisfy the demand.
            if ((fct[j].volumn - csm[t].demand >= 0) && (minCos >= G+csm[t].assignment_cost[j])) {
                bestServe = j;
                minCos = G+csm[t].assignment_cost[j];
            }
        }

        serveList[t] = bestServe;
        fct[serveList[t]].volumn -= csm[t].demand; // Update the current volumns of fct.
        isOpen[serveList[t]] = 1;                  // Update the specific fct to open.
        G = minCos;                                // Update the current total cost.
    }

    // Add the cost of open facility to cost.
    for (int i = 0; i < facNum; i++) {
        if (isOpen[i]) {
            G += fct[i].cost;
        }
    }

    currentBestSolution = G;

    return G;
}

int CFLP::simulateAnnealing() {
    float T_begin = 20000.0;
    float T_end = (1e-6);
    float q_rate = 0.98;
    float iterator = 5000;

    int initCost = initSolution();

    /*
    cout << "Initial Solution is: " << endl;;
    for (int i = 0; i < serveList.size(); i++) {
        cout << serveList[i] << " ";
    }
    cout << endl;
    cout << "Initial Open State is: " << endl;
    for (int i = 0; i < isOpen.size(); i++) {
        cout << isOpen[i] << " ";
    }
    cout << endl;
    */
    
    // Simulate Annealing.
    int globalBest = initCost;
    currentBestSolution = globalBest;
    // cout << "Current " << globalBest << endl;
    vector<facility> fctTemp = fct;
    vector<int> openTemp = isOpen;


    while (T_begin > T_end) {
        vector<facility> oldTemp = fct;
        vector<int> oldOpen = isOpen;
        vector<int> serveTemp = serveList;
        for (int i = 0; i < iterator; i++) {
            // According to serveList.

            fctTemp = fct;
            vector<int> newSolution = createNewSolution(fctTemp); // A new serveList.
            
            for (int i = 0; i < fctTemp.size(); i++) {
                openTemp[i] = (fctTemp[i].volumn == fctTemp[i].Max) ? 0 : 1;
            }

            int newSolutionLength = getCurrentCost(newSolution, openTemp);
            int delta = newSolutionLength - currentBestSolution;

            // Annealing.
            if (delta >= 0) {
                double r = (double)rand() / (RAND_MAX);
                // Keep the original solution.
                if (exp(-delta / T_begin) <= r) {
                    // Accept the bad solution.
                    serveList = newSolution;
                    fct = fctTemp;
                    isOpen = openTemp;
                    currentBestSolution = newSolutionLength;
                }
            }
            // Else Accept the solution.
            else {
                serveList = newSolution;
                fct = fctTemp;
                isOpen = openTemp;
                currentBestSolution = newSolutionLength;
            }
        }
        
        if (globalBest > currentBestSolution) {
            globalBest = currentBestSolution;
            fct = oldTemp;
            isOpen = oldOpen;
            serveList = serveTemp;
        }

        // cout << T_begin << " 当前全局最优结果为: " << globalBest << endl;

        // Update the temperature.
        T_begin *= q_rate;
    }

    // return globalBest;
    return globalBest;
}

int CFLP::getCurrentCost(vector<int> serve, vector<int> open) {
    int currentCost = 0;
    
    // Open cost.
    for (int i = 0; i < facNum; i++) {
        currentCost += fct[i].cost * open[i];
    }

    // Assignement cost.
    for (int i = 0; i < cosNum; i++) {
        if (serve[i] != -1) {
            currentCost += csm[i].assignment_cost[serve[i]];
        }
    }

    return currentCost;
}

int CFLP::initSolution() {
    isOpen = vector<int>(facNum, 0);
    serveList = vector<int>(cosNum, -1);
    for (int i = 0; i < cosNum; i++) {
        for (int j = 0; j < facNum; j++) {
            if (fct[j].volumn > csm[i].demand) {
                fct[j].volumn -= csm[i].demand;
                serveList[i] = j;
                isOpen[j] = 1;
                break;
            }
        }
    }
    int answer = getCurrentCost(serveList, isOpen);
    return answer;
}

vector<int> CFLP::createNewSolution(vector<facility>& fctTemp) {
    vector<int> newList = serveList;
    double p = ((double)rand()) / (RAND_MAX+1.0);

    // Case1: Randomly selecting one position and find the best serve.
    if (p <= 0.7) {
        int cosIndex = random(0, cosNum);
        //int newServe = random(0, facNum);


        // New serve should have enough space
        /*
        while ((newServe == serveList[cosIndex]) || (fctTemp[newServe].volumn < csm[cosIndex].demand)) {
            // If do not have enough space.
            // create a new index.
            newServe = random(0, facNum);
        }*/

        // Find the best new serve.
        int newServe = serveList[cosIndex];
        int minCost = currentBestSolution;
        for (int i = 0; i < facNum; i++) {
            // The i-th facility.
            // If the facility has enough space and not currently facility.
            if (serveList[cosIndex] != i && fctTemp[i].volumn >= csm[cosIndex].demand) {
                // Check whether current serve facility is close after csm[cosIndex] is not assigned.
                int currentOpen = (fctTemp[serveList[cosIndex]].volumn+csm[cosIndex].demand == fctTemp[serveList[cosIndex]].Max) ? -1 : 0;
                int newCost = minCost 
                            - csm[cosIndex].assignment_cost[serveList[cosIndex]] 
                            + csm[cosIndex].assignment_cost[i] 
                            + currentOpen * fctTemp[serveList[cosIndex]].cost;
                if (newCost < minCost) {
                    // Accept the new solution.
                    minCost = newCost;
                    newServe = i;
                }
            }
        }

        // Update message.
        if (newServe != serveList[cosIndex]) {
            fctTemp[newServe].volumn = fctTemp[newServe].volumn - csm[cosIndex].demand;
            fctTemp[serveList[cosIndex]].volumn = fctTemp[serveList[cosIndex]].volumn + csm[cosIndex].demand;
            newList[cosIndex] = newServe;
        }

    }

    // Case2: swap.
    else {
        // Random select two costomer position.
        int cosNumOne = random(0, cosNum);
        int cosNumTwo = random(0, cosNum);

        // Prevent the two position to be same.
        // Comfiren that the two position have enough space.
        while ((cosNumOne == cosNumTwo)
        || (fctTemp[serveList[cosNumOne]].volumn + csm[cosNumOne].demand < csm[cosNumTwo].demand)
        || (fctTemp[serveList[cosNumTwo]].volumn + csm[cosNumTwo].demand < csm[cosNumOne].demand)) {
            cosNumOne = random(0, cosNum);
            cosNumTwo = random(0, cosNum);
        }
        fctTemp[serveList[cosNumOne]].volumn = fctTemp[serveList[cosNumOne]].volumn + csm[cosNumOne].demand - csm[cosNumTwo].demand;
        fctTemp[serveList[cosNumTwo]].volumn = fctTemp[serveList[cosNumTwo]].volumn + csm[cosNumTwo].demand - csm[cosNumOne].demand;

        newList[cosNumOne] = serveList[cosNumTwo];
        newList[cosNumTwo] = serveList[cosNumOne];
    }

    // Case3
    /*
    else {
        // Select a facility randomly.
        int facIndex = random(0, facNum);
        int cosIndexOne = random(0, cosNum);
        int cosIndexTwo = random(0, cosNum);
        
        // Find two cosIndex that can merge
        while ((cosIndexOne == cosIndexTwo)
        || (facIndex == serveList[cosIndexOne])
        || (facIndex == serveList[cosIndexTwo])
        || (fctTemp[facIndex].volumn < (csm[cosIndexOne].demand+csm[cosIndexTwo].demand))) {
            // Comfirm have enough space.
            facIndex = random(0, facNum);
            cosIndexOne = random(0, cosNum);
            cosIndexTwo = random(0, cosNum);
        }

        fctTemp[facIndex].volumn -= (csm[cosIndexOne].demand + csm[cosIndexTwo].demand);
        fctTemp[serveList[cosIndexOne]].volumn += csm[cosIndexOne].demand;
        fctTemp[serveList[cosIndexTwo]].volumn += csm[cosIndexTwo].demand;
        newList[cosIndexOne] = facIndex;
        newList[cosIndexTwo] = facIndex;
    }*/

    return newList;
}

