/*
*****************************************************
Knowledge Assessment - Q3 Resolution
Davi Wei Tokikawa
*****************************************************
*/

#include <stdio.h>	
#include <stdlib.h>									
#include <assert.h>
#include <string.h>

typedef struct array_data
{
    float avg_value;
    int max_value;
    int min_value;
    int *even_array;
    int even_array_length;
}array_data_t;

/**
 * @brief Get relevant data from an int array
 * 
 * @param input_array 
 * @param input_array_length 
 * @return array_data_t 
 */
array_data_t get_data_from_int_array(int *input_array, int input_array_length)
{
    int max_value = input_array[0];
    int min_value = max_value;
    int sum = 0;
    int *even_array = (int *)malloc(input_array_length * sizeof(int));
    int even_count = 0;
    for(int i=0; i<input_array_length; i++)
    {
        if(input_array[i] > max_value)
            max_value = input_array[i];
        else if(input_array[i] < min_value)
            min_value = input_array[i];
        if(!(input_array[i] % 2))
        {
            even_array[even_count] = input_array[i];
            even_count++;
        }
        sum += input_array[i];
    }

    array_data_t array_data = {
        .avg_value = (float)sum / input_array_length,
        .max_value = max_value,
        .min_value = min_value,
        .even_array = even_array,
        .even_array_length = even_count,
    };

    return array_data;
}

// MAIN
int main(void)
{
    int raw_array1[10] = {99,5,4,7,8,1,3,2,10,5};
    int expected_even_array1[4] = {4,8,2,10};
    int raw_array2[11] = {5,50,4,-1,8,1,8,2,10,5,7};
    int expected_even_array2[6] = {50,4,8,8,2,10};
    
    array_data_t array1_data = get_data_from_int_array(raw_array1, sizeof(raw_array1)/sizeof(int));
    array_data_t array2_data = get_data_from_int_array(raw_array2, sizeof(raw_array2)/sizeof(int));

    assert(array1_data.avg_value == 14.4f);
    assert(array1_data.max_value == 99);
    assert(array1_data.min_value == 1);
    assert(array1_data.even_array_length == 4);
    assert(memcmp(array1_data.even_array, expected_even_array1, array1_data.even_array_length) == 0);
    free(array1_data.even_array);

    assert(array2_data.avg_value == 9.0f);
    assert(array2_data.max_value == 50);
    assert(array2_data.min_value == -1);
    assert(array2_data.even_array_length == 6);
    assert(memcmp(array2_data.even_array, expected_even_array2, array2_data.even_array_length) == 0);
    free(array2_data.even_array);

    return 0;
}