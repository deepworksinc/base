//
//  HRMModel.swift
//  Layer
//

import Foundation

struct HeartState {
    let bpm: Int
    let rr: Int
}

struct HeartStateMessage: Identifiable {
    let id = UUID()
    let value: HeartState
}
