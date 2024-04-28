#include <stdio.h>
#include <string.h>
#include <time.h>

int Write_todo(const char *item, const char *data) {
	FILE* Fptr; // Create a Pointer a File data type, used to hold the FD, 
	Fptr = fopen("todo.txt", "a"); // Open the File In Append Mode
	char buffer[200]; // Dont Need This lol

	

	if (Fptr == NULL) {
		printf("Failed To Open File"); // if the File pointer Holds a Null Value we failed to get a handle to the file, so we exit and print fail
		return 1;
	}

	fprintf(Fptr, "%s : %s\n",item, data); // fprintf is used to redirect a stream instead of going to STDOUT, its going to go into the FILE stream (Kind Of like Bash's > operator)

	printf("Succesfully Wrote To File\n"); // write a Success Message
	fflush(Fptr); // Flush The File Stream, any excess Data Will be immedialy written to file or dropped
	fclose(Fptr); // Close the Handle To File

	return 0; // Think This is Useless TBH
}

void read_todo() {
	FILE* file; // Create Pointer To the FILE structure, which then points to the file stream
	char buffer[200]; // Create Buffer For each line in file

	file = fopen("todo.txt", "r"); // Open File In Read Mode

	if(file == NULL) { 
		printf("Failed Opening File"); // If File Fails to Get Handle Print Fail
	}

	int i = 1; // Create Int Value So we can show number of lines
	while (fgets(buffer, sizeof(buffer), file)){ // Use Fgets To read each line from file and save it into Buffer
		printf("[%d] %s\n", i, buffer); // Print Line
		i++; // Inc I
	}
}


void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Read From STDIN till we reach a \0 or a \n, clearing the Input Buffer, 
    // We Do this Because when we use Fgets, it skips over it because there is still or a \n or \0 inside the buffer, so it thinks we already entered a string
}

int main() {

	time_t now; // Create a variable named Now and Store it in a time Data Type (Used For represening time in C)
    time(&now); // Use The Time SYS_CALL to get the current Time And Store in Now

    // Create a Struct To convert Time Into Local Time (Time Where i Am)
    struct tm *local_time = localtime(&now);

    
    char date_now[100]; // Create a Buffer Of 100 bytes to Store the Time, we Are converting To String
    strftime(date_now, sizeof(date_now), "%c", local_time); // date_now Is Where We Store the array of characters (String), with a max size of 100 bytes(sizeof = size of buffer), 
    														// %c To specify we want it saved as char, and local_time is where to grab the data from
    printf("%s\n", date_now); // Print the Current Time
	
	
	char choice;    // Create a Single Char for our choice
	char String_to_write[300]; // Create a Buffer Of 300 Bytes to Store the String to Write to File

	printf("Read Or Write To Todo File: ");  // Prompt User For Read Or Write to file
	scanf("%c", &choice); // Read From STDIN

	if (choice == 'r') {
		read_todo(); // If Choice == 'r', Jump To Read From File
	}

	if (choice == 'w') {
		printf("Enter Text To Write: "); // Prompt User For String to Write to File
		clear_input_buffer(); // Clear Buffer So Fgets Doesnt Read \n or \0
		fgets(String_to_write, sizeof(String_to_write), stdin); // Use FGETS to Read From STDIN, scanf stops reading after whitespace (spaces, tabs, enter)

		String_to_write[strcspn(String_to_write, "\n")] = '\0'; // If There is  new Line Char, replace it with a Null Terminator, So its a Proper C Style String
		Write_todo(String_to_write, date_now); // Use Write to Todo File Function, string_to_write and date_now are paramters to write

	}


	return 0;

}