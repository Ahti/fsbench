// Nakamichi is 100% FREE LZSS SUPERFAST decompressor.
// Home of Nakamichi: www.sanmayce.com/Nakamichi/index.html

// Nakamichi_Jiten.c, a 2bytes 'Washi', with 4/8 long matches, it uses only 64bit registers.
// Nakamichi_Washi.c, a branchless 'Kaibutsu' it is using 4MB window.
// Nakamichi_Kumataka.c, a branchless 'Kaibutsu' it is but with two Match_Length values and using only 31KB window.
// Min_Match_Length=16-1 i.e. 15/7
// 152,089 alice29.txt
// 106,023 alice29.txt.Nakamichi
// Min_Match_Length=16-3 i.e. 13/6
// 152,089 alice29.txt
// 100,555 alice29.txt.Nakamichi
// Min_Match_Length=16-5 i.e. 11/5
// 152,089 alice29.txt
//  96,350 alice29.txt.Nakamichi
// Min_Match_Length=16-7 i.e. 9/4
// 152,089 alice29.txt
//  93,693 alice29.txt.Nakamichi

// Nakamichi_Kaiju.c, a branchless 'Kaibutsu' it is.
// ML=9
// 68,352,060 enwik8.Kaiju.Nakamichi
// ML=8
// 63,748,036 enwik8.Kaiju.Nakamichi
// ML=7
// 59,771,603 enwik8.Kaiju.Nakamichi
// ML=6
// 57,090,382 enwik8.Kaiju.Nakamichi
// ML=5
// 56,188,976
// ML=4
// 58,954,436 enwik8.Kaiju.Nakamichi
// Nakamichi_Kaibutsu.c, three small tweaks in Kaidanji, a good idea to remove shiftings altogether by m^2 was used.
// Nakamichi_Kaidanji.c, is the very same '1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX'.

// Nakamichi, revision 1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX, written by Kaze, babealicious suggestion by m^2 enforced.
// Fixed! TO-DO: Known bug: the decompressed file sometimes has few additional bytes at the end.
// Change #1: Now instead of looking first in the leftmost end of the window a "preemptive" search is done 16*8*128 bytes before the rightmost end of the window, there is the hottest (cachewise&matchwise) zone, as a side effect the compression speed is much higher. Maybe in the future I will try hashing as well.
// Change #2: The full 16bits are used for offsets, 64KB window, that is.

// Compile line:
//icl /O3 Nakamichi_r1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX.c -D_N_GP /FAcs
//ren Nakamichi_r1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX.cod Nakamichi_r1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX_GP.cod
//ren Nakamichi_r1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX.exe Nakamichi_r1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX_GP.exe
//icl /O3 /QxSSE2 Nakamichi_r1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX.c -D_N_XMM /FAcs
//ren Nakamichi_r1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX.cod Nakamichi_r1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX_XMM.cod
//ren Nakamichi_r1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX.exe Nakamichi_r1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX_XMM.exe
//icl /O3 /QxAVX Nakamichi_r1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX.c -D_N_YMM /FAcs
//ren Nakamichi_r1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX.cod Nakamichi_r1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX_YMM.cod
//ren Nakamichi_r1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX.exe Nakamichi_r1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX_Kaidanji_FIX_YMM.exe

// Nakamichi, revision 1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy_FIX, written by Kaze, babealicious suggestion by m^2 enforced.
// Change #1: Nasty bug in Swampshine was fixed.
// Change #2: Sanity check in compression section was added thus avoiding 'index-Min_Match_Length' going below 0.

// Nakamichi, revision 1-RSSBO_1GB_Wordfetcher_TRIAD_NOmemcpy, written by Kaze, babealicious suggestion by m^2 enforced.
// Change #1: 'memcpy' replaced by GP/XMM/YMM TRIADs.
// Nakamichi, revision 1-RSSBO_1GB_Wordfetcher_TRIAD, written by Kaze.
// Change #1: Decompression fetches WORD instead of BYTE+BYTE.
// Change #2: Decompression stores three times 64bit instead of memcpy() for all transfers <=24 bytes.
// Change #3: Fifteenth bit is used and then unused, 16KB -> 32KB -> 16KB.
// 32KB window disappoints speedwise, also sizewise:
/*
D:\_KAZE\_KAZE_GOLD\Nakamichi_projectOLD\Nakamichi_vs_Yappy>Nakamichi_r1-RSSBO_1GB_15bit_Wordfetcher.exe enwik8
Nakamichi, revision 1-RSSBO_1GB_15bit_Wordfetcher, written by Kaze, based on Nobuo Ito's LZSS source.
Compressing 100000000 bytes ...
-; Each rotation means 128KB are encoded; Done 100%
RAM-to-RAM performance: 130 KB/s.

D:\_KAZE\_KAZE_GOLD\Nakamichi_projectOLD\Nakamichi_vs_Yappy>Nakamichi_r1-RSSBO_1GB_15bit_Wordfetcher.exe enwik8.Nakamichi
Nakamichi, revision 1-RSSBO_1GB_15bit_Wordfetcher, written by Kaze, based on Nobuo Ito's LZSS source.
Decompressing 65693566 bytes ...
RAM-to-RAM performance: 358 MB/s.

D:\_KAZE\_KAZE_GOLD\Nakamichi_projectOLD\Nakamichi_vs_Yappy>Nakamichi_r1-RSSBO_1GB_15bit_Wordfetcher.exe enwik9
Nakamichi, revision 1-RSSBO_1GB_15bit_Wordfetcher, written by Kaze, based on Nobuo Ito's LZSS source.
Compressing 1000000000 bytes ...
/; Each rotation means 128KB are encoded; Done 100%
RAM-to-RAM performance: 150 KB/s.

D:\_KAZE\_KAZE_GOLD\Nakamichi_projectOLD\Nakamichi_vs_Yappy>Nakamichi_r1-RSSBO_1GB_15bit_Wordfetcher.exe enwik9.Nakamichi
Nakamichi, revision 1-RSSBO_1GB_15bit_Wordfetcher, written by Kaze, based on Nobuo Ito's LZSS source.
Decompressing 609319736 bytes ...
RAM-to-RAM performance: 379 MB/s.
*/
// 1-RSSBO_1GB vs 1-RSSBO_1GB_15bit_Wordfetcher (16KB/32KB respectively):
// 069,443,065 vs 065,693,566
// 641,441,055 vs 609,319,736

// Nakamichi, revision 1-RSSBO_1GB, written by Kaze.
// Based on Nobuo Ito's source, thanks Ito.
// The main goal of Nakamichi is to allow supersimple and superfast decoding for English x-grams (mainly) in pure C, or not, heh-heh.
// Natively Nakamichi is targeted as 64bit tool with 16 threads, helping Kazahana to traverse faster when I/O is not superior.
// In short, Nakamichi is intended as x-gram decompressor.

// Eightfold Path ~ the Buddhist path to nirvana, comprising eight aspects in which an aspirant must become practised; 
// eightfold way ~ (Physics), the grouping of hadrons into supermultiplets by means of SU(3)); (b) adverb to eight times the number or quantity: OE.

// Note1: Fifteenth bit is not used, making the window wider by 1bit i.e. 32KB is not tempting, rather I think to use it as a flag: 8bytes/16bytes.
// Note2: English x-grams are as English texts but more redundant, in other words they are phraselists in most cases, sometimes wordlists.
// Note3: On OSHO.TXT, being a typical English text, Nakamichi's compression ratio is among the worsts:
//        206,908,949 OSHO.TXT
//        125,022,859 OSHO.TXT.Nakamichi
//        It struggles with English texts but decomprression speed is quite sweet (Core 2 T7500 2200MHz, 32bit code):
//        Nakamichi, revision 1-, written by Kaze.
//        Decompressing 125022859 bytes ...
//        RAM-to-RAM performance: 477681 KB/s.      
// Note4: Also I wanted to see how my 'Railgun_Swampshine_BailOut', being a HEAVYGUN i.e. with big overhead and latency, hits in a real-world application.

// Quick notes on PAGODAs (the padded x-gram lists):
// Every single word in English has its own PAGODA, in example below 'on' PAGODA is given (Kazahana_on.PAGODA-order-5.txt):
// PAGODA order 5 (i.e. with 5 tiers) has 5*(5+1)/2=15 subtiers, they are concatenated and space-padded in order to form the pillar 'on':
/*
D:\_KAZE\Nakamichi_r1-RSSBO>dir \_GW\ka*

04/12/2014  05:07 AM                14 Kazahana_on.1-1.txt
04/12/2014  05:07 AM         1,635,389 Kazahana_on.2-1.txt
04/12/2014  05:07 AM         1,906,734 Kazahana_on.2-2.txt
04/12/2014  05:07 AM        10,891,415 Kazahana_on.3-1.txt
04/12/2014  05:07 AM        15,797,703 Kazahana_on.3-2.txt
04/12/2014  05:07 AM        20,419,280 Kazahana_on.3-3.txt
04/12/2014  05:07 AM        22,141,823 Kazahana_on.4-1.txt
04/12/2014  05:07 AM        36,002,113 Kazahana_on.4-2.txt
04/12/2014  05:07 AM        33,236,772 Kazahana_on.4-3.txt
04/12/2014  05:07 AM        33,902,425 Kazahana_on.4-4.txt
04/12/2014  05:07 AM        24,795,989 Kazahana_on.5-1.txt
04/12/2014  05:07 AM        30,766,220 Kazahana_on.5-2.txt
04/12/2014  05:07 AM        38,982,816 Kazahana_on.5-3.txt
04/12/2014  05:07 AM        38,089,575 Kazahana_on.5-4.txt
04/12/2014  05:07 AM        34,309,057 Kazahana_on.5-5.txt
04/12/2014  05:07 AM       846,351,894 Kazahana_on.PAGODA-order-5.txt

D:\_KAZE\Nakamichi_r1-RSSBO>type \_GW\Kazahana_on.1-1.txt
9,999,999       on

D:\_KAZE\Nakamichi_r1-RSSBO>type \_GW\Kazahana_on.2-1.txt
9,999,999       on_the
1,148,054       on_his
0,559,694       on_her
0,487,856       on_this
0,399,485       on_your
0,381,570       on_my
0,367,282       on_their
...

D:\_KAZE\Nakamichi_r1-RSSBO>type \_GW\Kazahana_on.2-2.txt
0,545,191       based_on
0,397,408       and_on
0,334,266       go_on
0,329,561       went_on
0,263,035       was_on
0,246,332       it_on
0,229,041       down_on
0,202,151       going_on
...

D:\_KAZE\Nakamichi_r1-RSSBO>type \_GW\Kazahana_on.5-5.txt
0,083,564       foundation_osho_s_books_on
0,012,404       medium_it_may_be_on
0,012,354       if_you_received_it_on
0,012,152       medium_they_may_be_on
0,012,144       agree_to_also_provide_on
0,012,139       a_united_states_copyright_on
0,008,067       we_are_constantly_working_on
0,008,067       questions_we_have_received_on
0,006,847       file_was_first_posted_on
0,006,441       of_we_are_already_on
0,006,279       you_received_this_ebook_on
0,005,865       you_received_this_etext_on
0,005,833       to_keep_an_eye_on
...

D:\_KAZE\Nakamichi_r1-RSSBO>dir

04/12/2014  05:07 AM       846,351,894 Kazahana_on.PAGODA-order-5.txt

D:\_KAZE\Nakamichi_r1-RSSBO>Nakamichi.exe Kazahana_on.PAGODA-order-5.txt
Nakamichi, revision 1-RSSBO, written by Kaze.
Compressing 846351894 bytes ...
/; Each rotation means 128KB are encoded; Done 100%
RAM-to-RAM performance: 512 KB/s.

D:\_KAZE\Nakamichi_r1-RSSBO>dir

04/12/2014  05:07 AM       846,351,894 Kazahana_on.PAGODA-order-5.txt
04/15/2014  06:30 PM       293,049,398 Kazahana_on.PAGODA-order-5.txt.Nakamichi

D:\_KAZE\Nakamichi_r1-RSSBO>Nakamichi.exe Kazahana_on.PAGODA-order-5.txt.Nakamichi
Nakamichi, revision 1-RSSBO, written by Kaze.
Decompressing 293049398 bytes ...
RAM-to-RAM performance: 607 MB/s.

D:\_KAZE\Nakamichi_r1-RSSBO>Yappy.exe Kazahana_on.PAGODA-order-5.txt 4096
YAPPY: [b 4K] bytes 846351894 -> 191149889  22.6%  comp  33.8 MB/s  uncomp 875.4 MB/s

D:\_KAZE\Nakamichi_r1-RSSBO>Yappy.exe Kazahana_on.PAGODA-order-5.txt 8192
YAPPY: [b 8K] bytes 846351894 -> 184153244  21.8%  comp  35.0 MB/s  uncomp 898.3 MB/s

D:\_KAZE\Nakamichi_r1-RSSBO>Yappy.exe Kazahana_on.PAGODA-order-5.txt 16384
YAPPY: [b 16K] bytes 846351894 -> 180650931  21.3%  comp  28.8 MB/s  uncomp 906.4 MB/s

D:\_KAZE\Nakamichi_r1-RSSBO>Yappy.exe Kazahana_on.PAGODA-order-5.txt 32768
YAPPY: [b 32K] bytes 846351894 -> 178902966  21.1%  comp  35.0 MB/s  uncomp 906.4 MB/s

D:\_KAZE\Nakamichi_r1-RSSBO>Yappy.exe Kazahana_on.PAGODA-order-5.txt 65536
YAPPY: [b 64K] bytes 846351894 -> 178027899  21.0%  comp  34.5 MB/s  uncomp 914.6 MB/s

D:\_KAZE\Nakamichi_r1-RSSBO>Yappy.exe Kazahana_on.PAGODA-order-5.txt 131072
YAPPY: [b 128K] bytes 846351894 -> 177591807  21.0%  comp  34.9 MB/s  uncomp 906.4 MB/s

D:\_KAZE\Nakamichi_r1-RSSBO>
*/

