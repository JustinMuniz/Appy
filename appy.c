/* appy.c */

/* Included ncurses headers */
#include <ncurses.h> /* ncurses functions (includes stdio, termios, termio, and sgtty) */
#include <panel.h> /* Header file required for handling overlapping ncurses windows */
#include <menu.h> /* Header file required for handling menu items in ncurses */
#include <form.h> /* Required for creating and editing fields in an ncurses form */

/* Included standard headers */
#include <unistd.h> /* POSIX system call functions */
#include <stdlib.h> /* Standard C library */
#include <string.h> /* String variable functions */
#include <signal.h> /* Allows handling of application's signals passed from the operating system */

/* Function prototypes */
static int load_pkg_info(); /* Loads pkg database information into a structure */
static void exit_failure(char error_message[], int* exit_status, int* looping); /* Safely stop the main loop after providing an error message */
static void exit_application(); /* Release memory and terminate the application */
static int draw_menu_bar(WINDOW *menu_bar, MENU *menu_bar_menu); /* Create and post the menu displayed at the top of the screen */

/* Entry point for application */
int main(int argc, char *argv[]) {

	/* Type declarations */
	enum state_t {DETECT_PKG, LOAD_PKG_INFO, DRAW_WINDOWS, FINISHED}; /* Application's possible tasks as a type */

	/* Standard variable declaration */
	enum state_t application_state = DETECT_PKG; /* Instance of application state enumeration type set to initial state */
	const char *key_press_name; /* Variable to hold current key press English name */
	int key_press_value; /* Variable to catch key presses */
	int exit_status = EXIT_SUCCESS; /* Set initial exit status to be passed to the terminal, no malfunction */
	int looping = 1; /* When set to one, keep keep the application alive */

	/* ncurses variable declaration */
	WINDOW *menu_bar; /* Initialize a sub-windows to contain the menu bar at the top of the screen */
	MENU *menu_bar_menu; /* Initialize the menu structure that will be used at the top of the screen */
	
	/* ncurses initialization and configuration */
	initscr(); /* Initialize the ncurses window */
	noecho(); /* Do not echo user input from keyboard in ncurses */
	curs_set(FALSE);  /* Hide the cursor in ncurses */
	keypad(stdscr, TRUE); /* Enable detection of function keys and special keys */
	cbreak(); /* Turn off line buffering */
	nodelay(stdscr, TRUE); /* Receive key presses when they happen without waiting */

	/* Operating system signal responses */
	signal(SIGINT, exit_application); /* Reaction to interrupt signal from operating system */

	/* Main logic loop */
	while (looping == 1) {
		
		/* Key press, mouse action, and application signal detection */
		key_press_value = getch(); /* If a key was pressed, store the key's value */
		key_press_name = keyname(key_press_value); /* Convert the value to the name to determine if there are escape characters */

		/* This switch statement determines what part of the application needs to run */
		switch ( application_state ) {

			/* The first step is to make sure that pkg is ready to work */
			case DETECT_PKG:
				
				/*
				TODO: Make this a function that returns status code
				TODO: Create an interpretation of the returned status code
				TODO: Detect whether or not pkg is installed using "pkg query %v pkg"?
				TODO: Detect whether or not pkg info has been downloaded / initialized
				*/
				
				/* Wrap up current state, and signal to proceed to the next */
				application_state = LOAD_PKG_INFO; /* Move on to the next state for the next loop */
				break; /* End case */

			/* This step may run several times throughout the span of the application, it loads the database information */
			case LOAD_PKG_INFO:

				/* Perform the information parsing function, then interpret the returned status code */
				if ( load_pkg_info() ) { /* If the function failed react accordingly */
					exit_failure("Error loading pkg info, exiting", &exit_status, &looping); /* Stop the application with an error message */
				} /* End if */

				/* Wrap up current state, and signal to proceed to the next */
				application_state = DRAW_WINDOWS; /* Move on to drawing the UI */
				break; /* End case */

			/* Split the windows up into sub-windows, and load the menu bar */
			case DRAW_WINDOWS:

				/* Create sub-windows */
				draw_menu_bar(&*menu_bar, &*menu_bar_menu); /* Create menu bar at the top of the screen */
				
				/* Render changes by telling stdscr to refresh */
				touchwin(stdscr); /* Mark stdscr to be refreshed as though it was modified */
				refresh(); /* Draw all modified windows to the screen */

				/* Wrap up current state, and signal to proceed to the next */
				application_state = FINISHED; /* Used to test completion of other cases */
				break; /* End case */

			/* Used to prevent execution of other cases */
			case FINISHED:

				/* Continue looping without change, so developer can see current application process */
				break; /* End case */

			/* Case to catch invalid application states */
			default:

				/* Prepare to terminate the program, and throw an error message */
				exit_failure("Invalid application state, exiting", &exit_status, &looping); /* Display the appropriate error message */
				break; /* End case */
		} 
	} /* End while */

	/* Terminate the program */
	exit_application(exit_status); /* Process garbage collection to release memory, and terminate the application with the current exit status code */
} /*End main function */

