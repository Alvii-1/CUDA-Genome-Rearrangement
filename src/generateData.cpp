#include "generateData.h"

using namespace std;

vector<int> generateData(int n, mt19937& rng)
{
    vector<int> perm(n);

    // After creating the data vector we fill it in normal order
    // idx values are = idx + 1 (0 start array, so +1 offset)
    for (int i = 0; i < n; i++) {
        perm[i] = i + 1;
    }

    // Here we need to randomly shuffle it
    // After some research, I came to the conclusion that true random
    // shuffling (or close to it) is best done using Finisher-Yates approach

    // Continuously pick from decreasing vector range to shuffle
    // After placing an item, we don't touch it again
    for (int i = n - 1; i > 0; i--) {
        int j = rng() % (i + 1);
        swap(perm[i], perm[j]);
    }

    // Randomly flip signs using rng() which will generate a random integer
    // that we can check if even or odd. If even we flip sign, at a high
    // level it is essentially a coin toss to flip a sign or not
    // for (int i = 0; i < n; i++) {
    //     if (rng() % 2) { perm[i] = -perm[i]; }
    // }

    return perm;
}