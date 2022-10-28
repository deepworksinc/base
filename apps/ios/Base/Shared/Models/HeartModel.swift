//
//  HRMModel.swift
//

import Foundation

struct HeartState: Codable {
    // Beats per minute - the user's intensity
    let bpm: Int
    // Detrended fluctuation - the user's zone thresholds
    let df: Float
}

struct HeartStateMessage: Identifiable {
    let id = UUID()
    let value: HeartState
}
