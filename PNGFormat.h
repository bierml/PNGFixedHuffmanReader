#include<malloc.h>
#include<stdio.h>
#define SWAP_UINT16(x) (((x) >> 8) | ((x)<< 8))
#define SWAP_UINT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))
int tabl1[4][3] = {{24,95,8},{100,127,9},{0,23,7},{96,99,8}};
int tabl2[30][3] = {{0,0,1},{1,0,2},{2,0,3},{3,0,4},{4,1,5},{5,1,7},{6,2,9},{7,2,13},{8,3,17},{9,3,25},{10,4,33},{11,4,49},{12,5,65},{13,5,97},{14,6,129},{15,6,193},{16,7,257},{17,7,385},{18,8,513},{19,8,769},{20,9,1025},{21,9,1537},{22,10,2049},{23,10,3073},{24,11,4097},{25,11,6145},{26,12,8193},{27,12,12289},{28,13,16385},{29,13,24577}};
int tabl3[29][2] = {{0,3},{0,4},{0,5},{0,6},{0,7},{0,8},{0,9},{0,10},{1,11},{1,13},{1,15},{1,17},{2,19},{2,23},{2,27},{2,31},{3,35},{3,43},{3,51},{3,59},{4,67},{4,83},{4,99},{4,115},{5,131},{5,163},{5,195},{5,227},{0,258}};
int MASK(int nbits)
{
    return 255 >> (8-nbits);
}
int SWAPPER(int byte,int n)
{
    int res = 0;
    int mask =1;
    for(int i=0;i<n+1;i++)
    {
        res += ((byte & mask) ? 1:0)*pow(2,n-1-i);
        mask *= 2;
    }
    return res;
}
int convertvalue(int rnum,int l)
{
    int prefix;
    prefix=rnum>>l;
    if(prefix>=tabl1[0][0] && prefix<=tabl1[0][1])
        return rnum-48+0;
    if(prefix>=tabl1[1][0] && prefix<=tabl1[1][1])
        return rnum-400+144;
    if(prefix>=tabl1[2][0] && prefix<=tabl1[2][1])
        return rnum-0+256;
    if(prefix>=tabl1[3][0] && prefix<=tabl1[3][1])
        return rnum-192+280;
    return 0;
}
int addbits(int rnum)
{
    if(rnum>=tabl1[0][0] && rnum<=tabl1[0][1])
        return tabl1[0][2]-7;
    if(rnum>=tabl1[1][0] && rnum<=tabl1[1][1])
        return tabl1[1][2]-7;
    if(rnum>=tabl1[2][0] && rnum<=tabl1[2][1])
        return tabl1[2][2]-7;
    if(rnum>=tabl1[3][0] && rnum<=tabl1[3][1])
        return tabl1[3][2]-7;
    return 0;
}
#pragma pack(push, 1)
typedef struct IHDR
{
    unsigned long int length;
    unsigned long int chunk_type;
	unsigned long int width;
	unsigned long int height;
	unsigned char bitdepth;
	unsigned char colortype;
	unsigned char compmethod;
	unsigned char filtmethod;
	unsigned char interlace;
	unsigned long int CRC;
} IHDR_;
typedef struct pixel
{
    unsigned char red_;
    unsigned char green_;
    unsigned char blue_;
    unsigned char alpha_;
    unsigned char reserved_;
} pixel_;
typedef struct zlibheader
{
    unsigned char CMF;
    unsigned char FLG;
    unsigned long int DICTID;
} zheader;
typedef struct defchunk
{
    unsigned long int length;
    unsigned long int chunkname;
    unsigned char * chunkdata;
    unsigned long int CRC;
}   def_ch;
#pragma pack(pop)
def_ch readchunk(FILE* fp)
{
    def_ch result;
    unsigned char * chunkdata = NULL;
    unsigned long int length;
    unsigned long int chunkname;
    unsigned long int CRC;
    fread(&length,4,1,fp);
    length=SWAP_UINT32(length);
    fread(&chunkname,4,1,fp);
    chunkdata = (unsigned char*)malloc(length);
    fread(chunkdata,length,1,fp);
    fread(&CRC,4,1,fp);
    result.length=length;
    result.chunkname=chunkname;
    result.chunkdata=chunkdata;
    result.CRC=CRC;
    return result;
}
void freemem(def_ch str)
{
   free(str.chunkdata);
   return;
}
