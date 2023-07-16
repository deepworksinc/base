#include <gsl/gsl_multifit.h>

#include <Eigen/Dense>
#include <algorithm>
#include <boost/algorithm/string/trim.hpp>
#include <boost/date_time.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

struct Features {
    double timestamp;
    double heartrate;
    double rmssd;
    double sdnn;
    double alpha1;
};

Eigen::VectorXd LogSpaced(int num, double start, double end) {
    Eigen::ArrayXd base = Eigen::VectorXd::LinSpaced(num, start, end).array();
    return (base * std::log(10)).exp().matrix();
}

std::vector<std::string> Split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

Eigen::VectorXd Cumsum(const Eigen::VectorXd &vec) {
    Eigen::VectorXd cumsum(vec.size());
    double sum = 0.0;
    for (int i = 0; i < vec.size(); ++i) {
        sum += vec(i);
        cumsum(i) = sum;
    }
    return cumsum;
}

Eigen::VectorXd PolyFit(const Eigen::VectorXd &x, const Eigen::VectorXd &y, int degree) {
    int n = x.size();
    Eigen::VectorXd coeffs(degree + 1);
    gsl_multifit_linear_workspace *ws = gsl_multifit_linear_alloc(n, degree + 1);
    gsl_matrix *X = gsl_matrix_alloc(n, degree + 1);
    gsl_vector *Y = gsl_vector_alloc(n);
    gsl_vector *C = gsl_vector_alloc(degree + 1);
    gsl_matrix *Cov = gsl_matrix_alloc(degree + 1, degree + 1);
    double chisq;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= degree; j++) {
            gsl_matrix_set(X, i, j, std::pow(x[i], j));
        }
        gsl_vector_set(Y, i, y[i]);
    }

    gsl_multifit_linear(X, Y, C, Cov, &chisq, ws);

    for (int i = 0; i <= degree; i++) {
        coeffs[degree - i] = gsl_vector_get(C, i);
    }

    gsl_multifit_linear_free(ws);
    gsl_matrix_free(X);
    gsl_vector_free(Y);
    gsl_vector_free(C);
    gsl_matrix_free(Cov);

    return coeffs;
}

Eigen::VectorXd PolyVal(const Eigen::VectorXd &p, const Eigen::VectorXd &x) {
    int degree = p.size() - 1;
    int n = x.size();
    Eigen::VectorXd y(n);
    for (int i = 0; i < n; i++) {
        y[i] = 0;
        for (int j = 0; j <= degree; j++) {
            y[i] += p[degree - j] * std::pow(x[i], j);
        }
    }
    return y;
}

double Dfa(const Eigen::VectorXd &pp_values, double lower_scale_limit, double upper_scale_limit) {
    const int kScaleDensity = 30;
    const int kM = 1;

    double start = std::log10(lower_scale_limit);
    double stop = std::log10(upper_scale_limit);

    Eigen::VectorXd scales = LogSpaced(kScaleDensity, start, stop);
    scales = scales.unaryExpr([](double x) { return std::floor(x); });

    Eigen::VectorXd F = Eigen::VectorXd::Zero(scales.size());
    int count = 0;

    for (int i = 0; i < scales.size(); ++i) {
        double s = scales[i];
        std::vector<double> rms;

        Eigen::VectorXd original_data = pp_values;
        Eigen::VectorXd cumsum_data = Cumsum(original_data.array() - original_data.mean());

        int data_length = original_data.size();
        int shape[2] = {static_cast<int>(s), static_cast<int>(data_length / s)};

        Eigen::MatrixXd cumsum_data_matrix = Eigen::Map<Eigen::MatrixXd>(cumsum_data.data(), shape[0], shape[1]);
        Eigen::MatrixXd offset_data_matrix = Eigen::Map<Eigen::MatrixXd>(cumsum_data.data() + data_length - static_cast<int>(s) * shape[1], shape[0], shape[1]);

        Eigen::MatrixXd transposed_data_matrix = cumsum_data_matrix.transpose();
        Eigen::MatrixXd reversed_data_matrix = transposed_data_matrix.colwise().reverse().eval();

        Eigen::MatrixXd combined_matrix(cumsum_data_matrix.rows(), cumsum_data_matrix.cols() + offset_data_matrix.cols());
        combined_matrix << offset_data_matrix.colwise().reverse().eval(), reversed_data_matrix.transpose();

        if (combined_matrix.rows() <= reversed_data_matrix.rows() && combined_matrix.cols() <= reversed_data_matrix.cols()) {
            reversed_data_matrix.block(0, 0, combined_matrix.rows(), combined_matrix.cols()) = combined_matrix;
        }

        for (int cut = 0; cut < 2 * shape[1]; ++cut) {
            Eigen::VectorXd xcut = Eigen::VectorXd::LinSpaced(shape[0], 0, shape[0] - 1);
            Eigen::VectorXd pl = PolyFit(xcut, reversed_data_matrix.row(cut / 2), kM);
            Eigen::VectorXd y_fit = PolyVal(pl, xcut);
            Eigen::VectorXd arr = y_fit.transpose() - reversed_data_matrix.row(cut / 2);
            rms.push_back(std::sqrt(arr.array().square().mean()));
        }

        if (!rms.empty()) {
            F[count] = std::sqrt(std::transform_reduce(rms.begin(), rms.end(), 0.0, std::plus<>(), [](double x) { return x * x; }) / (2 * shape[1]));
        }
        ++count;
    }

    double alpha1 = 0.0;
    if (scales.size() > 1) {
        Eigen::VectorXd log_scales = scales.array().unaryExpr([](double x) { return std::log2(x); });
        Eigen::VectorXd log_F = F.array().unaryExpr([](double x) { return std::log2(x); });

        Eigen::MatrixXd X(scales.size(), 2);
        X.col(0) = Eigen::VectorXd::Ones(scales.size());
        X.col(1) = log_scales;

        Eigen::VectorXd coefficients = X.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(log_F);
        alpha1 = coefficients(1);
    }

    return alpha1;
}

