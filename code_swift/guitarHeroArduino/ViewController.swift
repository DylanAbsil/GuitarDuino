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
    var audioPlayer: AKAudioPlayer?
    var tracker: AKFrequencyTracker?
    
    @IBOutlet weak var musicChosenTextField: NSTextField!
    
    
    @IBOutlet weak var buttonDenis: NSButton!
    
    var com: ORSSerialPort?
    var szAudiofile: String = ""
    
    @IBOutlet weak var playSound: NSButton!
    override func viewDidLoad() {
        super.viewDidLoad()
        
        var arduinoCom: String = "/dev/cu."
        
        ORSSerialPortManager.shared().availablePorts.forEach { (port) in
            if port.name.contains("usbmodem") {
                arduinoCom.append(port.name)
            }
        }
        
        com = ORSSerialPort(path: arduinoCom)
        
        if let port = com {
            //port.delegate = self
            port.baudRate = 9600;
            port.numberOfStopBits = 1;
            port.open()
        }
        
        print( AudioKit.outputDevices! )
        
        
        
        // Do any additional setup after loading the view.
    }
    @IBAction func playSoundPressed(_ sender: Any) {
        do {
            //let url: URL = URL(fileURLWithPath: "/Users/dylanabsil/Documents/telethon.mp3")
            let url: URL = URL(fileURLWithPath: szAudiofile)
            
            print( "1" )
            let audio = try AKAudioFile(forReading: url)
            let audio2 = try AKAudioFile(forReading: url)
            audioPlayer = try AKAudioPlayer(file: audio)
            let laterAudioPlayer = try AKAudioPlayer(file: audio2)
            
            if let audio = audioPlayer {
                tracker = AKFrequencyTracker(audio)
                let silence = AKBooster(tracker, gain: 0)
                AudioKit.output = silence
                AudioKit.start()
                
                audio.volume = 1.0
                audio.play()
            }
            
            Timer.scheduledTimer(withTimeInterval: 2.0, repeats: false, block: { (timer) in
                AudioKit.output = laterAudioPlayer
                AudioKit.start()
                laterAudioPlayer.play()
            })
        } catch  {
            print("Error info: \(error)")
        }
        
        Timer.scheduledTimer(withTimeInterval: 0.2, repeats: true) { (timer) in
            if let audio = self.audioPlayer, let t = self.tracker, audio.isPlaying == true {
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
                
            }
        }
    }
    
    @IBAction func easyPressed(_ sender: Any) {
    }
    
    @IBAction func mediumPressed(_ sender: Any) {
    }
    
    @IBAction func extremePressed(_ sender: Any) {
    }
    
    
    @IBAction func chooseSoundPressed(_ sender: Any) {
        let dialog = NSOpenPanel();
        
        dialog.title                   = "Choose an audio file";
        dialog.showsResizeIndicator    = true;
        dialog.showsHiddenFiles        = false;
        dialog.canChooseDirectories    = true;
        dialog.canCreateDirectories    = false;
        dialog.allowsMultipleSelection = false;
        dialog.allowedFileTypes        = ["mp3", "wav", "m4a", "m4p"];
        
        if (dialog.runModal() == .OK) {
            if let result = dialog.url{
                szAudiofile = result.path
                musicChosenTextField.stringValue = szAudiofile;
                playSound.isEnabled = true
            }
        } else {
            // User clicked on "Cancel"
            return
        }
    }
    
}
/*
extension ViewController: ORSSerialPortDelegate {
    func serialPortWasOpened(_ serialPort: ORSSerialPort) {
        print( ":::::::: OPENED !!" )
    }
    
    func serialPortWasClosed(_ serialPort: ORSSerialPort) {
        print( ":::::::: CLOSEDDDDDD !!" )
    }
    func serialPortWasRemoved(fromSystem serialPort: ORSSerialPort) {
        print( ":::::::: REMOVEEEDDD!!" )
    }
    
    func serialPort(_ serialPort: ORSSerialPort, didEncounterError error: Error) {
        print( ":::::::::: EROOOOOORRRRRR" );
        print( error )
    }
}*/
