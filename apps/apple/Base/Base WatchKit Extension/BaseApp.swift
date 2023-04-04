//
//  BaseApp.swift
//

import SwiftUI

@main
struct BaseApp: App {
    let heartProvider = HeartProvider()
    
    @SceneBuilder var body: some Scene {
        WindowGroup {
            ContentView().environmentObject(heartProvider)
        }
    }
}
