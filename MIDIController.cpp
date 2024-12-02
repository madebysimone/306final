#include "daisy_seed.h"
#include "daisysp.h"
#include "hid/midi.h"
#include <random>
// #include "hid/midi_message.h"

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;


// JUN 6-V definitions
#define JUNO_DCO_LFO 77  // Verify this



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

GPIO LED_R1;
GPIO LED_R2;
GPIO LED_W1;
GPIO LED_W2;

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



// void InitI2C() {
//     // Initialize I2C




//     static constexpr I2CHandle::Config i2c_config = {
//         I2CHandle::Config::Peripheral::I2C_1, {{DSY_GPIOB, 8}, {DSY_GPIOB, 9}},
// 		I2CHandle::Config::Speed::I2C_100KHZ,  //3.3v does mushy square waves above this.
// 		I2CHandle::Config::Mode::I2C_MASTER
//         };




//     // // I2CHandle::Config i2c_config;
//     // i2c_config.periph = I2CHandle::Config::Peripheral::I2C_1;
//     // // i2c_config.pin_config.scl = hw.GetPin(D11);
//     // // i2c_config.pin_config.sda = hw.GetPin(D12);
//     // // i2c_config.speed = I2CHandle::Config::Speed::I2C_400KHZ;
//     // hw.i2c_1.Init(i2c_config);
// }

// void InitI2C()
// {
//     I2CHandle::Config i2c_cfg;
//     i2c_cfg.periph = I2CHandle::Config::Peripheral::I2C_1; // Use I2C1
//     i2c_cfg.speed  = I2CHandle::Config::Speed::I2C_400KHZ; // Fast mode
//     i2c_cfg.pin_config.scl = Pin(PORTB, 8);                // Default SCL
//     i2c_cfg.pin_config.sda = Pin(PORTB, 9);                // Default SDA
//     i2c.Init(i2c_cfg);
// }


// void InitAccelerometer() {
//     // turns on the accelerometer
//     accelerometer1_VCC.Init(D14, GPIO::Mode::OUTPUT);
//     accelerometer1_SC.Init(D13, GPIO::Mode::OUTPUT);
//     accelerometer1_VCC.Write(true); // turn on the accelerometer
//     accelerometer1_SC.Write(true); // put the ADXL345 in I2C mode
//     // accelerometer1_SDA.Init(D12, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
//     // accelerometer1_SCL.Init(D11, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
//     // Power sequence
//     uint8_t powerReg = 0x08;  // Measurement mode
//     uint8_t formatReg = 0x08;  // Full resolution, ±2g
    
//     // Wait for power-up
//     hw.DelayMs(100);
    
//     // Set power control - put in measurement mode
//     if(i2c.TransmitBlocking(ADXL345_ADDR, POWER_CTL, &powerReg, 1, 1000) != I2CHandle::Result::OK) {
//         hw.PrintLine("Failed to set power mode");
//         return;
//     }
    
//     // Set data format
//     if(i2c.TransmitBlocking(ADXL345_ADDR, DATA_FORMAT, &formatReg, 1, 1000) != I2CHandle::Result::OK) {
//         hw.PrintLine("Failed to set data format");
//         return;
//     }
    
//     hw.PrintLine("ADXL345 Initialized");
// }

// void InitAccelerometer() {
//     // Power and GPIO setup remains the same
//     accelerometer1_VCC.Init(D14, GPIO::Mode::OUTPUT);
//     accelerometer1_SC.Init(D13, GPIO::Mode::OUTPUT);
//     accelerometer1_VCC.Write(true);
//     accelerometer1_SC.Write(true);
    
//     hw.DelayMs(200); // Power-up delay


//     // Verify device is responding
//     uint8_t whoami[2] = {0x00, 0x00};  // Device ID register
//     if(i2c.TransmitBlocking(ADXL345_ADDR, whoami, 1, 1000) != I2CHandle::Result::OK) {
//         hw.PrintLine("Device not responding at address 0x%x", ADXL345_ADDR);
//         return;
//     }

//     // Setup power control register
//     uint8_t power_data[2] = {POWER_CTL, 0x08}; // Register address + data
//     if(i2c.TransmitBlocking(ADXL345_ADDR, power_data, 2, 1000) != I2CHandle::Result::OK) {
//         hw.PrintLine("Failed to set power mode");
//         return;
//     }

