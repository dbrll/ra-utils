/*
* Reads the samples from an arbitrary sized uint_8 IQ file,
* integrates them, and converts them to frequency bins.
* Requires libfftw3.
*
* Copyright (C) 2022 Damien Boureille
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<stdbool.h>
#include<stdint.h>
#include<math.h>
#include<fftw3.h>
#include<complex.h>

static int nbins = 512;
static fftw_complex* in;
static fftw_complex* out;
static fftw_plan p;

int help(void) {
        printf("Usage: fft [-b 64-8192] [-d ] FILENAME\n"                                
        "[-b frequency bins (default: 512)]\n"
        "[-d outputs decibels (not default)]\n"); 
        exit(0);                                        
}

double* fft(uint8_t* data, int count, double* power_spectrum) {
        double* power_spectrum_tmp = (double* ) malloc(sizeof(double) * nbins + 1);
        fftw_complex *cx_power_spectrum = (fftw_complex* ) malloc(sizeof(fftw_complex) * nbins + 1);
        int w = 0;
        int x = 0;

        // Convert RTLSDR IQ to complex
        for (int i = 0; i < nbins * 2; i+=2) {
                in[x][0] = (data[i]/(255.0/2.0))-1.0; 
                in[x][1] = (data[i+1]/(255.0/2.0))-1.0;
                x++;
        }

        fftw_execute(p);

        // c = conj(c)*c
        for (int i = 0; i< nbins; i++){
                double complex complex_tmp = out[i][0] + out[i][1]*I;
                complex_tmp = conj(complex_tmp) * complex_tmp;
                cx_power_spectrum[w][0] = creal(complex_tmp) ;
                cx_power_spectrum[w][1] =  cimag(complex_tmp);
                w++;
        }
    
        //complex to double
        for (int i = 0; i < nbins; i++)
                power_spectrum[i] = cx_power_spectrum[i][0];

        free(power_spectrum_tmp);
        free(cx_power_spectrum);

        return power_spectrum;
}

int main(int argc, char *argv[], const char** optstring){

        if (argc == 1)
                help();
        int opt;
        int i = 0;
        int d = 0;
        
        while((opt = getopt(argc, argv, "b:dh")) != -1){                     
                switch(opt){                                                  
                         case 'b':                                                
                                i = (int) strtol(optarg, (char**) NULL, 10);     
                                if (i >= 63 && i <= 8192)
                                        nbins = i; 
                                else {
                                        fprintf(stderr, "Frequency bins outside allowed range\n");
                                        exit(0);
                                }                                
                                break;                                           
                        case 'd':
                                d = 1;
                                fprintf(stderr,"Using decibels\n");
                                break;
                        case 'h':
                                help();
                }                                                                
        }

        FILE* fp;
        const char* file_in = argv[optind];
    
        if (! (fp = fopen(file_in, "r"))) {
                printf("Could not open %s. Exiting.\n", file_in);
                return 1;
        }

        /* Determines the file size and how many FFT chunks will fit */
        size_t fsize = 0, total_chunks = 0;
        while (fgetc(fp) != EOF )
                fsize++;
        fseek(fp, 0, SEEK_SET);
        total_chunks = (int) fsize / (nbins * 2);
        
        // fftw buffers
        in = (fftw_complex* ) fftw_malloc(sizeof(fftw_complex) * nbins + 1);
        out = (fftw_complex* ) fftw_malloc(sizeof(fftw_complex) * nbins + 1);
        p = fftw_plan_dft_1d(nbins, in , out, FFTW_FORWARD, FFTW_ESTIMATE);

        // main buffers
        uint8_t* chunk = calloc(nbins, nbins * 2 * sizeof(uint8_t));
        double* fft_tmp = calloc(nbins, nbins * sizeof(double));
        double* fft_final = calloc(nbins, nbins * sizeof(double));

        // For each chunk in the IQ file
        for (int c=0; c<total_chunks; c++){
                //fprintf(stderr, "Processing chunk %i/%i\n", c, total_chunks);
                // Create a chunk
                for (int i=0; i<nbins*2; i++)
                        chunk[i] = (uint8_t) fgetc(fp);
                //Transform it, then add it
                fft(chunk, nbins, fft_tmp);
                for (int i=0; i<nbins; i++)
                        fft_final[i] += fft_tmp[i];
        }
        free(chunk);
        
        //FFT shift
        for (int i=0; i<nbins; i++)
                 fft_tmp[i] = fft_final[i];
        for (int i = 0; i < nbins/2; i++)
                fft_final[i] = fft_tmp[i+(nbins/2)];
        for (int i = 0; i < nbins/2; i++)
                fft_final[i+(nbins/2)] = fft_tmp[i];
        free(fft_tmp);

        // Convert to dB
        if (d)
                for (int i=0; i<nbins; i++)
                        fft_final[i] = 20 * log10(fft_final[i]);
        

        for (int i=0; i<nbins; i++)
                printf("%lf\n", fft_final[i] - 127.34);
        
        free(fft_final);
        return 0;
}
