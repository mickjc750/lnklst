
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "lnklst.h"

    static void show_list(struct lnklst_struct *lst);

int main()
{
    struct lnklst_struct    *mylist;
    char                    *banana, *grape, *apple, *orange, *mandarin;

    //Create a new list
    mylist = lnklst_create();

    //Add various strings
    banana = lnklst_allocate(mylist, strlen("BANANA")+1);
    strcpy(banana, "BANANA");

    grape = lnklst_allocate(mylist, strlen("GRAPE")+1);
    strcpy(grape, "GRAPE");

    apple = lnklst_allocate(mylist, strlen("APPLE")+1);
    strcpy(apple, "APPLE");

    orange = lnklst_allocate(mylist, strlen("ORANGE")+1);
    strcpy(orange, "ORANGE");

    mandarin = lnklst_allocate(mylist, strlen("MANDARIN")+1);
    strcpy(mandarin, "MANDARIN");

    //show list and remove items in some arbitrary order
    show_list(mylist);
    lnklst_free(mylist, grape);
    show_list(mylist);
    lnklst_free(mylist, orange);
    show_list(mylist);
    lnklst_free(mylist, apple);
    show_list(mylist);
    lnklst_free(mylist, mandarin);
    show_list(mylist);
    lnklst_free(mylist, banana);
    show_list(mylist);

    lnklst_destroy(mylist);

    return 0;
}

static void show_list(struct lnklst_struct *lst)
{
    char *text;

    printf("list = ");
    text = lnklst_last(lst);    //get the most recent allocation
    while(text)                 //walk through entire list
    {
        printf("%s, ", text);
        text = lnklst_prior(lst, text);
    };
    printf("\r\n");
}
