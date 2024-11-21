#include "daisy_seed.h"
#include "daisysp.h"
#include "hid/midi.h"
// #include "hid/midi_message.h"

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;

DaisySeed hw;
MidiUsbHandler midi;
// daisy::GPIO button;

int main(void)
{   
     // hw.Configure(); 
    hw.Init();


    // hw.StartLog();
    // hw.DelayMs(100);
    // hw.PrintLine("Serial Logger started!");

    bool useSerialLogger = false;

    if (useSerialLogger) {
        // Start the Serial Logger
        hw.StartLog();
        // hw.DelayMs(5000);
        hw.PrintLine("Serial Logger started!");
        // turn on the LED to indicate in serial mode
        hw.SetLed(true);
        
    }
        
    else {
        // Initialize USB MIDI
        MidiUsbHandler::Config midi_cfg;
        midi_cfg.transport_config.periph = MidiUsbTransport::Config::INTERNAL;
        midi.Init(midi_cfg);
        hw.SetLed(false);
    }
   
    

    // Initialize button
    GPIO my_button;
    my_button.Init(D0, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);

    // bool button_pressed =  false;

    // button_pressed = true;

    // hw.DelayMs(2000);
    
    // hw.PrintLine("Serial Logger initialized!");

    while(1)
    {
        bool current_button_state = my_button.Read();
        // hw.PrintLine("Logger is enabled");
        hw.PrintLine("Button State: %s", current_button_state ? "true" : "false");
        // Check button state
        
        if (current_button_state == 0 )
        {
            hw.SetLed(true);
            // hw.PrintLine("Button pressed ");

            // Button pressed
            // button_pressed = true;
            //  MidiEvent midi1;
            // // Initialize MIDI message for Note On
            // // MidiEvent midi1;
            // midi1.type = NoteOn;
            // midi1.channel = 0;
            // midi1.data[0] = 60; // middle C
            // midi1.data[1] = 100; // velocity
            // MidiMessage midi1 = MidiMessage::NoteOn(60, 100);
            // midi.SendMessage(midi1);
            uint8_t note_on[3] = {0x90, 60, 100};  // 0x90 = Note On channel 0, middle C, velocity 100
            if (useSerialLogger) {
                hw.Print("Midi message is: ");
                for (int i = 0; i < 3; i++) {
                    hw.Print("%d ", note_on[i]);
                }
            }
            else {
                midi.SendMessage(note_on, sizeof(note_on));
            }
            // hw.Print("Midi message is %d", note_on);
            // midi.SendMessage(note_on, sizeof(note_on));
            
            // Note On for middle C with velocity 100
        }
        else if (current_button_state == 1)
        {
            hw.SetLed(false);
            // Button released
            // hw.PrintLine("Button released ");
            // button_pressed = false;
            uint8_t note_off[3] = {0x80, 60, 0};  // 0x90 = Note On channel 0, middle C, velocity 100
            if (useSerialLogger) {
                hw.Print("Midi message is: ");
                for (int i = 0; i < 3; i++) {
                    hw.Print("%d ", note_off[i]);
                }
            }
            else {
                midi.SendMessage(note_off, sizeof(note_off));
            }

            // hw.Print("Midi message is %d", note_off);
            // midi.SendMessage(note_off, sizeof(note_off));
            // midi.SendMessage(MidiEvent::NoteOff(60, 0)); // Note Off for middle C
        }
        hw.DelayMs(1);
    }
}