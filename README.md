# Langstone-V3 SDR Transceiver by Colin Durbridge G4EML

# Currently supports the Adalm Pluto or the HackRF One, the Raspberry Pi 5 and the official 7" V1 or V2 LCD Displays.

This is an experimental project to produce a simple VHF, UHF and Microwave SDR Transceiver operating on SSB CW and FM.

It was inspired by the very successful Portsdown Amateur Television system created by the British Amateur Television Club.

To install the software on a raspberry pi please follow the instructions further down the page.

**More information can also be found on the UK Microwave group wiki at https://wiki.microwavers.org.uk/Langstone_Project**

Currently only the following hardware is supported:-

- Raspberry Pi 5 (Pi 4 is not officially supported but is reported to work OK with the Pluto. Not with the HackRF)

- Official Original Raspberry Pi 7" 800 x 480 Version 1 touchscreen or Version 2 7" 1280 x 720 touchscreen.

- RPi5 to Touchscreen flat cable. (this may need to be purchased seperately as the cable supplied with the V1 Touch Screen is not suitable)

- Adalm Pluto or HackRF One SDR Modules.

- USB Audio module. Connected to loudspeaker or headphones and microphone. Devices using the CM108 Chip can also use the Volume Down button as a PTT input.

- USB Scroll mouse

- PTT via Raspberry Pi GPIO 17 (pin 11). This needs a pull up resistor to 3.3V. Grounding this pin will switch to Transmit.

- CW Key is via Raspberry Pi GPIO 18 (pin 12). This needs a pull up resistor to 3.3V. Grounding this pin will key the transmitter.

- Tx Output is via Raspberry Pi GPIO 21 (pin 40). This output goes high when the Langstone is transmitting. This can be used to switch antenna relays and amplifiers. (100ms delay included for sequencing)

- 8 Band select Outputs on GPIO 1 (pin 28), GPIO 19 (pin 35), GPIO 4 (pin 7), GPIO 25 (pin 22), GPIO 22 (pin 16), GPIO 24 (pin 18), GPIO 10 (pin 19), and GPIO 9 (Pin 21). These can be used to select external filters, amplifiers or Transverters. The state of these outputs is defined using the Band Bits setting.

- On the Adalm Pluto the TX output and first three of the Band Select outputs are also available on the Internal Pluto GPO connector. GPO0 is the Tx Output, GPO1-3 are the Band Select outputs.The main use for these is for when the Pluto is remotely mounted. Care must be taken as these pins are low voltage. They will need to be buffered before use.

To build a complete functional transceiver you will need to add suitable filters, preamplifiers and power amplifiers to the SDR Module.

All control is done using the touchscreen and mouse.

Tuning uses the mouse scrollwheel. The mouse left and right buttons select the tuning step. The centre button is used for the CW key.  Mouse movement is not used.

A mouse is used to provide the tuning input because it effectively hands the task of monitoring the tuning knob to a seperate processor (in the mouse). Rotary encoders can be tricky to handle reliably in linux.

Microphone input and headphone output uses the USB audio device. (a couple of pounds on Ebay)

The software consists of two parts. The SDR itself uses a python GNURadio Flowgraph (Lang_TRX_Pluto.py or Lang_TRX_Hack.py) which can be created on a PC running GNUradio companion with Lang_TRX_Pluto.grc or Lang_TRX_Hack.grc. This Python program is then manually edited by adding the code from ControlTRX_Pluto.py or ControlTRX_HAck.py so it can be controlled by the GUI part of the software (LangstoneGUI_Pluto.c or LangstoneGui_Hack.c). These are written in C and communicate with GNURadio using a Linux Pipe. However to build and use a Langstone transceiver you do not need to know this!



# Installation for Langstone Transceiver

The preferred installation method only needs a Windows PC connected to the same (internet-connected) network as your Raspberry Pi.  Do not connect a keyboard or HDMI display directly to your Raspberry Pi.

- Download and install the Raspberry Pi Imager utility from https://downloads.raspberrypi.org/imager/imager_latest.exe

- These instructions are based on verdsion 2.0.6 of the Imager program. Other versions may differ but you should try to acheive the same settings.

- Start the Imager Utility, Select 'Raspberry Pi 5' as the Raspberry Pi Device.

-  Select 'Raspberry Pi OS (Other)' then 'Raspberry Pi OS Lite (64 Bit)' as the operating system.

