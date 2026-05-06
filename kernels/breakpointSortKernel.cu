#include "breakpointSortKernel.cuh"
#include <cuda_runtime.h>

using namespace std;

__global__ void evalReversals(int* perm, int* results, int n)
{
    // printf("Hello from GPU evaluate reversals kernel!\n");

    int globalTID = blockIdx.x * blockDim.x + threadIdx.x;
    int i = globalTID / n;
    int j = globalTID % n;

    // skip pairs where the right/left index is in the wrong position or exceeds n
    if (i >= n || j >= n || j <= i) {
        results[globalTID] = INT_MAX;
        return;
    }
    
    // make a local copy of the permutation first, so we can test a candidate reversal
    int local[1024];
    for (int y = 0; y < n; y++) {
        local[y] = perm[y];
    } 

    // apply the reversal to the local copy of the permutation
    int left = i, right = j;
    while (left < right) {
        int temp = local[left];
        local[left] = local[right];
        local[right] = temp;

        // after doing the reversal, move the boundary checks
        left++; 
        right--;
    }

    // then count the breakpoints in the local copy and write the results
    // into the results buffer, which is used back into the while loop
    int bps = 0;
    for (int y = 0; y < n - 1; y++) {
        if (abs(local[y+1] - local[y]) != 1) { bps+= 1; }
    }

    results[globalTID] = bps; 
}

__global__ void reductionKernel(int* results, int* gpuBestIdx, int totalThreadCount)
{
    // printf("Hello from GPU reduction kernel!\n");

    // this kernel takes the result array from the previous kernel and finds the index
    // which has the lowest breakpoints resulting after candidate reversals

    if (threadIdx.x != 0 || blockIdx.x != 0) { return; }

    int minVal = INT_MAX;
    int minIdx = 0;

    // Just finding the min
    for (int i = 0; i < totalThreadCount; i++) {
        if (results[i] < minVal) {
            minVal = results[i];
            minIdx = i;
        }
    }
    // Copy the result back into the gpuBestIdx array's first space
    gpuBestIdx[0] = minIdx;
}

// same breakpoint reversal sort algorithm but this time we are calling the GPU kernel
double bpReversalSortKernel(vector<int>& perm)
{
    // init declarations
    int n = perm.size();
    int* cpuPerm = perm.data();
    int* gpuPerm;
    int* gpuResult;
    int* gpuBestIdx;

    // Allocating memory to GPU buffers & copying data from CPU
    cudaMalloc(&gpuPerm, n * sizeof(int));
    cudaMalloc(&gpuResult, n * n * sizeof(int));
    cudaMalloc(&gpuBestIdx, sizeof(int));    

    // timing setup using CUDA built-in timing
    cudaEvent_t s, e; // start and end
    cudaEventCreate(&s);
    cudaEventCreate(&e);
    cudaEventRecord(s); // start timing

    // need to initially get the dataa in the buffer before the loop begins
    cudaMemcpy(gpuPerm, cpuPerm, n * sizeof(int), cudaMemcpyHostToDevice);

    // Kernel launch items, threads and blocks (groups of threads) to launch
    int totalThreads = n * n;
    int blocksize = 256;
    int gridSize = (totalThreads + blocksize - 1) / blocksize;

    // The overall loop logic is basically the same here for the breakpoint reversal sort. But
    // now the kernel call takes place inside of the inner candidate evaluation loops

    int maxIter = n * n;  // upper bound on reversals needed
    int iter = 0;
    int reversals = 0;

    while (countBreakpoints(perm) > 0) {

        // Debug to make sure the loop isn't getting into infinite or very high iteration territory
        if (iter++ > maxIter) {
            cout << "WARNING: max iterations hit (GPU)" << endl;
            break;
        }

        // if-else branches are essentially the same as the CPU version of the algorithm, except
        // here the if branch calls the GPU to do candidate reversals instead of the O(n^2) double
        // for loop strategy of the original sequential algorithm
        if (checkDecStrip(perm)) {

            // Call the GPU kernel to do all candidate reversals in the brute-force method, and fill
            // the gpuResult array
            evalReversals<<<gridSize, blocksize>>>(gpuPerm, gpuResult, n);
            cudaDeviceSynchronize();

            // Do the reduction to
            reductionKernel<<<gridSize, blocksize>>>(gpuResult, gpuBestIdx, totalThreads);
            cudaDeviceSynchronize();

            // Recopy the index from the candidate reversal that had the lowest resulting breakpoints
            int cpuIdxCpy;
            cudaMemcpy(&cpuIdxCpy, gpuBestIdx, sizeof(int), cudaMemcpyDeviceToHost);

            // Do the reversal for real by recomputing the bounds from the matching index to the thread
            // on the GPU that had calculated this reversal
            int bestLeftBp = cpuIdxCpy / n;
            int bestRightBp = cpuIdxCpy % n;
            reverseSegment(perm, bestLeftBp, bestRightBp);
            reversals += 1;
        }
        else {
            // CPU branch: find increasing strip and flip it
            int i = -1, j = -1;
            for (int y = 0; y < n - 1; y++) {
                if (perm[y+1] - perm[y] == 1) {
                    i = y; j = y + 1;
                    while (j < n - 1 && perm[j+1] - perm[j] == 1)
                        j++;
                    break;
                }
            }
            if (i != -1) reverseSegment(perm, i, j);
        }

        // sync updated perm to device
        cudaMemcpy(gpuPerm, cpuPerm, n * sizeof(int), cudaMemcpyHostToDevice);
    }

    // stopping the timing and recording the seconds elapsed on this run
    cudaEventRecord(e);
    cudaEventSynchronize(e);
    float ms;
    cudaEventElapsedTime(&ms, s, e);

    // need to convert to seconds here again
    //cout << "GPU BP Sort time: " << ms / 1000 << "s" << endl;

    // cuda required manually freeing the memory allocated for the kernel
    cudaFree(gpuPerm);
    cudaFree(gpuResult);
    cudaFree(gpuBestIdx);
    //cout << ">> Reversals: " << reversals << " - ";
        

    return ms / 1000;
}