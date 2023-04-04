//
//  HeartProvider.swift
//

import Combine
import CoreBluetooth
import SwiftUI

let heartProviderCBUUID = CBUUID(string: "0x180D")
let heartMeasurementCharacteristicCBUUID = CBUUID(string: "2A37")

class HeartProvider: NSObject, ObservableObject {
    @ObservedObject private var connectivityManager = ConnectivityProvider.shared
    @Published var state = HeartState(bpm: 0, dfa1: 0.0, rmssd: 0.0)
    
    var cancellableBag = Set<AnyCancellable>()
    var centralManager: CBCentralManager!
    var heartPeripheral: CBPeripheral!
    
    override init() {
        super.init()
        
        connectivityManager.$heartStateMessage.sink { message in
            if message != nil {
                let newValue = message!.value
                self.setHeartState(value: newValue)
            }
        }.store(in: &cancellableBag)
        
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    func onHeartStateReceived(_ state: HeartState) {
        self.setHeartState(value: state)
        ConnectivityProvider.shared.updateHeartState(state)
    }
    
    func setHeartState(value: HeartState) {
        state = value
    }
}

extension HeartProvider: CBCentralManagerDelegate {
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        switch central.state {
        case .unknown:
            print("central.state is .unknown")
        case .resetting:
            print("central.state is .resetting")
        case .unsupported:
            print("central.state is .unsupported")
        case .unauthorized:
            print("central.state is .unauthorized")
        case .poweredOff:
            print("central.state is .poweredOff")
        case .poweredOn:
            print("central.state is .poweredOn")
            centralManager.scanForPeripherals(withServices: [heartProviderCBUUID])
        @unknown default:
            print("Fatal error")
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral,
                        advertisementData: [String : Any], rssi RSSI: NSNumber) {
        print(peripheral)
        heartPeripheral = peripheral
        heartPeripheral.delegate = self
        centralManager.stopScan()
        centralManager.connect(heartPeripheral)
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        print("Connected!")
        heartPeripheral.discoverServices([heartProviderCBUUID])
    }
}

extension HeartProvider: CBPeripheralDelegate {
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        guard let services = peripheral.services else { return }
        for service in services {
            print(service)
            peripheral.discoverCharacteristics(nil, for: service)
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        guard let characteristics = service.characteristics else { return }
        
        for characteristic in characteristics {
            print(characteristic)
            
            if characteristic.properties.contains(.read) {
                print("\(characteristic.uuid): properties contains .read")
                peripheral.readValue(for: characteristic)
            }
            if characteristic.properties.contains(.notify) {
                print("\(characteristic.uuid): properties contains .notify")
                peripheral.setNotifyValue(true, for: characteristic)
            }
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        switch characteristic.uuid {
        case heartMeasurementCharacteristicCBUUID:
            let state = parseHeartState(from: characteristic)
            onHeartStateReceived(state)
        default:
            print("Unhandled Characteristic UUID: \(characteristic.uuid)")
        }
    }
    
    private func parseHeartState(from characteristic: CBCharacteristic) -> HeartState {
        guard let characteristicData = characteristic.value else { return HeartState(bpm: -1, dfa1: -1, rmssd: -1.0) }
        let byteArray = [UInt8](characteristicData)
        
        let features = HRVWrapper().push(byteArray)!
        
        guard let bpm = features["bpm"] as? Int else { return HeartState(bpm: -1, dfa1: -1, rmssd: -1.0) }
        guard let dfa1 = features["dfa1"] as? Float else { return HeartState(bpm: -1, dfa1: -1, rmssd: -1.0) }
        guard let rmssd = features["rmssd"] as? Float else { return HeartState(bpm: -1, dfa1: -1, rmssd: -1.0) }

        return HeartState(bpm: bpm, dfa1: dfa1, rmssd: rmssd)
    }
}
