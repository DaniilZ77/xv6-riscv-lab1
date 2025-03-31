#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int num1;

int main() {
    printf("WITH GLOBAL VARIABLE\n");
    proc_pagetables(0, 0, 0);
    printf("\n--------------------\n\n");

    int num2 = 0;
    printf("WITH STACK VARIABLE\n");
    printf("%d\n", num2);
    proc_pagetables(0, 0, 0);
    printf("\n--------------------\n\n");

    int arr1[1000];
    arr1[0] = 0;
    printf("WITH STACK ARRAY\n");
    printf("%d\n", arr1[0]);
    proc_pagetables(0, 0, 0);
    printf("\n--------------------\n\n");

    char *arr2 = (char*)malloc(4097);
    arr2[0] = 0;
    printf("WITH HEAP ARRAY\n");
    printf("%d\n", arr2[0]);
    proc_pagetables(0, 0, 0);
    printf("\n--------------------\n\n");

    printf("WITHOUT CLEAR FLAGS\n");
    proc_pagetables(0, 0, 1);
    proc_pagetables(0, 0, 2);
    printf("\n--------------------\n\n");

    printf("WITH CLEAR FLAGS\n");
    proc_pagetables_unflag(0, 0, 0);
    proc_pagetables(0, 0, 1);
    proc_pagetables(0, 0, 2);
    printf("\n--------------------\n\n");

    printf("BEFORE GETTING\n");
    char* arr3 = (char*)malloc(4097);
    proc_pagetables_unflag(0, 0, 0);
    proc_pagetables(0, 0, 2);
    printf("AFTER\n");
    int var = arr3[0];
    (void)var;
    proc_pagetables(0, 0, 2);
    printf("\n--------------------\n\n");

    printf("BEFORE CHANGING\n");
    char* arr4 = (char*)malloc(4097);
    proc_pagetables_unflag(0, 0, 0);
    proc_pagetables(0, 0, 1);
    printf("AFTER\n");
    arr4[0] = 1;
    proc_pagetables(0, 0, 1);
    printf("\n--------------------\n\n");

    printf("BEFORE FREE\n");
    char* arr5 = (char*)malloc(4097);
    proc_pagetables_unflag(0, 0, 0);
    proc_pagetables(0, 0, 0);
    printf("AFTER\n");
    free(arr5);
    proc_pagetables(0, 0, 0);
    printf("\n--------------------\n\n");
}
