# ra-utils

Unix-like general purpose spectral processing utilities.
Those utilities are designed for minimal resource requirements and dependencies. They can be built statically and do not require Python, GNU Radio or any heavy framework.

The set is composed of the following binaries:

- `avgfft`
Reads the samples from an arbitrary sized rtl_sdr output file, integrates them, and converts them to frequency bins. Requires libfftw3.
- `avgpwr`
Reads samples from an RTLSDR dongle at a given frequency and sample rate, then computes their averaged power over a period of time. Requires libfftw3.
- `proc`
Reads the frequency/power values from a spectum and a reference file. Those files can be generated from either avgfft or from rtl_power_fftw if storing the IQ file isn't required. `proc` can then perform the reference division, RFI removal, slope correction, convertion to radial velocity, and finally outputs the resulting values
- `plot`
Plots the values from "proc" to a PNG file. Requires gonum/plot.
- `radec`
Translates ALTAZ pointing coordinates to RA:DEC coordinates, which can be fed to `vlsr`. Requires libnova (static building possible).
- `spectra.sh`
Full pipeline demonstration. Aquires samples center around the HI line, dumps it to a file, acquires a reference, runs avgfft, proc, and generates a plot. MISSING: VLSR correction.
- `yfact.sh`
Compute the Y factor of a device. Used to determine the system temperature (Tsys)

Those tools are composable and can be used for many sorts of continuum and spectral analysis and surveys, using only shell scripts. They can also work headlessly.

Tested on Linux/amd64 with both musl and glibc.

