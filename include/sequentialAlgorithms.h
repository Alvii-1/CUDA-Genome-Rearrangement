#pragma once
#include <iostream>
#include <vector>
#include <chrono>

void printPerm(std::vector<int>& perm);
void reverseSegment(std::vector<int>& perm, int i, int j);
double greedyReversalSort(std::vector<int>& perm);
int countBreakpoints(std::vector<int>& perm);
bool checkDecStrip(std::vector<int>& perm);
double bpReversalSort(std::vector<int>& perm);