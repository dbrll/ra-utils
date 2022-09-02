A la fin des années 1960, le marché de l'informatique est divisé entre grands-systèmes de gestion, onéreux, et mini-ordinateurs dédiés à l 'informatique industrielle tel que, pour Hewlett-Packard, le HP 2100 sorti en 1966.

HP remarqua, à leur surprise, que certains clients utilisaient le HP 2100 à une fin de gestion, jusque là l'apanage des grands-systèmes. Ce constat donna à HP l'idée de créer un mini-ordinateur dédié à la gestion. A cet effet, ce nouveau système différerait des HP 2100 en plusieurs points :
- une conception pour le temps partagé (16 à 32 utilisateurs)
- une optimisation du processeur pour les langages haut niveau, plutôt qu'un jeu d'instruction optimisé pour la facilité de développement en assembleur
- un système de gestion de base de données relationnelles intégré

Deux projets internes furent lancés pour concevoir cette machine.
Le projet Alpha repartirait du HP 2100 dans le but de le transformer en machine de gestion.
Le projet Omega partirait d'une feuille blanche et d'une architecture 32 bits à mémoire virtuelle.

Alors que le projet Omega s'avera être trop complexe et coûteux pour un développement en fonds propres, HP décida d'annuler le projet Omega en 1970.
Le projet Alpha, quant à lui, avait gagné en maturité pour devenir un "Omega" 16 bits, d'une capacité plus limitée (16 kmots de mémoire, un seul accumulateur), mais incorporant les principales innovations du projet Omega.

HP décida 


I AM SAFE
I have the right to feel safe (in an unsafe world)
CAUSES ARE NO LONGER YOURS


xt
ranger bureau (papiers...) pour avoir la tete légère
lire...


546BB7

1C1E4C


Qu'est-ce qu'un ordinateur ?

A few Unix-like utilies written in C and Go for neutral hydrogen spectral line radio-astronomy. The goal is to have minimal resource requirements and dependencies (no need for Python or GNUplot). Can also run headless. Built and tested on an Intel laptop running Alpine Linux.

- avgfft
Reads the samples from an arbitrary sized rtl_sdr output file, integrates them, and converts them to frequency bins. Requires libfftw3.
- avgpwr
Directly reads samples from an RTLSDR dongle at a given frequency and sample rate, then computes their averaged power over a period of time. Requires libfftw3.
- plot
plot the values from "proc" to a PNG file. Requires gonum/plot
- proc
Reads frequency:power values from a spectra and a reference. Those files can be generated from either avgfft, or from rtl_power_fftw if storing the IQ file isn't required. "proc" can then perform the reference division, RFI removal, slope correction, convertion to radial velocity, and finally outputs the resulting values
- radec
Translates ALTAZ pointing coordinates to RA:DEC coordinates, which can be fed to "vlsr". Requires libnova (static building by default).
- spectra.sh
Full pipeline demonstration. Aquires samples center around the HI line, dumps it to a file, run avgfft, radec, vlsr, proc, and generates a plot.
- yfact.sh
Compute the Y factor of a device. Used to determine the system temperature (Tsys)

TODO: LSR correction


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

![HI spectra](https://raw.githubusercontent.com/dbrll/ra-utils/assets/img/h1.png)

- `yfact.sh`
Compute the Y factor of a device. Used to determine the system temperature (Tsys)

Those utilities are composable and can be used for many sorts of continuum and spectral analysis, using only the command line and shell scripts.
Information and warnings are sent to stderr by default, redirecting it to /dev/null will only output the meaningful data.
Those utilities can also work remotely and headlessly.

Tested on Linux/amd64 with both musl and glibc.

## Examples
 
**Plot FM spectrum**
- Acquire 3 seconds of samples with rtl_sdr:
   `rtl_sdr -f 100000000 -n $((2048000 * 3)) output.bin`
- Average the IQ samples to 1024 frequency bins (Fourier transform):
`./avgfft output.bin -b 1024 > spectrum.txt`
- Plot the result:
`./plot -xlegend "Frequency (Hz)" -title "Power Spectral Density" spectrum.txt`

![FM plot](https://raw.githubusercontent.com/dbrll/ra-utils/assets/img/fm.png)

**Perform a survey**
Use a while loop that will:
- Acquire samples to a file (ideally sitting on a RAMdisk):

`for i in $(seq 1 1024); do rtl_sdr (...); done` 
- Convert the samples to the frequency domain, format the output to a row, and append it to a file:

`./avgfft output.bin | tr '\n' '  >> survey.txt`
- When done, plot using GNUplot:

`plot survey.txt binary array=(2048,1441) format="%float64" with image notitle`

![HI survey](https://raw.githubusercontent.com/dbrll/ra-utils/assets/img/survey.png)