// During compilation use one of these, the granularity of the padded 'memcpy', 4x2x8/2x2x16/1x2x32/1x1x64 respectively as GP/XMM/YMM/ZMM, the maximum literal length reduced from 127 to 63:
#define _N_GP
//#define _N_XMM
//#define _N_YMM
//#define _N_ZMM

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // uint64_t needed
#include <time.h>
#include <string.h>

#ifdef _N_XMM
#include <emmintrin.h> // SSE2 intrinsics
#include <smmintrin.h> // SSE4.1 intrinsics
#endif
#ifdef _N_YMM
#include <emmintrin.h> // SSE2 intrinsics
#include <smmintrin.h> // SSE4.1 intrinsics
#include <immintrin.h> // AVX intrinsics
#endif
#ifdef _N_ZMM
#include <emmintrin.h> // SSE2 intrinsics
#include <smmintrin.h> // SSE4.1 intrinsics
#include <immintrin.h> // AVX intrinsics
#include <zmmintrin.h> // AVX2 intrinsics, definitions and declarations for use with 512-bit compiler intrinsics.
#endif

#ifdef _N_XMM
void SlowCopy128bit (const char *SOURCE, char *TARGET) { _mm_storeu_si128((__m128i *)(TARGET), _mm_loadu_si128((const __m128i *)(SOURCE))); }
#endif
#ifdef _N_YMM
void SlowCopy128bit (const char *SOURCE, char *TARGET) { _mm_storeu_si128((__m128i *)(TARGET), _mm_loadu_si128((const __m128i *)(SOURCE))); }
#endif
#ifdef _N_ZMM
void SlowCopy128bit (const char *SOURCE, char *TARGET) { _mm_storeu_si128((__m128i *)(TARGET), _mm_loadu_si128((const __m128i *)(SOURCE))); }
#endif
/*
 * Move Unaligned Packed Integer Values
 * **** VMOVDQU ymm1, m256
 * **** VMOVDQU m256, ymm1
 * Moves 256 bits of packed integer values from the source operand to the
 * destination
 */
//extern __m256i __ICL_INTRINCC _mm256_loadu_si256(__m256i const *);
//extern void    __ICL_INTRINCC _mm256_storeu_si256(__m256i *, __m256i);
#ifdef _N_YMM
void SlowCopy256bit (const char *SOURCE, char *TARGET) { _mm256_storeu_si256((__m256i *)(TARGET), _mm256_loadu_si256((const __m256i *)(SOURCE))); }
#endif
//extern __m512i __ICL_INTRINCC _mm512_loadu_si512(void const*);
//extern void    __ICL_INTRINCC _mm512_storeu_si512(void*, __m512i);
#ifdef _N_ZMM
void SlowCopy512bit (const char *SOURCE, char *TARGET) { _mm512_storeu_si512((__m512i *)(TARGET), _mm512_loadu_si512((const __m512i *)(SOURCE))); }
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

// Comment it to see how slower 'BruteForce' is, for Wikipedia 100MB the ratio is 41KB/s versus 197KB/s.
#define ReplaceBruteForceWithRailgunSwampshineBailOut

static void SearchIntoSlidingWindow(unsigned int* retIndex, unsigned int* retMatch, char* refStart,char* refEnd,char* encStart,char* encEnd);
static unsigned int SlidingWindowVsLookAheadBuffer(char* refStart, char* refEnd, char* encStart, char* encEnd);
unsigned int JitenCompress(char* ret, char* src, unsigned int srcSize);
unsigned int JitenDecompress(char* ret, char* src, unsigned int srcSize);
static char * Railgun_Swampshine_BailOut(char * pbTarget, char * pbPattern, uint32_t cbTarget, uint32_t cbPattern);
static char * Railgun_Doublet (char * pbTarget, char * pbPattern, uint32_t cbTarget, uint32_t cbPattern);

// The pair SHORT/LONG to be respectively in range 3..8/9..24:
// 4/12:
// 846,351,894 Kazahana_on.PAGODA-order-5.txt
// 219,459,398 Kazahana_on.PAGODA-order-5.txt.Nakamichi
// 6/13:
// 846,351,894 Kazahana_on.PAGODA-order-5.txt
// 213,629,110 Kazahana_on.PAGODA-order-5.txt.Nakamichi
// 5/13:
//   846,351,894 Kazahana_on.PAGODA-order-5.txt
//   210,396,428 Kazahana_on.PAGODA-order-5.txt.Nakamichi
//   206,908,949 OSHO.TXT
//    99,739,184 OSHO.TXT.Nakamichi
// 1,000,000,000 enwik9
//   531,893,878 enwik9.Nakamichi
// 6/14:
// 846,351,894 Kazahana_on.PAGODA-order-5.txt
// 207,213,691 Kazahana_on.PAGODA-order-5.txt.Nakamichi
// 5/14:
// 846,351,894 Kazahana_on.PAGODA-order-5.txt
// 205,946,653 Kazahana_on.PAGODA-order-5.txt.Nakamichi
// 5/8:
// 1,000,000,000 enwik9
//   525,215,362 enwik9.Nakamichi
//   846,351,894 Kazahana_on.PAGODA-order-5.txt
//   271,833,018 Kazahana_on.PAGODA-order-5.txt.Nakamichi
//   206,908,949 OSHO.TXT
//    96,001,059 OSHO.TXT.Nakamichi

// Min_Match_Length=THRESHOLD=4 means 4 and bigger are to be encoded:
#define Min_Match_BAILOUT_Length (8)
#define Min_Match_Length (8)
#define Min_Match_Length_SHORT (5)
#define OffsetBITS (24-2 -7)
#define LengthBITS (1)

//12bit
//#define REF_SIZE (4095+Min_Match_Length)
//#define REF_SIZE ( ((1<<OffsetBITS)-1) + Min_Match_Length )
#define REF_SIZE ( ((1<<OffsetBITS)-1) )
//3bit
//#define ENC_SIZE (7+Min_Match_Length)
#define ENC_SIZE ( ((1<<LengthBITS)-1) + Min_Match_Length )
/*
int main( int argc, char *argv[] ) {
	FILE *fp;
	int SourceSize;
	int TargetSize;
	char* SourceBlock=NULL;
	char* TargetBlock=NULL;
	char* Nakamichi = ".Nakamichi\0";
	char NewFileName[256];
	clock_t clocks1, clocks2;

char *pointerALIGN;
int i, j;
clock_t clocks3, clocks4;
double duration;
int BandwidthFlag=0;

unsigned long long k;

	printf("Nakamichi 'Jiten', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced, muffinesque suggestion by Jim Dempsey enforced.\n");
	if (argc==1) {
		printf("Usage: Nakamichi filename\n"); exit(13);
	}
	if (argc==3) BandwidthFlag=1;
	if ((fp = fopen(argv[1], "rb")) == NULL) {
		printf("Nakamichi: Can't open '%s' file.\n", argv[1]); exit(13);
	}
	fseek(fp, 0, SEEK_END);
	SourceSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	// If filename ends in '.Nakamichi' then mode is decompression otherwise compression.
	if (strcmp(argv[1]+(strlen(argv[1])-strlen(Nakamichi)), Nakamichi) == 0) {
	SourceBlock = (char*)malloc(SourceSize+512);
	//TargetBlock = (char*)malloc(5*SourceSize+512);
	TargetBlock = (char*)malloc(1111*1024*1024+512);
	fread(SourceBlock, 1, SourceSize, fp);
	fclose(fp);
		printf("Decompressing %d bytes ...\n", SourceSize );
		clocks1 = clock();
		while (clocks1 == clock());
		clocks1 = clock();
		TargetSize = Decompress(TargetBlock, SourceBlock, SourceSize);
		clocks2 = clock();
		k = (((float)1000*TargetSize/(clocks2 - clocks1 + 1))); k=k>>20;
		printf("RAM-to-RAM performance: %d MB/s.\n", k);
		strcpy(NewFileName, argv[1]);
		*( NewFileName + strlen(argv[1])-strlen(Nakamichi) ) = '\0';
	} else {
	SourceBlock = (char*)malloc(SourceSize+512);
	TargetBlock = (char*)malloc(SourceSize+512+32*1024*1024); //+32*1024*1024, some files may be expanded instead of compressed.
	fread(SourceBlock, 1, SourceSize, fp);
	fclose(fp);
		printf("Compressing %d bytes ...\n", SourceSize );
		clocks1 = clock();
		while (clocks1 == clock());
		clocks1 = clock();
		TargetSize = Compress(TargetBlock, SourceBlock, SourceSize);
		clocks2 = clock();
		k = (((float)1000*SourceSize/(clocks2 - clocks1 + 1))); k=k>>10;
		printf("RAM-to-RAM performance: %d KB/s.\n", k);
		strcpy(NewFileName, argv[1]);
		strcat(NewFileName, Nakamichi);
	}
	if ((fp = fopen(NewFileName, "wb")) == NULL) {
		printf("Nakamichi: Can't write '%s' file.\n", NewFileName); exit(13);
	}
	fwrite(TargetBlock, 1, TargetSize, fp);
	fclose(fp);

	if (BandwidthFlag) {
// Benchmark memcpy() [
pointerALIGN = TargetBlock + 64 - (((size_t)TargetBlock) % 64);
//offset=64-int((long)data&63);
printf("Memory pool starting address: %p ... ", pointerALIGN);
if (((uintptr_t)(const void *)pointerALIGN & (64 - 1)) == 0) printf( "64 byte aligned, OK\n"); else printf( "NOT 64 byte aligned, FAILURE\n");
clocks3 = clock();
while (clocks3 == clock());
clocks3 = clock();
printf("Copying a %dMB block 1024 times i.e. %dGB READ + %dGB WRITTEN ...\n", 256, 256, 256);
	for (i = 0; i < 1024; i++) {
	memcpy(pointerALIGN+256*1024*1024, pointerALIGN, 256*1024*1024);
	}
clocks4 = clock();
duration = (double) (clocks4 - clocks3 + 1);
printf("memcpy(): (%dMB block); %dMB copied in %d clocks or %.3fMB per clock\n", 256, 1024*( 256 ), (int) duration, (float)1024*( 256 )/ ((int) duration));
// Benchmark memcpy() ]
k = (((float)1000*TargetSize/(clocks2 - clocks1 + 1))); k=k>>20;
j = (float)1000*1024*( 256 )/ ((int) duration);
printf("RAM-to-RAM performance vs memcpy() ratio (bigger-the-better): %d%%\n", (int)(k*100/j));
	}

	free(TargetBlock);
	free(SourceBlock);
	exit(0);
}*/

