#pragma pack(1)

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "list.h"
#include "exif.h"

#define GPSInfo                  0x8825

#define	GPS_VERSION_ID		0
#define	GPS_LATITUDE_REF	1
#define	GPS_LATITUDE		2
#define	GPS_LONGITUDE_REF	3
#define	GPS_LONGITUDE		4
#define	GPS_ALTITUDE_REF	5
#define	GPS_ALTITUDE		6
#define	GPS_TIME_STAMP		7
#define	GPS_SATELLITES		8
#define	GPS_STATUS		9
#define	GPS_MEASURE_MODE	10
#define	GPS_DOP			11
#define	GPS_SPEED_REF		12
#define	GPS_SPEED		13
#define	GPS_TRACK_REF		14
#define	GPS_TRACK		15
#define	GPS_IMG_DIRECTION_REF	16
#define	GPS_IMAGE_DIRECTION	17
#define	GPS_MAP_DATUM		18
#define	GPS_DEST_LATITUDE_REF	19
#define	GPS_DEST_LATITUDE	20
#define	GPS_DEST_LONGITUDE_REF	21
#define	GPS_DEST_LONGITUDE	22
#define	GPS_DEST_BEARING_REF	23
#define	GPS_DEST_BEARING	24
#define	GPS_DEST_DISTANCE_REF	25
#define	GPS_DEST_DISTANCE	26
#define	GPS_PROCESSING_METHOD	27
#define	GPS_AREA_INFORMATION	28
#define	GPS_DATE_STAMP		29
#define	GPS_DIFFERENTIAL	30

struct IFD {
	struct list_head head;
	unsigned short no_of_directory_entry;
	struct IFD* next_ifd;
};

struct JPEGHead {
	unsigned char SOI_maker[2];
	unsigned char APP1_maker[2];
	unsigned short APP1_size;
	struct IFD* ifd;
	struct exif_data* data;
};

struct directory_entry {
	struct list_head node;
	unsigned short tag_number;
	unsigned short type;
	unsigned int count;
	unsigned int data_value;
};

struct exif_data {
	unsigned char Exif_Header[6];
	unsigned char TIFF_Header[4];
	unsigned int offset_to_first_IFD;
	unsigned char data[0];
};

struct _GPS {
	unsigned char version_id[4];
	unsigned char latitude_ref[2];
	unsigned int latitude[6];
	unsigned char longitude_ref[2];
	unsigned int longitude[6];
	unsigned char altitude_ref;
	unsigned long altitude;
	unsigned long timestamp[3];
	unsigned char datestamp[11];
};

static void parseGPS(struct _GPS* gps, struct GPS* g)
{
	g->latitude[0] = (double)gps->latitude[0]/ (double)gps->latitude[1];
	g->latitude[1] = (double)gps->latitude[2]/(double)gps->latitude[3];
	g->latitude[2] = (double)gps->latitude[4]/(double)gps->latitude[5];

	g->longitude[0] = (double)gps->longitude[0]/(double)gps->longitude[1];
	g->longitude[1] = (double)gps->longitude[2]/(double)gps->longitude[3];
	g->longitude[2] = (double)gps->longitude[4]/(double)gps->longitude[5];

	memcpy(g->version, gps->version_id, 4);
	memcpy(g->datestamp, gps->datestamp, 11);
	g->latitude_ref = gps->latitude_ref[0];
	g->longitude_ref = gps->longitude_ref[0];
	g->altitude_ref = gps->altitude_ref;
	g->altitude = gps->altitude;

	g->timestamp.hour = gps->timestamp[0]&0xffffffff;
	g->timestamp.min = gps->timestamp[1]&0xffffffff;
	g->timestamp.sec = gps->timestamp[2]&0xffffffff;
}

static inline struct IFD* new_ifd()
{
	struct IFD* ifd = malloc(sizeof(struct IFD));
	ifd->next_ifd = NULL;	
	INIT_LIST_HEAD(&ifd->head);
	return ifd;
}

int readIFD(unsigned char *ptr, struct IFD* ifd)
{
	memcpy(&ifd->no_of_directory_entry, ptr, sizeof(ifd->no_of_directory_entry));
	ptr += sizeof(ifd->no_of_directory_entry);
	int offset = 0;
	for (int i=0; i< ifd->no_of_directory_entry; i++) {
		struct directory_entry *entry = malloc(sizeof(struct directory_entry));
		memcpy(&entry->tag_number, ptr, 12);
		ptr += 12;
		list_append(&entry->node, &ifd->head);
		offset = entry->data_value;
	}
	return offset;
}

#define	BYTE		1
#define	ASCII		2
#define	SHORT		3
#define	LONG		4
#define	RATIONAL	5
#define	UNDEFINED	7
#define	SLONG		9
#define	SRATIONAL	10

static inline int sizeof_type(int type)
{
	switch(type) {
		case BYTE: return 1;
		case ASCII: return 1;
		case SHORT: return 2;
		case LONG: return 4;
		case RATIONAL: return 8;
		case UNDEFINED: return 1;
		case SLONG: return 4;
		case SRATIONAL: return 8;
	}
}

