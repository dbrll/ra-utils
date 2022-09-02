// gcc proc.c -lm -O2 -o proc -Wall

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<unistd.h>
#include<math.h>

void help(void){
	printf("Usage: plot -d DATA -r REFERENCE [-f center frequency]\n");
	exit(0);
}

void fileinfo(FILE* fp, int* start, int* nbval) {
// Retrieve the start position and the number of rows in target file

	int ch, prev_line;
	ch = prev_line = 0;
	*nbval = *start = 0;

	while ((ch = fgetc(fp)) != EOF) {
		if (ch == '#'){
			// Discard comments
			while(fgetc(fp) != '\n')
				;
			*start = ftell(fp) + sizeof(char);
			//printf("Comment at position %i\n", start);
		}
		if (ch == '\n'){
			// Discard blank lines
			if (ftell(fp) - prev_line != 1)
				++*nbval;
			/*else
				printf("Empty line at position %li\n", ftell(fp)); */
			prev_line = ftell(fp);
		}
	}
}

void getcolval(FILE* fp, int* start, int* nbval, double* freq, float* pwr) {
// Populate arrays with values from file

	char* line = malloc(24 * sizeof(char));
	int ch, i, n = 0;

	fseek(fp, *start, SEEK_SET);
	while ((ch = fgetc(fp)) != EOF) {
		i=1;
		line[0] = ch;
		while ((line[i] = fgetc(fp)) != '\n')
			i++;
		line[i+1] = '\0';
		freq[n] = (double) strtod(strtok(line, " "), NULL);
		pwr[n] = (float) atof(strtok(NULL, " "));  
		//printf("%i freq:%.0f pwr:%f\n", n, freq[n], pwr[n]);
		n++;
		if (n == *nbval)
			break;
	}
}
void rm_rfi(float* pwr, int* nbval) {
	// RFI excision
	double avg;
	for(int i=10; i<*nbval; i++) {
		avg = 0;
		for(int n=i-10; n<i; n++)
			avg += pwr[n];
		avg /= 10;
		if (pwr[i] > 1.05 * avg || pwr[i] < 0.95 * avg)
			pwr[i] = avg;
	}
}

void slopecorr(float* pwr, int* nbval) {
// Correct the spectra slope
	int startmin =  *nbval / 5.7;
	int startavg = *nbval / 4.7;
	int startmax = *nbval / 4;
	int endmin = *nbval / 1.25;
	int endavg = *nbval / 1.2;
	int endmax = *nbval / 1.15;
    
    	double startavgpwr = 0;
    	for (int i=startmin; i< startmax; i++)
		startavgpwr += pwr[i];
    	startavgpwr /= (startmax - startmin);
    	double endavgpwr = 0;
    	for (int i=endmin; i<endmax; i++)
		endavgpwr += pwr[i];
	endavgpwr /= (endmax - endmin);

	double slope = (endavgpwr - startavgpwr) / (endavg - startavg); 

	for (int i=0; i<*nbval; i++)
		pwr[i] = pwr[i] - slope*i;
}

int main(int argc, char *argv[], const char** optstring) {

	int opt;
	double freq = 1420405751;
	const char* file_data = "";
	const char* file_ref = "";
    
    while((opt = getopt(argc, argv, "d:r:h")) != -1){
            switch(opt){
            		case 'd':
            				file_data = optarg;
            				break;
            		case 'r':
            				file_ref = optarg;
            				break;
                    case 'h':
                            help();
                            break;
            }
    }

    if ( (file_data[0] == '\0') || (file_ref[0] == '\0')){
    	help();
    }

	FILE* fp;
	int start, nbval;

	// Read the spectra file ==================================
	if (! (fp = fopen(file_data, "r"))) {
		fprintf(stderr, "Could not open %s. Exiting.\n", file_data);
		return 1;
	}
	// Get start position and number of rows
	fileinfo(fp, &start, &nbval);       
	// Malloc and populate the arrays
	double* freq_h1 = malloc(nbval * sizeof(double));
	float* pwr_h1 = malloc(nbval * sizeof(float));
	getcolval(fp, &start, &nbval, freq_h1, pwr_h1);
	fclose(fp);

	// Read the ref file ======================================
	if (! (fp = fopen(file_ref, "r"))) {
		fprintf(stderr, "Could not open %s. Exiting.\n", file_ref);
		return 1;
	}
	fileinfo(fp, &start, &nbval);       
	double* freq_ref = malloc(nbval * sizeof(double));
	float* pwr_ref = malloc(nbval * sizeof(float));
	getcolval(fp, &start, &nbval, freq_ref, pwr_ref);
	free(freq_ref);
	fclose(fp);

	// Processing =============================================
	for(int i=0; i<nbval; i++) {
		// dB to ratio conversion
		pwr_h1[i] = pow(10, pwr_h1[i] / 10);
		pwr_ref[i] = pow(10, pwr_ref[i] / 10);
		// Divide by ref
		pwr_h1[i] /= pwr_ref[i]; 
	}
	free(pwr_ref);

	rm_rfi(pwr_h1, &nbval);
	slopecorr(pwr_h1, &nbval);

	for(int i=0; i<nbval; i++) {
		// Convert freq to radial velocity
		freq_h1[i] = -(freq_h1[i] / freq - 1) * 299790;
		// Display processed result
		printf("%.2f %f\n", freq_h1[i], pwr_h1[i]);
	}
	return 0;
}
