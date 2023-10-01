# Solution

Now this is something that might got a bit harder than I wanted it to be.

The solution should not be __hard__, but would take some time. To speed it up, I added a few copy-pasted modules to the back side, just to let players experiment a bit...

The map implements a XOROSHIRO-type key stream encryptor / decyptor. The algoritm is in xoro.cpp. I tested it, and it's OK, the loop len is big enough. The crypto is 16 bit (2*8 bit state), and only uses XOR, ROT, SHIFT and ADD.

The contraption is set up so that without the set key, it would take far too long to fetch the flag. Also, only the first few bytes are shown, so reading the bits is a problem in itself.

Intended solution route
- identify it's a stream cipher
- extract algorithm
- extract bits
- implement in code
- bruteforce key and decode

Since the state is only 16 bits wide, bruteforcing can be quite fast.
