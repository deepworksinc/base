//
//  LayerApp.swift
//  Layer WatchKit Extension
//
//  Created by Shane on 10/2/22.
//

import SwiftUI

@main
struct LayerApp: App {
    @SceneBuilder var body: some Scene {
        WindowGroup {
            NavigationView {
                ContentView()
            }
        }

        WKNotificationScene(controller: NotificationController.self, category: "myCategory")
    }
}
