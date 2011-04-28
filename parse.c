#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

	if (argc < 3) {
		printf("Usage : %s file offset\n", argv[0]);
		return 1;
	}
	offset = strtoul(argv[2], NULL, 0);
	raw = fopen(argv[1], "rb");

	if(!raw) {
		printf("Can't open %s\n", argv[1]);
		return 1;
	}
	printf("Opened %s at offset %x\n",argv[1], offset);

	fseek(raw, offset, SEEK_SET);
	fread(data, sizeof(uint16_t), 16, raw);
	fclose(raw);

	for (i=0; i<16; i++) {
		printf("%04x ", data[i]);
	}
	printf("\n");
	printf("r : %04x %04x %04x %04x %04x %04x %04x\n", data[2], data[3], data[6], data[7], data[9], data[10], data[13]);

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
