Project History



PSXTAL V1.00: For PU8 / PU18 with XTALS and not later models using a clock synth circuit.
Uses a buttom combo (L1+R1+Circle, held for ~5 seconds) to switch video modes.
Reset on D2. Uses 2 physical XTALS on a PCB, the PCB doubles up all traces to the crystal oscillators EXCEPT the
power rail. The Arduino simply switches the power rails between the crystals, either activating/de-activating
them, so that one is active at a time. Stores the last video mode in EEPROM and retrieves last used video mode/ re-activates
the correct crystal at next boot.



PSXTAL V1.00A: For PU8 / PU18 with XTALS and not later models using a clock synth circuit.
Uses a buttom combo (L1+R1+Circle, held for ~5 seconds) to switch video modes.
Reset on D2. 
Uses an SI5351 clock synth board.



PSXTAL V1.00B: For PU8 / PU18 with XTALS and not later models using a clock synth circuit.
Auto detection of regions for video mode switching. 
Video mode sense pin D2, Reset D9. 
Uses an SI5351 clock synth board.



PSXTAL-3FO: For PU20, PU22, PU23 and PSOne models: At this stage doesn't offer In-Game-Reset.
Provides CPU Clock, GPU Clock and Sub-Carrier clocks. 
Automatically senses and switches between the correct GPU clock AND Sub-Carrier clocks depending
on game region detection (automatic via sense pin).
