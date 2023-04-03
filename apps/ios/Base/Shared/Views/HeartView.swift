//
//  HeartView.swift
//

import SwiftUI

struct HeartView: View {
    @EnvironmentObject private var heartProvider: HeartProvider
    
    var body: some View {
        VStack {
            Text("\(heartProvider.state.bpm)")
            Text("\(heartProvider.state.dfa1)")
            Text("\(heartProvider.state.rmssd)")
        }
    }
}

struct HeartView_Previews: PreviewProvider {
    static var previews: some View {
        HeartView()
    }
}
