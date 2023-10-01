#include <cstdint>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <unordered_set>
#include <vector>
class Xoroshiro16MinusMinus
{
  const uint8_t a = 5;
  const uint8_t b = 5;
  const uint8_t c = 2;
  const uint8_t d = 1;

  static inline uint8_t rol(uint8_t x, uint8_t k) noexcept
  {
    return (x << k) | (x >> ((sizeof(x) * 8) - k));
  }
public:
  uint8_t s0;
  uint8_t s1;

public:
      explicit Xoroshiro16MinusMinus(uint8_t s0 = 1, uint8_t s1 = 0) noexcept
    : s0(s0), s1(s1)
  {
  }

  inline uint8_t operator()() noexcept
  {
    uint8_t result = rol(s0 + s1, d) + s0;

    s1 ^= s0;
    s0 = rol(s0, a ) ^ s1 ^ (s1 << b);
    s1 = rol(s1, c);


    return result;
  }
};

#define SAMPLE_SIZE 256

void test8()
{
  Xoroshiro16MinusMinus prng(1, 0);
  uint8_t random;
  std::cout << "First " << std::dec << SAMPLE_SIZE << " outputs of Xoroshiro32++:" << std::endl; 
  for (uint64_t i = 0; i < SAMPLE_SIZE; i++)
  {
    random = prng();
    printf("%02x\n", random);
  }
  
}
void test8_len() {
  Xoroshiro16MinusMinus prng(1, 0);
  std::unordered_set<uint16_t> stateSet;
  uint16_t state;
  uint8_t random;
  for (int i = 0; i<(1<<18); i++) {
    state = prng.s0 << 8 | prng.s1;
    random = prng();
    if (stateSet.count(state)) {
        printf("Cycle detected! Len=%d\n", i);
        break;
    }
    stateSet.insert(state);
  }
}

void test_encrypt_decrypt() {
    const uint16_t step_to = 0xdead;
    uint8_t msg[14] = "sEcr3t f14g!";
    uint8_t msg_enc[14] = {0};
    
    Xoroshiro16MinusMinus enc_stream(1, 0);
    for (int i = 0; i < step_to; i++)
        enc_stream();
    
    for (int i = 0; i<sizeof(msg); i++)
        msg_enc[i] = msg[i] ^ enc_stream();
    
    
    // now try to decrypt
    Xoroshiro16MinusMinus dec_stream(1, 0);
    uint8_t buff[32] = {0};
    int ptr = 0;
    for (int i = 0; i<(1<<18); i++) {
        buff[ptr] = dec_stream();
        
        // printf("Prev: %d, curr: %d\n", buff[(ptr - 1)%32],  buff[ptr]);
        bool is_ok = true;
        for (int j = 0; j<sizeof(msg); j++) {
            if ((buff[(ptr - sizeof(msg) + 1 + j)] ^ msg_enc[j]) != msg[j])
                is_ok = false;
        }
        
        if (is_ok) {
            printf("Decrypted it!\n k = %d (%1$x)\n", i - sizeof(msg)+1);
            printf("%c\n", buff[ptr] ^ msg_enc[1]);
        }
        
        //if (i%100 == 0)
        //    printf("%d\n", i);
        
        ptr = (ptr + 1) % 32;
    }
}

void encrypt_flag() {
    const int seek = 0xf00d;
    uint8_t msg[] = "SecuriTeam{h0p3_y0u_1ik3_r3dst0n3}";
    // uint8_t msg[] = "ABC";
    Xoroshiro16MinusMinus prng(1, 0);
    for (int i = 0; i<seek; i++) {
        prng();
        //printf("A");
    }
    
    uint8_t msg_enc[sizeof(msg)] = {0};
    for (int i = 0; i<sizeof(msg) - 1; i++) {
        uint8_t k = prng();
        msg_enc[sizeof(msg) - i - 2] = msg[sizeof(msg)-i-2] ^ k;
        printf("B=%02x (%c) K=%02x V=%02x\n", msg[sizeof(msg)-i-2], msg[sizeof(msg)-i-2], k, msg_enc[sizeof(msg)-i-2]);
    }
    
    for (int i = 0; i<sizeof(msg) - 1; i++) {
        //printf("%d(%c) ^ %02x = %d\n%", msg[i], msg[i], msg[i] ^msg_enc[i], msg_enc[i]);
        printf("%d, ", msg_enc[i]);
    }
}

int main(int argc, char* argv[])
{
  //test8();
  encrypt_flag();
  std::cout << std::endl;
}