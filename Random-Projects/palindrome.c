#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void revstring(char* words[], int num_of_words);

int main(int argc, char *argv[])
{
	int arg_amount = argc - 1; // Calculate how many strings inputted; -1 for the program name
	char** buffer = (char **)malloc(arg_amount * sizeof(char*)); // allocate a pointer to a array of pointers, 

	for (int i = 0; i < arg_amount; i++) {
		buffer[i] = argv[i + 1]; // Save Each arg into the array
	}

	revstring(buffer, arg_amount); // jump to reverse string function

	free(buffer); // Free The Buffer
	return 0;
}

void revstring(char* words[], int num_of_words) {
	for (int i = 0; i < num_of_words; i++) { // Loop Through each string
		printf("\n");
		size_t len = strlen(words[i]); // Get String length
		char buffer[len + 1]; // Create buffer to hold the revesered string

		for (int j = 0; j < len; j++) { // loop through each char and save it into the buffer but start from end, [i] = string. [ - j] = character inside string
			buffer[j] = words[i][len - 1 - j];
		}
		buffer[len] = '\0'; // Null Terminate String

		if ((strcmp(buffer, words[i])) == 0) {
			printf("%s IS a palindrome!\n", words[i]); // Now Finally Check If they match
		} 
		else {
			printf("%s IS-NOT a palindrome!\n", words[i]);
		}
		
	}
}