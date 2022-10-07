//
//  HRMView.swift
//  Layer
//

import SwiftUI

struct HRMView: View {
    @EnvironmentObject private var hrmService: HRMService
    
    var body: some View {
        VStack {
            Text("\(hrmService.bpm)")
        }
    }
}

struct HRMView_Previews: PreviewProvider {
    static var previews: some View {
        HRMView()
    }
}