//     // Setup data format register
//     uint8_t format_data[2] = {DATA_FORMAT, 0x08}; // Register address + data
//     if(i2c.TransmitBlocking(ADXL345_ADDR, format_data, 2, 1000) != I2CHandle::Result::OK) {
//         hw.PrintLine("Failed to set data format");
//         return;
//     }

//     hw.PrintLine("ADXL345 Initialized");
// }

// void ScanI2C() {
//     hw.PrintLine("Scanning I2C bus...");
    
//     for(uint8_t addr = 1; addr < 128; addr++) {
//         uint8_t test_data = 0x00;
//         // Try both shifted and unshifted addresses
//         if(i2c.TransmitBlocking(addr << 1, &test_data, 1, 100) == I2CHandle::Result::OK) {
//             hw.PrintLine("Device found at address: 0x%x (7-bit) / 0x%x (8-bit)", 
//                         addr, addr << 1);
//         }
//     }

//     hw.PrintLine("I2C scan complete");
// }

// void ScanI2C() {
//     hw.PrintLine("Starting I2C bus scan...");
//     hw.PrintLine("Note: Addresses shown in hex format");
    
//     uint8_t devices_found = 0;
    
//     for(uint8_t addr = 1; addr < 128; addr++) {
//         uint8_t test_data = 0x00;
//         uint8_t addr_8bit = addr << 1;  // Convert 7-bit to 8-bit address
        
//         // Small delay between tries
//         hw.DelayMs(1);
        
//         if(i2c.TransmitBlocking(addr_8bit, &test_data, 1, 100) == I2CHandle::Result::OK) {
//             hw.PrintLine("Device found!");
//             hw.PrintLine("  7-bit address: 0x%02x", addr);
//             hw.PrintLine("  8-bit address: 0x%02x", addr_8bit);  
//             devices_found++;
//         }
//     }
    
//     if(devices_found == 0) {
//         hw.PrintLine("No I2C devices found");
//     } else {
//         hw.PrintLine("Scan complete - Found %d device(s)", devices_found);
//     }
// }


// void InitAccelerometer() {
//     // Power sequence
//     accelerometer1_VCC.Init(D14, GPIO::Mode::OUTPUT);
//     accelerometer1_SC.Init(D13, GPIO::Mode::OUTPUT);
//     accelerometer1_VCC.Write(true);
//     accelerometer1_SC.Write(true);
    
//     hw.DelayMs(300); // Extended delay for startup

//     // ScanI2C();

//     // // Try unshifted address first
//     // uint8_t whoami_reg = 0x00;
//     // if(i2c.TransmitBlocking(ADXL345_ADDR, &whoami_reg, 1, 1000) == I2CHandle::Result::OK) {
//     //     hw.PrintLine("Device responded to unshifted address");
//     // } else {
//     //     hw.PrintLine("Trying shifted address...");
//     //     if(i2c.TransmitBlocking(ADXL345_ADDR << 1, &whoami_reg, 1, 1000) != I2CHandle::Result::OK) {
//     //         hw.PrintLine("Device not responding to either address");
//     //         return;
//     //     }
//     // }
//     // 7-bit address is 0x53, shift left by 1 for 8-bit address (0xA6)
//     const uint8_t ADDR_7BIT = 0x53;
//     const uint8_t ADDR_8BIT = ADDR_7BIT << 1;
    
//     hw.PrintLine("Using 7-bit address: 0x%x (8-bit: 0x%x)", ADDR_7BIT, ADDR_8BIT);

//     // Try to read device ID
//     uint8_t whoami_reg = 0x00;
//     uint8_t whoami_val = 0;
    
