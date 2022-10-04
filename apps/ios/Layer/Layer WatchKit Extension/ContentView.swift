//
//  ContentView.swift
//  Layer WatchKit Extension
//
//  Created by Shane on 10/2/22.
//

import SwiftUI

struct ContentView: View {
    var body: some View {
        VStack(alignment: .center) {
            StatsView()
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
