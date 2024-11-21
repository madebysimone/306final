#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;

// Pin for the button
#define BUTTON_PIN 0 // Replace with your GPIO pin number

bool button_pressed = false;

void Setup()
{
    hw.Configure();
    hw.Init();

    // Initialize MIDI over USB
    hw.usb_handle.Init(UsbHandle::FS_INTERNAL);

    // Configure GPIO pin as input
    dsy_gpio pin_cfg;
    pin_cfg.mode = DSY_GPIO_MODE_INPUT;
    pin_cfg.pull = DSY_GPIO_PULLUP;
    pin_cfg.pin = hw.GetPin(BUTTON_PIN);
    dsy_gpio_init(&pin_cfg);
}

void SendMidiMessage(uint8_t status, uint8_t data1, uint8_t data2)
{
    uint8_t midi_message[3] = {status, data1, data2};
    hw.usb_handle.Transmit(midi_message, 3);
}

void Loop()
{
    // Read button state
    bool current_state = !dsy_gpio_read(&hw.GetPin(BUTTON_PIN)); // Inverted logic for pull-up

    if (current_state && !button_pressed)
    {
        button_pressed = true;

        // Send MIDI Note On (Channel 1, Note 60, Velocity 127)
        SendMidiMessage(0x90, 60, 127);
    }
    else if (!current_state && button_pressed)
    {
        button_pressed = false;

        // Send MIDI Note Off (Channel 1, Note 60, Velocity 0)
        SendMidiMessage(0x80, 60, 0);
    }

    // Delay to debounce button
    hw.Delay(10);
}

int main(void)
{
    Setup();
    while (1)
    {
        Loop();
    }
}
