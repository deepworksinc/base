//
//  ConnectivityService.swift
//  Shared
//

import WatchConnectivity

final class ConnectivityService: NSObject, ObservableObject {
    static let shared = ConnectivityService()
    @Published var heartStateMessage: HeartStateMessage? = nil
    
    private override init() {
        super.init()
        
        if WCSession.isSupported() {
            WCSession.default.delegate = self
            WCSession.default.activate()
        }
    }
    
    private let kMessageKey = "message"
    
    func updateHeartState(_ heartState: HeartState) {
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
        
        WCSession.default.sendMessage([kMessageKey : heartState], replyHandler: nil) { error in
            print("Cannot send message: \(String(describing: error))")
        }
    }
}

extension ConnectivityService: WCSessionDelegate {
    func session(_ session: WCSession, didReceiveMessage message: [String : Any]) {
        if let heartStateValue = message[kMessageKey] as? HeartState {
            DispatchQueue.main.async { [weak self] in
                self?.heartStateMessage = HeartStateMessage(value: heartStateValue)
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
