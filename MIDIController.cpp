#include "daisy_seed.h"
#include "daisysp.h"
#include "hid/midi.h"
#include <random>
// #include "hid/midi_message.h"

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;


// JUN 6-V definitions
#define JUNO_DCO_LFO 77
#define JUNO_DCO_SUB_LEVEL 85
#define JUNO_DCO_SUB_ON 82
#define JUNO_ATTACK  73
#define JUNO_DECAY   75
#define JUNO_SUSTAIN 76 
#define JUNO_RELEASE 72
#define JUNO_FILTER_CUTOFF 74 
#define JUNO_UNISON 93 
#define JUNO_CHORUS1 80
#define JUNO_REVERB_DECAY 91
#define JUNO_REVERB_LEVEL 81


bool unison_active = false; 
bool chorus_state = false;

// #define ADXL345_ADDR 0x53
// #define DATAX0 0x32
// #define POWER_CTL 0x2D
// #define SCALE_FACTOR 0.004 
// #define DATA_FORMAT 0x31 


DaisySeed hw;
MidiUsbHandler midi;
// I2CHandle i2c;
// daisy::GPIO button;

GPIO button1;
GPIO button2;
GPIO button3;
bool button1_last_state = true;
bool button2_last_state = true;
bool button3_last_state = true;

GPIO LED_R1;
GPIO LED_R2;
GPIO LED_W1;
GPIO LED_W2;
// bool oscillate_leds = false;
bool led_state = false;

enum LEDMode {
    OFF,
    RANDOM_LFO,    // Add mode for button 1
    UNISON_BLINK,
    CHORUS_CYCLE
};
LEDMode current_led_mode = OFF;
// GPIO accelerometer1_VCC;
// GPIO accelerometer1_SC;
// GPIO accelerometer1_SDA;
// GPIO accelerometer1_SCL;

GPIO encoderA;
GPIO encoderB;
int encoderPos = 0;
int lastEncA;
int lastEncB;

