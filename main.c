
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <string.h>
    #include <stdbool.h>

    #include "lnklst.h"

    static void show_list(struct lnklst_struct *lst);
    static void show_list_reverse(struct lnklst_struct *lst);
    static bool alphaorder(void* a, void* b);

int main()
{
    struct lnklst_struct    *mylist;
    char                    *banana, *grape, *apple, *orange, *mandarin;

    //Create a new list
    mylist = lnklst_create();

    printf("\r\nAdding strings\r\n");

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

    show_list(mylist);
    show_list_reverse(mylist);

    printf("\r\n%i items in list\r\n", lnklst_count(mylist));

    printf("\r\nSorting list\r\n");
    lnklst_sort(mylist, alphaorder);
    show_list(mylist);

    printf("\r\nindex 2 = %s\r\n", (char*)lnklst_index(mylist,2));

    //show list and remove items in some arbitrary order
    printf("\r\nRemoving strings\r\n");
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
        text = lnklst_before(lst, text);
    };
    printf("\r\n");
}

static void show_list_reverse(struct lnklst_struct *lst)
{
    char *text;

    printf("list in reverse order = ");
    text = lnklst_index(lst, lnklst_count(lst)-1);  //get the end of the list
    while(text)                 //walk through entire list
    {
        printf("%s, ", text);
        text = lnklst_after(lst, text);
    };
    printf("\r\n");
}

static bool alphaorder(void* a, void* b)
{
    bool retval=false;
    if(strcmp((char*)a,(char*)b)>0)
        retval=true;
    return retval;
}
