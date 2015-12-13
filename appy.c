// appy.c

// Included headers
#include <ncurses.h> // ncurses functions
#include <unistd.h> // POSIX system call functions
#include <stdio.h> // Standard stream calls such as stderr and stdout
#include <stdlib.h> // Standard C library
#include <string.h> // String variable functions

// Function prototypes
static int load_pkg_info(); // Loads pkg database information into a struct
static void exit_failure(); // Safely stop the application after providing an error message

// Entry point for application
int main(int argc, char *argv[]) {

	// Type Declarations
	enum state_t {DETECT_PKG, LOAD_PKG_INFO, FINISHED}; // Application's possible tasks as a type

	//Variable Declaration
	enum state_t application_state = DETECT_PKG; // Instance of application state enum type set to initial state
	int exit_status = EXIT_SUCCESS; // Set initial exit status to be passed to the terminal, no malfunction
	const char *key_press_name; // Variable to hold current key press English name
	int key_press_value; // Variable to catch key presses

	// Ncurses initialization and configuration
	initscr(); // Initialize the ncurses window
	noecho(); // Do not echo user input from keyboard in ncurses
	curs_set(FALSE);  // Hide the cursor in ncurses
	keypad(stdscr, TRUE); // Enable detection of function keys and special keys
	raw(); // Turn off line buffering, and control characters
	nodelay(stdscr, TRUE); // Receive key presses when they happen without waiting

	// Body of application
	int looping = 1; // When set to one, keep keep the application alive
	// Main logic loop
	while (looping == 1) {
		
		// Control character recognition to terminate application
		key_press_value = getch(); // If a key was pressed, store the key's value
		key_press_name = keyname(key_press_value); // Convert the value to the name to determine if there are escape characters
		if (!strncmp(key_press_name, "^C", 2)) { // Check to see if the key press is ctr-c
			looping = 0; // Flag the main loop for termination
			application_state = FINISHED; // Skip the main loop altogether
		} // End if

		// This switch statement determines what part of the application needs to run
		switch ( application_state ) {

			// The first step is to make sure that pkg is ready to work
			case DETECT_PKG:
				// TODO: Detect whether or not pkg is installed using "pkg query %v pkg"?				// TODO: Detect whether or not pkg info has been downloaded / initialized
				application_state = LOAD_PKG_INFO; // Move on to the next state for the next loop
				break; // End case

			// This step may run several times, it loads the database information
			case LOAD_PKG_INFO:
				if ( load_pkg_info() ) { // If the function failed react accordingly
					exit_failure("Error loading pkg info, exiting", &exit_status, &looping); // Stop the application with an error message
				// TODO: Figure out what to do here
				} // End if
				application_state = FINISHED; // Used to test completion of other cases
				break; // End case

			// Used to prevent execution of other cases
			case FINISHED:
				break; // End case

			// Case to catch invalid application states
			default:
				exit_failure("Invalid application state, exiting", &exit_status, &looping); // Display the appropriate error message
				break; // End case
		} 
		// TODO: Display UI, etc.
	} // End while

	// Terminate the program
	endwin(); // Restore normal terminal behavior
	// For testing purposes, next line
	fprintf(stdout, "Debug exit status: %i\n", exit_status); // Print the exit status to the terminal
	exit( exit_status ); // Reflect to the terminal whether the application completed successfully or not
} //End main function

// Function to load package information from pkg
static int load_pkg_info() {
	mvprintw(0,0,"Loading...\n"); // Print a loading status indicator in the ncurses window at position (0,0)
	refresh(); // Write window buffer to the terminal
	// TODO: Load in current info
	return(0); // Send success code back to main function
} // End load_pkg_info function

// Function to display a fatal error message and exit the application cleanly
static void exit_failure(char error_message[], int* exit_status, int* looping) {
	fprintf(stderr, "%s\n", error_message); // Echo the any error messages to the stderr stream of the terminal
	*exit_status = EXIT_FAILURE; // Set the return value for the terminal
	*looping = 0; // Set a flag to end the main loop
} // End exit_failure function


// Function Library

// sleep(1) // Wait for one second before continuing
// clear(); // Clear the ncurses window buffer
// cbreak(); // Turn off line buffering, keep control sequences as signals
