
### Base 128 Varints
Variable-width integers, or varints, are at the core of the wire format.
They allow encoding unsigned 64-bit integers using anywhere between one and ten bytes, with small values using fewer bytes.

Each byte in the varint has a continuation bit that indicates if the byte that follows it is part of the varint.
This is the most significant bit (MSB) of the byte (sometimes also called the sign bit).
The lower 7 bits are a payload; the resulting integer is built by appending together the 7-bit payloads of its constituent bytes.

So, for example, here is the number 1, encoded as `01` – it’s a single byte, so the MSB is not set:

```
0000 0001
^ msb
```
And here is 150, encoded as `9601` – this is a bit more complicated:

```
10010110 00000001
^ msb    ^ msb
```
How do you figure out that this is 150?
First you drop the MSB from each byte, as this is just there to tell us whether we’ve reached the end of the number 
(as you can see, it’s set in the first byte as there is more than one byte in the varint).
These 7-bit payloads are in little-endian order.
Convert to big-endian order, concatenate, and interpret as an unsigned 64-bit integer:
```
10010110 00000001        // Original inputs.
 0010110  0000001        // Drop continuation bits.
 0000001  0010110        // Convert to big-endian.
   00000010010110        // Concatenate.
 128 + 16 + 4 + 2 = 150  // Interpret as an unsigned 64-bit integer.
```

Because varints are so crucial to protocol buffers, in protoscope syntax, we refer to them as plain integers. 150 is the same as `9601`.

