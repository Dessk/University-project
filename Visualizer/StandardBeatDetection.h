#pragma once

//Calc variance
double Var(double* arr, double E_avg, int len);

//Calc sum of a[i]^2 + b[i]^2 where i <0,1024>. 
//Basically it calculates sum for each sample record.
double Sum(int i0, int add, short int* a, short int* b);

//Basic sum function. Sum of all elements in array
double Sum_Standard(double* arr, int dlug);

void StandardBeatDetect(FILE* file, double accuracy) {



	//Find end of data block
	long aktualny = ftell(file);
	fseek(file, 0, SEEK_END);
	long koniec = ftell(file);
	fseek(file, aktualny, SEEK_SET);


	FILE* output;
	if ((output = fopen("output.txt", "w")) == NULL) {
		exit(-1);
	}
	FILE* output_values;
	if ((output_values = fopen("output_values.txt", "w")) == NULL) {
		exit(-1);
	}
	FILE* output_values_c_avg;
	if ((output_values_c_avg = fopen("output_values_c_avg.txt", "w")) == NULL) {
		exit(-1);
	}


	//Stereo
	//Buffers for left/right channel.
	//16-bit per channel.
	short int right[1024];
	short int left[1024];

	//History buffer for last 43 instant energy values
	double c_buffer[43] = { {0.0} };

	int data_processed = 0;

	printf("=======PROCESSING DATA=====\n");
	while (ftell(file) != koniec) {
		//Read to buffers 1024 samples (Stereo). 
		for (int i = 0; i < 1024; i++) {
			fread(left + i, 2, 1, file);
			fread(right + i, 2, 1, file);
		}
		//Data processed update
		data_processed += 1024*4;
		//Info
		printf("\rCurrent progress: %d (bytes)", data_processed);

		//Calc 'e' based on new sample from data block 
		double e = Sum(0, 1024, right, left);

		//Calc <E> and C 
		double avg_E = (1.0 / 43.0) * Sum_Standard(c_buffer, 43);
		double C = -accuracy * Var(c_buffer, avg_E, 43) + 1.5142857;


		//shift with flush history buffer
		for (int i = 42; i > 0; i--) {
			c_buffer[i] = c_buffer[i - 1];
		}

		//set first value to 'e'
		c_buffer[0] = e;

		//compare 'e' to C*<E>
		if (c_buffer[0] > C* avg_E) {
			//printf("Beat detected!\n");
			fputc('1',output);
		}
		else {
			//printf("Beat not detected!\n");
			fputc('0', output);
		}
		fprintf(output_values, "%f ", c_buffer[0]);
		fprintf(output_values_c_avg, "%f ", C * avg_E);

	}
	printf("\n==========FINISHED=========\n");
	fclose(output);
	fclose(output_values);
	fclose(output_values_c_avg);
}

double Var(double* arr, double E_avg, int len) {
	double suma = 0;
	for (int i = 0; i < len; i++) {
		suma += pow(E_avg - arr[i], 2.0);
	}
	return (1.0 / 43.0) * suma;
}

double Sum(int i0, int add, short int* a, short int* b) {
	double suma = 0;
	for (int k = i0; k < i0 + add; k++) {
		suma += (pow((double)(a[k] / 32768.0), 2.0) + pow((double)(b[k] / 32768.0), 2.0));
	}

	return suma;
}
double Sum_Standard(double* arr, int dlug) {
	double suma = 0;
	for (int i = 0; i < dlug; i++) {
		suma += arr[i];
	}
	return suma;
}