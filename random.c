
/***************************************************************************************
Content: SOL1, Lab4, Exercise 1
Lecturer: Prof. Schoenen
Authors: Hilary Ogalagu,  HAW Hamburg
Date: 20201112
Short description:
****************************************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#define _USE_MATH_DEFINES /* for M_PI */
#include <math.h>
#include <stdlib.h> /* for rand() */
#include <time.h> /* for time() in srand() */
#define DEBUG 1 

#define PI 3.1415
#define XMIN -PI
#define XMAX +PI
#define XSTEP (2*PI/ROWS)
#define XEXTRA 0.01

#define COLUMNS 80
#define ROWS 40
#define DEBUG 1 
#define XEXTRA 0.01
#define TEST_SC 1 		// 1==true to give a lot of output		
#define MAX_DEVIATION 1e-6
#define DELTA_X 0.001
#define H 0.001

/* Global variables: */
char blank_char = ' '; // used in plot
char screenbuffer[ROWS][COLUMNS] = { ' ' }; // global

 // these are preprocessor statements, ignore them for now
 // we just need a portable clrscr() function to clear the console screen
#ifdef _WIN32 /* Windows */
#include <conio.h> // linker cannot find clrscr, so we need a workaround
#define clrscr() system("cls");
#else /* Linux */
#include <stdio.h>
#define clrscr() printf("\e[1;1H\e[2J") /* ANSI escape sequences */
/* system("clear"); // also usable for Linux */
#endif

#define print printf

//global variable
int histogram_array[60] = { 0 };//declaring and intializing an array
double gauss(double x);
int dice();
void dice_statistics(int throws, int experiments);
void plot_value(double value, double y_min, double y_max);
void do_plotting_frontend(); // prototype
void plot_function_by_key(char ch);
void plot_value_on_screenbuffer(double x, double y, double y_min, double y_max);
void plot_screenbuffer();

int main() {
	/* random number generator initialization */
	time_t t;
	srand((unsigned)time(&t));
	//testing cases
	for (double x = -2.0; x <= 2.0; x += 0.2) {
		double y = gauss(x);
		if (DEBUG)printf("x=%lf y=%lf\n", x, y);
	}
	for (int i = 0; i < 10; i++) {
		int z = dice();
		if (DEBUG)printf("z = %d\n", z);
	}
	 dice_statistics(1, 10);
	//dice_statistics(2, 100000);
	//dice_statistics(3, 100000);
	

	if (DEBUG) printf("Hello\n");
	//clrscr();
	if (DEBUG) printf("Fresh screen %dx%d\n", COLUMNS, ROWS);
	do_plotting_frontend();
	return 0;
}
// excerise 3a
//implementing gauss formular
double gauss(double x) {
	//gauss(x, µ, σ) 
	double sigma, nu, square, gaussian; // declaraction
	sigma = 1.0;
	nu = 0.0;
	square = ((x - nu) / sigma) * ((x - nu) / sigma);
	gaussian = (1 / (sigma * (sqrt(2 * M_PI)))) * (exp(-0.5) * square);
	//returning the result 
	return gaussian;
}
// excerise 3b
int dice() {
	//generating a random number
	int dice_roll = (rand() % 6) + 1;
	//returning the number generated
	return dice_roll;
}
// excerise 3c
void dice_statistics(int throws, int experiments) {
	//using nested for loop to obtain dice throw for each experiment
	for (int i = 0; i < experiments; i++) {//outer loop
		int sum = 0;
		for (int throw = 0; throw < throws; throw++) {//inner loop
			int result = dice();
			sum += result;
		}
		//the array records the value sum for each experiment
		histogram_array[sum]++;
		if (DEBUG)printf("Sum = %d\n", sum);
		//if (DEBUG)printf("Histogram[%d]=%d\n", i, histogram_array[sum]);
	}

	for (int i = throws; i < throws * 6 + 1; i++) {
		//printf("Histogram[%d]=%d\n", i, histogram_array[i]);
		printf("Probability[%d]=%lf\n", i, (double)histogram_array[i] / experiments);
	}
}

void plot_value(double value, double y_min, double y_max) {
	double range = y_max - y_min;
	int available_columns = (COLUMNS - 10);
	int y_value_column = (value - y_min) / range * available_columns; // between 0 .. 80
	int x_axis_column = -y_min / range * available_columns;

	for (int i = 0; i < available_columns; i++) {
		if (i == y_value_column)
			printf("*");
		else if (i == x_axis_column)
			printf("|");
		else
			printf("%c", blank_char);
	}
	printf("\n");
}