- **Note... It is important to check that you have selected the Lite version of the OS and not the Full version.**

- Insert your micro SD card into a card reader and select that device for the Storage.  Note:- Ignore and close any message boxes about reformatting the drive. This is not needed.

- When asked to choose a Hostname, you can choose anything you like or leave it at the Suggested Name.

- Set Your City, TimeZone and keyboard layout. You can normally accept the default settings.

- Set the username to 'pi' and the password to 'raspberry'

- Leave the WIFI settings Blank

- Selact 'Enable SSH' and 'Use Password authentication'

- Disable Raspberry Pi Connect

- Finally Click 'WRITE' to start writing the SD card.

- Make sure you use a good quality class 10 Micro-SD Card. (16GB is OK) The performance of the Raspberry Pi can be badly affected by poor quality cards.

- Connect the touchscreen display, USB mouse, USB Sound Card, and SDR Module.   Power up the RPi with the new card inserted, and a network connection.  Do not connect a keyboard or HDMI display to the Raspberry Pi.

- The Raspberry Pi may restart several times as it configures the SD card. Eventually it should display a full boot on the LCD.

- Find the IP address of your Raspberry Pi by looking at your internet router or by using an IP Scanner (such as Advanced IP Scanner http://filehippo.com/download_advanced_ip_scanner/ for Windows, or Fing on an iPhone) to get the RPi's IP address

- From your windows PC use Putty (http://www.chiark.greenend.org.uk/~sgtatham/putty/download.html) to open an SSH login to the IP address that you noted earlier.  You will get a Security warning the first time you try; this is normal.

- Log in (user: pi, password: raspberry) then cut and paste the following code in, one line at a time:

## For the Adalm Pluto
```sh
wget https://raw.githubusercontent.com/hb9tob/Langstone-V3/main/installPluto.sh
chmod +x installPluto.sh
./installPluto.sh
```
## For the HackRF One
```sh
wget https://raw.githubusercontent.com/g4eml/Langstone-V3/master/installHack.sh
chmod +x installHack.sh
./installHack.sh
```

The initial build can take some time, however it does not need any user input, so go and make a cup of coffee and keep an eye on the touchscreen.  When the build is finished the Pi will reboot and should start-up with the Langstone Transceiver. Occasionally a second reboot may be required. If after this it still does not appear to be working then see the Langstone wiki for some things to look at.

---

# Modifications HB9TOB (fork)

This fork adds the following changes to the original Langstone-V3 project (Pluto version).

## Touchscreen D-Pad

Four arrow buttons (35×35px) arranged as a directional pad have been added to the left side of the screen, allowing full control without a mouse.

```
     [ ^ ]
[ < ]     [ > ]
     [ v ]
```

- **Up / Down** — equivalent to the mouse scroll wheel (frequency tuning, volume, squelch, RIT, settings)
- **Left / Right** — equivalent to mouse left/right buttons (select tuning digit)
- The dial lock mechanism is not triggered when tapping Left then Right on the touchscreen

## RX chain fixes

### FM demodulator
The FM demodulator (`analog_nbfm_rx`) was incorrectly connected to the output of the 3 kHz SSB bandpass filter. It is now correctly connected to the output of the 48 kHz decimation filter.

### FFT / waterfall anti-aliasing
The FFT decimation block had no anti-aliasing filter, causing spectral aliasing and ghost signals in the waterfall. A FIR low-pass filter (`fir_filter_ccc`) with adaptive cutoff is now inserted before the decimation block. The cutoff frequency is automatically adjusted when the FFT zoom level changes:

```
cutoff = 48000 / 2^(FFT_SEL+1) × 0.9
```

## TX safety defaults

Three safety parameters have been added to the Settings menu, all **disabled by default**:

| Parameter | Default | Description |
|---|---|---|
| GPIO PTT | Disabled | Allow PTT via GPIO 17 |
| CW Key | Disabled | Allow CW key via GPIO 18 |
| Pluto TX | Disabled | Enable RF output on the Pluto |

In addition, the TX attenuation is set to its maximum value (−89 dB) at first install to prevent any unintentional RF output before the operator has verified the setup.

## EXTRA menu

A new **EXTRA** button (blue) has been added to the main function bar, between the MODE and SET buttons. It opens a submenu with advanced DSP features:

```
[ MENU ] [ NB1 ] [ COMP ] [     ] [     ] [     ] [ SET ]
```

- **NB1** (blue/red) — spectral noise reduction on the RX chain
- **COMP** (blue/red) — TX speech processor on the SSB TX chain
- **SET** (orange) — open parameter settings for NB1 or COMP

### NB1 — Spectral Noise Reduction

Pressing **NB1** dynamically inserts a spectral noise reduction chain into the audio receive path (before the audio sink). The button turns **red** when active. The original chain is fully restored when NB1 is deactivated.

**RX chain with NB1 active:**
```
... → audio_sink_0 becomes:
... → resample 48k→8k → spectral_nr_ff → resample 8k→48k → audio_sink_0
```

Requires the **gr-anr** custom GNU Radio module: https://github.com/hb9tob/gr-anr

**NB1 parameters** (accessible via SET → APPLY):

| Parameter | Default | Description |
|---|---|---|
| Algorithm | 0 (Wiener) | 0=Wiener, 1=Over-subtract, 2=MMSE |
| FFT Size | 256 | 64 / 128 / 256 / 512 / 1024 |
| Overlap | 4 | 2 / 4 / 8 / 16 |
| Alpha | 0.98 | Smoothing factor |
| Beta | 2.0 | Over-subtraction factor |
| Gain Floor | 0.010 | Minimum gain (noise floor) |

### COMP — TX Speech Processor

Pressing **COMP** dynamically inserts a speech processing chain into the SSB TX audio path, after the microphone gain block. The button turns **red** when active. The original chain is fully restored when COMP is deactivated.

**TX chain with COMP active:**
```
mic → gain → [HPF 300Hz] → [EQ1] → [EQ2] → [EQ3] → [LPF 2700Hz] → [AGC2] → modulator
```

| Stage | Description |
|---|---|
| HPF 300 Hz | IIR 1st-order high-pass — removes bass rumble and breath noise |
| EQ1/EQ2/EQ3 | 3-band parametric EQ (biquad peaking, Q=0.7) — shapes the voice spectrum |
| LPF 2700 Hz | FIR low-pass — limits SSB bandwidth |
| AGC2 | Compressor/AGC — normalises level and adds punch |

**COMP parameters** (accessible via SET → COMP> → APPLY):

| Parameter | Default | Range | Description |
|---|---|---|---|
| Attack | 0.041 | 0.001–0.500 | AGC attack rate |
| Decay | 0.033 | 0.001–0.500 | AGC decay rate |
| Ref | 0.90 | 0.10–0.99 | AGC reference level |
| Max Gain | 100 | 1–1000 | AGC maximum gain |
| LPF Cut | 2700 Hz | 500–5000 Hz | Low-pass cutoff |
| EQ1 Freq | 500 Hz | 200–4000 Hz | Band 1 centre frequency |
| EQ1 Gain | 0.0 dB | ±12.0 dB | Band 1 gain |
| EQ2 Freq | 1200 Hz | 200–4000 Hz | Band 2 centre frequency |
| EQ2 Gain | +4.0 dB | ±12.0 dB | Band 2 gain |
| EQ3 Freq | 2500 Hz | 200–4000 Hz | Band 3 centre frequency |
| EQ3 Gain | +6.0 dB | ±12.0 dB | Band 3 gain |

### Settings navigation

The NB1 and COMP parameter menus are cross-navigable via the **COMP>** / **NB1>** button (button 5). Parameters are only sent to GNU Radio when **APPLY** (button 6, orange) is pressed, avoiding unwanted rebuilds during scrolling.

---

# Updating an existing installation (HB9TOB fork)

Log into the Pi using SSH, then:

```bash
cd ~/Langstone
git fetch https://github.com/hb9tob/Langstone-V3.git main:hb9tob/main
git checkout hb9tob/main -- Lang_TRX_Pluto.py LangstoneGUI_Pluto.c
./build
```

To roll back to a specific tagged version (e.g. after a bad update):

```bash
cd ~/Langstone
git fetch https://github.com/hb9tob/Langstone-V3.git --tags
git checkout <tag-name> -- Lang_TRX_Pluto.py LangstoneGUI_Pluto.c
./build
```

Available tags: `v-ssb-audio-improve`, `v-ssb-3band-eq`

---

# Updating the Software (original instructions)

If you have a running Langstone-V3 you can update by doing the following.

Log into the Pi using SSH as described above.

cd Langstone

./stop

./update

sudo reboot
