//
//  HeartView.swift
//

import SwiftUI

struct HeartView: View {
    @EnvironmentObject private var heartProvider: HeartProvider
    
    var body: some View {
        VStack {
            Text("\(heartProvider.state.bpm)")
                .foregroundColor(.white)
            Text("\(heartProvider.state.dfa1)")
                .foregroundColor(.white)
            Text("\(heartProvider.state.rmssd)")
                .foregroundColor(.white)
        }
    }
}

struct HeartView_Previews: PreviewProvider {
    static var previews: some View {
        HeartView()
    }
}