void SearchIntoSlidingWindow(unsigned int* retIndex, unsigned int* retMatch, char* refStart,char* refEnd,char* encStart,char* encEnd){
	char* FoundAtPosition;
	unsigned int match=0;
	char* refStartCOLD1 = refStart;
	char* refStartHOT1 = refStart+((1<<OffsetBITS)-255*8*128);
	char* refStartHOTTER1 = refStart+((1<<OffsetBITS)-31*8*128);
	char* refStartCOLD2 = refStart;
	char* refStartHOT2 = refStart+((1<<OffsetBITS)-255*8*128);
	char* refStartHOTTER2 = refStart+((1<<OffsetBITS)-31*8*128);
	char* refStartCOLD3 = refStart;
	char* refStartHOT3 = refStart+((1<<OffsetBITS)-255*8*128);
	char* refStartHOTTER3 = refStart+((1<<OffsetBITS)-31*8*128);
	char* refStartCOLD4 = refStart;
	char* refStartHOT4 = refStart+((1<<OffsetBITS)-255*8*128);
	char* refStartHOTTER4 = refStart+((1<<OffsetBITS)-31*8*128);
	*retIndex=0;
	*retMatch=0;

#ifdef ReplaceBruteForceWithRailgunSwampshineBailOut

// 32>>0:
/*
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) [
	while (refStartHOTTER1 < refEnd) {
	FoundAtPosition = Railgun_Swampshine_BailOut(refStartHOTTER1, encStart, (uint32_t)(refEnd-refStartHOTTER1), (Min_Match_Length>>0));	
		if (FoundAtPosition!=NULL) {
			if ( (refEnd-FoundAtPosition) & 0xE0 ) { // Discard matches that have OFFSET with higher 3bits ALL zero.
				*retMatch=(Min_Match_Length>>0);
				*retIndex=(refEnd-FoundAtPosition)&0x3FFF; // [00]11
				return;
			}
			refStartHOTTER1=FoundAtPosition+1; // Exhaust the pool.
		} else break;
	}
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) ]

	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) [
	while (refStartHOT1 < refEnd) {
	FoundAtPosition = Railgun_Swampshine_BailOut(refStartHOT1, encStart, (uint32_t)(refEnd-refStartHOT1), (Min_Match_Length>>0));	
		if (FoundAtPosition!=NULL) {
			if ( (refEnd-FoundAtPosition) & 0xE0 ) { // Discard matches that have OFFSET with higher 3bits ALL zero.
				*retMatch=(Min_Match_Length>>0);
				*retIndex=(refEnd-FoundAtPosition)&0x3FFF; // [00]11
				return;
			}
			refStartHOT1=FoundAtPosition+1; // Exhaust the pool.
		} else break;
	}
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) ]
*/
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) [
	while (refStartCOLD1 < refEnd) {
	FoundAtPosition = Railgun_Swampshine_BailOut(refStartCOLD1, encStart, (uint32_t)(refEnd-refStartCOLD1), (Min_Match_Length>>0));	
		if (FoundAtPosition!=NULL) {
			if ( (refEnd-FoundAtPosition) & 0xF8 ) { // Discard matches that have OFFSET with higher 3bits ALL zero.
				*retMatch=(Min_Match_Length>>0);
				*retIndex=(refEnd-FoundAtPosition)&0x7FFF; // [00]11
				return;
			}
			refStartCOLD1=FoundAtPosition+1; // Exhaust the pool.
		} else break;
	}
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) ]


// 32>>1:
/*
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) [
	while (refStartHOTTER2 < refEnd) {
	FoundAtPosition = Railgun_Swampshine_BailOut(refStartHOTTER2, encStart, (uint32_t)(refEnd-refStartHOTTER2), (Min_Match_Length>>1));	
		if (FoundAtPosition!=NULL) {
			if ( (refEnd-FoundAtPosition) & 0xE0 ) { // Discard matches that have OFFSET with higher 3bits ALL zero.
				*retMatch=(Min_Match_Length>>1);
				*retIndex=((refEnd-FoundAtPosition)&0x3FFF)|0x4000; // [01]11
				return;
			}
			refStartHOTTER2=FoundAtPosition+1; // Exhaust the pool.
		} else break;
	}
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) ]

	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) [
	while (refStartHOT2 < refEnd) {
	FoundAtPosition = Railgun_Swampshine_BailOut(refStartHOT2, encStart, (uint32_t)(refEnd-refStartHOT2), (Min_Match_Length>>1));	
		if (FoundAtPosition!=NULL) {
			if ( (refEnd-FoundAtPosition) & 0xE0 ) { // Discard matches that have OFFSET with higher 3bits ALL zero.
				*retMatch=(Min_Match_Length>>1);
				*retIndex=((refEnd-FoundAtPosition)&0x3FFF)|0x4000; // [01]11
				return;
			}
			refStartHOT2=FoundAtPosition+1; // Exhaust the pool.
		} else break;
	}
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) ]
*/
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) [
	while (refStartCOLD2 < refEnd) {
	FoundAtPosition = Railgun_Swampshine_BailOut(refStartCOLD2, encStart, (uint32_t)(refEnd-refStartCOLD2), (Min_Match_Length>>1));	
		if (FoundAtPosition!=NULL) {
			if ( (refEnd-FoundAtPosition) & 0xF8 ) { // Discard matches that have OFFSET with higher 3bits ALL zero.
				*retMatch=(Min_Match_Length>>1);
				*retIndex=((refEnd-FoundAtPosition)&0x7FFF)|0x8000; // [10]11
				return;
			}
			refStartCOLD2=FoundAtPosition+1; // Exhaust the pool.
		} else break;
	}
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) ]

// 32>>2:
/*
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) [
	while (refStartHOTTER3 < refEnd) {
	FoundAtPosition = Railgun_Swampshine_BailOut(refStartHOTTER3, encStart, (uint32_t)(refEnd-refStartHOTTER3), (Min_Match_Length>>2));	
		if (FoundAtPosition!=NULL) {
			if ( (refEnd-FoundAtPosition) & 0xE0 ) { // Discard matches that have OFFSET with higher 3bits ALL zero.
				*retMatch=(Min_Match_Length>>2);
				*retIndex=((refEnd-FoundAtPosition)&0x3FFF)|0x8000; // [10]11
				return;
			}
			refStartHOTTER3=FoundAtPosition+1; // Exhaust the pool.
		} else break;
	}
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) ]

	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) [
	while (refStartHOT3 < refEnd) {
	FoundAtPosition = Railgun_Swampshine_BailOut(refStartHOT3, encStart, (uint32_t)(refEnd-refStartHOT3), (Min_Match_Length>>2));	
		if (FoundAtPosition!=NULL) {
			if ( (refEnd-FoundAtPosition) & 0xE0 ) { // Discard matches that have OFFSET with higher 3bits ALL zero.
				*retMatch=(Min_Match_Length>>2);
				*retIndex=((refEnd-FoundAtPosition)&0x3FFF)|0x8000; // [10]11
				return;
			}
			refStartHOT3=FoundAtPosition+1; // Exhaust the pool.
		} else break;
	}
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) ]

	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) [
	while (refStartCOLD3 < refEnd) {
	FoundAtPosition = Railgun_Swampshine_BailOut(refStartCOLD3, encStart, (uint32_t)(refEnd-refStartCOLD3), (Min_Match_Length>>2));	
		if (FoundAtPosition!=NULL) {
			if ( (refEnd-FoundAtPosition) & 0xF0 ) { // Discard matches that have OFFSET with higher 3bits ALL zero.
				*retMatch=(Min_Match_Length>>2);
				*retIndex=((refEnd-FoundAtPosition)&0x3FFF)|0x8000; // [10]11
				return;
			}
			refStartCOLD3=FoundAtPosition+1; // Exhaust the pool.
		} else break;
	}
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) ]
*/

// 32>>3:
/*
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) [
	while (refStartHOTTER4 < refEnd) {
	FoundAtPosition = Railgun_Swampshine_BailOut(refStartHOTTER4, encStart, (uint32_t)(refEnd-refStartHOTTER4), (Min_Match_Length>>3));	
		if (FoundAtPosition!=NULL) {
			if ( (refEnd-FoundAtPosition) & 0xE0 ) { // Discard matches that have OFFSET with higher 3bits ALL zero.
				*retMatch=(Min_Match_Length>>3);
				*retIndex=((refEnd-FoundAtPosition)&0x3FFF)|0xC000; // [11]11
				return;
			}
			refStartHOTTER4=FoundAtPosition+1; // Exhaust the pool.
		} else break;
	}
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) ]

	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) [
	while (refStartHOT4 < refEnd) {
	FoundAtPosition = Railgun_Swampshine_BailOut(refStartHOT4, encStart, (uint32_t)(refEnd-refStartHOT4), (Min_Match_Length>>3));	
		if (FoundAtPosition!=NULL) {
			if ( (refEnd-FoundAtPosition) & 0xE0 ) { // Discard matches that have OFFSET with higher 3bits ALL zero.
				*retMatch=(Min_Match_Length>>3);
				*retIndex=((refEnd-FoundAtPosition)&0x3FFF)|0xC000; // [11]11
				return;
			}
			refStartHOT4=FoundAtPosition+1; // Exhaust the pool.
		} else break;
	}
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) ]

	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) [
	while (refStartCOLD4 < refEnd) {
	FoundAtPosition = Railgun_Swampshine_BailOut(refStartCOLD4, encStart, (uint32_t)(refEnd-refStartCOLD4), (Min_Match_Length>>3));	
		if (FoundAtPosition!=NULL) {
			if ( (refEnd-FoundAtPosition) & 0xF0 ) { // Discard matches that have OFFSET with higher 3bits ALL zero.
				*retMatch=(Min_Match_Length>>3);
				*retIndex=((refEnd-FoundAtPosition)&0x3FFF)|0xC000; // [11]11
				return;
			}
			refStartCOLD4=FoundAtPosition+1; // Exhaust the pool.
		} else break;
	}
	// Pre-emptive strike, matches should be sought close to the lookahead (cache-friendliness) ]
*/

