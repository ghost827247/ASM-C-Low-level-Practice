#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cel_to_far(float celcisius_temp);
void far_to_cel(float faren_temp);

/** USAGE
 * ./temp_convert <tempature> <1 = celsius to farenheight, 2 = farenheight to celsius>
 * 
**/


int main(int argc, char  *argv[])
{
	int choice = atoi(argv[2]);

	switch(choice) {
		case 1:
			float celcisius_temp = atof(argv[1]);
			cel_to_far(celcisius_temp);
			break;
		case 2:
			float farenheight_temp = atof(argv[1]);
			far_to_cel(farenheight_temp);
			break;
		default:
			printf("Invalid Choice!");
	}

	return 0;
}

void cel_to_far(float celcisius_temp) {
	float far_temp;

	// °F = (°C × (9/5)) + 32
	far_temp = (celcisius_temp * 9/5) + 32;

	printf("Farenheight: %.2f\n", far_temp);
}

void far_to_cel(float farenheight_temp) {
	float celc_temp;

	// C° = (°F − 32) × 5/9 = 15.556°C
	celc_temp = (farenheight_temp - 32) * 5/9;
	printf("Celsius: %.2f", celc_temp);

}