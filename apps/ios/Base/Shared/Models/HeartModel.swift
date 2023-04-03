//
//  HRMModel.swift
//

import Foundation

struct HeartState: Codable {
    // Beats per minute
    let bpm: Int
    // Detrended fluctuation alpha 1
    let dfa1: Float
    // Root mean square of successive differences
    let rmssd: Float
}

struct HeartStateMessage: Identifiable {
    let id = UUID()
    let value: HeartState
}
