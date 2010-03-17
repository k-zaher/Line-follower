/*
 */

#include <avr/io.h>

#define QTR_EMITTERS_OFF 0
#define QTR_EMITTERS_ON 1
#define QTR_EMITTERS_ON_AND_OFF 2

unsigned int calibratedMinimumOn[6];
unsigned int calibratedMaximumOn[6];
unsigned int calibratedMinimumOff[6];
unsigned int calibratedMaximumOff[6];
unsigned int _maxValue = 1023;

int main(void)
{



    while(1)
    ;

    return 0;
}

void emittersOff()
{
    PORTD = PIND&(~(1<<PD7));
}

void emittersOn()
{
    PORTD = PIND|(1<<PD7);
}

void calibrateOnOrOff(unsigned int **calibratedMinimum,
										unsigned int **calibratedMaximum,
										unsigned char readMode)
{
	int i;
	int j;
	unsigned int sensor_values[16];
	unsigned int max_sensor_values[16];
	unsigned int min_sensor_values[16];

	// Allocate the arrays if necessary.


    for(i=0;i<6;i++)
        calibratedMaximum[i] = 0;

    for(i=0;i<6;i++)
        (*calibratedMinimum)[i] = _maxValue;



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