AdcChannelConfig fader1cfg;
AdcChannelConfig fader2cfg;
AdcChannelConfig fader3cfg;
AdcChannelConfig knob1cfg;
AdcChannelConfig knob2cfg;
AdcChannelConfig knob3cfg;
void initButtons() {
    // Initialize button
    button1.Init(D0, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
    button2.Init(D1, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
    button3.Init(D2, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
    // my_fader.Init(A0, GPIO::Mode::ANALOG);
    // GPIO my_button;
}

void testLEDs(){
    LED_R1.Init(D5, GPIO::Mode::OUTPUT);
    LED_R2.Init(D6, GPIO::Mode::OUTPUT);
    LED_W1.Init(D7, GPIO::Mode::OUTPUT);
    LED_W2.Init(D8, GPIO::Mode::OUTPUT);
    // LED_R1.Write(true); // turn on the LED
    // LED_R2.Write(true); // turn on the LED
    // LED_W1.Write(true); // turn on the LED
    // LED_W2.Write(true); // turn on the LED
}

void InitEncoder() {
    // Initialize encoder pins with pull-up resistors
    encoderA.Init(D3, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
    encoderB.Init(D4, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
    
    // Get initial state
    lastEncA = encoderA.Read();
    lastEncB = encoderB.Read();
}

 void InitFaders() {
        // Configure two ADC pins (using pins 21 and 22 as example)
        fader1cfg.InitSingle(hw.GetPin(15));  // First fader
        fader2cfg.InitSingle(hw.GetPin(16));  // Second fader
        fader3cfg.InitSingle(hw.GetPin(17));  // Third fader
        knob1cfg.InitSingle(hw.GetPin(18));  // First knob
        knob2cfg.InitSingle(hw.GetPin(19));  // Second knob
        knob3cfg.InitSingle(hw.GetPin(20));  // Third knob


        // Initialize ADC with both configs
        AdcChannelConfig cfg[6] = {fader1cfg, fader2cfg, fader3cfg, knob1cfg, knob2cfg, knob3cfg};
        hw.adc.Init(cfg, 6); 
        
        hw.adc.Start();
}

// void readAccelerometer(){
//     // devices is an ADXL345

// }

void ReadEncoder(bool useSerialLogger) {
    int currentA = encoderA.Read();
    int currentB = encoderB.Read();
    
    // State change detected
    if (currentA != lastEncA || currentB != lastEncB) {
        // CW: A leads B
        if (currentA != lastEncA) {
            if (currentA != currentB) {
                encoderPos++;
            } else {
                encoderPos--;
            }
            // hw.PrintLine("Encoder Position: %d", encoderPos);
            // i want the encoder value to be the seed value of random to be used elsewhere


            encoderPos = std::max(0, std::min(127, encoderPos));


            srand(encoderPos);
            if (useSerialLogger) { // if serial logger is enabled
                hw.PrintLine("Random Value: %d", rand());
            }
        }
        
        lastEncA = currentA;
        lastEncB = currentB;
    }
}

float remapWithCurve(float inputVal, float slopeVal) {
    // Map slopeVal (0-1023) to range [0, 1]
    float normalizedSlope = slopeVal / 1023.0;

    // Calculate the curve factor k
    float k = pow(2, (2 * normalizedSlope - 1));

    // Apply the curve based on k
    if (k < 1) {
        // Logarithmic mapping
        return pow(inputVal, k);
    } else if (k == 1) {
        // Linear mapping
        return inputVal;
    } else {
        // Exponential mapping
        return pow(inputVal, k);
    }
}

// void OscillateLEDs() {
//     static uint32_t last_update = 0;
//     static uint8_t brightness = 0;
//     static bool increasing = true;
    
//     if (!oscillate_leds) {
//         LED_W1.Write(0);
//         LED_W2.Write(0);
//         return;
//     }
    
//     uint32_t now = System::GetNow();
//     if (now - last_update > 5) {
//         if (increasing) {
//             brightness++;
//             if (brightness >= 100) increasing = false;
//         } else {
//             brightness--;
//             if (brightness == 0) increasing = true;
//         }
        
//         float pwm = brightness / 100.0f;
//         LED_W1.Write(pwm);
//         LED_W2.Write(pwm);
        
//         last_update = now;
//     }
// }


// float remapWithCurve(float inputVal, float slopeVal) {
//     // Map slopeVal (0-1023) to range [0, 1]
//     float normalizedSlope = slopeVal / 1023.0;

//     // Use 3 as base for medium curve intensity
//     float k = pow(3, (2 * normalizedSlope - 1));

//     // Apply curve with moderate shaping
//     if (k < 1) {
//         // Moderate logarithmic curve
//         return pow(inputVal, k * 0.75f);  // 0.75 for gentler log curve
//     } else if (k == 1) {
//         return inputVal;
//     } else {
//         // Moderate exponential curve
//         return pow(inputVal, k * 1.5f);   // 1.5 for gentler exp curve
//     }
// }
void UpdateLEDs() {
    static uint32_t last_toggle = 0;
    static uint8_t led_position = 0;
    static bool led_state = false;
    uint32_t now = System::GetNow();

    switch(current_led_mode) {
        case RANDOM_LFO:
            if (now - last_toggle > 100) {
                led_state = !led_state;
                LED_R1.Write(led_state);
                LED_R2.Write(!led_state);  // Alternate red LEDs
                last_toggle = now;
            }
            break;

        case UNISON_BLINK:
            if (now - last_toggle > 200) {
                led_state = !led_state;
                LED_W1.Write(led_state);
                LED_W2.Write(led_state);
                last_toggle = now;
            }
            break;

        case CHORUS_CYCLE:
            if (now - last_toggle > 150) {
                LED_R1.Write(led_position == 0);
                LED_R2.Write(led_position == 1);
                LED_W1.Write(led_position == 2);
                LED_W2.Write(led_position == 3);
                led_position = (led_position + 1) % 4;
                last_toggle = now;
            }
            break;

        case OFF:
            LED_R1.Write(false);
            LED_R2.Write(false);
            LED_W1.Write(false);
            LED_W2.Write(false);
            break;
    }
}

void ReadFaders(bool useSerialLogger) {
    // hw.PrintLine("reading faders called!");
    static float last_fader1 = 0.0f;
    static float last_fader2 = 0.0f;
    static float last_fader3 = 0.0f;
    static float last_knob1 = 0.0f;
    static float last_knob2 = 0.0f;
    static float last_knob3 = 0.0f;
    const float threshold = 0.01f; // 1% change threshold

    float fader1_value = hw.adc.GetFloat(0); 
    float fader2_value = hw.adc.GetFloat(1);
    float fader3_value = hw.adc.GetFloat(2);
    float knob1_value = 1.0f - hw.adc.GetFloat(3);
    float knob2_value = 1.0f - hw.adc.GetFloat(4);
    float knob3_value = 1.0f - hw.adc.GetFloat(5);
    
    // Check if values changed more than threshold 
    if (fabsf(fader1_value - last_fader1) > threshold) {
        // Scale to MIDI range
        int sub_volume = (int)(fader1_value * 127);
        bool sub_active = sub_volume > 10;  // Above 1% threshold
        
        // Send sub on/off message if needed
        static bool last_sub_active = false;
        if (sub_active != last_sub_active) {
            uint8_t cc_sub_on[3] = {0xB0, JUNO_DCO_SUB_ON, sub_active ? 127 : 0};
            midi.SendMessage(cc_sub_on, sizeof(cc_sub_on));
            last_sub_active = sub_active;
        }
        
        // Send volume message
        uint8_t cc_sub_vol[3] = {0xB0, JUNO_DCO_SUB_LEVEL, sub_volume};
        midi.SendMessage(cc_sub_vol, sizeof(cc_sub_vol));
        
        if (useSerialLogger) {
            hw.PrintLine("Sub: %s Vol: %d", sub_active ? "ON" : "OFF", sub_volume);
        }
        
        last_fader1 = fader1_value;
    }
    
    // if (fabsf(fader2_value - last_fader2) > threshold) {
    //     int fader2_int = (int)(fader2_value * 1000);
    //     hw.PrintLine("Fader2: %d", fader2_int);
    //     last_fader2 = fader2_value;
    // }
    if (fabsf(fader2_value - last_fader2) > threshold || fabsf(knob2_value - last_knob2) > threshold) {
        
        // Basic level from fader2
        int basic_level = (int)(fader2_value * 127);
        
        // Apply curve to decay value only
        float curved_decay = remapWithCurve(fader2_value, knob2_value * 1023.0f);
        int decay_value = (int)(curved_decay * 127);
        
        // Create and send MIDI messages
        uint8_t cc_level[3] = {0xB0, JUNO_REVERB_LEVEL, basic_level};
        uint8_t cc_decay[3] = {0xB0, JUNO_REVERB_DECAY, decay_value};
        
        if (useSerialLogger) {
            hw.PrintLine("Reverb Level: %d, Decay: %d (Curve: %.2f)", 
                        basic_level, decay_value, knob2_value);
        } else {
            midi.SendMessage(cc_level, sizeof(cc_level));
            midi.SendMessage(cc_decay, sizeof(cc_decay));
        }
        
        last_fader2 = fader2_value;
        last_knob2 = knob2_value;
    }
    // if (fabsf(fader3_value - last_fader3) > threshold) {
    if (fabsf(fader3_value - last_fader3) > threshold || fabsf(knob3_value - last_knob3) > threshold) { // Filter cutoff control of the VST with curve control from knob3
        // int fader3_int = (int)(fader3_value * 1000);
        // hw.PrintLine("Fader3: %d", fader3_int);
        // last_fader3 = fader3_value;
         float curved_value = remapWithCurve(fader3_value, knob3_value * 1023.0f);
        int cutoff_value = (int)(curved_value * 127);
        
        // Create and send MIDI message
        uint8_t cc_cutoff[3] = {0xB0, JUNO_FILTER_CUTOFF, cutoff_value};
        
        if (useSerialLogger) {
            hw.PrintLine("Cutoff: %d (Curve: %.2f)", cutoff_value, knob3_value);
        } else {
            midi.SendMessage(cc_cutoff, sizeof(cc_cutoff));
        }
        
        last_fader3 = fader3_value;
        last_knob3 = knob3_value;
    }
    if (fabsf(knob1_value - last_knob1) > threshold) { // ADSR control of the VST
        int adr_value = (int)(knob1_value * 127);
        int s_value = 25 + (int)(knob1_value * 102);
        static int last_sent_value = -1;

        // int knob1_int = (int)(knob1_value * 1000);
        // hw.PrintLine("Knob1: %d", knob1_int);
        // Create CC messages for all ADSR parameters
        if (adr_value != last_sent_value) {
            uint8_t cc_attack[3] = {0xB0, JUNO_ATTACK, adr_value};
            uint8_t cc_decay[3] = {0xB0, JUNO_DECAY, adr_value};
            uint8_t cc_sustain[3] = {0xB0, JUNO_SUSTAIN, s_value};
            uint8_t cc_release[3] = {0xB0, JUNO_RELEASE, adr_value};
            
            if (useSerialLogger) {
                hw.PrintLine("ADSR Value: %d", adr_value);
            } else {
                // Send all ADSR messages
                midi.SendMessage(cc_attack, sizeof(cc_attack));
                midi.SendMessage(cc_decay, sizeof(cc_decay));
                midi.SendMessage(cc_sustain, sizeof(cc_sustain));
                midi.SendMessage(cc_release, sizeof(cc_release));
            }
            last_sent_value = adr_value; 
        
        }
        last_knob1 = knob1_value;
    }
    
        // last_knob1 = knob1_value;
    
    if (fabsf(knob2_value - last_knob2) > threshold) {
        int knob2_int = (int)(knob2_value * 1000);
        hw.PrintLine("Knob2: %d", knob2_int);
        last_knob2 = knob2_value;
    }
    // if (fabsf(knob3_value - last_knob3) > threshold) {
    //     int knob3_int = (int)(knob3_value * 1000);
    //     hw.PrintLine("Knob3: %d", knob3_int);
    //     last_knob3 = knob3_value;
    // }

    // // just print the fader values for now
    // int fader1_int = (int)(fader1_value * 1000);
    // int fader2_int = (int)(fader2_value * 1000);
    // hw.PrintLine("Fader1: %d, Fader2: %d", fader1_int, fader2_int);
}




void ReadButtons(bool useSerialLogger) {

         // Read current button states (invert because of pull-up)

        bool button1_current = !button1.Read();
        bool button2_current = !button2.Read();
        bool button3_current = !button3.Read();

        // Only handle state changes
        if (button1_current != button1_last_state) { // button 1 is to momentary send a random value generated from the read encoder to the DCO LFO of the VST
            if (button1_current) {
                current_led_mode = RANDOM_LFO;
                
                // uint8_t note_on[3] = {0x90, 60, 100};  // 0x90 = Note On channel 0, middle C, velocity 100
                uint8_t ccDCO_LFO[3] = {0xB0, JUNO_DCO_LFO, rand() % 128};  // 0xB0 = CC channel 0, DCO LFO, random value
                if (useSerialLogger) {
                    hw.PrintLine("Button 1 pressed:  Sending random value to DCO LFO");
                    hw.Print("Midi message is: ");
                    for (int i = 0; i < 3; i++) {
                        hw.Print("%d ", ccDCO_LFO[i]);
                    }
                    hw.PrintLine("");


                    // hw.PrintLine("Button 1 pressed\n");
                }
                else {
                    midi.SendMessage(ccDCO_LFO, sizeof(ccDCO_LFO));
                }

            } else {
                current_led_mode = OFF;
                hw.PrintLine("!!! Button 1 released");
                // hw.PrintLine("Button 1 released: stopping random DCO LFO");
                uint8_t ccDCO_LFO[3] = {0xB0, JUNO_DCO_LFO, 0};  // 0xB0 = CC channel 0, DCO LFO, random value
                if (useSerialLogger) {
                    hw.PrintLine("Button 1 released: stopping random DCO LFO");
                    hw.Print("Midi message is: ");
                    for (int i = 0; i < 3; i++) {
                        hw.Print("%d ", ccDCO_LFO[i]);
                    }
                    // hw.PrintLine("Button 1 released\n");
                }
                else {
                    midi.SendMessage(ccDCO_LFO, sizeof(ccDCO_LFO));
                }

            }
            button1_last_state = button1_current;


        }
        // if (button1_current) { // if button 1 is pressed, alternate the red LEDs
        //     static uint32_t last_toggle = 0;
        //     uint32_t now = System::GetNow();
        //     if (now - last_toggle > 100) {  // 100ms interval
        //         led_state = !led_state;  // Toggle state
        //         LED_R1.Write(led_state);     // One LED on
        //         LED_R2.Write(!led_state);    // Other LED off
        //         last_toggle = now;
        //     }
        // } else {
        //     // When button released, turn both off
        //     LED_R1.Write(false);
        //     LED_R2.Write(false);
        // }
        

        if (button2_current != button2_last_state) {
            if (button2_current) {
                // Toggle chorus and LED oscillation
                // oscillate_leds = !oscillate_leds;  // Enable LED oscillation
                
                // Send chorus ON message (value 127)
                unison_active = !unison_active;  // Toggle state
                current_led_mode = unison_active ? UNISON_BLINK : OFF;
                // uint8_t cc_chorus[3] = {0xB0, JUNO_CHORUS1, 127};
                uint8_t cc_chorus[3] = {0xB0, JUNO_UNISON, unison_active ? 127 : 0};
                if (useSerialLogger) {
                    hw.PrintLine("Button 2 pressed: Chorus ON");
                } 
                else {
                    midi.SendMessage(cc_chorus, sizeof(cc_chorus));
                }
            }
            // else {
            //     // Turn off chorus and stop LED oscillation
            //     // oscillate_leds = false;
            //     uint8_t cc_chorus[3] = {0xB0, JUNO_CHORUS1, 0};
            //     if (useSerialLogger) {
            //         hw.PrintLine("Button 2 released: Chorus OFF");
            //     } else {
            //         midi.SendMessage(cc_chorus, sizeof(cc_chorus));
            //     }
            // }
            button2_last_state = button2_current;
        }
        // if (unison_active) { // if button 1 is pressed, alternate the red LEDs
        //     static uint32_t last_toggle = 0;
        //     uint32_t now = System::GetNow();
        //     if (now - last_toggle > 200) {  
        //         led_state = !led_state;  // Toggle state
        //         LED_W1.Write(led_state);     
        //         LED_W2.Write(led_state);    
        //         last_toggle = now;
        //     }
        // } else {
        //     // When button released, turn both off
        //     LED_W1.Write(false);
        //     LED_W2.Write(false);
        // }

        // if (button3_current != button3_last_state) {
        //     if (button3_current) {
        //         hw.PrintLine("Button 3 pressed");
        //         hw.SetLed(true);
        //     } else {
        //         hw.PrintLine("Button 3 released");
        //         hw.SetLed(false);
        //     }
        //     button3_last_state = button3_current;
        // }
        if (button3_current != button3_last_state) {
            if (button3_current) {  // Button pressed
                chorus_state = !chorus_state;  // Toggle state
                current_led_mode = chorus_state ? CHORUS_CYCLE : OFF;
                // Send MIDI message based on new state
                uint8_t cc_chorus[3] = {0xB0, JUNO_CHORUS1, chorus_state ? 127 : 0};
                
                if (useSerialLogger) {
                    hw.PrintLine("Chorus %s", chorus_state ? "ON" : "OFF");
                } else {
                    midi.SendMessage(cc_chorus, sizeof(cc_chorus));
                }
            }
            button3_last_state = button3_current;
        }
        
        //     if (chorus_state) { // LED logic
        //     static uint32_t last_toggle = 0;
        //     static uint8_t led_position = 0;
        //     uint32_t now = System::GetNow();
            
        //     if (now - last_toggle > 150) {  // 150ms per LED
        //         // Turn off all LEDs
        //         LED_R1.Write(false);
        //         LED_R2.Write(false);
        //         LED_W1.Write(false);
        //         LED_W2.Write(false);
                
        //         // Turn on next LED in sequence
        //         switch(led_position) {
        //             case 0: LED_R1.Write(true); break;
        //             case 1: LED_R2.Write(true); break;
        //             case 2: LED_W1.Write(true); break;
        //             case 3: LED_W2.Write(true); break;
        //         }
                
        //         // Advance position
        //         led_position = (led_position + 1) % 4;
        //         last_toggle = now;
        //     }
        // } else {
        //     // Turn off all LEDs when chorus is off
        //     LED_R1.Write(false);
        //     LED_R2.Write(false);
        //     LED_W1.Write(false);
        //     LED_W2.Write(false);
        // }
        


}
int main(void)
{   
     // hw.Configure(); 

    hw.Init();
    hw.Configure();
    initButtons();
    InitFaders();
    testLEDs();
    InitEncoder();

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
   

    // bool button_pressed =  false;
    
    // hw.PrintLine("Serial Logger initialized!");

    // while(1)
    // {
    //     hw.DelayMs(1);
    //     // print to serial the fader value
    //     // hw.PrintLine("Fader Value: %f", hw.adc.GetFloat(0));


    //     bool button1_state = ! button1.Read();
    //     bool button2_state = ! button2.Read();
    //     bool button3_state = ! button3.Read();
    //     // hw.PrintLine("Logger is enabled");
    //     // hw.PrintLine("Button State: %s", button1_state ? "true" : "false");
    //     // Check button state
    //     if (button1_state != button1_last_state){
    //         button1_state = button1.Read(); // read again to make sure the state is stable
    //         if (button1_state == 0) {
    //             hw.SetLed(true);
    //             // hw.PrintLine("Button pressed ");
    //             uint8_t note_on[3] = {0x90, 60, 100};  // 0x90 = Note On channel 0, middle C, velocity 100
    //             if (useSerialLogger) {
    //                 // hw.Print("Midi message is: ");
    //                 // for (int i = 0; i < 3; i++) {
    //                 //     hw.Print("%d ", note_on[i]);
    //                 // }
    //                 hw.PrintLine("Button 1 pressed\n");
    //             }
    //             else {
    //                 midi.SendMessage(note_on, sizeof(note_on));
    //             }
    //             // hw.Print("Midi message is %d", note_on);
    //             // midi.SendMessage(note_on, sizeof(note_on));
                
    //             // Note On for middle C with velocity 100
    //         }
    //         else {
    //             hw.SetLed(false);
    //             // Button released
    //             // hw.PrintLine("Button released ");
    //             // button_pressed = false;
    //             uint8_t note_off[3] = {0x80, 60, 0};  // 0890 = Note Off channel 0, middle C, velocity 0
    //             if (useSerialLogger) {
    //                 // hw.Print("Midi message is: ");
    //                 // for (int i = 0; i < 3; i++) {
    //                 //     hw.Print("%d ", note_off[i]);
    //                 // }
    //                 hw.PrintLine("Button 1 released\n");
    //             }
    //             else {
    //                 midi.SendMessage(note_off, sizeof(note_off));
    //             }

    //             // hw.Print("Midi message is %d", note_off);
    //             // midi.SendMessage(note_off, sizeof(note_off));
    //             // midi.SendMessage(MidiEvent::NoteOff(60, 0)); // Note Off for middle C
    //         }
    //         button1_last_state = button1_state;
            
    //     }
    //    
        
    // }
    while(1) {
        hw.DelayMs(1);

        // read faders
        ReadFaders(useSerialLogger);

        // read encoder
        ReadEncoder(useSerialLogger);

        // read buttons
        ReadButtons(useSerialLogger);
        
        // oscillate LEDs
        // OscillateLEDs();
        // read accelerometer
        // ReadAccelerometer();
        UpdateLEDs();
    }
}