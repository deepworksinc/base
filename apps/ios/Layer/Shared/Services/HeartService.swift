//
//  HeartService.swift
//  Shared
//

import Combine
import CoreBluetooth
import SwiftUI

let heartServiceCBUUID = CBUUID(string: "0x180D")
let heartMeasurementCharacteristicCBUUID = CBUUID(string: "2A37")

class HeartService: NSObject, ObservableObject {
    @ObservedObject private var connectivityManager = ConnectivityService.shared
    @Published var heartState = HeartState(bpm: 0, rr: 0)
    
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
    
    func onHeartStateReceived(_ heartState: HeartState) {
        self.setHeartState(value: heartState)
        ConnectivityService.shared.updateHeartState(heartState)
    }
    
    func setHeartState(value: HeartState) {
        heartState = value
    }
}

extension HeartService: CBCentralManagerDelegate {
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
            centralManager.scanForPeripherals(withServices: [heartServiceCBUUID])
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
        heartPeripheral.discoverServices([heartServiceCBUUID])
    }
}

extension HeartService: CBPeripheralDelegate {
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
            let heartState = parseHeartState(from: characteristic)
            onHeartStateReceived(heartState)
        default:
            print("Unhandled Characteristic UUID: \(characteristic.uuid)")
        }
    }
    
    private func parseHeartState(from characteristic: CBCharacteristic) -> HeartState {
        guard let characteristicData = characteristic.value else { return HeartState(bpm: -1, rr: -1) }
        let byteArray = [UInt8](characteristicData)
        
        // See: https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.heart_rate_measurement.xml
        // The heart rate mesurement is in the 2nd, or in the 2nd and 3rd bytes, i.e. one one or in two bytes
        // The first byte of the first bit specifies the length of the heart rate data, 0 == 1 byte, 1 == 2 bytes
        var bpm = 0
        let firstBitValue = byteArray[0] & 1
        if firstBitValue == 0 {
            // Heart Rate Value Format is in the 2nd byte
            bpm = Int(byteArray[1])
        } else {
            // Heart Rate Value Format is in the 2nd and 3rd bytes
            bpm = (Int(byteArray[1]) << 8) + Int(byteArray[2])
        }
        var rr = 0
        let fifthBitValue = byteArray[0] & 16
        if fifthBitValue != 0 {
            switch byteArray[0] {
            case 16,18,20,22:
                //rr-value in [2] und [3]
                rr = Int(byteArray[2]) + (Int(byteArray[3]) << 8)
            case 17,19,21,23:
                //rr-value in [3] und [4]
                rr = Int(byteArray[3]) + (Int(byteArray[4]) << 8)
            case 24,26,28,30:
                //rr-value in [4] und [5]
                rr = Int(byteArray[4]) + (Int(byteArray[5]) << 8)
            case 25,27,29,31:
                //rr-value in [5] und [6]
                rr = Int(byteArray[4]) + (Int(byteArray[5]) << 8)
            default:
                print("RR intervals not found despite flag")
            }
        }
        return HeartState(bpm: bpm, rr: rr)
    }
}
