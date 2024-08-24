#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_STEGO_BUF_SIZE 8
#define MAX_FIL_BUF_SIZE (MAX_STEGO_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

//./a.out stego.bmp output.txt
typedef struct _DecodeInfo
{
    // stego file info
    char *src_stego_fname;
    FILE *fptr_stego_image;
    char stego_data[MAX_STEGO_BUF_SIZE];
    char *magic_data;

    // secret file info
    char *d_extn_secret_file;
    int sec_file_size;
    FILE *fptr_d_dest_image;

    char *d_secret_fname;
    FILE *fptr_d_secret;

} DecodeInfo;

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get the file pointers for stego.bmp and secret.txt file */
Status open_decode_files(DecodeInfo *decInfo);

/* Fetch magic string */
Status decode_magic_string(DecodeInfo *decInfo);

/* Encode function, which does real decoding */
Status decode_data_from_image(int size, FILE *fptr_stego_image, DecodeInfo *decInfo);

/* Decode byte from lsb of stego image data array */
Status decode_byte_from_lsb(char *data, char *stego_buffer);

/* Decode secret file extension size */
Status decode_size(int size, FILE *fptr_stego_image);

/* Decode size from lsb */
Status decode_size_from_lsb(char *buffer, int *size);

/* Decode secret file extension */
Status decode_secret_file_extn(char *file_ext, DecodeInfo *decInfo);

/* Decode extension data from image */
Status decode_extension_data_from_image(int size, FILE *fptr_stego_image, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(int file_size, DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif