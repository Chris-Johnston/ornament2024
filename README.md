# Xmas Ornament 2024

Another ATTiny404-based LED Xmas tree ornament.

This is a continuation of a series of these boards that I've worked on in [2023], [2022], and [2021].

[2023]: https://github.com/Chris-Johnston/ornament2023/
[2022]: https://github.com/Chris-johnston/ornament2022
[2021]: https://github.com/Chris-Johnston/snowflake-ornament

This year's iteration was pretty similar to [2023]'s wreath-shaped ornament. Some new things I tried this year were:

- Another new design, largely adapted from a stock image library
- Using negative solder mask (solder mask mask?) to include exposed pads for aesthetic effect
- Simplified the design and opted for single color LEDs, instead of the bi-color ones from the last 2 years

Of these, the new design came out well and the exposed pads look nice. I used these on another board this year and I think I'll continue to do so in the future.

# Timeline

Last year I said I was in a time crunch, but this year I waited even longer to start, and had plenty of other things going on.

| Date | |
| - | - | 
| TODO | Started working on the first design revision |
| TODO | Scrapped first design revision to work on another |
| November 23 2024 | Ordered parts. |
| December 1 2024 | Ordered circuitboards. |
| December 2 2024 | Parts arrive. |
| December 12 2024 | Circuitboards arrive. | 
| December 14 2024 | Put the first one together, LEDS are way too bright. |
| December 20 2024 | Replaced resistors to address LED brightness problem. |
| December 21 2024 | Assembled 15 boards |
| December 23 2024 | Still working on the code |

# Resources

- [svg2mod](https://github.com/svg2mod/svg2mod) was used to generate the footprint of the board, silkscreen layers, and negative soldermask.
  - I used another vector editor to create the designs, exported these to Inkscape, and then used `svg2mod` to create the footprints.

I was able to reuse [my UPDI board for programming these boards from 2 years ago](https://github.com/Chris-johnston/ornament2022?tab=readme-ov-file#changes-from-last-years-version).

# Bill of Materials

| Reference | Description | Part No. | Price |
| -- | -- | -- | -- |
| PCB | Made via fabricator in China. | | Absurdly cheap |
| C1 | 1 uf 1206 capacitor | CL31B105KAHNNNE | $0.0534 |
| D1-D6 | 1206 White LED | TODO | |
| R1-R6 | 1206 1K Resistor | Generic | |
| SW1 | 6x6mm Pushbutton | Generic | |
| SW2 | Slide Switch | JS102011SAQN | |
| U1 | ATTiny404 | Attiny404 | | 
| B1 | CR2023 Battery Holder | BH-25C-1 | |

Unit cost without shipping: **TODO**

# Oops

In redoing the circuit this year (since I only wanted to use single color LEDs), I thought it would be simpler to only use Digital I/O pins on PORTA of the ATTiny404. I also misread the ATTiny404 manual and thought that each of these pins had PWM. This wasn't correct, these pins had a ADC but not PWM outputs.

To add to the problem, the LEDs I had picked out this time around were _absurdly_ bright. This could be a skill issue, but I found it hard to find in-stock and cheap surface-mount white LEDs which were bright enough to *indicate* but not *illuminate*. As a result, the ones I ordered were some high number of lumens in output. This year was a time crunch, and so I ordered the resistors I thought I had needed with the LEDs without testing them.

This meant that I was stuck with white LEDs which were way too bright, on pins that didn't built-in PWM.

To fix this I ended up implementing a software PWM implementation. It relies on one of the ATTiny404's timer interrupts, which updates a timer, and then updates the state each of the LED's I/O. This seems to work fine so long as the timer interrupt consistently fires.

In setting up the ISR, I unintentionally messed with the timer used to track runtime, used by `millis()`. Dividing this value by 8 first solves the problem to the extent that I care to fix it.