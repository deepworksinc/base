//
//  ConnectivityService.swift
//  Shared
//

import Foundation
import WatchConnectivity

struct CounterMessage: Identifiable {
    let id = UUID()
    let value: Int
}

final class ConnectivityService: NSObject, ObservableObject {
    static let shared = ConnectivityService()
    @Published var counterMessage: CounterMessage? = nil
    
    private override init() {
        super.init()
        
        if WCSession.isSupported() {
            WCSession.default.delegate = self
            WCSession.default.activate()
        }
    }
    
    private let kMessageKey = "message"
    
    func updateCounter(_ counter: Int) {
        guard WCSession.default.activationState == .activated else {
          return
        }
        #if os(iOS)
        guard WCSession.default.isWatchAppInstalled else {
            return
        }
        #else
        guard WCSession.default.isCompanionAppInstalled else {
            return
        }
        #endif
        
        WCSession.default.sendMessage([kMessageKey : counter], replyHandler: nil) { error in
            print("Cannot send message: \(String(describing: error))")
        }
    }
}

extension ConnectivityService: WCSessionDelegate {
    func session(_ session: WCSession, didReceiveMessage message: [String : Any]) {
        if let counterValue = message[kMessageKey] as? Int {
            DispatchQueue.main.async { [weak self] in
                self?.counterMessage = CounterMessage(value: counterValue)
            }
        }
    }
    
    func session(_ session: WCSession,
                 activationDidCompleteWith activationState: WCSessionActivationState,
                 error: Error?) {}
    
    #if os(iOS)
    func sessionDidBecomeInactive(_ session: WCSession) {}
    func sessionDidDeactivate(_ session: WCSession) {
        session.activate()
    }
    #endif
}
