/***************************************************************************************
Content: SOL1, Lab4, Exercise 1
Lecturer: Prof. Schoenen
Authors: Hilary Ogalagu,  HAW Hamburg
Date: 20201112
Short description:
****************************************************************************************/

 #define _CRT_NONSTDC_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES /* for M_PI */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h> /* for rand() */
#include <time.h> /* for time() in srand() */

#define DEBUG 0 /* 1==true to give a lot of output */
#define TEST_SC 1 /* 1==true to give a lot of output */
#define COLUMNS 80
#define ROWS 30
#define MAX_DEVIATION 1e-6
#define DELTA_X 0.001
#define H 0.001    // defining h for derivation function


#define PI M_PI //PI 3.1415
#define XMIN -PI
#define XMAX +PI
//#define XSTEP (2*PI/ROWS)
#define XEXTRA 0.01

#define MATRIX_ROWS    3
#define MATRIX_COLUMNS 3
# define INT_STEPS 100	// steps for the integral_1arg() function


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


/* Global variables: */
char blank_char = ' '; // used in plot
char screenbuffer [ROWS][COLUMNS] = { ' ' }; // global

/*
// this is a trick/hack to be able to set and change a real C function y=f(x) which is used inside the math functions here
typedef double(*myfunction)(double); // just the datatype 'function_of_one_double_argument'
myfunction fkt = NULL; // = &funktion; // global (pointer) variable to such a function.
*/

void do_plotting_frontend();
void plot_function_by_key(char ch);
void plot_value(double value, double y_min, double y_max);
void plot_value_on_screenbuffer(double x, double y, double y_min, double y_max);
void plot_screenbuffer();
double derivation(double x, double h); // the derivation function, y = f'(x)
double f(double x);
double f1(double x);
double area(double left_x, double right_x);
double integral(double left_x, double right_x, int steps);
double integral_1arg(double x);
double gauss(double x);


int main() {
    /* random number generator initialization */
    time_t t;
    srand((unsigned)time(&t));
    int var = 42; // not a global variable
    if (DEBUG) printf("Hello\n");
    clrscr(); // if (DEBUG) printf("Fresh screen %dx%d\n", COLUMNS, ROWS);

    do_plotting_frontend(); // only this line

    char c = getch();
    return 0;
}


/* PLOTTING */
void do_plotting_frontend() {
    if (DEBUG) printf("### do_plotting_frontend()\n");
    char ch; // character from keyboard
    int key_pressed = ' '; // keyboard key value
    while (key_pressed != '.') { // loop
        printf("Please select function to plot (keys a..z, .=quit)\n");
        key_pressed = getch(); // get keyboard key
        ch = key_pressed; // char <- int conversion
        clrscr(); // clear screen
        if (DEBUG) printf("Key pressed was '%c'=%04d=0x%04X\n", ch, key_pressed, key_pressed);
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
        case 's': y = sin(x); break;
        case 'c': y = cos(x); break;
        case 't': y = tan(x); break;
        case 'f': y = f1(x); break; // our new sinus function
        case 'd': y = derivation(x, DELTA_X); break; // the first derivatice
        case 'g': y = gauss(x); break; // the first integral
        case 'i': y = integral_1arg(x); break; // the first integral
        //case 'g': y = fkt(x); break; // a generic function (global, can be set from outside)
        default: y = 0.0;
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
            plot_value(y, y_min, y_max); // old way
            //if (DEBUG) printf("x=%.3lf y=%.3lf\n", x, y);
            printf("x=%6.3lf ", x);
            if (index == index_of_x_zero)
                blank_char = '-'; // global variable
            else
                blank_char = ' '; // global variable
        }
        else { // new way:
            plot_value_on_screenbuffer(x, y, y_min, y_max);
        }
    }
    if (!plot_sideways) {
        plot_screenbuffer();
    }
    if (DEBUG) printf("### plot_function_by_key() finished.\n");
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


// plot_value_on_screenbuffer(y, y_min, y_max);
void plot_value_on_screenbuffer(double x, double y, double y_min, double y_max) {
    // was defined as: char screenbuffer[ROWS][COLUMNS];
    // we have: XMIN XMAX y_min y_max ROWS COLUMNS
    int available_columns = (COLUMNS - 1);
    int available_rows = (ROWS - 1);
    double delta_x = (double)(XMAX - XMIN) / available_columns;
    double delta_y = (y_max - y_min) / available_rows;
    int col = (x - XMIN) / delta_x; // x
    int row = (y_max - y) / delta_y; // y

    if (col >= 0 && row >= 0 && col < COLUMNS && row < ROWS){
        screenbuffer[row][col] = '*';
        screenbuffer[row][COLUMNS / 2 - 1] = '|';//for the y-axis
        screenbuffer[0][COLUMNS / 2 - 1] = '^';//for the y -axis
        screenbuffer[ROWS / 2][COLUMNS - 1] = '>';//for the x -axis
    }
    else
        printf("Error: out of bounds\n");
}


void plot_screenbuffer() {
    // was defined as: char screenbuffer[ROWS][COLUMNS];
    int row, columns;
    for (row = 0; row < ROWS; row++) { // rows
        for (columns = 0; columns < COLUMNS; columns++) { // columns
            char c = screenbuffer [row][columns];
            printf("%c", c);
        }
        printf("\n");
    }
}
 
double f1(double x) {
    return sin(x); // the function, y = f(x)
}


double derivation(double x, double h) {	// the derivation function, y = f'(x)
    double first_dev;
    first_dev = (f1(x + H) - f1(x - H)) / (2 * H); // calculating the derivation using a formula
    return first_dev;
}


double f(double x) {
    return cos(M_PI * x);
}


//1a calculates the area of a rectangle
double area(double left_x, double right_x) {
    double x, width, area;
    x = (left_x + right_x) / 2; // x is the mid-point
    width = right_x - left_x;
    area = width * f(x);
    return area;// returns the area of the rectangular
}


//1b calculate the integral from 
double integral(double left_x, double right_x, int steps) {
    double sum = 0;
    double x0, x1, width;
    width = (right_x - left_x) / steps;
    for (int i = 0; i < steps; i++) {
        x0 = left_x + width * i;
        x1 = width + x0;
        sum += area(x0, x1);
    }
    return sum;
}

double integral_1arg(double x) {
    double left_x = 0, right_x = x;

    int steps = INT_STEPS;
    double increment = (right_x - left_x) / steps;
    //if (DEBUG) printf("increment is %lf\n", increment);

    double temp_area = 0.0, total_area = 0.0;
    //if (DEBUG) printf("temp_area = %lf, total_area = %lf\n", temp_area, total_area);
    //using a for loop to sum the total area
    for (int i = 0; i < steps; i++) {
        temp_area = area(left_x, left_x + increment);
        left_x += increment;
        total_area += temp_area;//sums up the area
        //printf("temp_area = %lf, total_area = %lf\n", temp_area, total_area);
    }

    //printf("\ntemp_area = %lf, total_area = %lf\n", temp_area, total_area);
    //printf("\n");
    return total_area;
}

// 3a. implementing guass formular for normal distribution curve
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