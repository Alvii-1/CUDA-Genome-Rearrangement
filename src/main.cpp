/*
Zohaib Alvi
CS 502 Computation Biology Algorithms 
GPU Acccelerated Analysis of Genome Rearrangement Algorithms
Spring 2026

Hardware setup (Windows 11 Desktop PC)
---
CPU: Ryzen 5 9600X
GPU: RTX 4070 Desktop  
*/

#include <iostream>
#include <algorithm>
#include <random>
#include <vector>

#include "generateData.h"
#include "sequentialAlgorithms.h"
#include "breakpointSortKernel.cuh"
#include "greedySortKernel.cuh"

using namespace std;

void runBenchmark(const string& label, int trialCount, mt19937 rng, vector<int>& testSizes, double(*sortFn)(vector<int>&))
{
    cout << endl << label << " Runtime Data: " << endl << "---" << endl;

    // Loop for the test sizes 
    for (int i : testSizes) {
        double total = 0;

        // Loop for the number of trials
        for (int trial = 0; trial < trialCount; trial++) {
            // Generate the data, call the function and return the total for averaging below
            vector<int> perm = generateData(i, rng);
            total += sortFn(perm);
        }
        cout << ">> Size: " << i << " - Avg Time: " << total / trialCount << " s" << endl;
    }
}

int main()
{
    // random number seed, reset for algorithm test
    mt19937 rng1(1), rng2(1), rng3(1), rng4(1); 

    // Test settings
    int trialCount = 1;
    bool runGreedyCPU = 1,
         runGreedyGPU = 1,
         runBpCPU = 1,
         runBpGPU = 1;
    
    
    // Generate permutations of varying sizes for the 2 algorithm tests
    vector<int> greedyTestSizes = {100, 1000, 10000, 100000}, 
                bpTestSizes = {25, 50, 100, 200};

    cout << endl << "GPU Acccelerated Analysis of Genome Rearrangement Algorithms (Zohaib Alvi)" 
         << endl << "Running " << trialCount << " trial(s) for each test..." << endl;

    // call the test based on the flags above
    if (runGreedyCPU) runBenchmark("Greedy Sort (CPU)", trialCount, rng1, greedyTestSizes, greedyReversalSort);
    if (runGreedyGPU) runBenchmark("Greedy Sort (GPU)", trialCount, rng2, greedyTestSizes, greedyReversalSortKernel);
    if (runBpCPU) runBenchmark("Breakpoint Sort (CPU)", trialCount, rng3, bpTestSizes, bpReversalSort);
    if (runBpGPU) runBenchmark("Breakpoint (GPU)", trialCount, rng4, bpTestSizes, bpReversalSortKernel);

    return 0;
}  