#else				
	while(refStart < refEnd){
		match=SlidingWindowVsLookAheadBuffer(refStart,refEnd,encStart,encEnd);
		if(match > *retMatch){
			*retMatch=match;
			*retIndex=refEnd-refStart;
		}
		if(*retMatch >= Min_Match_BAILOUT_Length) break;
		refStart++;
	}
#endif
}

unsigned int SlidingWindowVsLookAheadBuffer( char* refStart, char* refEnd, char* encStart,char* encEnd){
	int ret = 0;
	while(refStart[ret] == encStart[ret]){
		if(&refStart[ret] >= refEnd) break;
		if(&encStart[ret] >= encEnd) break;
		ret++;
		if(ret >= Min_Match_BAILOUT_Length) break;
	}
	return ret;
}

unsigned int JitenCompress(char* ret, char* src, unsigned int srcSize){
	unsigned int srcIndex=0;
	unsigned int retIndex=0;
	unsigned int index=0;
	unsigned int match=0;
	unsigned int notMatch=0;
	unsigned char* notMatchStart=NULL;
	char* refStart=NULL;
	char* encEnd=NULL;
	/*int Melnitchka=0;
	char *Auberge[4] = {"|\0","/\0","-\0","\\\0"};
	int ProgressIndicator;*/

	unsigned int NumberOfFullLiterals=0;
	int GLOBALlongM=0;
	int GLOBALmediumM=0;
	int GLOBALshortM=0;
	int GLOBALtinyM=0;

	while(srcIndex < srcSize){
		if(srcIndex>=REF_SIZE)
			refStart=&src[srcIndex-REF_SIZE];
		else
			refStart=src;
		if(srcIndex>=srcSize-ENC_SIZE)
			encEnd=&src[srcSize];
		else
			encEnd=&src[srcIndex+ENC_SIZE];
		// Fixing the stupid 'search-beyond-end' bug:
		if(srcIndex+ENC_SIZE < srcSize) {
			SearchIntoSlidingWindow(&index,&match,refStart,&src[srcIndex],&src[srcIndex],encEnd);
			if ( match==4 ) GLOBALtinyM++;
			if ( match==8 ) GLOBALshortM++;
			if ( match==16 ) GLOBALmediumM++;
			if ( match==32 ) GLOBALlongM++;
		}
		else
			match=0; // Nothing to find.
		//if ( match<Min_Match_Length ) {
		//if ( match<Min_Match_Length || match<8 ) {
  		if ( match==0 ) {
			if(notMatch==0){
				notMatchStart=&ret[retIndex];
				retIndex++;
			}
			//else if (notMatch==(127-64-32)) {
			else if (notMatch==(127-64-32-16-8)) {
NumberOfFullLiterals++;
				//*notMatchStart=(unsigned char)((127-64-32)<<3);
				*notMatchStart=(unsigned char)((127-64-32-16-8)<<(4-4));
				notMatch=0;
				notMatchStart=&ret[retIndex];
				retIndex++;
			}
			ret[retIndex]=src[srcIndex];
			retIndex++;
			notMatch++;
			srcIndex++;
			/*if ((srcIndex-1) % (1<<16) > srcIndex % (1<<16)) {
				ProgressIndicator = (int)( (srcIndex+1)*(float)100/(srcSize+1) );
				printf("%s; Each rotation means 64KB are encoded; Done %d%%\r", Auberge[Melnitchka++], ProgressIndicator );
				Melnitchka = Melnitchka & 3; // 0 1 2 3: 00 01 10 11
			}*/
		} else {
			if(notMatch > 0){
				*notMatchStart=(unsigned char)((notMatch)<<(4-4));
				notMatch=0;
			}
// ---------------------| 
//                     \ /

			//ret[retIndex] = 0x80; // Assuming seventh/fifteenth bit is zero i.e. LONG MATCH i.e. Min_Match_BAILOUT_Length*4
	  		//if ( match==Min_Match_BAILOUT_Length ) ret[retIndex] = 0xC0; // 8bit&7bit set, SHORT MATCH if seventh/fifteenth bit is not zero i.e. Min_Match_BAILOUT_Length
//                     / \
// ---------------------|
/*
			ret[retIndex] = 0x01; // Assuming seventh/fifteenth bit is zero i.e. LONG MATCH i.e. Min_Match_BAILOUT_Length*4
	  		if ( match==Min_Match_BAILOUT_Length ) ret[retIndex] = 0x03; // 2bit&1bit set, LONG MATCH if 2bit is not zero i.e. Min_Match_BAILOUT_Length
*/
// No need of above, during compression we demanded lowest 2bits to be not 00.
			// 1bit+3bits+12bits:
			//ret[retIndex] = ret[retIndex] | ((match-Min_Match_Length)<<4);
			//ret[retIndex] = ret[retIndex] | (((index-Min_Match_Length) & 0x0F00)>>8);
			// 1bit+1bit+14bits:
			//ret[retIndex] = ret[retIndex] | ((match-Min_Match_Length)<<(8-(LengthBITS+1))); // No need to set the matchlength
// The fragment below is outrageously ineffective - instead of 8bit&7bit I have to use the lower TWO bits i.e. 2bit&1bit as flags, thus in decompressing one WORD can be fetched instead of two BYTE loads followed by SHR by 2.
// ---------------------| 
//                     \ /
			//ret[retIndex] = ret[retIndex] | (((index-Min_Match_Length) & 0x3F00)>>8); // 2+4+8=14
			//retIndex++;
			//ret[retIndex] = (char)((index-Min_Match_Length) & 0x00FF);
			//retIndex++;
//                     / \
// ---------------------|
			// Now the situation is like LOW:HIGH i.e. FF:3F i.e. 0x3FFF, 16bit&15bit used as flags,
			// should become LOW:HIGH i.e. FC:FF i.e. 0xFFFC, 2bit&1bit used as flags.
/*
			ret[retIndex] = ret[retIndex] | (((index-Min_Match_Length) & 0x00FF)<<2); // 6+8=14
			//ret[retIndex] = ret[retIndex] | (((index-Min_Match_Length) & 0x00FF)<<1); // 7+8=15
			retIndex++;
			ret[retIndex] = (char)(((index-Min_Match_Length) & 0x3FFF)>>6);
			//ret[retIndex] = (char)(((index-Min_Match_Length) & 0x7FFF)>>7);
			retIndex++;
*/
// No need of above, during compression we demanded lowest 2bits to be not 00, use the full 16bits and get rid of the stupid '+/-' Min_Match_Length.
			//if (index>0xFFFF) {printf ("\nFatal error: Overflow!\n"); exit(13);}
			memcpy(&ret[retIndex],&index,2+1 -1); // copy lower 2 bytes
			retIndex++;
			retIndex++;
			 //retIndex++;
//                     / \
// ---------------------|
			srcIndex+=match;
			/*if ((srcIndex-match) % (1<<16) > srcIndex % (1<<16)) {
				ProgressIndicator = (int)( (srcIndex+1)*(float)100/(srcSize+1) );
				printf("%s; Each rotation means 64KB are encoded; Done %d%%\r", Auberge[Melnitchka++], ProgressIndicator );
				Melnitchka = Melnitchka & 3; // 0 1 2 3: 00 01 10 11
			}*/
		}
	}
	if(notMatch > 0){
		*notMatchStart=(unsigned char)((notMatch)<<(4-4));
	}/*
	printf("%s; Each rotation means 64KB are encoded; Done %d%%\n", Auberge[Melnitchka], 100 );
	printf("NumberOfFullLiterals (lower-the-better): %d\n", NumberOfFullLiterals );
printf("NumberOfTinyMatches: %d\n", GLOBALtinyM);
printf("NumberOfShortMatches: %d\n", GLOBALshortM);
printf("NumberOfMediumMatches: %d\n", GLOBALmediumM);
printf("NumberOfLongMatches: %d\n", GLOBALlongM);*/

	return retIndex;
}

// Core 2 T7500 2200MHz:
/*
D:\_KAZE\Nakamichi_Kaidanji_benchmark\Nakamichi_benchmark\Nakamichi_Jiten_JB>dir

05/16/2014  08:22 AM       206,908,949 OSHO.TXT
08/29/2014  02:06 AM        87,532,655 OSHO.TXT.Nakamichi

D:\_KAZE\Nakamichi_Kaidanji_benchmark\Nakamichi_benchmark\Nakamichi_Jiten_JB>Nakamichi_Jiten_GP.exe OSHO.TXT.Nakamichi
Nakamichi 'Jiten', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced, muffinesque suggestion by Jim Dempsey enforced.
Decompressing 87532655 bytes ...
RAM-to-RAM performance: 664 MB/s.

D:\_KAZE\Nakamichi_Kaidanji_benchmark\Nakamichi_benchmark\Nakamichi_Jiten_JB>
*/


unsigned int JitenDecompress(char* ret, char* src, unsigned int srcSize){
	//unsigned int srcIndex=0; // Dummy me
	//unsigned int retIndex=0; // Dummy me
	// The muffinesque suggestion by Jim Dempsey enforced:
	char* retLOCAL = ret;
	char* srcLOCAL = src;
	char* srcEndLOCAL = src+srcSize;
	unsigned int DWORDtrio;
	//while(srcIndex < srcSize){ // Dummy me
	while(srcLOCAL < srcEndLOCAL){
		//DWORDtrio = *(unsigned short int*)&src[srcIndex]; // Dummy me
		DWORDtrio = *(unsigned short int*)srcLOCAL;
// |1stLSB   |2ndLSB   |
// ---------------------
// |xxxTT|TTT|xxxxxx|xL| ! For texts it is better to reduce LL to L i.e. 8/4 matches and 32KB window !
// ---------------------
// [1bit          16bit]
// LL = 0 means MatchLength (8>>LL) or 8
// LL = 1 means MatchLength (8>>LL) or 4
		if ( (DWORDtrio & 0xF8) == 0 ) {
			#ifdef _N_GP
			//*(uint64_t*)(ret+retIndex+8*(0)) = *(uint64_t*)(src+srcIndex+1+16*(0)); // Dummy me
			*(uint64_t*)(retLOCAL+8*(0)) = *(uint64_t*)(srcLOCAL+1+16*(0));
			//memcpy(retLOCAL, (const char *)( (uint64_t)(srcLOCAL+1) ), 8);
			#endif
			//retIndex+= (DWORDtrio & 0xFF); // Dummy me
			retLOCAL+= (DWORDtrio & 0xFF); 
			//srcIndex+= ((DWORDtrio & 0xFF)+1); // Dummy me
			srcLOCAL+= ((DWORDtrio & 0xFF)+1);
		} else {
			#ifdef _N_GP
			//*(uint64_t*)(ret+retIndex+8*(0)) = *(uint64_t*)(ret+retIndex-(DWORDtrio&0x7FFF)); // Dummy me
			*(uint64_t*)(retLOCAL+8*(0)) = *(uint64_t*)(retLOCAL-(DWORDtrio&0x7FFF));
			//memcpy(retLOCAL, (const char *)( (uint64_t)(retLOCAL-(DWORDtrio&0x7FFF)) ), 8);
			#endif
			//retIndex+= (Min_Match_Length>>(DWORDtrio>>(23-8))); // Dummy me
			retLOCAL+= (Min_Match_Length>>(DWORDtrio>>(23-8)));
			//srcIndex+= (3-1); // Dummy me
			srcLOCAL+= (3-1);
		}
	}
	//return retIndex; // Dummy me
	return (unsigned int)(retLOCAL - ret);
}


