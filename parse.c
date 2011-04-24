#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint16_t checksum(uint8_t *data, int len)
{
  int i;
  int csum=0;
  for (i=0; i<len; i++) {
      csum = (73*csum + data[i])%0xFFEF;
  }
  return csum;
}

int main(int argc, char *argv[])
{
	FILE *raw;
	int offset;
	uint16_t data[16]; 
	uint8_t *data8 = (uint8_t *)data;
	int i;

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
	for (i=0; i<32; i++)
		printf("%02x ", data8[i]);
	printf("\n");

	printf("%x\n", checksum(data8+4,12));

	fclose(raw);
	return 0;
}