/* Function to load package information from pkg */
static int load_pkg_info() {

	/* Write loading status to the screen to inform user of progress */
	mvprintw(0,0,"Loading...\n"); /* Print a loading status indicator in the ncurses window at position (0,0) */
	refresh(); /* Write window buffer to the terminal */

	/*
	TODO: Figure out what to do here
	TODO: Load in current info
	*/

	/* Wrap up function and return to caller function with success code */
	return(0); /* Send success code back to main function */
} /* End load_pkg_info function */

/* Function to generate and place the navigation menu bar found at the top of the screen */
static int draw_menu_bar(WINDOW *menu_bar, MENU *menu_bar_menu) {

	/* Variable declaration */
	char *menu_bar_categories[] = {"File", "About", (char*)NULL}; /*  */
	ITEM **menu_bar_items = (ITEM **)calloc(2, sizeof(ITEM *)); /*  */

	/*
	TODO: Determine if this should be a panel instead
	TODO: Have number of categories algorithmically determined instead of a magic number
	TODO: Hide selected item highlighting when menu bar is not active
	*/

	/* Generate the structures required to display the menu bar */
	for(int i = 0; i< 2; i++) /* Loop through each menu category */
		menu_bar_items[i] = new_item(menu_bar_categories[i],menu_bar_categories[i]); /* Create a menu label and description from the categories */
	menu_bar_menu = new_menu((ITEM **)menu_bar_items); /* Load each menu item into the menu structure */
	menu_bar = subwin(stdscr,1, 80, 0, 0); /* Associate the menu container window with placement on stdscr */

	/* Place menu structures into windows: a container, and a menu window */
	werase(menu_bar); /* Clear out the loading status */
	set_menu_win(menu_bar_menu,menu_bar); /* Associate menu with it's container window */
	set_menu_sub(menu_bar_menu, derwin(menu_bar, 1, 80, 0, 0)); /* Associate the menu window with it's placement in the container window */

	/* Specify menu formating */
	set_menu_format(menu_bar_menu, 1, 2); /* Menu requires one row with a column for each menu item */
	menu_opts_off(menu_bar_menu, O_SHOWDESC); /* The menu items should have a label visible, but no description */
	set_menu_mark(menu_bar_menu, NULL); /* Hide the cursor that shows currently selected item */

	/* Display the menu bar and return a success code */
	post_menu(menu_bar_menu); /* Display the menu */
	return(0); /* Return to caller with success code */
} /* End draw_menu_bar function */

/* Function to display a fatal error message and exit the application cleanly */
static void exit_failure(char error_message[], int* exit_status, int* looping) {

	/* Print error message and signal application shutdown */
	fprintf(stderr, "%s\n", error_message); /* Echo the any error messages to the stderr stream of the terminal */
	*exit_status = EXIT_FAILURE; /* Set the return value for the terminal */
	*looping = 0; /* Set a flag to end the main loop */
} /* End exit_failure function */

/* Function to exit while releasing memory used in application */
static void exit_application(int sig) {

	/* Garbage collection */

	/*
	TODO: menu bar collection
	*/

	/* Close program with exit status code */
	endwin(); /* Restore normal terminal behavior */
	/* For testing purposes, next line */
	fprintf(stdout, "Debug exit status: %i\n", sig); /* Print the exit status to the terminal */
	exit( sig ); /* Reflect to the terminal whether the application completed successfully or not */
} /* End exit_application function */


/* TODO:

Add in resize functionality
Add in screen width / height recognition
Comment everything
Add in error detection for everything
Create navigation sub-menus
Test for memory leaks
*/


/* Function Library

FUNCTION								|	DESCRIPTION
------------------------------------------------------------------------------------------------------
sleep(1)								|	Wait for one second before continuing
clear()									|	Clear the ncurses window buffer
cbreak()								|	Turn off line buffering, keep control sequences as signals
startcolor()							|	
curs_set(0)								|	
init_pair(1, COLOR_WHITE, COLOR_BLUE)	|	
wbkgd( , )								|	
waddstr( , )							|	
wattron( , )							|	
wattroff( , )							|	
nonl()									|	Tell curses not to do NL -> CR/NL on output
*/


/* Code Snippets

CODE										|	COMMENTS
------------------------------------------------------------------------------------------------------
if (!strncmp(key_press_name, "^C", 2)) {	|	Check to see if the key press is ctr-c
	looping = 0;							|	Flag the main loop for termination
	application_state = FINISHED;			|	Skip the main loop altogether
}											|	End if
*/