void do_plotting_frontend() {
	if (DEBUG) printf("### do_plotting_frontend()\n");
	char ch; // character from keyboard
	int key_pressed = ' '; // keyboard key value
	// loop:
	while (key_pressed != '.') {
		print("Please select function to plot (keys a..z, .=quit)\n");
		key_pressed = _getch();
		ch = key_pressed; // char <- int conversion
		if (DEBUG) print("Key pressed was '%c'=%04d=0x%04X\n", ch, key_pressed, key_pressed);
		plot_function_by_key(ch);
	}
	if (DEBUG) printf("### do_plotting_frontend() finished.\n");
}


void plot_function_by_key(char ch) {
	if (DEBUG) printf("### plot_function_by_key('%c')\n", ch);
	int index = 0;
	double x, y, y_min = 0.0, y_max = 0.0, x_step;
	double x_array[100], y_array[100];
	static int plot_sideways = 0; // 0 is just starting value at program start.
	if (plot_sideways)
		x_step = (2.0 * M_PI / ROWS);
	else {
		x_step = (double)(XMAX - XMIN) / COLUMNS;
		memset(screenbuffer, ' ', ROWS * COLUMNS /* bytes */); // clean board
	}
	// loop over range of x values:
	for (x = XMIN; x <= XMAX + XEXTRA; x = x + x_step) { // #steps is ROWS
		switch (ch) { // depend on key pressed
		case 'w': plot_sideways = 1 - plot_sideways; break;
		case 'g': y = gauss(x); break;
		default: y = 0;
		}
		//if (DEBUG) printf("x=%.3lf y=%.3lf\n", x, y);
		x_array[index] = x; // store into table
		y_array[index] = y; // store into table
		y_min = (y < y_min) ? y : y_min;
		y_max = (y > y_max) ? y : y_max;
		index++; // increase the index
	}
	int num_values = index; // so many elements in the table
	if (DEBUG) printf("num_values=%d, y_min=%.3f, y_max=%.3f\n", num_values, y_min, y_max);
	int index_of_x_zero = (0 - XMIN) / x_step;
	for (index = 0; index < num_values; index++) { // #steps is ROWS
		x = x_array[index]; // read out values from table
		y = y_array[index];
		if (plot_sideways) {
			printf("x=%6.3lf ", x);
			plot_value(y, y_min, y_max); // old way
			//if (DEBUG) printf("x=%.3lf y=%.3lf\n", x, y);
			if (index == (index_of_x_zero - 1))
				blank_char = '-'; // global variable
			else
				blank_char = ' '; // global variable
		}
		else { // new way:
			plot_value_on_screenbuffer(x, y, y_min, y_max); // new way
		}
	}
	if (!plot_sideways) {
		plot_screenbuffer();
	}
	if (DEBUG) printf("### plot_function_by_key() finished.\n");
}

void plot_value_on_screenbuffer(double x, double y, double y_min, double y_max) {
	// was defined as: char screenbuffer[ROWS][COLUMNS];
	// we have: XMIN XMAX y_min y_max ROWS COLUMNS
	int available_columns = (COLUMNS - 1);
	int available_rows = (ROWS - 1);
	double delta_x = (double)(XMAX - XMIN) / available_columns;
	double delta_y = (y_max - y_min) / available_rows;
	int col = (x - XMIN) / delta_x; // x
	int row = (y_max - y) / delta_y; // y

	if (col >= 0 && row >= 0 && col < COLUMNS && row < ROWS) {
		screenbuffer[row][col] = '*';
		screenbuffer[ROWS / 2][col] = '-';
		screenbuffer[row][COLUMNS / 2 - 1] = '|';
		screenbuffer[0][COLUMNS / 2 - 1] = '^';
		screenbuffer[ROWS / 2][COLUMNS - 1] = '>';
	}
	else
		printf("Error: out of bounds\n");
}

void plot_screenbuffer() {
	// was defined as: char screenbuffer[ROWS][COLUMNS];
	int row, columns;
	for (row = 0; row < ROWS; row++) { // rows
		for (columns = 0; columns < COLUMNS; columns++) { // columns
			char c = screenbuffer[row][columns];
			printf("%c", c);
		}
		printf("\n");
	}
}


