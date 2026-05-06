#include "greedySortKernel.cuh"
#include <cuda_runtime.h>

using namespace std;

__global__ void findPosition(int* perm, int* result, int target, int n) 
{
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid >= n) return;
    if (abs(perm[tid]) == target)
        result[0] = tid;
}

double greedyReversalSortKernel(vector<int>& perm) 
{
    int n = perm.size();
    int* cpuPerm = perm.data();
    int* gpuPerm;
    int* gpuResult;
    cudaMalloc(&gpuPerm, n * sizeof(int));
    cudaMalloc(&gpuResult, sizeof(int));
    

    cudaEvent_t s, e;
    cudaEventCreate(&s);
    cudaEventCreate(&e);
    cudaEventRecord(s);

    cudaMemcpy(gpuPerm, cpuPerm, n * sizeof(int), cudaMemcpyHostToDevice);

    int blockSize = 256;
    int gridSize = (n + blockSize - 1) / blockSize;
    int reversals = 0;

    for (int i = 0; i < n; i++) {
        findPosition<<<gridSize, blockSize>>>(gpuPerm, gpuResult, i + 1, n);
        cudaDeviceSynchronize();

        int j = i;
        cudaMemcpy(&j, gpuResult, sizeof(int), cudaMemcpyDeviceToHost);

        if (j != i) {
            reverseSegment(perm, i, j);
            reversals += 1;
            cudaMemcpy(gpuPerm, cpuPerm, n * sizeof(int), cudaMemcpyHostToDevice);
        }
    }

    cudaEventRecord(e);
    cudaEventSynchronize(e);
    float ms;
    cudaEventElapsedTime(&ms, s, e);

    cudaFree(gpuPerm);
    cudaFree(gpuResult);
    //cout << ">> Reversals: " << reversals << " - ";
    return ms / 1000;
}