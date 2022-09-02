/*
* Reads samples from an RTLSDR dongle at a given frequency and sample rate,
* then computes their averaged power over a period of time.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <rtl-sdr.h>

int main(int argc, char *argv[], const char** optstring){

        int opt;
        int freq=408000000;
        int sec=2;
        int i=0;

        while((opt = getopt(argc, argv, "hf:t:")) != -1){
                switch(opt){
                        case 'f':
                                i = (int) strtol(optarg, (char**) NULL, 10);
                                if (i > 28 && i < 1700)
                                        freq = i * 1000000;
                                else {
                                        fprintf(stderr, "Invalid frequency\n");
                                        return 0;
                                } 
                                break;
                        case 't':
                                i = (int) strtol(optarg, (char**) NULL, 10);
                                if (i >= 1 && i <= 4)
                                        sec = i;
                                else
                                        fprintf(stderr, "Invalid integration time\n"
                                                "Defaulting to 2s\n");
                                break;
                        case 'h':
                                printf("Usage:\n"
                                "[-f center frequency in MHz (default: 408)]\n"
                                "[-t integration time (1..5) (default: 2s)]\n");
                                return 0;
                                break;
                }
        } 

        int retval;
        int devices;
        static rtlsdr_dev_t *dev;
        int samples_read;
        int n;
        uint8_t *buf;
        int sr = 2048000;
        long nsamples = sr*sec;

        devices = rtlsdr_get_device_count();
        
        for(n=0;n<devices;n++){
                fprintf(stderr,"Device %d: %s\n",devices,rtlsdr_get_device_name(n));
        }
        
        if (devices>0){
                //open last device, first if only one connected
                retval = rtlsdr_open(&dev, n-1);        
        }else{
                fprintf(stderr,"No Devices found...!");
                exit(0);
        }

        retval = rtlsdr_set_sample_rate(dev, sr);
        fprintf(stderr,"srate: %d\n",rtlsdr_get_sample_rate(dev));
        retval = rtlsdr_set_center_freq(dev, freq);
        fprintf(stderr,"freq: %d\n", rtlsdr_get_center_freq(dev));
        // Set gain. 0 for AGC, 1 for manual
        retval = rtlsdr_set_tuner_gain_mode(dev, 1);
        retval = rtlsdr_set_tuner_gain(dev, 402);
        fprintf(stderr,"gain: %d\n", rtlsdr_get_tuner_gain(dev));
        retval = rtlsdr_reset_buffer(dev);
        fprintf(stderr, "time: %is\n", sec);

        buf = malloc(nsamples  * sizeof(uint8_t));
        retval = rtlsdr_read_sync(dev, buf, nsamples, &samples_read);
        if (retval < 0) {
                fprintf(stderr,"Samples read: %d\n",samples_read);
                fprintf(stderr,"read failed!\n");
                exit(0);
        }

        rtlsdr_close(0);
        double avg = 0;
        uint8_t sample = 0;
        for(int i=0; i<nsamples; i++){
                sample = (buf[i] - 127) * (buf[i] - 127);
                //printf("%i ", sample);
                avg += sample;
        }
        avg /= sr*sec;
        fprintf(stderr,"Averaged power at %iHz (%i seconds): ", freq, sec);
        printf("%f\n", avg);
        return 0;
}


