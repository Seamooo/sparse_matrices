#include "main.h"

//return arg1 - arg2
struct timespec time_delta(struct timespec end, struct timespec start)
{
	struct timespec rv;
	rv.tv_sec = end.tv_sec - start.tv_sec;
	if(end.tv_nsec >= start.tv_nsec)
		rv.tv_nsec = end.tv_nsec - start.tv_nsec;
	else{
		rv.tv_sec--;
		rv.tv_nsec = (int)(1E9) + end.tv_nsec - start.tv_nsec;
	}
	if(rv.tv_sec < 0){
		fprintf(stderr, "Timing returned negative result\n");
		exit(EXIT_FAILURE);
	}
	return rv;
}

//return arg1 + arg2
struct timespec time_sum(struct timespec ts1, struct timespec ts2)
{
	struct timespec rv;
	rv.tv_sec = ts1.tv_sec + ts2.tv_sec;
	if(ts1.tv_nsec + ts2.tv_nsec >= (int)1E9){
		rv.tv_nsec = ts1.tv_nsec + ts2.tv_nsec - (int)1E9;
		rv.tv_sec++;
	}
	else
		rv.tv_nsec = ts1.tv_nsec + ts2.tv_nsec;
	return rv;
}

//return value is UTC + rv seconds
#ifdef _WIN32
int get_timezone_offset()
{
	_tzset();
	long rv;
	_get_timezone(&rv);
	return (-1)*(int)rv;
}
#elif defined __unix__
int get_timezone_offset()
{
	tzset();
	return (-1)*(int)timezone;
}
#endif

#ifdef _WIN32
void get_utc_time(struct timespec *ts)
{
	timespec_get(ts, TIME_UTC);
}
#elif defined __unix__
void get_utc_time(struct timespec *ts)
{
	clock_gettime(CLOCK_REALTIME, ts);
}
#endif