/*
; 'Jiten' decompression loop, 60-11+2=81 bytes long:
; mark_description "Intel(R) C++ Intel(R) 64 Compiler XE for applications running on Intel(R) 64, Version 12.1.1.258 Build 20111";
; mark_description "-O3 -D_N_GP -FAcs";

.B6.3::                         
  00011 0f b7 0a         movzx ecx, WORD PTR [rdx]              
  00014 f7 c1 f8 00 00 
        00               test ecx, 248                          
  0001a 75 18            jne .B6.5 
.B6.4::                         
  0001c 44 0f b6 d9      movzx r11d, cl                         
  00020 0f b6 c9         movzx ecx, cl                          
  00023 ff c1            inc ecx                                
  00025 4c 8b 52 01      mov r10, QWORD PTR [1+rdx]             
  00029 4d 89 11         mov QWORD PTR [r9], r10                
  0002c 4d 03 cb         add r9, r11                            
  0002f 48 03 d1         add rdx, rcx                           
  00032 eb 29            jmp .B6.6 
.B6.5::                         
  00034 41 89 ca         mov r10d, ecx                          
  00037 48 83 c2 02      add rdx, 2                             
  0003b 49 81 e2 ff 7f 
        00 00            and r10, 32767                         
  00042 49 f7 da         neg r10                                
  00045 4d 03 d1         add r10, r9                            
  00048 c1 e9 0f         shr ecx, 15                            
  0004b 4d 8b 1a         mov r11, QWORD PTR [r10]               
  0004e 41 ba 08 00 00 
        00               mov r10d, 8                            
  00054 41 d3 ea         shr r10d, cl                           
  00057 4d 89 19         mov QWORD PTR [r9], r11                
  0005a 4d 03 ca         add r9, r10                            
.B6.6::                         
  0005d 49 3b d0         cmp rdx, r8                            
  00060 72 af            jb .B6.3 
*/


// Dirty-written 'Jiten':
/*
unsigned int Decompress(char* ret, char* src, unsigned int srcSize){
	unsigned int srcIndex=0;
	unsigned int retIndex=0;
	unsigned int DWORDtrio;

	while(srcIndex < srcSize){
		DWORDtrio = *(unsigned short int*)&src[srcIndex];
// |1stLSB   |2ndLSB   |
// ---------------------
// |xxxTT|TTT|xxxxxx|xL| ! For texts it is better to reduce LL to L i.e. 8/4 matches and 32KB window !
// ---------------------
// [1bit          16bit]
// LL = 0 means MatchLength (8>>LL) or 8
// LL = 1 means MatchLength (8>>LL) or 4
		if( (DWORDtrio & 0xF8) == 0){
			#ifdef _N_GP
			*(uint64_t*)(ret+retIndex+8*(0)) = *(uint64_t*)(src+srcIndex+1+16*(0));
			#endif
			retIndex+= (DWORDtrio & 0xFF); 
			srcIndex+= ((DWORDtrio & 0xFF)+1);
		}
		else{
			#ifdef _N_GP
			*(uint64_t*)(ret+retIndex+8*(0)) = *(uint64_t*)(ret+retIndex-(DWORDtrio&0x7FFF));
			#endif
			retIndex+= (Min_Match_Length>>(DWORDtrio>>(23-8)));
			srcIndex+= (3-1);
		}
	}
	return retIndex;
}
*/

/*
; 'Jiten' decompression loop, 68-16+2=84 bytes long:
; mark_description "Intel(R) C++ Intel(R) 64 Compiler XE for applications running on Intel(R) 64, Version 12.1.1.258 Build 20111";
; mark_description "-O3 -D_N_GP -FAcs";

.B6.3::                         
  00016 44 89 cb         mov ebx, r9d                           
  00019 0f b7 0c 13      movzx ecx, WORD PTR [rbx+rdx]          
  0001d f7 c1 f8 00 00 
        00               test ecx, 248                          
  00023 75 15            jne .B6.5 
.B6.4::                         
  00025 0f b6 c9         movzx ecx, cl                          
  00028 48 8b 5c 13 01   mov rbx, QWORD PTR [1+rbx+rdx]         
  0002d 4a 89 1c 10      mov QWORD PTR [rax+r10], rbx           
  00031 03 c1            add eax, ecx                           
  00033 45 8d 4c 09 01   lea r9d, DWORD PTR [1+r9+rcx]          
  00038 eb 2b            jmp .B6.6 
.B6.5::                         
  0003a 41 89 cb         mov r11d, ecx                          
  0003d 41 83 c1 02      add r9d, 2                             
  00041 89 c3            mov ebx, eax                           
  00043 49 81 e3 ff 7f 
        00 00            and r11, 32767                         
  0004a 49 03 da         add rbx, r10                           
  0004d 49 f7 db         neg r11                                
  00050 4c 03 db         add r11, rbx                           
  00053 c1 e9 0f         shr ecx, 15                            
  00056 4d 8b 1b         mov r11, QWORD PTR [r11]               
  00059 4c 89 1b         mov QWORD PTR [rbx], r11               
  0005c bb 08 00 00 00   mov ebx, 8                             
  00061 d3 eb            shr ebx, cl                            
  00063 03 c3            add eax, ebx                           
.B6.6::                         
  00065 45 3b c8         cmp r9d, r8d                           
  00068 72 ac            jb .B6.3 
*/

// Well-written 'Jiten':
/*
unsigned int Decompress(char* ret, char* src, unsigned int srcSize){
	unsigned int srcIndex=0;
	unsigned int retIndex=0;
	unsigned int DWORDtrio;
	unsigned int Flag;
	uint64_t FlagMASK; //=       0xFFFFFFFFFFFFFFFF;
	uint64_t FlagMASKnegated; //=0x0000000000000000;

	while(srcIndex < srcSize){
		DWORDtrio = *(unsigned int*)&src[srcIndex];
// |1stLSB   |2ndLSB   |
// ---------------------
// |xxxxT|TTT|xxxxxx|LL| ! For texts it is better to reduce LL to L i.e. 8/4 matches and 32KB window !
// ---------------------
// [1bit          16bit]
// LL = 0 UNUSED [means MatchLength (32>>LL) or 32]
// LL = 1 means MatchLength (32>>LL) or 16
// LL = 2 means MatchLength (32>>LL) or 8
// LL = 3 means MatchLength (32>>LL) or 4
		//Flag=!(DWORDtrio & 0xE0);
		Flag=!(DWORDtrio & 0xF0);
		// In here Flag=0|1
		FlagMASKnegated= Flag - 1; // -1|0
		FlagMASK= ~FlagMASKnegated;
				#ifdef _N_XMM
		SlowCopy128bit( (const char *)( ((uint64_t)(src+srcIndex+1+16*(0))&FlagMASK) + ((uint64_t)(ret+retIndex-(DWORDtrio&0x3FFF))&FlagMASKnegated) ), (ret+retIndex+16*(0)));
//		SlowCopy128bit( (const char *)( ((uint64_t)(src+srcIndex+1+16*(1))&FlagMASK) + ((uint64_t)(ret+retIndex-(DWORDtrio&0x3FFF)+16*(1))&FlagMASKnegated) ), (ret+retIndex+16*(1)));
				#endif
//				#ifndef _N_YMM
//		memcpy((ret+retIndex+16*(0)), (const char *)( ((uint64_t)(src+srcIndex+1)&FlagMASK) + ((uint64_t)(ret+retIndex-(DWORDtrio&0x3FFF))&FlagMASKnegated) ), 32);
//				#endif
//				#ifdef _N_YMM
//		SlowCopy256bit( (const char *)( ((uint64_t)(src+srcIndex+1)&FlagMASK) + ((uint64_t)(ret+retIndex-(DWORDtrio&0x3FFF))&FlagMASKnegated) ), (ret+retIndex+16*(0)));
//				#endif
		srcIndex+= ((uint64_t)((DWORDtrio & 0xFF)+1)&FlagMASK) + ((uint64_t)(3-1)&FlagMASKnegated) ;
		retIndex+= ((uint64_t)((DWORDtrio & 0xFF))&FlagMASK) +   ((uint64_t)(Min_Match_Length>>((DWORDtrio&0xFFFF)>>(22-8)))&FlagMASKnegated) ;
	}
	return retIndex;
}
*/
/*
; 'Jiten' decompression loop, b5-34+6=135 bytes long:
; mark_description "Intel(R) C++ Intel(R) 64 Compiler XE for applications running on Intel(R) 64, Version 12.1.1.258 Build 20111";
; mark_description "-O3 -QxSSE2 -D_N_XMM -FAcs";

.B7.3::                         
  00034 42 8b 34 12      mov esi, DWORD PTR [rdx+r10]           
  00038 45 33 e4         xor r12d, r12d                         
  0003b f7 c6 f0 00 00 
        00               test esi, 240                          
  00041 44 0f 44 e0      cmove r12d, eax                        
  00045 49 89 f5         mov r13, rsi                           
  00048 41 ff cc         dec r12d                               
  0004b 49 81 e5 ff 3f 
        00 00            and r13, 16383                         
  00052 49 f7 dd         neg r13                                
  00055 4c 89 e7         mov rdi, r12                           
  00058 4d 03 e9         add r13, r9                            
  0005b 48 f7 d7         not rdi                                
  0005e 4a 8d 5c 12 01   lea rbx, QWORD PTR [1+rdx+r10]         
  00063 4d 03 eb         add r13, r11                           
  00066 48 23 df         and rbx, rdi                           
  00069 4d 23 ec         and r13, r12                           
  0006c 0f b7 ce         movzx ecx, si                          
  0006f c1 e9 0e         shr ecx, 14                            
  00072 f3 41 0f 6f 44 
        1d 00            movdqu xmm0, XMMWORD PTR [r13+rbx]     
  00079 40 0f b6 de      movzx ebx, sil                         
  0007d 4d 89 e5         mov r13, r12                           
  00080 ff c3            inc ebx                                
  00082 49 83 e5 02      and r13, 2                             
  00086 48 23 df         and rbx, rdi                           
  00089 49 03 dd         add rbx, r13                           
  0008c 49 03 da         add rbx, r10                           
  0008f 41 89 da         mov r10d, ebx                          
  00092 40 0f b6 de      movzx ebx, sil                         
  00096 be 20 00 00 00   mov esi, 32                            
  0009b d3 ee            shr esi, cl                            
  0009d 48 23 df         and rbx, rdi                           
  000a0 49 23 f4         and rsi, r12                           
  000a3 48 03 de         add rbx, rsi                           
  000a6 49 03 db         add rbx, r11                           
  000a9 f3 43 0f 7f 04 
        19               movdqu XMMWORD PTR [r9+r11], xmm0      
  000af 41 89 db         mov r11d, ebx                          
  000b2 45 3b d0         cmp r10d, r8d                          
  000b5 0f 82 79 ff ff 
        ff               jb .B7.3 
*/

