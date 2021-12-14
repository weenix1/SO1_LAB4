/***************************************************************************************
Content: SOL1, Lab4, Exercise 1
Lecturer: Prof. Schoenen
Authors: Hilary Ogalagu,  HAW Hamburg
Date: 20201112
Short description:
****************************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#define COLUMNS 80
# define INT_STEPS 100	// steps for the integral_1arg() function


double f(double x);
double area(double left_x, double right_x);
double integral(double left_x, double right_x, int steps);
double integral_1arg(double x);

int main() {

	double result = area(-0.5, 0.5);
	printf("%lf\n", result);// prints the result as 1.00000
	double ans = integral(-0.5, 0.5, 10);
	printf("%lf\n", ans);// prints the result as 0.639245
	double integ = integral_1arg(0.5);
	printf("%lf\n", integ);// prints the result as 1.00

 return 0;
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

//1c 
double integral_1arg(double x) {
	double left_x = 0, right_x = x;

	int steps = INT_STEPS;
	double increment = (right_x - left_x) / steps;
	//if (DEBUG) printf("increment is %lf\n", increment);

	double temp_area = 0.0, total_area = 0.0;
	//if (DEBUG) printf("temp_area = %lf, total_area = %lf\n", temp_area, total_area);

	for (int i = 0; i < steps; i++) {
		temp_area = area(left_x, left_x + increment);
		left_x += increment;
		total_area += temp_area;
		//printf("temp_area = %lf, total_area = %lf\n", temp_area, total_area);
	}

	//printf("\ntemp_area = %lf, total_area = %lf\n", temp_area, total_area);
	//printf("\n");
	return total_area;
}
