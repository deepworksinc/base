//
//  HRMModel.swift
//

import Foundation

struct HeartState: Codable {
    let bpm: Int
    let rr: Int
    let df: Float
}

struct HeartStateMessage: Identifiable {
    let id = UUID()
    let value: HeartState
}
