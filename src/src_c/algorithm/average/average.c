#include "average.h"

double average(double *array, UINT32 len)
{
	UINT32 i = 0;
	double sum = 0;
	double ave = 0;

	if(array == NULL)
	{
		return 0;
	}

	for(i = 0; i < len; i++)
	{
		sum = sum + array[i];
	}

	ave = sum/len;

	return ave;
}


