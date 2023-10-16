/**
 * @file symstack.c
 * @author Adrián Ponechal (xponec01@stud.fit.vutbr.cz)
 * @brief Testing ovrer all functionality - push, pop and dispose function
 * @date 2023-10-16
 */

#include <stdio.h>
#include <stdlib.h>
#include "symstack.h"

#define DISPLAY_WIDTH 1

int main(void)
{
    symstack_t stack;
    init_symstack(&stack);

    // push node
    printf("symstack_push:\n");
    data_t data;
    data.c = "itmem1";
    data.id = 1;
    symstack_push(&stack, data);

    data.c = "item2";
    data.id = 2;
    symstack_push(&stack, data);

    data.c = "item3";
    data.id = 2;
    symstack_push(&stack, data);
    print_stack(&stack, DISPLAY_WIDTH);

    printf("\nsymstack_pop:\n");
    symstack_pop(&stack);
    print_stack(&stack, DISPLAY_WIDTH);

    printf("\nsymstack_dispose:\n");
    // remove node
    symstack_dispose(&stack);
    if (!symstack_is_empty(&stack))
    {
        printf("Stack is not empty.\n");
    }
    else
    {
        printf("Stack is empty.\n");
    }
    print_stack(&stack, DISPLAY_WIDTH);
    return EXIT_SUCCESS;
}
