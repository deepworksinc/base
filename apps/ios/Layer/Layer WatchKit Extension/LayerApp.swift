//
//  LayerApp.swift
//  Layer WatchKit Extension
//

import SwiftUI

@main
struct LayerApp: App {
    let hrmService = HRMService()
    
    @SceneBuilder var body: some Scene {
        WindowGroup {
            NavigationView {
                ContentView().environmentObject(hrmService)
            }
        }
    }
}
