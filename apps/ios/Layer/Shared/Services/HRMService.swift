//
//  HRMService.swift
//  Shared
//

import Combine
import SwiftUI

class HRMService: ObservableObject {
    
    @Published var counter: Int = 0
    @ObservedObject private var connectivityManager = ConnectivityService.shared
    var cancellableBag = Set<AnyCancellable>()

    init() {
        connectivityManager.$counterMessage.sink { message in
            if message != nil {
                let newValue = message!.value
                self.set(value: newValue)
            }
        }.store(in: &cancellableBag)
    }
    
    func increment() {
        counter += 1
        ConnectivityService.shared.updateCounter(counter)
    }
    
    func set(value: Int) {
        counter = value
    }
}
