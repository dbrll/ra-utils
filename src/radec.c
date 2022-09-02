/*
* Translates ALT-AZ pointing coordinates to RA:DEC coordinates, which 
* can be used to compute the LSR correction.
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
#include<string.h>
#include<unistd.h>
#include<getopt.h>
#include<libnova/libnova.h>

void help(void) {
	printf(	"Usage: radec \n"
			"--date \"YYYY-MM-DD HH:MM:SS\" \n"
			"--lat [decimal obs latitude]\n"
			"--long [decimal obs longitude]\n"
			"--alt [decimal target altitude]\n"
			"--az [decimal target azimuth]\n"
			"All parameters are mandatory\n");			
	exit(0);
}

int main (int argc, char *const argv[]) {

	static struct option long_options[] =
	{
	    {"lat", required_argument, NULL, 'l'},
	    {"long", required_argument, NULL, 'L'},
	    {"alt", required_argument, NULL, 'A'},
	    {"az", required_argument, NULL, 'a'},
	    {"date", required_argument, NULL, 'd'},
	    {"help", no_argument, NULL, 'h'},
	    {NULL, 0, NULL, 0}
	};

	int opt = 0;
	int long_index = 0;
	char* date = NULL;
	double lat = 0.0;
	double lng = 0.0;
	double az = 0.0;
	double alt = 0.0;


	while ((opt = getopt_long(argc, argv, "l:L:a:A:d:", long_options, &long_index)) != -1) {
	    switch (opt) {
	            case 'l':
		            	lat = strtod(optarg, (char**) NULL);
		         		break;
	         	case 'L':
		            	lng = strtod(optarg, (char**) NULL);
		         		break;
	            case 'a':
		            	az = strtod(optarg, (char**) NULL);
		         		break;
	         	case 'A':
		            	alt = strtod(optarg, (char**) NULL);
		         		break;
	         	case 'd':
		            	date = strdup(optarg);
		         		break;
		        case 'h':
		        		help();
	    				break;
	    }
	}

	//printf("long: %f lat: %f alt: %f az: %f date: %s\n", lng, lat, alt, az, date);

	if ( (!date) || (!lng) || (!lat) || (!alt) || (!az) )
		help();

	char* time=NULL;
	int year=0;
	int month=0;
	int day=0;
	int hour=0;
	int minutes=0;
	int seconds=0;

	date = strtok(date, " ");
	time = strtok(NULL, " ");

	// Probably not the most elegant string extraction...
	year    = (int) strtol(strtok(date,"-"), (char**) NULL,5);
	month   = (int) strtol(strtok(NULL,"-"), (char**) NULL,3);
	day     = (int) strtol(strtok(NULL,"-"), (char**) NULL,3);

	hour    = (int) strtol(strtok(time,":"), (char**) NULL,3);
	minutes = (int) strtol(strtok(NULL,":"), (char**) NULL,3);
	seconds = (int) strtol(strtok(NULL,":"), (char**) NULL,3);
	
    struct ln_lnlat_posn observer;
    observer.lat = lat;
    observer.lng = lng;

	struct ln_hrz_posn target;
	target.az = az;
	target.alt = alt;

	struct ln_equ_posn coord;
    
    struct ln_date obsdate;
    obsdate.years = year;
    obsdate.months = month;
    obsdate.days = day;
    obsdate.hours = hour;
    obsdate.minutes = minutes;
    obsdate.seconds = seconds;

   	double JD;
    JD = ln_get_julian_day(&obsdate);

    fprintf(stderr, "Julian day: %f\n", JD);
	ln_get_equ_from_hrz(&target, &observer, JD, &coord);
	fprintf(stdout, "RA: %f DEC: %f\n", coord.ra, coord.dec);	
}
