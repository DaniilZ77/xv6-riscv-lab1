#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
   int res = add(5, 10);
   printf(res == 15 ? "Test 1: success\n" : "Test 1: error\n");

   res = add(123, 321);
   printf(res == 444 ? "Test 2: success\n" : "Test 2: error\n");

   res = add(-5, 10);
   printf(res == 5 ? "Test 3: success\n" : "Test 3: error\n");

   res = add(5, -10);
   printf(res == -5 ? "Test 4: success\n" : "Test 4: error\n");
}