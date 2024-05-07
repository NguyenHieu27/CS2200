/**
 * Name: Hieu Nguyen
 * GTID: 903681705
 */

/*  PART 2: A CS-2200 C implementation of the arraylist data structure.
    Implement an array list.
    The methods that are required are all described in the header file. 
    Description for the methods can be found there.

    Hint 1: Review documentation/ man page for malloc, calloc, and realloc.
    Hint 2: Review how an arraylist works.
    Hint 3: You can use GDB if your implentation causes segmentation faults.

    You will submit this file to gradescope.
*/

#include "arraylist.h"

/* Student code goes below this point */

/**
 * Create a new arraylist with given capacity.
 * @param capacity the initial length of backing array
 * @return pointer to the new arraylist, or NULL if memory allocation failed
*/
arraylist_t *create_arraylist(uint capacity) {
    arraylist_t *arraylist = (arraylist_t *)calloc(1, sizeof(arraylist_t));
    if (arraylist == NULL) {
        free(arraylist);
        return NULL; // Return NULL and free arraylist if memory allocation fails
    }
    arraylist->backing_array = (char **)calloc(capacity, sizeof(char *));
    if (arraylist->backing_array == NULL) {
        destroy(arraylist);
        return NULL; // Return NULL and destroy arraylist if memory allocation fails
    }
    // Setting capacity
    arraylist->capacity = capacity;
    return arraylist;
}

/**
 * Add data to the given index in the arraylist.
 * @param arraylist the arraylist that data will be added in
 * @param data the data to be added
 * @param index the index at which the data will be added
*/
void add_at_index(arraylist_t *arraylist, char *data, int index) {
    if (arraylist == NULL || data == NULL) {
        return; // Check and return if arraylist or data is NULL
    }
    if (arraylist->size == arraylist->capacity) {
        resize(arraylist); // If arraylist is full, resize it
    }

    // Shifting elements to add new data
    for (uint i = arraylist->size; i > index; i--) {
        arraylist->backing_array[i] = arraylist->backing_array[i - 1];
    }
    // Insert data and increment size
    arraylist->backing_array[index] = data;
    arraylist->size++;
}

/**
 * Using add_at_index method to append data at the end of the arraylist.
 * @param arraylist the arraylist that data will be added in
 * @param data the data to be added
*/
void append(arraylist_t *arraylist, char *data) {
    if (arraylist == NULL || data == NULL) {
        return; // Check and return if arraylist or data is NULL
    }
    add_at_index(arraylist, data, arraylist->size);
}

/**
 * Remove data from the given index from the arraylist.
 * @param arraylist the arraylist that the data will be removed from
 * @param index the index at which the data will be removed
 * @return removed the data that is removed, or NULL if arraylist is NULL or index is out of bounds
*/
char *remove_from_index(arraylist_t *arraylist, int index) {
    if (index < 0 || index > arraylist->capacity || arraylist == NULL) {
        return NULL; //Check and return NULL if arraylist is NULL or index is out of bounds
    }
    char *removed = arraylist->backing_array[index];

    if (arraylist->size > 1) { //If arraylist has more than one element
        // Shift elements to remove data
        for (uint i = index; i < arraylist->size - 1; i++) {
            arraylist->backing_array[i] = arraylist->backing_array[i + 1];
        }
    }

    if (arraylist->size != 0) { //If arraylist has only one element (size is not 0)
        arraylist->backing_array[arraylist->size - 1] = NULL; // Set the last element to NULL
        arraylist->size--; // Decrement size
    }

    return removed;
}

/**
 * Resize the arraylist by doubling its capacity.
 * @param arraylist the arraylist that will be resized
*/
void resize(arraylist_t *arraylist) {
    if (arraylist == NULL) {
        return; // Return if arraylist is NULL
    }

    size_t new_size; // Creating new size capacity that is double the original capacity
    if (arraylist->capacity != 0) {
        new_size = (arraylist->capacity) * 2 * sizeof(char *);
    } else { // If capacity is set to be 0, set default to 1
        new_size = 1;
    }

    // Reallocate memory for the new resized array
    arraylist->backing_array = realloc(arraylist->backing_array, new_size);

    if (arraylist->backing_array == NULL) {
        return; // Return if memory allocation fails
    }

    arraylist->capacity *= 2; // Update capacity of the arraylist
}

/**
 * Destroy the arraylist and free allocated memory.
 * @param arraylist the arraylist to be destroyed
*/
void destroy(arraylist_t *arraylist) {
    if (arraylist == NULL) {
        return; // Return if arraylist is NULL (either invalid or already destroyed)
    }
    // Set each element to NULL to destroy every element
    for (int i = 0; i < arraylist->size; i++) {
        arraylist->backing_array[i] = NULL;
    }
    // Free memory allocated for array and arraylist structure
    free(arraylist->backing_array);
    free(arraylist);
}