//     if(i2c.TransmitBlocking(ADDR_8BIT, &whoami_reg, 1, 1000) == I2CHandle::Result::OK) {
//         if(i2c.ReceiveBlocking(ADDR_8BIT, &whoami_val, 1, 1000) == I2CHandle::Result::OK) {
//             hw.PrintLine("Device ID: 0x%x", whoami_val);
//         }
//     } else {
//         hw.PrintLine("Failed to communicate with ADXL345");
//         return;
//     }



    // // ADXL345 requires 7-bit address shifted left + R/W bit
    // uint8_t device_addr = ADXL345_ADDR << 1;
    // hw.PrintLine("Trying address: 0x%x", device_addr);

    // // Read Device ID (should be 0xE5)
    // // uint8_t whoami_reg = 0x00;
    // uint8_t whoami_val = 0;

    // // Write register address first
    // if(i2c.TransmitBlocking(device_addr, &whoami_reg, 1, 1000) != I2CHandle::Result::OK) {
    //     hw.PrintLine("Failed to write register address");
    //     return;
    // }

    // // Then read the value
    // if(i2c.ReceiveBlocking(device_addr, &whoami_val, 1, 1000) != I2CHandle::Result::OK) {
    //     hw.PrintLine("Failed to read device ID");
    //     return;
    // }

    // hw.PrintLine("Device ID: 0x%x", whoami_val);
// }

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



