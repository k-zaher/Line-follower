/*
 */

#include <avr/io.h>

#define QTR_EMITTERS_OFF 0
#define QTR_EMITTERS_ON 1
#define QTR_EMITTERS_ON_AND_OFF 2

unsigned int calibratedMinimum[6];
unsigned int calibratedMaximum[6];

unsigned int _maxValue = 1023;

void emittersOff()
{
    PORTD = PIND&(~(1<<PD7));
}

void emittersOn()
{
    PORTD = PIND|(1<<PD7);
}

void calibrateOnOrOff(unsigned int calibratedMinimum[],unsigned int calibratedMaximum[])
{
	int i;
	int j;
	unsigned int sensor_values[16]; // can be removed, and use the global variable instead
	unsigned int max_sensor_values[16];
	unsigned int min_sensor_values[16];

	// Allocate the arrays if necessary.


    for(i=0;i<6;i++)
        calibratedMaximum[i] = 0;

    for(i=0;i<6;i++)
        calibratedMinimum[i] = _maxValue;



	for(j=0;j<10;j++)
	{
		//  take it from samadony 		read(sensor_values,readMode);
		for(i=0;i<6;i++)
		{
			// set the max we found THIS time
			if(j == 0 || max_sensor_values[i] < sensor_values[i])
				max_sensor_values[i] = sensor_values[i];

			// set the min we found THIS time
			if(j == 0 || min_sensor_values[i] > sensor_values[i])
				min_sensor_values[i] = sensor_values[i];
		}
	}

	// record the min and max calibration values
	for(i=0;i<6;i++)
	{
		if(min_sensor_values[i] > calibratedMaximum[i])
			calibratedMaximum[i] = min_sensor_values[i];
		if(max_sensor_values[i] < calibratedMinimum[i])
			calibratedMinimum[i] = max_sensor_values[i];
	}
}


// Resets the calibration.
void resetCalibration(unsigned int calibratedMinimum[],unsigned int calibratedMaximum[])
{
	unsigned char i;
	for(i=0;i<6;i++)
	{
		if(calibratedMinimum)
			calibratedMinimum[i] = _maxValue;

		if(calibratedMaximum)
			calibratedMaximum[i] = 0;

	}
}


void readCalibrated()
{
	int i;

	// if not calibrated, do nothing
    if(!calibratedMinimum || !calibratedMaximum)
        return;

	// read the needed values
	//read(sensor_values,readMode);

	for(i=0;i<6;i++)
	{
		unsigned int calmin,calmax;
		unsigned int denominator;

		// find the correct calibration

        calmax = calibratedMaximum[i];
        calmin = calibratedMinimum[i];



		denominator = calmax - calmin;

		signed int x = 0;
		if(denominator != 0)
			x = (((signed long)sensor_values[i]) - calmin)
				* 1000 / denominator;
		if(x < 0)
			x = 0;
		else if(x > 1000)
			x = 1000;
		sensor_values[i] = x;
	}

}
