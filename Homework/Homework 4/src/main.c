/**
 * Name: Hieu Nguyen
 * GTID: 903681705
 */

/*
 * Part 1 and 3 are found here!
 * You will submit this file to gradescope.
 */

#include "main.h"

int length = 0;
int tests = 0;

/**
 * Generates a random string based on the size passed in through the parameter.
 *
 */
int main(int argc, char *argv[])
{
    /* **************************************************************************************************
     * PART 1:
     * TODO: take in the command line arguments here
     * Any input from the l argument should be put into length variable as an int
     * If the user uses the t argument, then tests should be set to 1.
     * Using getopt() to take in the arguments is recommended, but not required.
     * 
     * Place your code below this comment, but above the return statement immediately below.
     * This line will allow you to compile only main.c so that you can test your implementation
     * of getting arguments.
     * 
     * When you are done with this section and have gotten your GDB screenshot, delete the entire line.
     * 
     */
    int option;
    while ((option = getopt(argc, argv, ":tl:")) != -1) {
        switch(option) { //switch options between l and t
            case 'l':
                length = atoi(optarg);
                //tests = 0;
                break;
            case 't':
                //length = 0;
                tests = 1;
                break;
            default:
                //break;
                return 1;
        }
    }

    if (tests == 1)
    {
        run_tests();
    }
    else
    {
        char *message = generateMessage();
        printf("Message: %s\n", message);
        free(message);
    }

    return 0;
}

// /* PART 3: Bug Fixing
//  * The GenerateMessage function below is so close to working! Arrg!
//  * If only there were no seg faults!
//  * TODO: Correct the seg faults so that the program runs.
//  * Using GDB is heavily recommended.
//  *
//  * Hint 1: The causes of segfaults may or may not be isolated to this file.
//  *       The provided code of the main method is clean of seg faults
//  * Hint 2: The errors can be all be corrected by only changing the lines of code already there.
//  *       Adding new lines of code to fix a bug is a viable for some may likely be the most common solution.
//  * Hint 3: There are 3 errors in the code causing a segfault
//  *
//  * Important: Any changes to the code should not cause the comments to no longer accurately describe what the code is doing!

// /**
//  * Generates a pseudo random message of the size passed in from the comand line parameters.
//  * This method only needs to work when the length of the message
//  * is smaller than the length of the dicitonary which -should- be 16.

char *generateMessage()
{
    // Converts the dictionary array (provided in main.h) into an arraylist for easy access
    arraylist_t *dictionary_as_list = create_arraylist(dictionary_length);
    for (int i = 0; i < dictionary_length; i++)
    {
        append(dictionary_as_list, dictionary[i]);
    }

    // Removes a word from the dictionary arraylist and adds that word to the end of the message array list
    arraylist_t *message = create_arraylist(length);
    for (int i = 0; i < length; i++)
    {
        char *word = remove_from_index(dictionary_as_list, i % dictionary_as_list->size);
        add_at_index(message, word, i); // SEG FAULT ERROR #1 (index starts at 1 if i + 1, should be 0)
    }

    // Adds the word "half" at the half way point in the list (round down if half is not an integer)
    add_at_index(message, "Half", message->size / 2);

    // Creates the the message as a string to be printed.
    char *string_message = NULL;
    for (int i = 0; i < length; i++)
    {
        // Removes the first word from the list
        char *word = remove_from_index(message, 0);

        // Calculates the new size needed for string message for the word to be appended.

        // SEG FAULT ERROR #2 (Potentially strlen a null pointer, should be checking for null character
        // when calculating strlen and index bound)
        // Original: size_t new_size = strlen(string_message) + strlen(word) + 1;

        int word_length = strlen(word);
        size_t new_size;
        if (string_message == NULL) {
            new_size = 0;
        } else {
            new_size = strlen(string_message) + 1;
        }

        if (i == 0 && i == length - 1) {
            new_size += word_length;
        } else {
            new_size += word_length + 1;
        }

        // Reserves the memory space in the heap
        string_message = realloc(string_message, new_size);

        // Checks realloc is successful.
        // Hint: this if statement is bug free, it is good form to do this when you access the heap
        if (string_message == NULL)
        {
            fprintf(stderr, "OUT OF MEMORY");
            exit(1);
        }

        // If it is the first word, different steps need to be taken.
        // In this step we want to "zero out" the memory that we are using if this is the first word.
        // Otherwise we want to append a space so that each word is not on top of each other.
        // Is there any other line where adding the first word needs some sort of check?
        // Hint: review the string methods documentation provided in pdf.
        if (i == 0)
        {
            memset(string_message, 0, new_size);
        }
        else
        {
            strcat(string_message, " ");
        }
        // Concatenates the word to the end of the string.
        strcat(string_message, word);
    }

    destroy(dictionary_as_list);
    destroy(message);

    return string_message;
}