void parseGPSIFD(struct GPS* gps, struct IFD* ifd, struct exif_data* data)
{
	struct list_head* node = ifd->head.next;
	struct _GPS g;
	while (node != &ifd->head) {
		char* ptr = NULL;
		struct directory_entry* entry = container_of(node, struct directory_entry, node);
		int size = sizeof_type(entry->type)*entry->count;
		if (size > 4) {
			ptr = data->TIFF_Header+entry->data_value;
		} else {
			ptr = (char*)&entry->data_value;
		}
		switch(entry->tag_number) {
			case GPS_VERSION_ID:
				memcpy(&g.version_id, ptr, size);
				break;
			case GPS_LATITUDE_REF:
				memcpy(&g.latitude_ref, ptr, size);
				break;
			case GPS_LATITUDE:
				memcpy(&g.latitude, ptr, size);
				break;
			case GPS_LONGITUDE_REF:
				memcpy(&g.longitude_ref, ptr, size);
				break;
			case GPS_LONGITUDE:
				memcpy(&g.longitude, ptr, size);
				break;
			case GPS_ALTITUDE_REF:
				memcpy(&g.altitude_ref, ptr, size); 
				break;
			case GPS_ALTITUDE:
				memcpy(&g.altitude, ptr, size);
				break;
			case GPS_TIME_STAMP:
				memcpy(&g.timestamp, ptr, size);
				break;
			case GPS_SATELLITES: break;
			case GPS_STATUS: break;
			case GPS_MEASURE_MODE: break;
			case GPS_DOP: break;
			case GPS_SPEED_REF: break;
			case GPS_SPEED: break;
			case GPS_TRACK_REF: break;
			case GPS_TRACK: break;
			case GPS_IMG_DIRECTION_REF: break;
			case GPS_IMAGE_DIRECTION: break;
			case GPS_MAP_DATUM: break;
			case GPS_DEST_LATITUDE_REF: break;
			case GPS_DEST_LATITUDE: break;
			case GPS_DEST_LONGITUDE_REF: break;
			case GPS_DEST_LONGITUDE: break;
			case GPS_DEST_BEARING_REF: break;
			case GPS_DEST_BEARING: break;
			case GPS_DEST_DISTANCE_REF: break;
			case GPS_DEST_DISTANCE: break;
			case GPS_PROCESSING_METHOD: break;
			case GPS_AREA_INFORMATION: break;
			case GPS_DATE_STAMP:
				memcpy(&g.datestamp, ptr, size);
				break;
			case GPS_DIFFERENTIAL: break;
			default: break;
		}
		node = node->next;
	}
	parseGPS(&g, gps);
}

void parse_entry(struct GPS* gps, struct directory_entry* entry, struct JPEGHead* head)
{
	struct exif_data* data = head->data;
	switch(entry->tag_number) {
		case GPSInfo:
			head->ifd->next_ifd = new_ifd();
			readIFD(data->TIFF_Header+entry->data_value, head->ifd->next_ifd);
			parseGPSIFD(gps, head->ifd->next_ifd, data);
			break;
		default : break;
	}
}

void parseIFD(struct GPS* gps, struct JPEGHead* head)
{
	struct IFD* ifd = head->ifd;
	struct list_head* node;
	while (ifd) {
		node = ifd->head.next;
		while (node != &ifd->head) {
			struct directory_entry *entry = container_of(node, struct directory_entry, node);
			parse_entry(gps, entry, head);
			node = node->next;
		}
		ifd = ifd->next_ifd;
	}
}

void parse_exif_data(struct exif_data* ed, struct JPEGHead* jh)
{
	int size = jh->APP1_size;
	unsigned char* ptr = ed->data;
	int offset = readIFD(ptr, jh->ifd);
}

struct JPEGHead* read_exif_head(char* filename)
{
	FILE* file = fopen(filename, "rb");
	if (!file) return NULL;
	struct JPEGHead* head = malloc(sizeof(struct JPEGHead));
	fread(head, offsetof(struct JPEGHead, ifd), 1, file);
	if (not_exif(head)) {
		free(head);
		return NULL;
	}
	head->data = malloc(head->APP1_size);
	head->ifd = new_ifd();
	fread(head->data, head->APP1_size, 1, file);
	fclose(file);
	parse_exif_data(head->data, head);
	return head;
}

_Bool is_exif(struct JPEGHead* head)
{	
	if (	head &&
		head->SOI_maker[0] == 0xff &&
		head->SOI_maker[1] == 0xd8 &&
		head->APP1_maker[0] == 0xFF &&
		head->APP1_maker[1] == 0xE1) {
		return true;
	}
	return false;
}

static void delete_entry(struct list_head* head)
{
	struct list_head* node, *tmp;
	node = head->next;
	while (node != head) {
		tmp = node;
		node = node->next;
		free(container_of(tmp, struct directory_entry, node));
	}
}

void delete_JPEGHead(struct JPEGHead* head)
{
	if (!head) return ;

	struct IFD* ifd = head->ifd;
	struct IFD* tmp = NULL;
	while (ifd) {
		tmp = ifd;
		ifd = ifd->next_ifd;
		delete_entry(&tmp->head);
		free(tmp);
	}
	free(head->data);
	free(head);
}

void read_gps_info(struct JPEGHead* head, struct GPS* gps)
{
	if (not_exif(head)) {
		memset(gps, 0, sizeof(struct GPS));
		return;
	}
	parseIFD(gps, head);
}