// With full use of the 64KB, Results_Core2_T7500.txt:
/*
Nakamichi 'Nekomata', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced.
Decompressing 52146282 bytes ...
RAM-to-RAM performance: 433 MB/s.
Nakamichi 'Nekomata', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced.
Decompressing 52146282 bytes ...
RAM-to-RAM performance: 435 MB/s.
Nakamichi 'Nekomata', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced.
Decompressing 52146282 bytes ...
RAM-to-RAM performance: 433 MB/s.

Nakamichi 'Hitomi', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced.
Decompressing 52146282 bytes ...
RAM-to-RAM performance: 435 MB/s.
Nakamichi 'Hitomi', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced.
Decompressing 52146282 bytes ...
RAM-to-RAM performance: 433 MB/s.
Nakamichi 'Hitomi', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced.
Decompressing 52146282 bytes ...
RAM-to-RAM performance: 435 MB/s.

Nakamichi 'Kaiju', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced.
Decompressing 63748036 bytes ...
RAM-to-RAM performance: 607 MB/s.
Nakamichi 'Kaiju', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced.
Decompressing 63748036 bytes ...
RAM-to-RAM performance: 607 MB/s.
Nakamichi 'Kaiju', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced.
Decompressing 63748036 bytes ...
RAM-to-RAM performance: 607 MB/s.

Nakamichi 'Kaidanji', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced.
Decompressing 63430147 bytes ...
RAM-to-RAM performance: 676 MB/s.
Nakamichi 'Kaidanji', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced.
Decompressing 63430147 bytes ...
RAM-to-RAM performance: 676 MB/s.
Nakamichi 'Kaidanji', written by Kaze, based on Nobuo Ito's LZSS source, babealicious suggestion by m^2 enforced.
Decompressing 63430147 bytes ...
RAM-to-RAM performance: 671 MB/s.

YAPPY: [b 1K] bytes 100000000 -> 73533773  73.5%  comp  41.3 MB/s  uncomp 658.4 MB/s 
YAPPY: [b 2K] bytes 100000000 -> 67516056  67.5%  comp  38.0 MB/s  uncomp 602.5 MB/s 
YAPPY: [b 4K] bytes 100000000 -> 61757720  61.8%  comp  34.4 MB/s  uncomp 547.0 MB/s 
YAPPY: [b 8K] bytes 100000000 -> 57701807  57.7%  comp  30.6 MB/s  uncomp 524.0 MB/s 
YAPPY: [b 64K] bytes 100000000 -> 54162908  54.2%  comp  28.7 MB/s  uncomp 509.1 MB/s 
YAPPY: [b 1024K] bytes 100000000 -> 53687370  53.7%  comp  28.3 MB/s  uncomp 509.5 MB/s 
*/

// Kaibutsu
// Decompression main loop:
/*
; mark_description "Intel(R) C++ Compiler XE for applications running on IA-32, Version 12.1.1.258 Build 20111011";
; mark_description "-O3 -QxSSE2 -D_N_XMM -FAcs";

.B7.3:                          
  0001e 0f b7 34 3a      movzx esi, WORD PTR [edx+edi]          
  00022 8b de            mov ebx, esi                           
  00024 81 e3 ff 00 00 
        00               and ebx, 255                           
  0002a 83 fb 10         cmp ebx, 16                            
  0002d 72 1d            jb .B7.5 
.B7.4:                          
  0002f 8b 4c 24 10      mov ecx, DWORD PTR [16+esp]            
  00033 f7 de            neg esi                                
  00035 83 c2 02         add edx, 2                             
  00038 8d 1c 01         lea ebx, DWORD PTR [ecx+eax]           
  0003b 83 c0 07         add eax, 7                             
  0003e 03 f3            add esi, ebx                           
  00040 8b 0e            mov ecx, DWORD PTR [esi]               
  00042 8b 76 04         mov esi, DWORD PTR [4+esi]             
  00045 89 0b            mov DWORD PTR [ebx], ecx               
  00047 89 73 04         mov DWORD PTR [4+ebx], esi             
  0004a eb 15            jmp .B7.6 
.B7.5:                          
  0004c f3 0f 6f 44 3a 
        01               movdqu xmm0, XMMWORD PTR [1+edx+edi]   
  00052 8b 4c 24 10      mov ecx, DWORD PTR [16+esp]            
  00056 8d 54 1a 01      lea edx, DWORD PTR [1+edx+ebx]         
  0005a f3 0f 7f 04 08   movdqu XMMWORD PTR [eax+ecx], xmm0     
  0005f 03 c3            add eax, ebx                           
.B7.6:                          
  00061 3b 54 24 18      cmp edx, DWORD PTR [24+esp]            
  00065 72 b7            jb .B7.3 
*/

// Decompression main loop:
/*
; mark_description "Intel(R) C++ Compiler XE for applications running on IA-32, Version 12.1.1.258 Build 20111011";
; mark_description "-O3 -D_N_GP -FAcs";

.B6.3:                          
  00026 8b 4c 24 14      mov ecx, DWORD PTR [20+esp]            
  0002a 0f b7 1c 2a      movzx ebx, WORD PTR [edx+ebp]          
  0002e f6 c3 07         test bl, 7                             
  00031 8d 0c 01         lea ecx, DWORD PTR [ecx+eax]           
  00034 75 45            jne .B6.5 
.B6.4:                          
  00036 8b 74 2a 01      mov esi, DWORD PTR [1+edx+ebp]         
  0003a 8b 7c 2a 05      mov edi, DWORD PTR [5+edx+ebp]         
  0003e 0f b6 db         movzx ebx, bl                          
  00041 89 31            mov DWORD PTR [ecx], esi               
  00043 89 79 04         mov DWORD PTR [4+ecx], edi             
  00046 8b 74 2a 09      mov esi, DWORD PTR [9+edx+ebp]         
  0004a 8b 7c 2a 0d      mov edi, DWORD PTR [13+edx+ebp]        
  0004e c1 eb 03         shr ebx, 3                             
  00051 89 71 08         mov DWORD PTR [8+ecx], esi             
  00054 03 c3            add eax, ebx                           
  00056 89 79 0c         mov DWORD PTR [12+ecx], edi            
  00059 8b 74 2a 11      mov esi, DWORD PTR [17+edx+ebp]        
  0005d 8b 7c 2a 15      mov edi, DWORD PTR [21+edx+ebp]        
  00061 89 71 10         mov DWORD PTR [16+ecx], esi            
  00064 89 79 14         mov DWORD PTR [20+ecx], edi            
  00067 8b 74 2a 19      mov esi, DWORD PTR [25+edx+ebp]        
  0006b 8b 7c 2a 1d      mov edi, DWORD PTR [29+edx+ebp]        
  0006f 8d 54 1a 01      lea edx, DWORD PTR [1+edx+ebx]         
  00073 89 71 18         mov DWORD PTR [24+ecx], esi            
  00076 89 79 1c         mov DWORD PTR [28+ecx], edi            
  00079 eb 14            jmp .B6.6 
.B6.5:                          
  0007b f7 db            neg ebx                                
  0007d 83 c2 02         add edx, 2                             
  00080 03 d9            add ebx, ecx                           
  00082 83 c0 08         add eax, 8                             
  00085 8b 33            mov esi, DWORD PTR [ebx]               
  00087 8b 5b 04         mov ebx, DWORD PTR [4+ebx]             
  0008a 89 31            mov DWORD PTR [ecx], esi               
  0008c 89 59 04         mov DWORD PTR [4+ecx], ebx             
.B6.6:                          
  0008f 3b 54 24 1c      cmp edx, DWORD PTR [28+esp]            
  00093 72 91            jb .B6.3 

; mark_description "Intel(R) C++ Compiler XE for applications running on IA-32, Version 12.1.1.258 Build 20111011";
; mark_description "-O3 -QxSSE2 -D_N_XMM -FAcs";

.B7.3:                          
  0001e 8b 4c 24 10      mov ecx, DWORD PTR [16+esp]            
  00022 0f b7 34 3a      movzx esi, WORD PTR [edx+edi]          
  00026 f7 c6 07 00 00 
        00               test esi, 7                            
  0002c 8d 1c 01         lea ebx, DWORD PTR [ecx+eax]           
  0002f 74 16            je .B7.5 
.B7.4:                          
  00031 f7 de            neg esi                                
  00033 83 c2 02         add edx, 2                             
  00036 03 f3            add esi, ebx                           
  00038 83 c0 08         add eax, 8                             
  0003b 8b 0e            mov ecx, DWORD PTR [esi]               
  0003d 8b 76 04         mov esi, DWORD PTR [4+esi]             
  00040 89 0b            mov DWORD PTR [ebx], ecx               
  00042 89 73 04         mov DWORD PTR [4+ebx], esi             
  00045 eb 24            jmp .B7.6 
.B7.5:                          
  00047 81 e6 ff 00 00 
        00               and esi, 255                           
  0004d c1 ee 03         shr esi, 3                             
  00050 f3 0f 6f 44 3a 
        01               movdqu xmm0, XMMWORD PTR [1+edx+edi]   
  00056 f3 0f 6f 4c 3a 
        11               movdqu xmm1, XMMWORD PTR [17+edx+edi]  
  0005c f3 0f 7f 03      movdqu XMMWORD PTR [ebx], xmm0         
  00060 f3 0f 7f 4b 10   movdqu XMMWORD PTR [16+ebx], xmm1      
  00065 03 c6            add eax, esi                           
  00067 8d 54 32 01      lea edx, DWORD PTR [1+edx+esi]         
.B7.6:                          
  0006b 3b 54 24 18      cmp edx, DWORD PTR [24+esp]            
  0006f 72 ad            jb .B7.3 
*/


