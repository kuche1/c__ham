
// TODO move bytes that used to be at checksum position to the end

#include <stdio.h>
#include <inttypes.h> // same
#include <stdint.h>   // same
#include <stdlib.h>

#define DEBUG

#ifdef DEBUG
	#define UINTN_CHECK(n, in) do{ \
		uint16_t zeroes_mask = (~(uint16_t)0) << n; \
		uint16_t zeroes = (in & zeroes_mask); \
		if(zeroes){ \
			printf("ERROR: this is not uint%d_t...\n", n); \
			exit(1); \
		} \
	}while(0);
#else
	#define UINTN_CHECK
#endif


uint8_t get_ham_hash(uint16_t in){
	uint8_t ham = 0;
	for(int i=0; i<16; ++i){
		if(in & (1<<i)){
			ham ^= i;
		}
	}
	UINTN_CHECK(4, ham);
	return ham;
}

uint16_t ham_encode(uint16_t in){
	UINTN_CHECK(11, in);

	int16_t cor = in << 1;

	for(int i=0; i<4; ++i){
		uint16_t offset = 1 << i;
		uint16_t mask = 1 << offset;

		uint8_t bit = !!(cor & mask);
		cor &= ~mask;
		cor |= bit << (12+i);
	}

	uint8_t ham = get_ham_hash(cor);

	for(int i=0; i<4; ++i){
		uint16_t offset = 1 << i;
		uint16_t bit = !!(ham & (1<<i));
		cor |= (bit << offset);
	}
	
	return cor;
}

uint16_t ham_decode(uint16_t in){
	uint16_t cor = in;
	uint8_t err = get_ham_hash(in);
	cor ^= (1<<err);

	for(int i=0; i<4; ++i){
		uint32_t offset = 1 << i;
		uint32_t mask = 1 << offset;

		uint8_t bit = !!(cor & (1 << (12+i)));
		cor &= ~(1 << (12+i));
		cor &= ~mask;
		cor |= bit << offset;
	}

	cor >>= 1;
	return cor;
}

int main(void){

	uint16_t code_in = 2047; // this is uint11_t
	printf("in : %d\n", code_in);

	uint16_t code_out = ham_encode(code_in);
	printf("out: %d\n", code_out);

	code_out ^= 1 << 11; // see ? it works; you can do the same with any field (as long as you do it once)
	uint16_t code_checked = ham_decode(code_out);
	printf("pas: %d\n", code_checked);

	return 0;
}

