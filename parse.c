#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct dist {
	int16_t num1;
	int16_t num2;
	int16_t num3;
	int16_t num4;
	int16_t num5;
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
	uint16_t data[16]; 
	uint8_t *data8 = (uint8_t *)data;
	int i;
	struct dist d;
	double t1, t2, t3, t4;

	if (argc < 3) {
		printf("Usage : %s file offset", argv[0]);
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

	for (i=0; i<32; i++) {
		printf("%02x ", data8[i]);
	}
	printf("\n");

	if (verify_checksums(data))
		printf("Checksum NOK\n");
	else
		printf("Checksum OK\n");

	
	d.num1 = data[12];
	d.num2 = data[5];
	d.num3 = data[8];
	d.num4 = data[4];
	d.num5 = data[11];
	printf("%d %d %d %d %d\n", d.num1, d.num2, d.num3, d.num4, d.num5);
	t1 = 1.0/(d.num2/32768.0+1.0);
	t2 = t1*d.num3/32768.0;
	t3 = d.num4/32768.0*t1;
	t4 = d.num5/32768.0*t1;
	printf("%f %f %f %f\n", t1, t2, t3, t4);
	return 0;
}
