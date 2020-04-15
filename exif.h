#ifndef	EXIF_H
#define	EXIF_H

#include <stdio.h>

struct GPS_timestamp {
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
};

struct GPS {
	double latitude[3];
	double longitude[3];
	unsigned long altitude;
	unsigned char version[4];
	unsigned char altitude_ref;
	char latitude_ref;
	char longitude_ref;
	unsigned char datestamp[11];
	struct GPS_timestamp timestamp;
};

struct JPEGHead;

struct JPEGHead* read_exif_head(char* filename);
void delete_JPEGHead(struct JPEGHead*);
_Bool is_exif(struct JPEGHead*);
#define	not_exif(head)	(!is_exif(head))
void read_gps_info(struct JPEGHead*, struct GPS*);

#endif
