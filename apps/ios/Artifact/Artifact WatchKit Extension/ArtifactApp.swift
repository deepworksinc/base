//
//  ArtifactApp.swift
//

import SwiftUI

@main
struct ArtifactApp: App {
    let heartProvider = HeartProvider()
    
    @SceneBuilder var body: some Scene {
        WindowGroup {
            NavigationView {
                ContentView().environmentObject(heartProvider)
            }
        }
    }
}
