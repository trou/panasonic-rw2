#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>

struct dist {
	double scale;
	double a, b, c;
	double n;
};

uint16_t checksum(uint8_t *data, int len)
{
	int i;
	int csum=0;

	for (i=0; i<len; i++) {
		csum = (73*csum + data[i])%0xFFEF;
	}
	return csum;
}

void fix_checksums(uint16_t *data)
{
	int i;
	uint8_t *data8 = (uint8_t *)data;
	uint8_t even[16], odd[16]; 
	uint16_t csum1, csum2, csum3, csum4;

	csum1=checksum(data8+4,12);
	csum2=checksum(data8+16,12);

	data[1] = csum1;
	data[14] = csum2;
	for (i=0; i<16; i++) {
		even[i] = data8[i*2];
		odd[i] = data8[i*2+1];
	}

	csum3=checksum(even+1,14);
	csum4=checksum(odd+1,14);
	
	data[0] = csum3;
	data[15] = csum4;
	return;
}

int verify_checksums(uint16_t *data)
{
	int i;
	uint8_t *data8 = (uint8_t *)data;
	uint8_t even[16], odd[16]; 
	uint16_t csum1, csum2, csum3, csum4;
	int res;

	for (i=0; i<16; i++) {
		even[i] = data8[i*2];
		odd[i] = data8[i*2+1];
	}
	csum1=checksum(data8+4,12);
	csum2=checksum(data8+16,12);
	csum3=checksum(even+1,14);
	csum4=checksum(odd+1,14);
	res = 0;
	res ^= (csum1 ^ data[1]);
	res ^= (csum2 ^ data[14]);
	res ^= (csum3 ^ data[0]);
	res ^= (csum4 ^ data[15]);
	return res;
}

int main(int argc, char *argv[])
{
	FILE *raw;
	int offset;
	int16_t data[16]; 
/*	uint8_t *data8 = (uint8_t *)data;*/
	int i;
	struct dist d;
	int opt, mod = 0, index = 0;
	int16_t val;

	while ((opt = getopt(argc, argv, "m:i:")) != -1) {
		switch (opt) {
			case 'm':
				mod = 1;
				val = strtoul(optarg, NULL, 0); 
				break;
			case 'i':
				index = atoi(optarg);
				break;
			default: /* '?' */
				fprintf(stderr, "Usage : %s file offset\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	if (argc-optind <= 1) {
		fprintf(stderr, "Usage : %s file offset\n", argv[0]);
		return 1;
	}

	offset = strtoul(argv[optind+1], NULL, 0);
	raw = fopen(argv[optind], "rb+");

	if(!raw) {
		printf("Can't open %s\n", argv[1]);
		return 1;
	}
	printf("Opened %s at offset %x\n", argv[optind], offset);

	fseek(raw, offset, SEEK_SET);
	fread(data, sizeof(int16_t), 16, raw);

	printf("Tag flag : %d\n", data[7]);
	if(mod) {
		printf("Modifiying entry %d to %04hx\n", index, val);
		data[index] = val;
		fix_checksums((uint16_t *)data);
		fseek(raw, offset, SEEK_SET);
		fwrite(data, sizeof(int16_t), 16, raw);
	}

	fclose(raw);

	printf("Tag data :\n");
	for (i=0; i<16; i++) {
		printf("%04hx ", data[i]);
	}
	printf("\n");
	/*printf("r : %04hx %04hx %04hx %04hx %04hx %04hx\n", data[2], data[3], data[6], data[9], data[10], data[13]);*/

	if (verify_checksums((uint16_t *)data))
		printf("Checksum NOK\n");
	else
		printf("Checksum OK\n");

	
	d.n = data[12];
	d.scale = 1.0/(1.0+(data[5]/32768.0));
	d.a = d.scale*(data[8]/32768.0);
	d.b = d.scale*(data[4]/32768.0);
	d.c = d.scale*(data[11]/32768.0);
	printf("%f %f %f %f %f\n", d.n, d.scale, d.a, d.b, d.c);
	return 0;
}
