#include <stdio.h>
#include <unistd.h>

#include "exif.h"

int example(char* filename)
{
	puts(filename);
	struct JPEGHead* head = read_exif_head(filename);
	printf("is_exif: %s\n", is_exif(head)?"true":"false");
	struct GPS gps;
	read_gps_info(head, &gps);
	delete_JPEGHead(head);

	printf("version: %d.%d.%d.%d\n", gps.version[0], gps.version[1], gps.version[2], gps.version[3]);
	printf("latitude_ref: %c\n", gps.latitude_ref);
	printf("latitude: %lf %lf %lf\n", gps.latitude[0], gps.latitude[1], gps.latitude[2]);
	printf("longitude_ref: %c\n", gps.longitude_ref);
	printf("longitude: %lf %lf %lf\n", gps.longitude[0], gps.longitude[1], gps.longitude[2]);
	printf("altitude_ref: %d\n", gps.altitude_ref);
	printf("timestamp: %ld:%ld:%ld\n", gps.timestamp.hour, gps.timestamp.min, gps.timestamp.sec);
	printf("datestamp: %s\n\n", gps.datestamp);
}

int main(int argc, char** argv)
{
	int i = 1;
	for (; i < argc; i++) {
		example(argv[i]);
	}
}