Features ComputeFeatures(const Eigen::MatrixXd &data) {
    Eigen::VectorXd array_rr = data.col(1) * 1000;
    double timestamp = data(data.rows() - 1, 0);

    double heartrate = std::round(60000 / array_rr.mean() * 100) / 100;

    double rmssd = 0;
    if (array_rr.size() > 1) {
        Eigen::VectorXd NNdiff = array_rr.tail(array_rr.size() - 1) - array_rr.head(array_rr.size() - 1);
        rmssd = std::round(std::sqrt(NNdiff.array().square().mean()) * 100) / 100;
    }

    double mean = array_rr.mean();
    double variance = (array_rr.array() - mean).square().mean();
    double sdnn = std::round(std::sqrt(variance) * 100) / 100;

    double alpha1 = 0;
    if (array_rr.size() > 1) {
        alpha1 = Dfa(array_rr, 4, 16);
    }

    return Features {timestamp, heartrate, rmssd, sdnn, alpha1};
}

Eigen::MatrixXd ReadCsv(const std::string &filename) {
    std::ifstream file(filename);
    std::vector<double> timestamps;
    std::vector<double> rrs;
    std::string line;
    bool skip_header = true;
    double cumulative_time = 0;

    while (std::getline(file, line, '\r')) {
        std::vector<std::string> columns = Split(line, ',');

        if (columns.size() == 3) {
            if (skip_header) {
                skip_header = false;
                continue;
            }

            double rr = std::stod(columns[1]) / 1000;
            rrs.push_back(rr);
            cumulative_time += rr;
            timestamps.push_back(cumulative_time);
        }
    }

    Eigen::MatrixXd data(timestamps.size(), 2);
    for (int i = 0; i < timestamps.size(); ++i) {
        data(i, 0) = timestamps[i];
        data(i, 1) = rrs[i];
    }

    return data;
}

Eigen::MatrixXd CorrectArtifacts(const Eigen::MatrixXd &data, double artifact_correction_threshold) {
    Eigen::VectorXd timestamps = data.col(0);
    Eigen::VectorXd rrs = data.col(1);

    std::vector<double> corrected_timestamps;
    std::vector<double> corrected_rrs;

    double cumulative_time = timestamps[0];
    double prev_rr = rrs[0];

    corrected_timestamps.push_back(cumulative_time);
    corrected_rrs.push_back(prev_rr);

    for (int i = 1; i < rrs.size(); ++i) {
        double lower_threshold = prev_rr * (1 - artifact_correction_threshold);
        double upper_threshold = prev_rr * (1 + artifact_correction_threshold);

        double current_rr = rrs[i];

        if (current_rr > lower_threshold && current_rr < upper_threshold) {
            cumulative_time += current_rr;
            corrected_timestamps.push_back(cumulative_time);
            corrected_rrs.push_back(current_rr);
        }
        prev_rr = current_rr;
    }

    Eigen::MatrixXd corrected_data(corrected_timestamps.size(), 2);
    for (int i = 0; i < corrected_timestamps.size(); ++i) {
        corrected_data(i, 0) = corrected_timestamps[i];
        corrected_data(i, 1) = corrected_rrs[i];
    }

    return corrected_data;
}

int main() {
    int step = 120;
    Eigen::MatrixXd data = ReadCsv("./mock/Ruck1.csv");
    Eigen::MatrixXd corrected_data = CorrectArtifacts(data, 0.05);

    std::vector<Features> features_history;

    for (int index = 0; index < std::round(corrected_data.col(0).maxCoeff() / step); ++index) {
        std::vector<double> timestamp_values;
        std::vector<double> rr_values;
        
        for (int i = 0; i < corrected_data.rows(); ++i) {
            if (corrected_data(i, 0) >= index * step && corrected_data(i, 0) < (index + 1) * step) {
                timestamp_values.push_back(corrected_data(i, 0));
                rr_values.push_back(corrected_data(i, 1));
            }
        }

        Eigen::MatrixXd step_data(timestamp_values.size(), 2);
        for (int i = 0; i < timestamp_values.size(); ++i) {
            step_data(i, 0) = timestamp_values[i];
            step_data(i, 1) = rr_values[i];
        }

        if (step_data.rows() > 0) {
            features_history.push_back(ComputeFeatures(step_data));
        }
    }

    for (const auto &features : features_history) {
        std::cout << features.alpha1 << std::endl;
    }

    return 0;
}