void ReadEncoder() {
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
            if (serialLogger) {
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


// void ReadAccelerometer() {
//     uint8_t data[6];
//     uint8_t reg = DATAX0;
    
//     // Read all axis data at once (6 bytes)
//     if(i2c.TransmitBlocking(ADXL345_ADDR, &reg, 1, 1000) != I2CHandle::Result::OK) {
//         hw.PrintLine("Failed to send register address");
//         return;
//     }
    
//     if(i2c.ReceiveBlocking(ADXL345_ADDR, data, 6, 1000) != I2CHandle::Result::OK) {
//         hw.PrintLine("Failed to read accelerometer data");
//         return;
//     }

//     // Convert raw data to 16-bit integers
//     int16_t x = (data[1] << 8) | data[0];
//     int16_t y = (data[3] << 8) | data[2];
//     int16_t z = (data[5] << 8) | data[4];

//     // Convert to g-forces
//     float x_g = x * SCALE_FACTOR;
//     float y_g = y * SCALE_FACTOR;
//     float z_g = z * SCALE_FACTOR;

//     // Print values (multiplied by 1000 for better readability)
//     hw.PrintLine("X: %d Y: %d Z: %d", 
//         (int)(x_g * 1000), 
//         (int)(y_g * 1000), 
//         (int)(z_g * 1000));
// }


void ReadFaders() {
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
    float knob1_value = hw.adc.GetFloat(3);
    float knob2_value = hw.adc.GetFloat(4);
    float knob3_value = hw.adc.GetFloat(5);
    
    // Check if values changed more than threshold
    if (fabsf(fader1_value - last_fader1) > threshold) {
        int fader1_int = (int)(fader1_value * 1000);
        hw.PrintLine("Fader1: %d", fader1_int); // TODO: dirty ground right now, need to fix
        last_fader1 = fader1_value;
    }
    
    if (fabsf(fader2_value - last_fader2) > threshold) {
        int fader2_int = (int)(fader2_value * 1000);
        hw.PrintLine("Fader2: %d", fader2_int);
        last_fader2 = fader2_value;
    }
    if (fabsf(fader3_value - last_fader3) > threshold) {
        int fader3_int = (int)(fader3_value * 1000);
        hw.PrintLine("Fader3: %d", fader3_int);
        last_fader3 = fader3_value;
    }
    if (fabsf(knob1_value - last_knob1) > threshold) {
        int knob1_int = (int)(knob1_value * 1000);
        hw.PrintLine("Knob1: %d", knob1_int);
        last_knob1 = knob1_value;
    }
    if (fabsf(knob2_value - last_knob2) > threshold) {
        int knob2_int = (int)(knob2_value * 1000);
        hw.PrintLine("Knob2: %d", knob2_int);
        last_knob2 = knob2_value;
    }
    if (fabsf(knob3_value - last_knob3) > threshold) {
        int knob3_int = (int)(knob3_value * 1000);
        hw.PrintLine("Knob3: %d", knob3_int);
        last_knob3 = knob3_value;
    }

    // // just print the fader values for now
    // int fader1_int = (int)(fader1_value * 1000);
    // int fader2_int = (int)(fader2_value * 1000);
    // hw.PrintLine("Fader1: %d, Fader2: %d", fader1_int, fader2_int);
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
    // InitI2C();
    // ScanI2C();
    // InitAccelerometer();










    // InitAccelerometer();
    //This is our ADC configuration
    // AdcChannelConfig fader1cfg;
    // AdcChannelConfig fader2cfg;
    //Configure pin 21 as an ADC input. This is where we'll read the knob.
    // fader1cfg.InitSingle(hw.GetPin(15));
    // fader2cfg.InitSingle(hw.GetPin(16));
    // AdcChannelConfig faderCfg[2] = {fader1cfg, fader2cfg};

    //Initialize the adc with the config we just made
    // hw.adc.Init(&faderCfg, 2);
    //Start reading values
    // hw.adc.Start();


    // hw.StartLog();
    // hw.DelayMs(100);
    // hw.PrintLine("Serial Logger started!");

    bool useSerialLogger = true;

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

    // button_pressed = true;

    // hw.DelayMs(2000);
    
    // hw.PrintLine("Serial Logger initialized!");
    bool button1_last_state = true;
    bool button2_last_state = true;
    bool button3_last_state = true;
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
    //     if (button2_state != button2_last_state){
    //         button2_state = button2.Read(); // read again to make sure the state is stable
    //         if (button2_state == 0) {
    //             hw.SetLed(true);
    //             // hw.PrintLine("Button pressed ");
    //             uint8_t note_on[3] = {0x90, 60, 100};  // 0x90 = Note On channel 0, middle C, velocity 100
    //             if (useSerialLogger) {
    //                 // hw.Print("Midi message is: ");
    //                 // for (int i = 0; i < 3; i++) {
    //                 //     hw.Print("%d ", note_on[i]);
    //                 // }
    //                 hw.PrintLine("Button 2 pressed\n");
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
    //                 hw.PrintLine("Button 2 released\n");
    //             }
    //             else {
    //                 midi.SendMessage(note_off, sizeof(note_off));
    //             }

    //             // hw.Print("Midi message is %d", note_off);
    //             // midi.SendMessage(note_off, sizeof(note_off));
    //             // midi.SendMessage(MidiEvent::NoteOff(60, 0)); // Note Off for middle C
    //         }
    //         button2_last_state = button2_state;
            
    //     }
        
    // }
    while(1) {
        hw.DelayMs(1);

        // Read current button states (invert because of pull-up)
        bool button1_current = !button1.Read();
        bool button2_current = !button2.Read();
        bool button3_current = !button3.Read();

        // Only handle state changes
        if (button1_current != button1_last_state) { // button 1 is to momentary send a random value generated from the read encoder to the DCO LFO of the VST
            if (button1_current) {
                
                // uint8_t note_on[3] = {0x90, 60, 100};  // 0x90 = Note On channel 0, middle C, velocity 100
                uint8_t cc[3] = {0xB0, JUNO_DCO_LFO, rand() % 128};  // 0xB0 = CC channel 0, DCO LFO, random value
                if (useSerialLogger) {
                    hw.PrintLine("Button 1 pressed:  Sending random value to DCO LFO");
                    hw.Print("Midi message is: ");
                    for (int i = 0; i < 3; i++) {
                        hw.Print("%d ", cc[i]);
                    }
                    hw.PrintLine("Button 1 pressed\n");
                }
                else {
                    midi.SendMessage(cc, sizeof(cc));
                }
                // hw.SetLed(true);
                // flash red leds
                while (button1_current) {
                    LED_R1.Write(true);
                    LED_R2.Write(false);
                    hw.DelayMs(100);
                    LED_R1.Write(false);
                    LED_R2.Write(true);
                    hw.DelayMs(100);
                    button1_current = !button1.Read();
                }
            } else {
                hw.PrintLine("Button 1 released: stopping random DCO LFO");
                // hw.SetLed(false);
            }
            button1_last_state = button1_current;
        }

        if (button2_current != button2_last_state) {
            if (button2_current) {
                hw.PrintLine("Button 2 pressed");
                hw.SetLed(true);
            } else {
                hw.PrintLine("Button 2 released");
                hw.SetLed(false);
            }
            button2_last_state = button2_current;
        }
        if (button3_current != button3_last_state) {
            if (button3_current) {
                hw.PrintLine("Button 3 pressed");
                hw.SetLed(true);
            } else {
                hw.PrintLine("Button 3 released");
                hw.SetLed(false);
            }
            button3_last_state = button3_current;
        }


        // read faders
        ReadFaders();

        // read encoder
        ReadEncoder();

        // read accelerometer
        // ReadAccelerometer();
    }
}