#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int is_leap(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int main(int argc, char *argv[]) {
    uint64 ns = rtc();
    int year = 1970;
    int monthes_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    char* monthes_names[] = {"Yan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sen", "Oct", "Nov", "Dec"};
    while (ns > 0) {
        uint64 ns_per_year = 365 * 24 * 60 * 60 * 1000000000L;
        if (is_leap(year)) {
            ns_per_year += 24 * 60 * 60 * 1000000000L;
        }

        if (ns < ns_per_year) {
            break;
        }

        ns -= ns_per_year;
        year++;
    }
    int month = 0;
    while (ns > 0) {
      uint64 ns_per_month = monthes_days[month] * 24 * 60 * 60 * 1000000000L;
      if (month == 1 && is_leap(year)) {
        ns_per_month += 24 * 60 * 60 * 1000000000L;
      }

      if (ns < ns_per_month) {
        break;
      }

      ns -= ns_per_month;
      month++;
    }

    int day = ns / 24 / 60 / 60 / 1000000000L + 1;
    ns -= (day - 1) * 24 * 60 * 60 * 1000000000L;
    int hour = ns / 60 / 60 / 1000000000L;
    ns -= hour * 60 * 60 * 1000000000L;
    int minute = ns / 60 / 1000000000L;
    ns -= minute * 60 * 1000000000L;
    int second = ns / 1000000000L;
    ns -= second * 1000000000L;

    printf("Year: %d, Month: %s, Day: %d, Hour: %d, Minute: %d, Second: %d.%lu\n", year, monthes_names[month], day, hour, minute, second, ns / 100000000L);
}