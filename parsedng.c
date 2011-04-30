#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>


unsigned char header[] = {0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA4, 0x00, 0x00, 0x00, 0x03};

int swap8(unsigned char *d) {
	unsigned char t[8];
	memcpy(t, d, 8);
	d[0] = t[7];
	d[1] = t[6];
	d[2] = t[5];
	d[3] = t[4];
	d[4] = t[3];
	d[5] = t[2];
	d[6] = t[1];
	d[7] = t[0];

	return 0;
}

int main(int argc, char *argv[]) 
{
	FILE *dng;
	unsigned char *buffer, *warp;
	struct stat stats;
	double *data;
	int i;

	if (argc < 2) { 
		return 1;
	}
	dng = fopen(argv[1], "rb");
	if (!dng) {
		return 1;
	}
	fstat(fileno(dng), &stats);

	buffer = (unsigned char *) malloc(stats.st_size);
	if (!buffer) {
		return 1;
	}

	fread(buffer, 1, stats.st_size, dng);
	fclose(dng);

	warp = memmem(buffer, stats.st_size, header, sizeof(header));
	warp += sizeof(header);
	printf("%lx\n", (unsigned long)(warp-buffer));
	data = (double *)warp;
	for(i=0; i<20; i++) {
		swap8(warp+i*8);
	}
	printf("r : %f %f %f %f %f %f\n", data[0], data[1], data[2], data[3], data[4], data[5]);
	data+=6;
	printf("g : %f %f %f %f %f %f\n", data[0], data[1], data[2], data[3], data[4], data[5]);
	data+=6;
	printf("b : %f %f %f %f %f %f\n", data[0], data[1], data[2], data[3], data[4], data[5]);
	return 0;	
}
