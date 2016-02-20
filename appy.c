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
static int initialization_tests(); /* Detects if system can support appy */
static void exit_failure(char error_message[], int* exit_status, int* looping); /* Safely stop the main loop after providing an error message */
static void exit_application(); /* Release memory and terminate the application */

/* ncurses window declaration, document wide so to be deallocated */
static WINDOW *menu_bar; /* Initialize a sub-window to contain the menu bar at the top of the screen */

/* Entry point for application */
int main(int argc, char *argv[]) {

	/* Type declarations */
	enum state_t {INITIALIZATION_TESTS, LOAD_CONFIGURATION, DRAW_WINDOWS, FINISHED}; /* Application's possible tasks as a type */

	/* Standard variable declaration */
	enum state_t application_state = INITIALIZATION_TESTS; /* Instance of application state enumeration type set to initial state */
	const char *key_press_name; /* Variable to hold current key press English name */
	int key_press_value; /* Variable to catch key presses */
	int exit_status = EXIT_SUCCESS; /* Set initial exit status to be passed to the terminal, no malfunction */
	int looping = 1; /* When set to one, keep keep the application alive */
	
	/* ncurses initialization and configuration */
	initscr(); /* Initialize the ncurses window */
	noecho(); /* Do not echo user input from keyboard in ncurses */
	curs_set(FALSE);  /* Hide the cursor incurses */
	keypad(stdscr, TRUE); /* Enable detection of function keys and special keys */
	cbreak(); /* Turn off line buffering */
	nodelay(stdscr, TRUE); /* Receive key presses when they happen without waiting */

	/* Operating system signal responses */
	signal(SIGINT, exit_application); /* Reaction to interrupt signal from operating system */

	/* Main logic loop */
	while (looping == 1) {
		
		/* Specific key press, mouse action, and application signal detection */
		key_press_value = getch(); /* If a key was pressed, store the key's value */
		key_press_name = keyname(key_press_value); /* Convert the value to the name to determine if there are escape characters */

		 /*  */
		if (!strncmp(key_press_name, "KEY_F(1)", 2)) { 
			mvprintw(1,1, key_press_name); /*  */
		} /*  */


		/* This switch statement determines what part of the application needs to run */
		switch ( application_state ) {

			/* This first state is to detect that all is well with the system, and that appy is able to execcute properly */
			case INITIALIZATION_TESTS:

				/* Perform the information parsing function, then interpret the returned status code */
				if ( initialization_tests() ) { /* If the function failed react accordingly */
					exit_failure("Error initializing appy, exiting", &exit_status, &looping); /* Stop the application with an error message */
				} /* End if */

				/* Wrap up current state, and signal to proceed to the next */
				application_state = LOAD_CONFIGURATION; /* Move on to drawing the UI */
				break; /* End case */

			/* */
			case LOAD_CONFIGURATION:

				/*
				TODO: Test if configuration files exist
				TODO: Load configuration appy files
				TODO: Load pkg configuration files
				*/

				/* Wrap up current state, and signal to proceed to the next */
				application_state = DRAW_WINDOWS; /* Used to test completion of other cases */
				break; /* */

			/* Split the windows up into sub-windows, and load the menu bar */
			case DRAW_WINDOWS:

				/* Create sub-windows */
				menu_bar = subwin(stdscr,1, 80, 0, 0); /* Associate the menu container window with placement on stdscr */
				mvwprintw(menu_bar, 0, 0, " File   About"); /* Create menu bar at the top of the screen */
				
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
static int initialization_tests() {

	/* Write loading status to the screen to inform user of progress */
	mvprintw(0,0,"Loading...\n"); /* Print a loading status indicator in the ncurses window at position (0,0) */
	refresh(); /* Write window buffer to the terminal */

	/*
	TODO: Make sure user is running FreeBSD
	TODO: Make sure user is capible of using pkg
	TODO: Make sure that user provided arguements are sane
	*/

	/* Wrap up function and return to caller function with success code */
	return(0); /* Send success code back to main function */
} /* End initialization_tests function */

/* Function to display a fatal error message and exit the application cleanly */
static void exit_failure(char error_message[], int* exit_status, int* looping) {

	/* Print error message and signal application shutdown */
	fprintf(stderr, "%s\n", error_message); /* Echo the any error messages to the stderr stream of the terminal */
	*exit_status = EXIT_FAILURE; /* Set the return value for the terminal */
	*looping = 0; /* Set a flag to end the main loop */
} /* End exit_failure function */

/* Function to exit while releasing memory used in application */
static void exit_application(int sig) {

	/* Deallocate the ncurses windows */
	delwin(menu_bar); /* Delete the container window for the navigation bar */
	endwin(); /* Restore normal terminal behavior */

	/* Close program with exit status code */
	/* For testing purposes, next line */
	fprintf(stdout, "Debug exit status: %i\n", sig); /* Print the exit status to the terminal */
	exit( sig ); /* Reflect to the terminal whether the application completed successfully or not */
} /* End exit_application function */


/* TODO:

Combine draw_menu_bar and traverse_menu_bar
Add in resize functionality
Add in screen width / height recognition
Add a note to the navigation menu specifying to press F1 to access the menu
Add in error detection for everything
Create dynamic navigation sub-menus
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


/* Broken code */

///* Function to generate and place the navigation menu bar found at the top of the screen */
// static int draw_menu_bar(WINDOW *menu_bar, MENU *menu_bar_menu) {

// 	static MENU *menu_bar_menu; /* Initialize the menu structure that will be used at the top of the screen */
// 	static ITEM **menu_bar_items; /* Initialize the menu items array for the navigation menuz */
// 	static WINDOW *menu_bar_sub_window; /* Initialize the proper navigation menu sub-window */


// 	/* Variable declaration */
// 	char *menu_bar_categories[] = {"File", "About", (char*)NULL}; /* An array of menu item labels */
// 	int number_of_categories = sizeof(menu_bar_categories) / sizeof(menu_bar_categories[0]); /* Determine the number of menu items required */
// 	menu_bar_items = (ITEM **)calloc( number_of_categories, sizeof(ITEM *)); /* Allocate the memory required for the menu items */

// 	/*
// 	TODO: Hide selected item highlighting when menu bar is not active
// 	*/

// 	/* Generate the structures required to display the menu bar */
// 	for(int i = 0; i< number_of_categories - 1; i++) /* Loop through each menu category save the null character */
// 		menu_bar_items[i] = new_item(menu_bar_categories[i],menu_bar_categories[i]); /* Create a menu label and description from the categories */
// 	menu_bar_items[number_of_categories - 1] = (ITEM *)NULL; /* Null terminated array */
// 	menu_bar_menu = new_menu((ITEM **)menu_bar_items); /* Load each menu item into the menu structure */
// 	//menu_bar = subwin(stdscr,1, 80, 0, 0); /* Associate the menu container window with placement on stdscr */
// 	menu_bar_sub_window = derwin(menu_bar, 1, 80, 0, 0); /* Create the window to hold the navigation menu */

// 	/* Place menu structures into windows: a container, and a menu window */
// 	werase(menu_bar); /* Clear out the loading status */
// 	set_menu_win(menu_bar_menu, menu_bar); /* Associate menu with it's container window */
// 	set_menu_sub(menu_bar_menu, menu_bar_sub_window); /* Associate the menu window with it's placement in the container window */

// 	/* Specify menu formating */
// 	set_menu_format(menu_bar_menu, 1, 2); /* Menu requires one row with a column for each menu item */
// 	menu_opts_off(menu_bar_menu, O_SHOWDESC); /* The menu items should have a label visible, but no description */

// 	//set_menu_mark(menu_bar_menu, NULL); /* Hide the cursor that shows currently selected item */
// 	//set_menu_fore(menu_bar_menu, A_NORMAL); /* Disguise the selected item as unselected until navigation menu is in forcus */

// 	/* Display the menu bar and return a success code */
// 	post_menu(menu_bar_menu); /* Display the menu */
// 		int bug = menu_driver(menu_bar_menu, REQ_RIGHT_ITEM);
// 	mvprintw(10,10, "%i", bug);
// 	menu_driver(menu_bar_menu, REQ_LEFT_ITEM);

// 	/* Return menu_bar appearance to normal */
// 	mvwprintw(menu_bar, 0, 0, " File   About"); /* Create menu bar at the top of the screen */

// 	return(0); /* Return to caller with success code */
// } /* End draw_menu_bar function */

// /*  */
// static int traverse_menu_bar() {
// 	//set_menu_fore(menu_bar_menu, A_NORMAL); /*  */
// 	int bug = menu_driver(menu_bar_menu, REQ_DOWN_ITEM);
// 	mvprintw(0,0, "%i", bug);

	
// 	TODO: Select first menu item
// 	TODO: Show "File" sub menu
// 	TODO: Select first sub menu item
// 	TODO: Accept input (left, right, up, down, enter, F1, esc)
// 	TODO: Free sub menus
// 	TODO: Hide selected menu highlighting
	
// 	mvprintw(10,10,"hi");
// 	//post_menu(menu_bar_menu);


	/* Deallocate windows, menus, and items */
	//for(int i = 0; i < sizeof(menu_bar_items) / sizeof(menu_bar_items[0]); i++) /* Loop through each menu category */
	//	free_item(menu_bar_items[i]); /* Delete each navigation menu item */
	//free_menu(menu_bar_menu); /* Delete the navigation menu */
	//delwin(menu_bar_sub_window); /* delete the proper navigation bar window */

// 	return(0); /*  */
// } /*  */
