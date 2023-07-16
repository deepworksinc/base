import numpy as np
import pandas as pd
import math


def DFA(pp_values, lower_scale_limit, upper_scale_limit):
    scaleDensity = 30  # scales DFA is conducted between lower_scale_limit and upper_scale_limit
    # order of polynomial fit (linear = 1, quadratic m = 2, cubic m = 3, etc...)
    m = 1

    # initialize, we use logarithmic scales
    start = np.log(lower_scale_limit) / np.log(10)
    stop = np.log(upper_scale_limit) / np.log(10)

    scales = np.floor(np.logspace(np.log10(math.pow(10, start)),
                      np.log10(math.pow(10, stop)), scaleDensity))
    
    F = np.zeros(len(scales))
    count = 0

    for s in scales:
        rms = []
        # Step 1: Determine the "profile" (integrated signal with subtracted offset)
        x = pp_values
        y_n = np.cumsum(x - np.mean(x))

        # Step 2: Divide the profile into N non-overlapping segments of equal length s
        L = len(x)
        shape = [int(s), int(np.floor(L/s))]
        nwSize = int(shape[0]) * int(shape[1])

        # beginning to end, here we reshape so that we have a number of segments based on the scale used at this cycle
        Y_n1 = np.reshape(y_n[0:nwSize], shape, order="F")
        Y_n1 = Y_n1.T
        # end to beginning
        Y_n2 = np.reshape(y_n[len(y_n) - (nwSize):len(y_n)], shape, order="F")
        Y_n2 = Y_n2.T
        # concatenate
        Y_n = np.vstack((Y_n1, Y_n2))

        # Step 3: Calculate the local trend for each 2Ns segments by a least squares fit of the series
        for cut in np.arange(0, 2 * shape[1]):
            xcut = np.arange(0, shape[0])
            pl = np.polyfit(xcut, Y_n[cut, :], m)
            Yfit = np.polyval(pl, xcut)
            arr = Yfit - Y_n[cut, :]
            rms.append(np.sqrt(np.mean(arr * arr)))

        if (len(rms) > 0):
            F[count] = np.power((1 / (shape[1] * 2)) *
                                np.sum(np.power(rms, 2)), 1/2)
        count = count + 1

    pl2 = np.polyfit(np.log2(scales), np.log2(F), 1)
    alpha1 = pl2[0]
    return alpha1


def computeFeatures(df):
    features = []
    step = 120
    for index in range(0, int(round(np.max(x)/step))):

        array_rr = df.loc[(df['timestamp'] >= (index*step))
                          & (df['timestamp'] <= (index+1)*step), 'RR']*1000
        
        timestamp = df.loc[(df['timestamp'] >= (index*step))
                   & (df['timestamp'] <= (index+1)*step), 'timestamp'].max()

        # compute heart rate
        heartrate = round(60000/np.mean(array_rr), 2)
        # compute rmssd
        NNdiff = np.abs(np.diff(array_rr))
        rmssd = round(np.sqrt(np.sum((NNdiff * NNdiff) / len(NNdiff))), 2)
        # compute sdnn
        sdnn = round(np.std(array_rr), 2)
        # dfa, alpha 1
        alpha1 = DFA(array_rr.to_list(), 4, 16)

        curr_features = {
            'timestamp': timestamp,
            'heartrate': heartrate,
            'rmssd': rmssd,
            'sdnn': sdnn,
            'alpha1': alpha1,
        }

        features.append(curr_features)

    features_df = pd.DataFrame(features)
    return features_df

logger_file = pd.read_csv('./mock/Ruck1.csv')
# here we keep only the RR intervals so that the rest of the code is the same regardless of what data you loaded
RRs = []
RRs = round(logger_file.iloc[:, 1]/1000, 15)  # same format as fit file
RRs = RRs.tolist()

artifact_correction_threshold = 0.05
filtered_RRs = []

for i in range(len(RRs)):

    if len(filtered_RRs) == 0:
        filtered_RRs.append(RRs[i])
        continue

    prev_rr = RRs[i-1]
    curr_rr = RRs[i]
    lower_threshold = round(prev_rr * (1 - artifact_correction_threshold), 15)
    upper_threshold = round(prev_rr * (1 + artifact_correction_threshold), 15)

    if lower_threshold < curr_rr < upper_threshold:
        filtered_RRs.append(curr_rr)

x = np.cumsum(filtered_RRs)

df = pd.DataFrame()
df['timestamp'] = x
df['RR'] = filtered_RRs

features_df = computeFeatures(df)

for index, row in features_df.iterrows():
    print(row['alpha1'])
