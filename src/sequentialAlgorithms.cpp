#include "sequentialAlgorithms.h"

using namespace std;

// helper to print the permutation
void printPerm(vector<int>& perm)
{
    cout << "Sorted Result: ";
    for (int x: perm) {
        cout << x << " ";
    }
    cout << endl;
}

void reverseSegment(vector<int>& perm, int i, int j) 
{
    // We wan to stop when i == j, or the position and value match
    int left = i, right = j;
    while (left < right) {

        // Do a swap operation
        swap(perm[left], perm[right]);

        // Adjust swap bounds until your done
        left++;
        right--;
    }
}

double greedyReversalSort(vector<int>& perm)
{
    int n = perm.size();
    int reversals = 0;

    // timing here
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < n; i++) {
    
        // Get the position in perm of value i
        int j; // We will assign this below

        // Find where the position j of value i is in perm
        for (int y = i; y < n; y++) {

            // Since using signed ints, we check to find out where the abs value of 
            // the value i + 1 is. i + 1 is used because the array is 0-indexed
            if (abs(perm[y]) == i + 1) {
                j = y;
                break;
            }
        }
        
        // Apply the reversal if the position and value mismatch from identity
        if (j != i) { 
            reverseSegment(perm, i, j); 
            reversals += 1; 
        }
       
    }

    // Do a final sign swap on everything (if we verify its the identity matrix)
    // for (int i = 0; i < n; i++) {
    //     if (perm[i] < 0) { perm[i] = -perm[i]; }
    // }

    auto finish = chrono::high_resolution_clock::now();
    return chrono::duration<double>(finish - start).count();

    //cout << ">> Reversals: " << reversals << " - ";
}


int countBreakpoints(vector<int>& perm)
{
    int n = perm.size();
    int bps = 0;

    // loop over the  elements 
    for (int i = 0; i < n - 1; i++) {

        // if pair not adjecant, that is a breakpoint
        if (abs(perm[i+1] - perm[i]) != 1) bps++;
    }

    return bps;

}

bool checkDecStrip(vector<int>& perm)
{
    int n = perm.size();
    bool hasIncStrip = false;

    // we just need to find out if there is a strip thats decreasing
    for (int i = 0; i < n - 1; i++) {
        if (perm[i + 1] - perm[i] == -1) { return true; }
        if (perm[i + 1] - perm[i] == 1) { hasIncStrip = true; }
    }

    return !hasIncStrip;
}

// This is the "improved" version of the algorithm from lecture, we use this because
// the better greedy approach might end up running infinitely
double bpReversalSort(vector<int>& perm)
{   
    int n = perm.size();
    int reversals = 0;
    int maxIterations = n * n;  // upper bound on reversals needed
    int iter = 0;

    // timing here
    auto start = chrono::high_resolution_clock::now();

    // while we have breakpoints in the permutation
    while (countBreakpoints(perm) > 0) {

        // This was used in debugging to try and predict when outlier runtimes woudl occur
        if (iter++ > maxIterations) {
            cout << "WARNING: max iterations hit, possible infinite loop" << endl;
            break;
        }

        if (checkDecStrip(perm)) { 
            // choose reversal p that minimizes b(perm * p)

            int lowestBps = INT_MAX;    // need this to easily accept a lower value from comparison
            int bestLeftBp = -1;        // these vals will be the left and right index of the segment
            int bestRightBp = -1;       // that, when reversed, minimizes the breakpoints in this pass

            for (int i = 0; i < n; i++) {
                for (int j = i + 1; j < n; j++) {

                    // count the breakpoints done by all possible reversals
                    reverseSegment(perm, i, j);         // do the reversal
                    int bps = countBreakpoints(perm);   // count the breakpoints
                    reverseSegment(perm, i, j);         // undo the reversal

                    // then check if the new number of breakpoints is lower than the max
                    if (bps < lowestBps) {
                        lowestBps = bps;
                        bestLeftBp = i;
                        bestRightBp = j;
                    }
                }
            }
            //Then do the final reversal that will decrease bps the most
            if (bestLeftBp != -1) { 
                reverseSegment(perm, bestLeftBp, bestRightBp);
                reversals += 1; 
            }
        }
        else {
            // if no decreasing strips, choose reversal p that flips an increasing strip in perm

            int i = -1, j = -1; // left and right indexes of the strip, assigned below

            // we need to find where an increasing strip is, loop through the indexes
            // until we reach its end, and then flip it
            for (int y = 0; y < n - 1; y++) {
                if (perm[y + 1] - perm[y] == 1) {
                    i = y; j = y + 1; // baseline, we have established these adjcent pairs are increasing

                    // keep iterating until the end of this increasing strip, making sure to stay in bounds
                    // on the right index of the strip (j < n-1). we also increment j to keep moving the right
                    // index onwards until the loop condition breaks and we are at the end
                    while (j < n - 1 && perm[j + 1] - perm[j] == 1) {
                        j += 1;
                    }
                    break; // we have a strip, so just stop iterating the for loop here
                }
            }
            
            // if we've assigned i, then we know we can do a valid reversal
            if (i != -1) { reverseSegment(perm, i, j); }
        }
    }

    // Do a final sign swap on everything (if we verify its the identity matrix)
    // for (int i = 0; i < n; i++) {
    //     if (perm[i] < 0) { perm[i] = -perm[i]; }
    // }

    auto finish = chrono::high_resolution_clock::now();
    return chrono::duration<double>(finish - start).count();
    //cout << ">> Reversals: " << reversals << " - ";
}