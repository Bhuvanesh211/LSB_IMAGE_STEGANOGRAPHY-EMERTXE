#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* open decode files */

Status open_decode_files(DecodeInfo *decInfo)
{
    // output image file
    decInfo->fptr_stego_image = fopen(decInfo->src_stego_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->src_stego_fname);

        return e_failure;
    }

    // Secret file
    decInfo->fptr_d_secret = fopen(decInfo->d_secret_fname, "w");
    // Do Error handling
    if (decInfo->fptr_d_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->d_secret_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/* read and validate arguements */

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // get .bmp file
    if (argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        // fetch the name of output image file
        decInfo->src_stego_fname = argv[2];
    }
    else
    {
        return e_failure;
    }
    if (argv[3] != NULL)
    {
        decInfo->d_secret_fname = argv[3];
    }
    else
    {
        decInfo->d_secret_fname = "output.txt";
    }
    return e_success;
}

/* decode byte from lsb */

Status decode_byte_from_lsb(char *data, char *stego_buffer)
{
    int bit = 7;
    unsigned char ch = 0x00;
    for (int i = 0; i < 8; i++)
    {
        ch = ((stego_buffer[i] & 0x01) << bit--) | ch;
    }
    *data = ch;
    return e_success;
}

/* decode data from stego image */

Status decode_data_from_image(int size, FILE *fptr_stego, DecodeInfo *decInfo)
{
    char str[8];
    for (int i = 0; i < size; i++)
    {
        // read 8 bytes of rgb from stego.bmp
        fread(str, 8, sizeof(char), decInfo->fptr_stego_image);
        decode_byte_from_lsb(&decInfo->magic_data[i], str);
    }
    return e_success;
}

/* decode magic string */

Status decode_magic_string(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    int i = strlen(MAGIC_STRING);

    decInfo->magic_data = malloc(strlen(MAGIC_STRING) + 1);
    decode_data_from_image(strlen(MAGIC_STRING), decInfo->fptr_stego_image, decInfo);

    decInfo->magic_data[i] = '\0';

    if (strcmp(decInfo->magic_data, MAGIC_STRING) == 0)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/* decode size from lsb */

Status decode_size_from_lsb(char *buffer, int *size)
{
    int j = 31;
    int num = 0x00;
    for (int i = 0; i < 32; i++)
    {
        num = ((buffer[i] & 0x01) << j--) | num;
    }

    *size = num;
}

/* decode extension size */

Status decode_size(int size, FILE *fptr_stego_image)
{
    char str[32];
    int length;

    fread(str, 32, sizeof(char), fptr_stego_image);
    decode_size_from_lsb(str, &length);

    if (length == size)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/* decode extension data from image */

Status decode_extension_data_from_image(int size, FILE *fptr_stego_image, DecodeInfo *decInfo)
{
    for (int i = 0; i < size; i++)
    {
        fread(decInfo->src_stego_fname, 8, 1, fptr_stego_image);
        decode_byte_from_lsb(&decInfo->d_extn_secret_file[i], decInfo->src_stego_fname);
    }
    return e_success;
}

/* decode secret file extension */

Status decode_secret_file_extn(char *file_ext, DecodeInfo *decInfo)
{
    file_ext = ".txt";
    int i = strlen(file_ext);
    decInfo->d_extn_secret_file = malloc(i + 1);
    decode_extension_data_from_image(strlen(file_ext), decInfo->fptr_stego_image, decInfo);
    decInfo->d_extn_secret_file[i] = '\0';

    if (strcmp(decInfo->d_extn_secret_file, file_ext) == 0)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/* decode secret file size */

Status decode_secret_file_size(int file_size, DecodeInfo *decInfo)
{
    char str[32];
    fread(str, 32, sizeof(char), decInfo->fptr_stego_image);
    decode_size_from_lsb(str, &file_size);
    decInfo->sec_file_size = file_size;
    printf("secret_file_size : %d bytes\n", decInfo->sec_file_size);
    return e_success;
}

/* decode secret file data */

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char ch;
    for (int i = 0; i < decInfo->sec_file_size; i++)
    {
        fread(decInfo->src_stego_fname, 8, sizeof(char), decInfo->fptr_stego_image);
        decode_byte_from_lsb(&ch, decInfo->src_stego_fname);
        fputc(ch, decInfo->fptr_d_secret);
    }
    return e_success;
}

/* do decoding */

Status do_decoding(DecodeInfo *decInfo)
{
    if (open_decode_files(decInfo) == e_success)
    {
        printf("Opened all files successfully\n");
        printf("Started decoding....\n");
        if (decode_magic_string(decInfo) == e_success)
        {
            printf("Magic string decoded successfully\n");
            if (decode_size(strlen(".txt"), decInfo->fptr_stego_image) == e_success)
            {
                printf("Decoding secret file extension size successfull(length of string is 4)\n");
                if (decode_secret_file_extn(decInfo->d_extn_secret_file, decInfo) == e_success)
                {
                    printf("Successfully decoded secret file extension\n");
                    if (decode_secret_file_size(decInfo->sec_file_size, decInfo) == e_success)
                    {
                        
                        printf("Decoded secret file size successfully\n");
                        if (decode_secret_file_data(decInfo) == e_success)
                        {
                            printf("Decoded secret data successfully\n");
                        }
                        else
                        {
                            printf("Failed to decode the secret data\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Failed to decode secret file size\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Failed to decode secret file extension\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Failed to decode secret file extension size\n");
                return e_failure;
            }
        }
        else
        {
            printf("Error while decoding magic string\n");
            return e_failure;
        }
    }
    else
    {
        printf("Failure: opening the files\n");
        return e_failure;
    }
    return e_success;
}