// Railgun_Swampshine_BailOut, copyleft 2014-Apr-27, Kaze.
// 2014-Apr-27: The nasty SIGNED/UNSIGNED bug in 'Swampshines' which I illustrated several months ago in my fuzzy search article now is fixed here too:
/*
The bug is this (the variables 'i' and 'PRIMALposition' are uint32_t):
Next line assumes -19 >= 0 is true:
if ( (i-(PRIMALposition-1)) >= 0) printf ("THE NASTY BUG AGAIN: %d >= 0\n", i-(PRIMALposition-1));
Next line assumes -19 >= 0 is false:
if ( (signed int)(i-(PRIMALposition-1)) >= 0) printf ("THE NASTY BUG AGAIN: %d >= 0\n", i-(PRIMALposition-1));
And the actual fix:
...
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4
// FIX from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
	// The line below is BUGGY:
	//if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
	// The line below is OKAY:
	if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {
...
*/
// Railgun_Swampshine_BailOut, copyleft 2014-Jan-31, Kaze.
// Caution: For better speed the case 'if (cbPattern==1)' was removed, so Pattern must be longer than 1 char.
#define NeedleThreshold2vs4swampLITE 9+10 // Should be bigger than 9. BMH2 works up to this value (inclusive), if bigger then BMH4 takes over.
char * Railgun_Swampshine_BailOut (char * pbTarget, char * pbPattern, uint32_t cbTarget, uint32_t cbPattern)
{
	char * pbTargetMax = pbTarget + cbTarget;
	register uint32_t ulHashPattern;
	signed long count;

	unsigned char bm_Horspool_Order2[256*256]; // Bitwise soon...
	uint32_t i, Gulliver;

	uint32_t PRIMALposition, PRIMALpositionCANDIDATE;
	uint32_t PRIMALlength, PRIMALlengthCANDIDATE;
	uint32_t j, FoundAtPosition;

	if (cbPattern > cbTarget) return(NULL);

	if ( cbPattern<4 ) { 
		// SSE2 i.e. 128bit Assembly rules here:
		// ...
        	pbTarget = pbTarget+cbPattern;
		ulHashPattern = ( (*(char *)(pbPattern))<<8 ) + *(pbPattern+(cbPattern-1));
		if ( cbPattern==3 ) {
			for ( ;; ) {
				if ( ulHashPattern == ( (*(char *)(pbTarget-3))<<8 ) + *(pbTarget-1) ) {
					if ( *(char *)(pbPattern+1) == *(char *)(pbTarget-2) ) return((pbTarget-3));
				}
				if ( (char)(ulHashPattern>>8) != *(pbTarget-2) ) { 
					pbTarget++;
					if ( (char)(ulHashPattern>>8) != *(pbTarget-2) ) pbTarget++;
				}
				pbTarget++;
				if (pbTarget > pbTargetMax) return(NULL);
			}
		} else {
		}
		for ( ;; ) {
			if ( ulHashPattern == ( (*(char *)(pbTarget-2))<<8 ) + *(pbTarget-1) ) return((pbTarget-2));
			if ( (char)(ulHashPattern>>8) != *(pbTarget-1) ) pbTarget++;
			pbTarget++;
			if (pbTarget > pbTargetMax) return(NULL);
		}
	} else { //if ( cbPattern<4 )
		if ( cbPattern<=NeedleThreshold2vs4swampLITE ) { 
			// BMH order 2, needle should be >=4:
			ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
			for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
			for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=1;
			i=0;
			while (i <= cbTarget-cbPattern) {
				Gulliver = 1; // 'Gulliver' is the skip
				if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]] != 0 ) {
					if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1-2]] == 0 ) Gulliver = cbPattern-(2-1)-2; else {
						if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
							count = cbPattern-4+1; 
							while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
								count = count-4;
							if ( count <= 0 ) return(pbTarget+i);
						}
					}
				} else Gulliver = cbPattern-(2-1);
				i = i + Gulliver;
				//GlobalI++; // Comment it, it is only for stats.
			}
			return(NULL);
		} else { // if ( cbPattern<=NeedleThreshold2vs4swampLITE )

// Swampwalker_BAILOUT heuristic order 4 (Needle should be bigger than 4) [
// Needle: 1234567890qwertyuiopasdfghjklzxcv            PRIMALposition=01 PRIMALlength=33  '1234567890qwertyuiopasdfghjklzxcv'
// Needle: vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv            PRIMALposition=29 PRIMALlength=04  'vvvv'
// Needle: vvvvvvvvvvBOOMSHAKALAKAvvvvvvvvvv            PRIMALposition=08 PRIMALlength=20  'vvvBOOMSHAKALAKAvvvv'
// Needle: Trollland                                    PRIMALposition=01 PRIMALlength=09  'Trollland'
// Needle: Swampwalker                                  PRIMALposition=01 PRIMALlength=11  'Swampwalker'
// Needle: licenselessness                              PRIMALposition=01 PRIMALlength=15  'licenselessness'
// Needle: alfalfa                                      PRIMALposition=02 PRIMALlength=06  'lfalfa'
// Needle: Sandokan                                     PRIMALposition=01 PRIMALlength=08  'Sandokan'
// Needle: shazamish                                    PRIMALposition=01 PRIMALlength=09  'shazamish'
// Needle: Simplicius Simplicissimus                    PRIMALposition=06 PRIMALlength=20  'icius Simplicissimus'
// Needle: domilliaquadringenquattuorquinquagintillion  PRIMALposition=01 PRIMALlength=32  'domilliaquadringenquattuorquinqu'
// Needle: boom-boom                                    PRIMALposition=02 PRIMALlength=08  'oom-boom'
// Needle: vvvvv                                        PRIMALposition=01 PRIMALlength=04  'vvvv'
// Needle: 12345                                        PRIMALposition=01 PRIMALlength=05  '12345'
// Needle: likey-likey                                  PRIMALposition=03 PRIMALlength=09  'key-likey'
// Needle: BOOOOOM                                      PRIMALposition=03 PRIMALlength=05  'OOOOM'
// Needle: aaaaaBOOOOOM                                 PRIMALposition=02 PRIMALlength=09  'aaaaBOOOO'
// Needle: BOOOOOMaaaaa                                 PRIMALposition=03 PRIMALlength=09  'OOOOMaaaa'
PRIMALlength=0;
for (i=0+(1); i < cbPattern-((4)-1)+(1)-(1); i++) { // -(1) because the last BB order 4 has no counterpart(s)
	FoundAtPosition = cbPattern - ((4)-1) + 1;
	PRIMALpositionCANDIDATE=i;
	while ( PRIMALpositionCANDIDATE <= (FoundAtPosition-1) ) {
		j = PRIMALpositionCANDIDATE + 1;
		while ( j <= (FoundAtPosition-1) ) {
			if ( *(uint32_t *)(pbPattern+PRIMALpositionCANDIDATE-(1)) == *(uint32_t *)(pbPattern+j-(1)) ) FoundAtPosition = j;
			j++;
		}
		PRIMALpositionCANDIDATE++;
	}
	PRIMALlengthCANDIDATE = (FoundAtPosition-1)-i+1 +((4)-1);
	if (PRIMALlengthCANDIDATE >= PRIMALlength) {PRIMALposition=i; PRIMALlength = PRIMALlengthCANDIDATE;}
	if (cbPattern-i+1 <= PRIMALlength) break;
	if (PRIMALlength > 128) break; // Bail Out for 129[+]
}
// Swampwalker_BAILOUT heuristic order 4 (Needle should be bigger than 4) ]

// Here we have 4 or bigger NewNeedle, apply order 2 for pbPattern[i+(PRIMALposition-1)] with length 'PRIMALlength' and compare the pbPattern[i] with length 'cbPattern':
PRIMALlengthCANDIDATE = cbPattern;
cbPattern = PRIMALlength;
pbPattern = pbPattern + (PRIMALposition-1);

// Revision 2 commented section [
/*
if (cbPattern-1 <= 255) {
// BMH Order 2 [
			ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
			for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]= cbPattern-1;} // cbPattern-(Order-1) for Horspool; 'memset' if not optimized
			for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=i; // Rightmost appearance/position is needed
			i=0;
			while (i <= cbTarget-cbPattern) { 
				Gulliver = bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]];
				if ( Gulliver != cbPattern-1 ) { // CASE #2: if equal means the pair (char order 2) is not found i.e. Gulliver remains intact, skip the whole pattern and fall back (Order-1) chars i.e. one char for Order 2
				if ( Gulliver == cbPattern-2 ) { // CASE #1: means the pair (char order 2) is found
					if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) {
						count = cbPattern-4+1; 
						while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
							count = count-4;
// If we miss to hit then no need to compare the original: Needle
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4

// FIX from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
	// The line below is BUGGY:
	//if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
	// The line below is OKAY:
	if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {

		if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
			count = PRIMALlengthCANDIDATE-4+1; 
			while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
				count = count-4;
			if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));	
		}
	}
}
					}
					Gulliver = 1;
				} else
					Gulliver = cbPattern - Gulliver - 2; // CASE #3: the pair is found and not as suffix i.e. rightmost position
				}
				i = i + Gulliver;
				//GlobalI++; // Comment it, it is only for stats.
			}
			return(NULL);
// BMH Order 2 ]
} else {
			// BMH order 2, needle should be >=4:
			ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
			for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
			for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=1;
			i=0;
			while (i <= cbTarget-cbPattern) {
				Gulliver = 1; // 'Gulliver' is the skip
				if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]] != 0 ) {
					if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1-2]] == 0 ) Gulliver = cbPattern-(2-1)-2; else {
						if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
							count = cbPattern-4+1; 
							while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
								count = count-4;
// If we miss to hit then no need to compare the original: Needle
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4

// FIX from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
	// The line below is BUGGY:
	//if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
	// The line below is OKAY:
	if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {

		if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
			count = PRIMALlengthCANDIDATE-4+1; 
			while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
				count = count-4;
			if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));	
		}
	}
}
						}
					}
				} else Gulliver = cbPattern-(2-1);
				i = i + Gulliver;
				//GlobalI++; // Comment it, it is only for stats.
			}
			return(NULL);
}
*/
// Revision 2 commented section ]

		if ( cbPattern<=NeedleThreshold2vs4swampLITE ) { 

			// BMH order 2, needle should be >=4:
			ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
			for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
			for (i=0; i < cbPattern-1; i++) bm_Horspool_Order2[*(unsigned short *)(pbPattern+i)]=1;
			i=0;
			while (i <= cbTarget-cbPattern) {
				Gulliver = 1; // 'Gulliver' is the skip
				if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1]] != 0 ) {
					if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+cbPattern-1-1-2]] == 0 ) Gulliver = cbPattern-(2-1)-2; else {
						if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
							count = cbPattern-4+1; 
							while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
								count = count-4;
// If we miss to hit then no need to compare the original: Needle
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4

// FIX from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
	// The line below is BUGGY:
	//if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
	// The line below is OKAY:
	if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {

		if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
			count = PRIMALlengthCANDIDATE-4+1; 
			while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
				count = count-4;
			if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));	
		}
	}
}
						}
					}
				} else Gulliver = cbPattern-(2-1);
				i = i + Gulliver;
				//GlobalI++; // Comment it, it is only for stats.
			}
			return(NULL);

		} else { // if ( cbPattern<=NeedleThreshold2vs4swampLITE )

			// BMH pseudo-order 4, needle should be >=8+2:
			ulHashPattern = *(uint32_t *)(pbPattern); // First four bytes
			for (i=0; i < 256*256; i++) {bm_Horspool_Order2[i]=0;}
			// In line below we "hash" 4bytes to 2bytes i.e. 16bit table, how to compute TOTAL number of BBs, 'cbPattern - Order + 1' is the number of BBs for text 'cbPattern' bytes long, for example, for cbPattern=11 'fastest fox' and Order=4 we have BBs = 11-4+1=8:
			//"fast"
			//"aste"
			//"stes"
			//"test"
			//"est "
			//"st f"
			//"t fo"
			//" fox"
			//for (i=0; i < cbPattern-4+1; i++) bm_Horspool_Order2[( *(unsigned short *)(pbPattern+i+0) + *(unsigned short *)(pbPattern+i+2) ) & ( (1<<16)-1 )]=1;
			//for (i=0; i < cbPattern-4+1; i++) bm_Horspool_Order2[( (*(uint32_t *)(pbPattern+i+0)>>16)+(*(uint32_t *)(pbPattern+i+0)&0xFFFF) ) & ( (1<<16)-1 )]=1;
			// Above line is replaced by next one with better hashing:
			for (i=0; i < cbPattern-4+1; i++) bm_Horspool_Order2[( (*(uint32_t *)(pbPattern+i+0)>>(16-1))+(*(uint32_t *)(pbPattern+i+0)&0xFFFF) ) & ( (1<<16)-1 )]=1;
			i=0;
			while (i <= cbTarget-cbPattern) {
				Gulliver = 1;
				//if ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2]&0xFFFF) ) & ( (1<<16)-1 )] != 0 ) { // DWORD #1
				// Above line is replaced by next one with better hashing:
				if ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2]&0xFFFF) ) & ( (1<<16)-1 )] != 0 ) { // DWORD #1
					//if ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) ) & ( (1<<16)-1 )] == 0 ) Gulliver = cbPattern-(2-1)-2-4; else {
					// Above line is replaced in order to strengthen the skip by checking the middle DWORD,if the two DWORDs are 'ab' and 'cd' i.e. [2x][2a][2b][2c][2d] then the middle DWORD is 'bc'.
					// The respective offsets (backwards) are: -10/-8/-6/-4 for 'xa'/'ab'/'bc'/'cd'.
					//if ( ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) ) & ( (1<<16)-1 )] ) < 3 ) Gulliver = cbPattern-(2-1)-2-4-2; else {
					// Above line is replaced by next one with better hashing:
					// When using (16-1) right shifting instead of 16 we will have two different pairs (if they are equal), the highest bit being lost do the job especialy for ASCII texts with no symbols in range 128-255.
					// Example for genomesque pair TT+TT being shifted by (16-1):
					// T            = 01010100
					// TT           = 01010100 01010100
					// TTTT         = 01010100 01010100 01010100 01010100
					// TTTT>>16     = 00000000 00000000 01010100 01010100
					// TTTT>>(16-1) = 00000000 00000000 10101000 10101000 <--- Due to the left shift by 1, the 8th bits of 1st and 2nd bytes are populated - usually they are 0 for English texts & 'ACGT' data.
					//if ( ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) ) & ( (1<<16)-1 )] ) < 3 ) Gulliver = cbPattern-(2-1)-2-4-2; else {
					// 'Maximus' uses branched 'if', again.
					if ( \
					( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6 +1]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6 +1]&0xFFFF) ) & ( (1<<16)-1 )] ) == 0 \
					|| ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4 +1]>>(16-1))+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4 +1]&0xFFFF) ) & ( (1<<16)-1 )] ) == 0 \
					) Gulliver = cbPattern-(2-1)-2-4-2 +1; else {
					// Above line is not optimized (several a SHR are used), we have 5 non-overlapping WORDs, or 3 overlapping WORDs, within 4 overlapping DWORDs so:
// [2x][2a][2b][2c][2d]
// DWORD #4
// [2a] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]>>16) =     !SHR to be avoided! <--
// [2x] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]&0xFFFF) =                        |
//     DWORD #3                                                                       |
// [2b] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]>>16) =     !SHR to be avoided!   |<--
// [2a] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) = ------------------------  |
//         DWORD #2                                                                      |
// [2c] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]>>16) =     !SHR to be avoided!      |<--
// [2b] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) = ---------------------------  |
//             DWORD #1                                                                     |
// [2d] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]>>16) =                                 |
// [2c] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]&0xFFFF) = ------------------------------
//
// So in order to remove 3 SHR instructions the equal extractions are:
// DWORD #4
// [2a] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) =  !SHR to be avoided! <--
// [2x] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]&0xFFFF) =                        |
//     DWORD #3                                                                       |
// [2b] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) =  !SHR to be avoided!   |<--
// [2a] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) = ------------------------  |
//         DWORD #2                                                                      |
// [2c] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]&0xFFFF) =  !SHR to be avoided!      |<--
// [2b] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) = ---------------------------  |
//             DWORD #1                                                                     |
// [2d] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]>>16) =                                 |
// [2c] (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]&0xFFFF) = ------------------------------
					//if ( ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-6]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-0]&0xFFFF)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-2]&0xFFFF) ) & ( (1<<16)-1 )] ) < 3 ) Gulliver = cbPattern-(2-1)-2-6; else {
// Since the above Decumanus mumbo-jumbo (3 overlapping lookups vs 2 non-overlapping lookups) is not fast enough we go DuoDecumanus or 3x4:
// [2y][2x][2a][2b][2c][2d]
// DWORD #3
//         DWORD #2
//                 DWORD #1
					//if ( ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-4]&0xFFFF) ) & ( (1<<16)-1 )] ) + ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-8]>>16)+(*(uint32_t *)&pbTarget[i+cbPattern-1-1-2-8]&0xFFFF) ) & ( (1<<16)-1 )] ) < 2 ) Gulliver = cbPattern-(2-1)-2-8; else {
						if ( *(uint32_t *)&pbTarget[i] == ulHashPattern) {
							// Order 4 [
						// Let's try something "outrageous" like comparing with[out] overlap BBs 4bytes long instead of 1 byte back-to-back:
						// Inhere we are using order 4, 'cbPattern - Order + 1' is the number of BBs for text 'cbPattern' bytes long, for example, for cbPattern=11 'fastest fox' and Order=4 we have BBs = 11-4+1=8:
						//0:"fast" if the comparison failed here, 'count' is 1; 'Gulliver' is cbPattern-(4-1)-7
						//1:"aste" if the comparison failed here, 'count' is 2; 'Gulliver' is cbPattern-(4-1)-6
						//2:"stes" if the comparison failed here, 'count' is 3; 'Gulliver' is cbPattern-(4-1)-5
						//3:"test" if the comparison failed here, 'count' is 4; 'Gulliver' is cbPattern-(4-1)-4
						//4:"est " if the comparison failed here, 'count' is 5; 'Gulliver' is cbPattern-(4-1)-3
						//5:"st f" if the comparison failed here, 'count' is 6; 'Gulliver' is cbPattern-(4-1)-2
						//6:"t fo" if the comparison failed here, 'count' is 7; 'Gulliver' is cbPattern-(4-1)-1
						//7:" fox" if the comparison failed here, 'count' is 8; 'Gulliver' is cbPattern-(4-1)
							count = cbPattern-4+1; 
							// Below comparison is UNIdirectional:
							while ( count > 0 && *(uint32_t *)(pbPattern+count-1) == *(uint32_t *)(&pbTarget[i]+(count-1)) )
								count = count-4;
// count = cbPattern-4+1 = 23-4+1 = 20
// boomshakalakaZZZZZZ[ZZZZ] 20
// boomshakalakaZZ[ZZZZ]ZZZZ 20-4
// boomshakala[kaZZ]ZZZZZZZZ 20-8 = 12
// boomsha[kala]kaZZZZZZZZZZ 20-12 = 8
// boo[msha]kalakaZZZZZZZZZZ 20-16 = 4

// If we miss to hit then no need to compare the original: Needle
if ( count <= 0 ) {
// I have to add out-of-range checks...
// i-(PRIMALposition-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4

// FIX from 2014-Apr-27:
// Because (count-1) is negative, above fours are reduced to next twos:
// i-(PRIMALposition-1)+(count-1) >= 0
// &pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4
	// The line below is BUGGY:
	//if ( (i-(PRIMALposition-1) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) && (&pbTarget[i-(PRIMALposition-1)+(count-1)] <= pbTargetMax - 4) ) {
	// The line below is OKAY:
	if ( ((signed int)(i-(PRIMALposition-1)+(count-1)) >= 0) && (&pbTarget[i-(PRIMALposition-1)] <= pbTargetMax - 4) ) {

		if ( *(uint32_t *)&pbTarget[i-(PRIMALposition-1)] == *(uint32_t *)(pbPattern-(PRIMALposition-1))) { // This fast check ensures not missing a match (for remainder) when going under 0 in loop below:
			count = PRIMALlengthCANDIDATE-4+1; 
			while ( count > 0 && *(uint32_t *)(pbPattern-(PRIMALposition-1)+count-1) == *(uint32_t *)(&pbTarget[i-(PRIMALposition-1)]+(count-1)) )
				count = count-4;
			if ( count <= 0 ) return(pbTarget+i-(PRIMALposition-1));	
		}
	}
}

							// In order to avoid only-left or only-right WCS the memcmp should be done as left-to-right and right-to-left AT THE SAME TIME.
							// Below comparison is BIdirectional. It pays off when needle is 8+++ long:
//							for (count = cbPattern-4+1; count > 0; count = count-4) {
//								if ( *(uint32_t *)(pbPattern+count-1) != *(uint32_t *)(&pbTarget[i]+(count-1)) ) {break;};
//								if ( *(uint32_t *)(pbPattern+(cbPattern-4+1)-count) != *(uint32_t *)(&pbTarget[i]+(cbPattern-4+1)-count) ) {count = (cbPattern-4+1)-count +(1); break;} // +(1) because two lookups are implemented as one, also no danger of 'count' being 0 because of the fast check outwith the 'while': if ( *(uint32_t *)&pbTarget[i] == ulHashPattern)
//							}
//							if ( count <= 0 ) return(pbTarget+i);
								// Checking the order 2 pairs in mismatched DWORD, all the 3:
								//if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1]] == 0 ) Gulliver = count; // 1 or bigger, as it should
								//if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1+1]] == 0 ) Gulliver = count+1; // 1 or bigger, as it should
								//if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1+1+1]] == 0 ) Gulliver = count+1+1; // 1 or bigger, as it should
							//	if ( bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1]] + bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1+1]] + bm_Horspool_Order2[*(unsigned short *)&pbTarget[i+count-1+1+1]] < 3 ) Gulliver = count; // 1 or bigger, as it should, THE MIN(count,count+1,count+1+1)
								// Above compound 'if' guarantees not that Gulliver > 1, an example:
								// Needle:    fastest tax
								// Window: ...fastast tax...
								// After matching ' tax' vs ' tax' and 'fast' vs 'fast' the mismathced DWORD is 'test' vs 'tast':
								// 'tast' when factorized down to order 2 yields: 'ta','as','st' - all the three when summed give 1+1+1=3 i.e. Gulliver remains 1.
								// Roughly speaking, this attempt maybe has its place in worst-case scenarios but not in English text and even not in ACGT data, that's why I commented it in original 'Shockeroo'.
								//if ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+count-1]>>16)+(*(uint32_t *)&pbTarget[i+count-1]&0xFFFF) ) & ( (1<<16)-1 )] == 0 ) Gulliver = count; // 1 or bigger, as it should
								// Above line is replaced by next one with better hashing:
//								if ( bm_Horspool_Order2[( (*(uint32_t *)&pbTarget[i+count-1]>>(16-1))+(*(uint32_t *)&pbTarget[i+count-1]&0xFFFF) ) & ( (1<<16)-1 )] == 0 ) Gulliver = count; // 1 or bigger, as it should
							// Order 4 ]
						}
					}
				} else Gulliver = cbPattern-(2-1)-2; // -2 because we check the 4 rightmost bytes not 2.
				i = i + Gulliver;
				//GlobalI++; // Comment it, it is only for stats.
			}
			return(NULL);

		} // if ( cbPattern<=NeedleThreshold2vs4swampLITE )
		} // if ( cbPattern<=NeedleThreshold2vs4swampLITE )
	} //if ( cbPattern<4 )
}

