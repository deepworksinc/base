//
//  DetrendedFluctuation.cpp
//

// Todo move shared service in monorepo root (artifact/services/detrended-fluctuation)

#include "DetrendedFluctuation.hpp"
#include <vector>
#include <numeric>

using std::vector;
using std::accumulate;

float DetrendedFluctuation::computeDF(vector<int>& rr) {
    return float(accumulate(rr.begin(), rr.end(), 0) / rr.size());
}
