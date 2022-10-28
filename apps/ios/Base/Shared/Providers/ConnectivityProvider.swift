//
//  ConnectivityProvider.swift
//

import WatchConnectivity

final class ConnectivityProvider: NSObject, ObservableObject {
    static let shared = ConnectivityProvider()
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
        let encoder = JSONEncoder()
        let heartStateData = try! encoder.encode(heartState)
        WCSession.default.sendMessage([kMessageKey: heartStateData], replyHandler: nil) { error in
            print("Cannot send message: \(String(describing: error))")
        }
    }
}

extension ConnectivityProvider: WCSessionDelegate {
    func session(_ session: WCSession, didReceiveMessage message: [String : Any]) {
        
        let decoder = JSONDecoder()
        if message[kMessageKey] != nil {
            let heartStateValue = try! decoder.decode(HeartState.self, from: message[kMessageKey] as! Data) as HeartState
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
