#include "main.h"

//need to do some rigorous testing on this function
//free return val of this function;
char *epoch_to_date(struct timespec time)
{
	struct timespec local_time;
	int offset = get_timezone_offset();
	local_time.tv_nsec = 0;
	if(offset < 0){
		local_time.tv_sec = (-1)*offset;
		time = time_delta(time, local_time);
	}
	else{
		local_time.tv_sec = offset;
		time = time_sum(time, local_time);
	}
	int mins = (time.tv_sec / 60) % 60;
	int hours = (time.tv_sec / 60 / 60) % 24;
	int days = time.tv_sec / 60 / 60 / 24;
	int year = 1970;
	int month_cum[] = {31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
	while(days > 365){
		if(year % 4 != 0)
			days -= 365;
		else if(year % 100 != 0)
			days -= 366;
		else if(year % 400 != 0)
			days -= 365;
		else
			days -= 366;
		year++;
	}
	//check not a leap year
	if(!(year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)) && days == 365){
		days -= 365;
		year += 1;
	}
	bool isleap = year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
	int month = 0;
	for(;month < 12; ++month){
		if(isleap && month > 9){
			month_cum[month]++;
		}
		if(days < month_cum[month]){
			++month;
			break;
		}
	}
	int mday = days + 1;
	if(month > 1){
		mday -= month_cum[month - 2];
	}
	//size of rv is guarenteed
	char *rv = malloc(30 * sizeof(char));
	sprintf(rv, "%02d%02d%04d_%02d%02d", mday, month, year, hours, mins);
	return rv;
}

void create_log_file(struct timespec call_time, OPERATION operation, int numfiles, char *filename1, char *filename2, int num_threads, bool isval, mat_rv result)
{
	char logfilename[40] = "21727809_";
	char *date = epoch_to_date(call_time);
	strcat(logfilename, date);
	free(date);
	switch(operation){
	case SCAL_MUL:
		strcat(logfilename, "_sm.out");
		break;
	case TRACE:
		strcat(logfilename, "_tr.out");
		break;
	case ADD:
		strcat(logfilename, "_ad.out");
		break;
	case TRANSPOSE:
		strcat(logfilename, "_ts.out");
		break;
	case MAT_MUL:
		strcat(logfilename, "_mm.out");
		break;
	default:
		fprintf(stderr, "Internal error attempting to log no operation\n");
		exit(EXIT_FAILURE);
		break;
	}
	FILE *logfp = fopen(logfilename,"wb");
	switch(operation){
	case SCAL_MUL:
		fputs("sm",logfp);
		break;
	case TRACE:
		fputs("tr",logfp);
		break;
	case ADD:
		fputs("ad",logfp);
		break;
	case TRANSPOSE:
		fputs("ts",logfp);
		break;
	case MAT_MUL:
		fputs("mm",logfp);
		break;
	default:
		fprintf(stderr, "Internal error attempting to log no operation\n");
		exit(EXIT_FAILURE);
		break;
	}
	enum {
		NO_SYSTEM,
		WINDOWS,
		UNIX
	} system;
	system = NO_SYSTEM;
	switch(strlen(OS)){
	case 7:
		if(strncmp(OS,"WINDOWS",7*sizeof(char)) == 0)
			system = WINDOWS;
		break;
	case 5:
		if(strncmp(OS,"LINUX",7*sizeof(char)) == 0)
			system = UNIX;
		break;
	case 3:
		if(strncmp(OS,"MAC",7*sizeof(char)) == 0)
			system = UNIX;
		break;
	default:
		fprintf(stderr, "Internal error specifying OS\n");
		exit(EXIT_FAILURE);
	}
	if(system == NO_SYSTEM){
		fprintf(stderr,"Internal error specifying OS\n");
		exit(EXIT_FAILURE);
	}
	if(system == WINDOWS)
		fputs("\r\n",logfp);
	else
		fputc('\n',logfp);
	if(numfiles > 0){
		fputs(filename1, logfp);
		if(system == WINDOWS)
			fputs("\r\n",logfp);
		else
			fputc('\n',logfp);
	}
	if(numfiles > 1){
		fputs(filename2, logfp);
		if(system == WINDOWS)
			fputs("\r\n",logfp);
		else
			fputc('\n',logfp);
	}
	fprintf(logfp,"%d",num_threads);
	if(system == WINDOWS)
		fputs("\r\n",logfp);
	else
		fputc('\n',logfp);
	if(isval){
		if(result.type == MAT_INT)
			fprintf(logfp,"%d",result.vals.i[0]);
		else
			fprintf(logfp,"%f",result.vals.f[0]);
	}
	else{
		if(result.type == MAT_INT)
			fputs("int",logfp);
		else
			fputs("float",logfp);
		if(system == WINDOWS)
			fputs("\r\n",logfp);
		else
			fputc('\n',logfp);
		fprintf(logfp,"%d",result.rows);
		if(system == WINDOWS)
			fputs("\r\n",logfp);
		else
			fputc('\n',logfp);
		fprintf(logfp,"%d",result.cols);
		if(system == WINDOWS)
			fputs("\r\n",logfp);
		else
			fputc('\n',logfp);
		for(int i = 0; i < result.rows; ++i){
			for(int j = 0; j < result.cols; ++j){
				if(result.type == MAT_INT)
					fprintf(logfp,"%d",result.vals.i[i*result.cols + j]);
				else
					fprintf(logfp,"%f",result.vals.f[i*result.cols + j]);
				if(i == result.rows - 1 && j == result.cols - 1){
					break;
				}
				fputc(' ', logfp);
			}
		}
	}
	if(system == WINDOWS)
		fputs("\r\n",logfp);
	else
		fputc('\n',logfp);
	fprintf(logfp, "%Lf", (long double)result.t_construct.tv_sec + (long double)result.t_construct.tv_nsec / 1E9);
	if(system == WINDOWS)
		fputs("\r\n",logfp);
	else
		fputc('\n',logfp);
	fprintf(logfp, "%Lf", (long double)result.t_process.tv_sec + (long double)result.t_process.tv_nsec / 1E9);
}
