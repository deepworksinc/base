//
//  HRMModel.swift
//

import Foundation

struct HeartState: Codable {
    // Beats per minute - the user's intensity
    let bpm: Int
    // DFA1 - the user's threshold zone proxy
    let dfa1: Float
}

struct HeartStateMessage: Identifiable {
    let id = UUID()
    let value: HeartState
}
