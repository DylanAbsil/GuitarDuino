//
//  ViewController.swift
//  guitarHeroArduino
//
//  Created by Dylan Absil on 28/03/2018.
//  Copyright © 2018 Dylan Absil. All rights reserved.
//

import Cocoa
import AudioKit
import ORSSerial

class ViewController: NSViewController {
    var audioPlayer: AKAudioPlayer? = nil
    var laterAudioPlayer: AKAudioPlayer? = nil
    
    var tracker: AKFrequencyTracker?
    var gameEndAlreadyTriggered: Bool = false
    
    @IBOutlet weak var progressBarMusic: NSProgressIndicator!
    @IBOutlet weak var labelInformation: NSTextField!
    @IBOutlet weak var difficultyDropDown: NSPopUpButton!
    @IBOutlet weak var musicChosenTextField: NSTextField!
    @IBOutlet weak var currentLabel: NSTextField!
    @IBOutlet weak var endLabel: NSTextField!
    
    var com: ORSSerialPort?
    var szAudiofile: String = ""
    
    @IBOutlet weak var playSound: NSButton!
    override func viewDidLoad() {
        super.viewDidLoad()
        
        difficultyDropDown.removeAllItems()
        difficultyDropDown.addItems(withTitles: ["Easy", "Ah wé qd mm", "Ah"])
        
        var arduinoCom: String = "/dev/cu."
        
        ORSSerialPortManager.shared().availablePorts.forEach { (port) in
            if port.name.contains("usbmodem") {
                arduinoCom.append(port.name)
            }
        }
        
        com = ORSSerialPort(path: arduinoCom)
        
        if let port = com {
            port.delegate = self
            port.baudRate = 9600;
            port.numberOfStopBits = 1;
            port.open()
        }
        
        print( AudioKit.outputDevices! )
        
        
        
        // Do any additional setup after loading the view.
    }
    @IBAction func playSoundPressed(_ sender: Any) {
        gameEndAlreadyTriggered = false
        playSound.isEnabled = false
        self.currentLabel.stringValue = "00:00"
        
        if let c = self.com {
            // init the game
            var data = Data()
            data.append(0);
            c.send(data);
        }
        
        if let a = audioPlayer, let b = laterAudioPlayer {
            a.stop()
            b.stop()
            
            laterAudioPlayer = nil
        }
        
        do {
            let url: URL = URL(fileURLWithPath: szAudiofile)
            
            let audio = try AKAudioFile(forReading: url)
            let audio2 = try AKAudioFile(forReading: url)
            audioPlayer = try AKAudioPlayer(file: audio)
            laterAudioPlayer = try AKAudioPlayer(file: audio2)
            
            if let duration = laterAudioPlayer?.duration {
                self.progressBarMusic.maxValue = duration
                let second = Int(duration.truncatingRemainder(dividingBy: 60.0))
                let minute = Int(duration/60)
                
                self.endLabel.stringValue = "\(minute < 10 ? "0" : "")\(minute):\(second < 10 ? "0" : "")\(second)"
            }
            
            if let audio = audioPlayer {
                tracker = AKFrequencyTracker(audio)
                let silence = AKBooster(tracker, gain: 0)
                AudioKit.output = silence
                AudioKit.start()
                
                audio.volume = 1.0
                audio.play()
            }
            
            Timer.scheduledTimer(withTimeInterval: 2.0, repeats: false, block: { (timer) in
                if( self.laterAudioPlayer != nil ) {
                    AudioKit.output = self.laterAudioPlayer
                    AudioKit.start()
                    self.laterAudioPlayer!.play()
                }
            })
        } catch  {
            print("Error info: \(error)")
        }
        
        let index = self.difficultyDropDown.indexOfSelectedItem
        
        let time = 0.5 - (index * 0.15)
        
        Timer.scheduledTimer(withTimeInterval: time, repeats: true) { (timer) in
            if let audioLater = self.laterAudioPlayer {
                let currentTime = audioLater.currentTime
                self.progressBarMusic.doubleValue = currentTime
                let second = Int(currentTime.truncatingRemainder(dividingBy: 60.0))
                let minute = Int(currentTime/60)
                
                self.currentLabel.stringValue = "\(minute < 10 ? "0" : "")\(minute):\(second < 10 ? "0" : "")\(second)"
            }
            
            if let audio = self.audioPlayer, let t = self.tracker {
                if( audio.isPlaying == true ) {
                    let amplitude = t.amplitude * 100.0
                    let minValue = min( amplitude, 100.0);
                    
                    if let c = self.com {
                        let i = minValue
                        var data = Data()
                        var value: UInt8
                        
                        if( i >= 0.0 && i < 25.0 ) {
                            value = 0
                        } else if( i >= 25.0 && i < 50.0 ) {
                            value = 1
                        } else if( i >= 50.0 && i < 75.0 ) {
                            value = 2
                        } else {
                            value = 3
                        }
                        
                        data.append(value)
                        c.send(data)
                    }
                } else if( self.gameEndAlreadyTriggered == false ) {
                    self.gameEndAlreadyTriggered = true;
                    
                    if let c = self.com {
                        Timer.scheduledTimer(withTimeInterval: 2.0, repeats: false, block: { (timer_end) in
                            var data = Data()
                            data.append(4);
                            c.send(data);
                        })
                    }
                    
                    timer.invalidate()
                }
                
            }
        }
    }
    
    
    @IBAction func chooseSoundPressed(_ sender: Any) {
        let dialog = NSOpenPanel()
        
        dialog.title                   = "Choose an audio file"
        dialog.showsResizeIndicator    = true
        dialog.showsHiddenFiles        = false
        dialog.canChooseDirectories    = true
        dialog.canCreateDirectories    = false
        dialog.allowsMultipleSelection = false
        dialog.allowedFileTypes        = ["mp3"]
        // MP3 ONLY
        
        if (dialog.runModal() == .OK) {
            if let result = dialog.url{
                szAudiofile = result.path
                musicChosenTextField.stringValue = szAudiofile;
                playSound.isEnabled = true
            }
        }
    }
    
}

extension ViewController: ORSSerialPortDelegate {
    func serialPort(_ serialPort: ORSSerialPort, didReceive data: Data) {
        
        guard let dataString = String(data: data, encoding: String.Encoding.utf8) else { return }
        
        let index = dataString.index(dataString.startIndex, offsetBy: 1)
        guard let value = Int(dataString[..<index]) else { return }
        guard let score = Int(dataString[index...]) else { return }
        
        switch value {
        case 5:
            playSound.isEnabled = true
            labelInformation.isHidden = false
            labelInformation.stringValue = "Game Over \n Score : \(score)"
            audioPlayer?.stop()
            laterAudioPlayer?.stop()
            gameEndAlreadyTriggered = true
        case 6:
            playSound.isEnabled = true
            labelInformation.isHidden = false
            labelInformation.stringValue = "Game Won \n Score : \(score)"
            audioPlayer?.stop()
            laterAudioPlayer?.stop()
            gameEndAlreadyTriggered = true
        default:
            break
        }
    }
    func serialPortWasOpened(_ serialPort: ORSSerialPort) {
        print( "Serial port is opened !" )
        serialPort.rts = true
        serialPort.delegate = self
        
    }
    
    func serialPortWasClosed(_ serialPort: ORSSerialPort) {
        print( "Serial port was closed .." )
    }
    func serialPortWasRemoved(fromSystem serialPort: ORSSerialPort) {
        print( "Port has been removed !" )
    }
    
    func serialPort(_ serialPort: ORSSerialPort, didEncounterError error: Error) {
        print( "Error with serial port : \(error)")
    }
}
