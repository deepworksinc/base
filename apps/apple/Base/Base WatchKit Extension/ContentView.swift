//
//  ContentView.swift
//

import SwiftUI

struct ContentView: View {
    var body: some View {
        ZStack {
            LinearGradient(
                gradient: Gradient(
                    stops: [
                        .init(color: Color(red: 250/255, green: 221/255, blue: 132/255), location: 0),
                        .init(color: Color(red: 240/255, green: 137/255, blue: 74/255), location: 0.1),
                        .init(color: Color(red: 169/255, green: 92/255, blue: 124/255), location: 0.3),
                        .init(color: Color(red: 88/255, green: 78/255, blue: 141/255), location: 1)
                    ]),
                startPoint: UnitPoint(x: 1, y: 0),
                endPoint: UnitPoint(x: 0, y: 1)
            )
            .ignoresSafeArea()
            VStack {
                HeartView()
            }
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
