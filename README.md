
# ra-utils

Unix-like general purpose spectral processing utilities.
Those utilities are designed for minimal resource requirements and dependencies. They can be built statically and do not require Python, GNU Radio or any heavy framework.

The set is composed of the following binaries:

- `avgfft`
Reads the samples from an arbitrary sized uint_8 IQ file, integrates them, and converts them to frequency bins. Requires libfftw3.
- `avgpwr`
Reads samples from an RTLSDR dongle at a given frequency and sample rate, then computes their averaged power over a period of time. Requires libfftw3.
- `proc`
Reads the frequency/power values from a spectum and a reference file. Those files can be generated from either avgfft or from rtl_power_fftw if storing the IQ file isn't required. `proc` can then perform the reference division, RFI removal, slope correction, conversion to radial velocity, and finally outputs the resulting values
- `plot`
Plots the values from "proc" to a PNG file. Requires golang and gonum/plot. `plot`is freestanding, it does not require GTK, QT, Cairo or any toolkit. 
- `radec`
Translates ALT-AZ pointing coordinates to RA:DEC coordinates, which can be fed to `vlsr`. Requires libnova (static building possible).
- `spectra.sh`
Full pipeline demonstration. Acquires samples center around the HI line, dumps it to a file, acquires a reference, runs avgfft, proc, and generates a plot. MISSING: VLSR correction.
After feeding the data through `plot`:

<img src="https://raw.githubusercontent.com/dbrll/ra-utils/assets/img/h1.png" alt="drawing" width="400"/>

- `yfact.sh`
Compute the Y factor of a device. Used to determine the system temperature (Tsys)

Those utilities are composable and can be used for many sorts of continuum and spectral analysis, using only the command line and shell scripts.
Information and warnings are sent to stderr by default, redirecting it to /dev/null will only output the meaningful data.
Those utilities can also work remotely and headlessly.

Tested on Linux/amd64 with both musl and glibc.

## Examples
 
**Plot FM spectrum**
- Acquire 3 seconds of samples with rtl_sdr:

$rtl_sdr -f 100000000 -n $((2048000 * 3)) output.bin`
- Average the IQ samples to 1024 frequency bins (Fourier transform):

`./avgfft output.bin -b 1024 > spectrum.txt`

- Plot the result:
`./plot -xlegend "Frequency (Hz)" -title "Power Spectral Density" spectrum.txt`

<img src="https://raw.githubusercontent.com/dbrll/ra-utils/assets/img/fm.png" alt="drawing" width="400"/>

**Perform a survey**

Use a while loop that will:
- Acquire samples to a file (ideally sitting on a RAMdisk):

`for i in $(seq 1 1024); do rtl_sdr (...); done` 
- Convert the samples to the frequency domain, format the output to a row, and append it to a file:

`./avgfft output.bin | tr '\n' '  >> survey.txt`
- When done, plot using GNUplot:

`plot survey.txt binary array=(2048,1441) format="%float64" with image notitle`

<img src="https://raw.githubusercontent.com/dbrll/ra-utils/assets/img/survey.png" alt="drawing" width="400"/>