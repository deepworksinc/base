//
//  ConnectivityService.swift
//  Shared
//

import Foundation
import WatchConnectivity

struct BPMMessage: Identifiable {
    let id = UUID()
    let value: Int
}

final class ConnectivityService: NSObject, ObservableObject {
    static let shared = ConnectivityService()
    @Published var bpmMessage: BPMMessage? = nil
    
    private override init() {
        super.init()
        
        if WCSession.isSupported() {
            WCSession.default.delegate = self
            WCSession.default.activate()
        }
    }
    
    private let kMessageKey = "message"
    
    func updateBPM(_ bpm: Int) {
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
        
        WCSession.default.sendMessage([kMessageKey : bpm], replyHandler: nil) { error in
            print("Cannot send message: \(String(describing: error))")
        }
    }
}

extension ConnectivityService: WCSessionDelegate {
    func session(_ session: WCSession, didReceiveMessage message: [String : Any]) {
        if let bpmValue = message[kMessageKey] as? Int {
            DispatchQueue.main.async { [weak self] in
                self?.bpmMessage = BPMMessage(value: bpmValue)
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
