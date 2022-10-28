//
//  DetrendedFluctuation.cpp
//

// Todo move shared service in monorepo root (base/services/detrended-fluctuation)

#include "DetrendedFluctuation.hpp"
#include <deque>
#include <iostream>
#include <numeric>
#include <vector>

using std::accumulate;
using std::deque;
using std::vector;

const int max_buffer_size = 300;
deque<int> buffer;

float DetrendedFluctuation::compute(vector<int>& rrs) {
    for (int i = 0; i < rrs.size(); ++i) {
        // Todo test if multiple values are captured (test at very high HR)
        // Artifact Correction Threshold 0.05
        // Use last rr value
        // last_rr = buffer[buffer.size() - 1];
        buffer.push_back(rrs[i]);
        if (buffer.size() > max_buffer_size) {
            buffer.pop_front();
        }
    }
    return float(accumulate(buffer.begin(), buffer.end(), 0) / buffer.size());
}