// Fixed version from 2012-Feb-27.
// Caution: For better speed the case 'if (cbPattern==1)' was removed, so Pattern must be longer than 1 char.
char * Railgun_Doublet (char * pbTarget, char * pbPattern, uint32_t cbTarget, uint32_t cbPattern)
{
	char * pbTargetMax = pbTarget + cbTarget;
	register uint32_t ulHashPattern;
	uint32_t ulHashTarget, count, countSTATIC;

	if (cbPattern > cbTarget) return(NULL);

	countSTATIC = cbPattern-2;

	pbTarget = pbTarget+cbPattern;
	ulHashPattern = (*(uint16_t *)(pbPattern));

	for ( ;; ) {
		if ( ulHashPattern == (*(uint16_t *)(pbTarget-cbPattern)) ) {
			count = countSTATIC;
			while ( count && *(char *)(pbPattern+2+(countSTATIC-count)) == *(char *)(pbTarget-cbPattern+2+(countSTATIC-count)) ) {
				count--;
			}
			if ( count == 0 ) return((pbTarget-cbPattern));
		}
		pbTarget++;
		if (pbTarget > pbTargetMax) return(NULL);
	}
}

// Last change: 2014-Aug-29
// If you want to help me to improve it, email me at: sanmayce@sanmayce.com
// Enfun!
