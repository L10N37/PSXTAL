## Project History



### PSXTAL V1.00: For PU8 / PU18 with XTALS (not later models using a clock synth circuit).<br />
Uses a buttom combo (L1+R1+Circle, held for ~5 seconds) to switch video modes.<br />
Reset on D2.<br />
Uses 2 physical XTALS on a PCB, the PCB doubles up all traces to the crystal oscillators EXCEPT the
power rail. The Arduino simply switches the power rails between the crystals, either activating/de-activating
them, so that one is active at a time. Stores the last video mode in EEPROM and retrieves last used video mode/ re-activates
the correct crystal at next boot.<br />
Have done complete play throughs on Parasite Eve and half of Parasite Eve 2, no issues. Have also installed
on multiple pre-modded consoles with no issues. Reset (IGR) combos are in the INO.<br />
<br />
<br />
<br />
### PSXTAL V1.00A: For PU8 / PU18 with XTALS (not later models using a clock synth circuit).<br />
Uses a buttom combo (L1+R1+Circle, held for ~5 seconds) to switch video modes.<br />
Reset on D2.<br />
Uses an SI5351 clock synth board. Stores the last video mode in EEPROM and retrieves last used video mode/ programs the<br />
SI5351 to last used mode on boot. Have installed on pre-modded consoles with no issues. Reset (IGR) combos are in the INO.<br />
<br />
<br />
<br />
### PSXTAL V1.00B: For PU8 / PU18 with XTALS (not later models using a clock synth circuit).<br />
Auto detection of regions for video mode switching.<br />
Video mode sense pin D2, Reset D9.<br />
Uses an SI5351 clock synth board.<br />
Reset (IGR) combos are in the INO.<br />
<br />
<br />
<br />
### PSXTAL-3FO: For PU20, PU22, PU23 and PSOne models: At this stage doesn't offer In-Game-Reset.<br />
Provides CPU Clock, GPU Clock and Sub-Carrier clocks.<br />
Automatically senses and switches between the correct GPU clock AND Sub-Carrier clocks depending<br />
on game region detection (automatic via sense pin). Skips the normal NTSC443 mode at boot (on PAL models)<br />
and launches straight into the native consoles video mode.<br />
This has being used a lot on a personal console with a Mayumi V4 (didn't want 2 arduinos in the system) with zero issues.
<br />
Note: PU22+ folder was named mistakingly, it should have being PU20+, hence at this stage 2 folders from when it was realised.<br />
<br />
<br />
### PSXTAL V1.00Bx: For PU8 / PU18 with XTALS (not later models using a clock synth circuit).<br />
Auto detection of regions for video mode switching.<br />
Video mode sense pin D2, Reset D9<br />
NTSC XTAL PIN (From PSXTAL Dual XTAL PCB) to D16/A2<br />
PAL XTAL PIN (From PSXTAL Dual XTAL PCB) to D15/A1<br />
Uses the PSXTAL Dual XTAL PCB from the original V1.00.<br />
Reset (IGR) combos are in the INO (They're the same as other versions).<br />
The NTSC and PAL functions differ slightly from the original V1.00. Rather than writing one pin low (to deactivate) and one pin high (to activate) on region switches, it instead toggles the unused pin to an input and the used pin to an output (The analogue/digital pins used still had voltage on them when written low, so the unused XTAL was receiving a small amount of current, causing interference).
<br />
<br />
<br />
### Photos are in folders of example installs/ controller wiring for D10,D11,D13 (You can also wire directly to the rear of the controller plugs input PCB). There are also wiring photos for the SI5351 clock synth board. You can use the SO pin from DFO install diagrams to run the sense pin to on auto sensing of region versions. On PU8 kynar wire fits through a small slit in the board that has nothing underneath it (when using DFO diagram point), otherwise use the same via on the top of the board. On PU18 there's a demonstration video on youtube showing the preferred sense pin location on the console mainboard.
