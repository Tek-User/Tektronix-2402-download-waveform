# Tektronix-2402-download-waveform
DOS software to run on a Tektronix 2402 2402A to read waveforms from the oscilloscope

This is a Borland Turbo-C V2.x or V3.x project.  It is intended to run under DOS.  the code can easily be ported to more modern versions of C and run on other machines.

This includes an example of the config.sys tile which loads the National Instruments GPIB 488.1 driver.  This is very hard to find on the NI website, so I include it here.  In addition, I include the NI-488.1 C Library object file for the C program.

This code uses a polled technique to listen to the oscilloscope and respond when one of the oscilloscope user buttons is pushed to inititiate the transfer of the waveform(s).  They will be saved in scalable vector graphics (SVG) format on drove A, B, or C.

It is possible to modify the code to support waiting for the oscilloscope to assert the SRQ signal, and then read the waiting event.  