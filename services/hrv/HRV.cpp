#include "HRV.hpp"
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
using Eigen::MatrixXf;

// Service state data
bool initialized = false;
vector<float> rr_history;
deque<float> rr_buffer;

// Algorithm constants
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

Features HRV::push(vector<uint8_t>& bytes) {

    // Decode bytes
    Data data = _decode(bytes);

    // Loop over RR values and update buffer
    for (int i = 0; i < data.rrs.size(); ++i) {
        
        // Artifact Correction Threshold 0.05
        int new_rr = data.rrs[i];
        if (rr_buffer.size()) {
            int last_rr = rr_buffer[rr_buffer.size() - 1];
            float lower_correction_threshold = last_rr * (1 - artifact_correction_threshold);
            float upper_correction_threshold = last_rr * (1 + artifact_correction_threshold);
            if (lower_correction_threshold < new_rr < upper_correction_threshold) {
                rr_history.push_back(new_rr);
                rr_buffer.push_back(new_rr);
            };
        } else {
            rr_history.push_back(new_rr);
            rr_buffer.push_back(new_rr);
        }

        // Remove oldest RR value from buffer
        if (rr_buffer.size() > max_buffer_size) {
            rr_buffer.pop_front();
        }
    }
    
    // float hrv_features = _compute();
    
    // return hrv_features;

    return {
        data.bpm,
        float(0.0),
        float(accumulate(rr_buffer.begin(), rr_buffer.end(), 0) / rr_buffer.size())
    };
}

float HRV::_compute() {
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
            
        // // Step 2: Divide the profile into N non-overlapping segments of equal length s
        // int N = floor(L / s);
        // vector<vector<float>> y_n_segments(N);
        // for (int i = 0; i < N; ++i) {
        //     y_n_segments[i] = vector<float>(s);
        //     for (int j = 0; j < s; ++j) {
        //         y_n_segments[i][j] = y_n[i * s + j];
        //     }
        // }
        
        // // Step 3: Calculate the local trend for each 2Ns segments by a least squares fit of the series
        // for (int i = 0; i < N; ++i) {
        //     MatrixXf X = MatrixXf::Zero(s, m + 1);
        //     for (int j = 0; j < s; ++j) {
        //         X(j, 0) = 1;
        //         X(j, 1) = j;
        //     }
        //     MatrixXf Y = MatrixXf::Zero(s, 1);
        //     for (int j = 0; j < s; ++j) {
        //         Y(j, 0) = y_n_segments[i][j];
        //     }
        //     MatrixXf X_transpose = X.transpose();
        //     MatrixXf X_transpose_X = X_transpose * X;
        //     MatrixXf X_transpose_X_inverse = X_transpose_X.inverse();
        //     MatrixXf X_transpose_X_inverse_X_transpose = X_transpose_X_inverse * X_transpose;
        //     MatrixXf beta = X_transpose_X_inverse_X_transpose * Y;
        //     MatrixXf Y_hat = X * beta;
        //     MatrixXf Y_hat_minus_Y = Y_hat - Y;
        //     MatrixXf Y_hat_minus_Y_transpose = Y_hat_minus_Y.transpose();
        //     MatrixXf Y_hat_minus_Y_transpose_Y_hat_minus_Y = Y_hat_minus_Y_transpose * Y_hat_minus_Y;
        //     float rms_i = sqrt(Y_hat_minus_Y_transpose_Y_hat_minus_Y(0, 0) / s);
        //     rms.push_back(rms_i);
        // }
        
        // // Step 4: Calculate the root mean square (RMS) of the detrended profile
        // float rms_mean = accumulate(rms.begin(), rms.end(), 0) / N;
        // F[count] = rms_mean;
        // count++;
    }

    return 1.0;
    
    // // Step 5: Calculate the scaling exponent alpha 1
    // vector<float> log_F;
    // for (int i = 0; i < F.size(); ++i) {
    //     log_F.push_back(log(F[i]));
    // }
    // vector<float> log_s;
    // for (int i = 0; i < scales.size(); ++i) {
    //     log_s.push_back(log(scales[i]));
    // }
    // float log_F_mean = accumulate(log_F.begin(), log_F.end(), 0) / log_F.size();
    // float log_s_mean = accumulate(log_s.begin(), log_s.end(), 0) / log_s.size();
    // float numerator = 0;
    // float denominator = 0;
    // for (int i = 0; i < log_F.size(); ++i) {
    //     numerator += (log_F[i] - log_F_mean) * (log_s[i] - log_s_mean);
    //     denominator += pow(log_s[i] - log_s_mean, 2);
    // }
    // return numerator / denominator;
}

void HRV::_initialize() {
    scales = _scales(log10(pow(10, start)), log10(pow(10, stop)), scale_density);
    F.resize(scales.size());
    initialized = true;
}

vector<float> HRV::_scales(double a, double b, int k) {
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

Data HRV::_decode(vector<uint8_t>& bytes) {
    int bpm = 0;
    int first_bit = bytes[0] & 1;
    if (first_bit == 0) {
        // BPM value Format is in the 2nd byte
        bpm = int(bytes[1]);
    } else {
        // Heart Rate Value Format is in the 2nd and 3rd bytes
        bpm = (int(bytes[1]) << 8) + int(bytes[2]);
    }
    vector<int> rrs;
    int fifth_bit = (bytes[0] & 16);
    if (fifth_bit > 0) {
        if (bytes[0] % 2 == 0 && bytes[0] >= 16 && bytes[0] <= 22) {
            //rr-value in [2] und [3]
            rrs.push_back(int(bytes[2]) + (int(bytes[3]) << 8));
        }
        if (bytes[0] % 2 != 0 && bytes[0] >= 17 && bytes[0] <= 23) {
            //rr-value in [3] und [4]
            rrs.push_back(int(bytes[3]) + (int(bytes[4]) << 8));
        }
        if (bytes[0] % 2 == 0 && bytes[0] >= 24 && bytes[0] <= 30) {
            //rr-value in [4] und [5]
            rrs.push_back(int(bytes[4]) + (int(bytes[5]) << 8));
        }
        if (bytes[0] % 2 != 0 && bytes[0] >= 25 && bytes[0] <= 31) {
            //rr-value in [5] und [6]
            rrs.push_back(int(bytes[5]) + (int(bytes[6]) << 8));
        }
    }
    return Data{bpm, rrs};
}

