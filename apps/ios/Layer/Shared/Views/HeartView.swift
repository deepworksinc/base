//
//  HeartView.swift
//  Layer
//

import SwiftUI

struct HeartView: View {
    @EnvironmentObject private var heartService: HeartService
    
    var body: some View {
        VStack {
            Text("\(heartService.heartState.bpm)")
            Text("\(heartService.heartState.rr)")
        }
    }
}

struct HeartView_Previews: PreviewProvider {
    static var previews: some View {
        HeartView()
    }
}
