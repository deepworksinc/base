//
//  DetrendedFluctuation.cpp
//

#include "DetrendedFluctuation.hpp"
#include <algorithm>
#include <cmath>
#include <deque>
#include <iostream>
#include <numeric>
#include <vector>
#include <Eigen/Dense>

using std::accumulate;
using std::cout;
using std::deque;
using std::transform;
using std::vector;
using Eigen::MatrixXd;

// DF service state data
bool initialized = false;
vector<float> rr_history;
deque<float> rr_buffer;

// DF algorithm constants
const int max_buffer_size = 120;
const float artifact_correction_threshold = 0.05;
const int lower_scale_limit = 4;
const int upper_scale_limit = 16;
const int scale_density = 30;
const int m = 1; // Polynomial order m
const float start = log(lower_scale_limit) / log(10);
const float stop = log(upper_scale_limit) / log(10);
vector<float> scales;
vector<int> F;
int count = 0;

float DetrendedFluctuation::push(vector<int>& rrs) {
    // Loop over RR values and update buffer
    for (int i = 0; i < rrs.size(); ++i) {
        
        // Artifact Correction Threshold 0.05
        const int last_rr = rr_buffer[rr_buffer.size() - 1];
        const int new_rr = rrs[i];
        const float lower_correction_threshold = last_rr * (1 - artifact_correction_threshold);
        const float upper_correction_threshold = last_rr * (1 + artifact_correction_threshold);
        if (lower_correction_threshold < new_rr < upper_correction_threshold) {
            rr_history.push_back(new_rr);
            rr_buffer.push_back(new_rr);
        };

        // Remove oldest RR value from buffer
        if (rr_buffer.size() > max_buffer_size) {
            rr_buffer.pop_front();
        }
    }
    
    float df = _compute();
    
    return df;
}

float DetrendedFluctuation::_compute() {
    if (!initialized) {
        _initialize();
    }
    
    for (float s: scales) {
        vector<float> rms;
        
        // Step 1: Determine the "profile" (integrated signal with subtracted offset)
        float L = rr_buffer.size();
        float mean = accumulate(rr_buffer.begin(), rr_buffer.end(), 0) / L;
        vector<float> rrs_sub_mean(L);
        for (int i = 0; i < L; ++i) {
            rrs_sub_mean[i] = rr_buffer[i] - mean;
        }
        vector<int> y_n(rrs_sub_mean.size());
        partial_sum(rrs_sub_mean.begin(), rrs_sub_mean.end(), y_n.begin());
            
        // Step 2: Divide the profile into N non-overlapping segments of equal length s
        int first = static_cast<int>(s);
        int second = static_cast<int>(floor(L / s));
        vector<int> shape = { first, second };
        int nwSize = shape[0] * shape[1];
        
        // Todo polyfit!
        
        // https://eigen.tuxfamily.org/dox-devel/group__TutorialReshape.html
        // beginning to end, here we reshape so that we have a number of segments based on the scale used at this cycle
        // Y_n1 = np.reshape(y_n[0:nwSize], shape, order="F")
        // Y_n1 = Y_n1.T
        // end to beginning
        // Y_n2 = np.reshape(y_n[len(y_n) - (nwSize):len(y_n)], shape, order="F")
        // Y_n2 = Y_n2.T
        // concatenate
        // Y_n = np.vstack((Y_n1, Y_n2))

        // Step 3: Calculate the local trend for each 2Ns segments by a least squares fit of the series
        // for cut in np.arange(0, 2 * shape[1]):
            // xcut = np.arange(0, shape[0])
            // pl = np.polyfit(xcut, Y_n[cut,:], m)
            // Yfit = np.polyval(pl, xcut)
            // arr = Yfit - Y_n[cut,:]
            // rms.append(np.sqrt(np.mean(arr * arr)))

        // if (len(rms) > 0):
            // F[count] = np.power((1 / (shape[1] * 2)) * np.sum(np.power(rms, 2)), 1/2)
        // count = count + 1
    }
    
    // pl2 = np.polyfit(np.log2(scales), np.log2(F), 1)
    // alpha = pl2[0]
    // return alpha
    
    return 1.0;
}

void DetrendedFluctuation::_initialize() {
    scales = _scales(log10(pow(10, start)), log10(pow(10, stop)), scale_density);
    F.resize(scales.size());
    initialized = true;
}

vector<float> DetrendedFluctuation::_scales(double a, double b, int k) {
    const auto exp_scale = (b - a) / (k - 1);
    vector<double> logspace;
    logspace.reserve(k);
    for (int i = 0; i < k; i++) {
        logspace.push_back(i * exp_scale);
    }
    vector<float> scales;
    for_each(logspace.begin(), logspace.end(), [&scales](double &x) {
        scales.push_back(floor(pow(10, x)));
    });
    return scales;
}
