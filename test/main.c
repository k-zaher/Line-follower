/*
 */

#include <avr/io.h>

#define QTR_EMITTERS_OFF 0
#define QTR_EMITTERS_ON 1
#define QTR_EMITTERS_ON_AND_OFF 2

    unsigned int *calibratedMinimumOn;
	unsigned int *calibratedMaximumOn;
	unsigned int *calibratedMinimumOff;
	unsigned int *calibratedMaximumOff;
	unsigned int _maxValue;

int main(void)
{



    while(1)
    ;

    return 0;
}

void emittersOff()
{
    PORTD = PIND&(1<<PD7);
}

void emittersOn()
{
    PORTD = PIND&(~1<<PD7);
}

void calibrateOnOrOff(unsigned int **calibratedMinimum,
										unsigned int **calibratedMaximum,
										unsigned char readMode)
{
	int i;
	unsigned int sensor_values[16];
	unsigned int max_sensor_values[16];
	unsigned int min_sensor_values[16];

	// Allocate the arrays if necessary.
	if(*calibratedMaximum == 0)
	{
		//***** put something instead of malloc

		// Initialize the max and min calibrated values to values that
		// will cause the first reading to update them.

		for(i=0;i<6;i++)
			(*calibratedMaximum)[i] = 0;
	}
	if(*calibratedMinimum == 0)
	{
		//***** put something instead of malloc
		// If the malloc failed, don't continue.
		if(*calibratedMinimum == 0)
			return;

		for(i=0;i<6;i++)
			(*calibratedMinimum)[i] = _maxValue;
	}

	int j;
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
		if(min_sensor_values[i] > (*calibratedMaximum)[i])
			(*calibratedMaximum)[i] = min_sensor_values[i];
		if(max_sensor_values[i] < (*calibratedMinimum)[i])
			(*calibratedMinimum)[i] = max_sensor_values[i];
	}
}


// Resets the calibration.
void resetCalibration()
{
	unsigned char i;
	for(i=0;i<6;i++)
	{
		if(calibratedMinimumOn)
			calibratedMinimumOn[i] = _maxValue;
		if(calibratedMinimumOff)
			calibratedMinimumOff[i] = _maxValue;
		if(calibratedMaximumOn)
			calibratedMaximumOn[i] = 0;
		if(calibratedMaximumOff)
			calibratedMaximumOff[i] = 0;
	}
}

void calibrate(unsigned char readMode)
{
	if(readMode == QTR_EMITTERS_ON_AND_OFF || readMode == QTR_EMITTERS_ON)
	{
		calibrateOnOrOff(&calibratedMinimumOn,
						 &calibratedMaximumOn,
						 QTR_EMITTERS_ON);
	}


	if(readMode == QTR_EMITTERS_ON_AND_OFF || readMode == QTR_EMITTERS_OFF)
	{
		calibrateOnOrOff(&calibratedMinimumOff,
						 &calibratedMaximumOff,
						 QTR_EMITTERS_OFF);
	}
}
