# Parallelized RC4 Stream Cipher: Fast and Memory-Safe Encryption Done in Parallel
This program allows the user to encrypt or decrypt any plaintext or ciphertext file, using a random byte key of their choosing, as long as it is between 5 and 256 bytes so it is complaint with the RC4 stream cipher algorithm. The user can specify whether they want their output to be written to *STDOUT* or to a file, so that they can either pipe the output elsewhere, or simply store it according to their needs. 

Internally, this program uses dynamically allocated buffers so that memory is allocated appropriately and efficiently. The user can easily modify the code, by changing two macros, to specify the initial length of the buffer, and the amount that it grows by when capacity is reached. If the user knows the types of files they are often dealing with, and the size of these files, modifying these parameters can greatly increase the speed and the efficiency of the program and can take advantage of compiler optimization and OS pipelining. This implementation is very fast, memory efficient, and designed for avoiding the memory-corruption issues that can affect multi-threaded applications when memory is being dynamically allocated. 

Additionally, if a user wanted to use this as a template for other stream ciphers, that is easily doable. Simply by modifying one function, the *generateKeyStream* function, they can implement any other algorithm, and all other features of this program, its speed, safe and efficient memory allocation, safeguards for thread interaction, and write features remain.

## Features
- **Very fast**: Implemented in C, uses 4 concurrent threads, 5 threads in total
- **Memory-Safe**: Dynamically allocates memory as needed, designed to avoid cross-thread memory corruption
- **Secure**: Implements RC4 stream cipher utilizing proper algorithm specifications, such as proper key length
- **Modifiable**: The user can easily modify the size of the initial dynamic array and the amount it grows by when its capacity is reached
- **Template**: This program can easily serve as a template for other parallelized stream ciphers, simply by modifying one function

## Threads
This program uses C's Pthread library, in order to be portable across Unix based systems. 
5 threads run in total, with 4 threads being concurrent at any one time. 2 of the threads are launched internally from another thread.
This is what they do:
1. Read plaintext or ciphertext files into memory
2. Stage environment for RC4 algorithm
3. Read key file into memory
4. Use key file to generate key stream indefinitely
5. XOR plaintext/ciphertext and key stream and write output

Whether a plaintext/ciphertext file is a few kb, and read into memory near instantaneously, or a few gb, and is continuously read into memory over time, the program is designed to handle it. 

## Usage
#### To Compile:
```
gcc rc4.c -o rc4 -lpthread
```
#### Execution Format:
```
./rc4 <plaintext/ciphertext file> <key file> <stdout>
./rc4 <plaintext/ciphertext file> <key file> <file> <file name>
```
#### STDOUT Example:
```
./rc4 taleOfTwoCitiesExample.txt key.txt stdout > CT.txt
./rc4 CT.txt key.txt STDOUT > PT.txt
```
#### File Example:
```
./rc4 taleOfTwoCitiesExample.txt key.txt file CT.txt
./rc4 CT.txt key.txt file PT.txt
```
## Example
#### Given this Plaintext File:
```
It was the best of times, it was the worst of times, it was the age of wisdom, it was the age of foolishness, 
it was the epoch of belief, it was the epoch of incredulity, it was the season of Light, it was the season of 
Darkness, it was the spring of hope, it was the winter of despair, we had everything before us, we had nothing
before us, we were all going direct to Heaven, we were all going direct the other way - in short, the period was
so far like the present period, that some of its noisiest authorities insisted on its being received, for good
or for evil, in the superlative degree of comparison only.
```
#### We Run:
```
./rc4 taleOfTwoCitiesExample.txt key.txt file CT.txt
```
#### Inside CT.txt:
```
Ç^BÊUSïI^AÙ<99>^[3c¿M<9b>M<9a>^Gj^W^P`FEA+^\o^@<80>÷·"<96>Zzªk4AR<8e>Ì^Y]O¿¸/R¿´¡9/>M^\Ü<9f>H<9e>o2N´*r^K_<8c>
ì<89>rîÙÌÛM5{<9d>bo^[B^@^?Ë^CÝ<96>ÛÜ)Àwf¨<9d>bÿ<99>Ô<80>Ç<90>Z$^Wò^WH    Q^ý^L<89>e^_P<99>,Z^QãW^XcùÈ¼òb<92><90>
^@Þ^PÒuõ^V^YÖ§^D^G<93>^\Á<99>|"/ï¸p&Ká<96>gô^[^U^Tðv^AQçô(^W!Ã$tp^CÅ  g^Bq<92>lP¡3^]y<9f>0H<96>(®£8´í4b{"&<82>
d±÷c0Kêp3^E^]?Ç<8d>'"°*WâöM à<    8a>2O^?éôÉfÕ<97>5È<96>ªÓ{Oâ ¬>'^\-îfÉî,<92>|MÎþÆBz¨GT^Xp^FI^H©*ç5»^CÕ/+;xzXU
40F<85><86>Ö<89>%Õ<93>û<8b>F%²¢T®¶Ñ^_i<99>RÛë<82>®<93>-ê÷¦l^SÞ^SñJ%<86>*ïÇÀ%<95>ÅÇ0¶^A¦FÂ\VSÖ¹<96>^\j×h    <90>
R4<95>Á<8d>yáYhY   È&ëû<9e>dVþÜ½Ë¶^A9«ùÏE<96>?<8c>&q^VOø0@qø<80>×_<8c>×MZý]&<99>Ú<88>Ó<80>±%<88>P-Ï<82>±^NÔ<87>
Ó^_ßc<80>ÔÐ¶^N?Û<83>@à<9b>ÉP§<8a><91><91>ågÖõâ^@ös<84>^K:^Z±        ³Ã"!´^LË<87><95>Ï,<84>ùÒùÝ~^N<98>íWNùíÝHýÖ¥
<80>¼^G*rwÿ®;0<98>vút±<99>^Yw<99>&µ] ½¢ZÒ{ç0<9e>FïvÕo^? ^AJA<91>þç^B¥Ðö^\´qÝÐà¦j¯cÝÄ:ß<Á<88>Iç<9e>E,»^K¥¼ü^B<90>
^AZË«[à<81>KðBÅ0)B¬þsÏW\    ;×²)í¤^Ubß^H,½#È^]<92>^U^XÖ¥é¶^]<96>z^H5
```
#### We Run:
```
./rc4 CT.txt key.txt file PT.txt
```
#### Inside PT.txt:
```
It was the best of times, it was the worst of times, it was the age of wisdom, it was the age of foolishness, 
it was the epoch of belief, it was the epoch of incredulity, it was the season of Light, it was the season of 
Darkness, it was the spring of hope, it was the winter of despair, we had everything before us, we had nothing
before us, we were all going direct to Heaven, we were all going direct the other way - in short, the period was
so far like the present period, that some of its noisiest authorities insisted on its being received, for good
or for evil, in the superlative degree of comparison only.
```

## Notes
- This program is **not designed** to deal with **Infinite Text/File Streams** 
     - This involves continuously de-allocating memory once it has been processed
          - This program does not do that until the very end of the program 
     - This program does an fseek call to the end of the file being read in, which is problematic if the file is always being written to 
     - This functionality could be added however, and may be in the future 
- **Please make your own key**, do not use the *key.txt* file that came with this program, it is for example purposes only
     - Use the *createRandomKey.sh* script, which in turn uses the ```openssl rand <Byte Amount>``` script
