//
//  DetrendedFluctuation.cpp
//

#include "DetrendedFluctuation.hpp"

#include <cmath>
#include <deque>
#include <iostream>
#include <numeric>
#include <vector>
#include <Eigen/Dense>

using std::accumulate;
using std::cout;
using std::deque;
using std::vector;

// Todo move logspace to math utils
std::vector<double> logspace(double a, double b, int k) {
  const auto exp_scale = (b - a) / (k - 1);
  std::vector<double> logspace;
  logspace.reserve(k);
  for (int i = 0; i < k; i++) {
    logspace.push_back(i * exp_scale);
  }
  std::for_each(logspace.begin(), logspace.end(),
                [](double &x) { x = pow(10, x); });
  return logspace;
}

// DF data
// vector<int> rr_history;
deque<int> rr_buffer;

// DF constants
const int max_buffer_size = 120;
const float artifact_correction_threshold = 0.05;
const int lower_scale_limit = 4;
const int upper_scale_limit = 16;
const int scale_density = 30;
const int polynomial_order = 1;
const float start = log(lower_scale_limit) / log(10);
const float stop = log(upper_scale_limit) / log(10);

// Todo get floor of each double in logspace response
// const vector<float> scales = floor(logspace(log10(pow(10, start)), log10(pow(10, stop)), scale_density));

float DetrendedFluctuation::push(vector<int>& rrs) {
    
    // Loop over RR values and update buffer
    for (int i = 0; i < rrs.size(); ++i) {
        
        // Artifact Correction Threshold 0.05
        // Use last rr value
        // last_rr = buffer[buffer.size() - 1];
        
        const int new_rr = rrs[i];
        
        // rr_history.push_back(new_rr);
        
        rr_buffer.push_back(new_rr);

        if (rr_buffer.size() > max_buffer_size) {
            rr_buffer.pop_front();
        }
    }
    
    // Calculate df and return
    // Todo use Eigen to compute least square polynomial
    
    return float(accumulate(rr_buffer.begin(), rr_buffer.end(), 0) / rr_buffer.size());
}
