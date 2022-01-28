## Project History



### PSXTAL V1.00: For PU8 / PU18 with XTALS (not later models using a clock synth circuit).
Uses a buttom combo (L1+R1+Circle, held for ~5 seconds) to switch video modes.
Reset on D2. Uses 2 physical XTALS on a PCB, the PCB doubles up all traces to the crystal oscillators EXCEPT the
power rail. The Arduino simply switches the power rails between the crystals, either activating/de-activating
them, so that one is active at a time. Stores the last video mode in EEPROM and retrieves last used video mode/ re-activates
the correct crystal at next boot. Have done complete play throughs on Parasite Eve and half of Parasite Eve 2, no issues. Have also installed
on multiple pre-modded consoles with no issues. Reset (IGR) combos are in the INO.



### PSXTAL V1.00A: For PU8 / PU18 with XTALS (not later models using a clock synth circuit).
Uses a buttom combo (L1+R1+Circle, held for ~5 seconds) to switch video modes.
Reset on D2. 
Uses an SI5351 clock synth board. Stores the last video mode in EEPROM and retrieves last used video mode/ programs the
SI5351 to last used mode on boot. Have installed on pre-modded consoles with no issues. Reset (IGR) combos are in the INO.



### PSXTAL V1.00B: For PU8 / PU18 with XTALS (not later models using a clock synth circuit).
Auto detection of regions for video mode switching. 
Video mode sense pin D2, Reset D9. 
Uses an SI5351 clock synth board. Reset (IGR) combos are in the INO.



### PSXTAL-3FO: For PU20, PU22, PU23 and PSOne models: At this stage doesn't offer In-Game-Reset.
Provides CPU Clock, GPU Clock and Sub-Carrier clocks. 
Automatically senses and switches between the correct GPU clock AND Sub-Carrier clocks depending
on game region detection (automatic via sense pin). Skips the normal NTSC443 mode at boot (on PAL models)
and launches straight into the native consoles video mode. This has being used a lot on a personal console
with a Mayumi V4 (didn't want 2 arduinos in the system) with zero issues.
