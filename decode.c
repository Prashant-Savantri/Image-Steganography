#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"

// validating the arguments passed through command line for decoding
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(argv[2]!=NULL && strstr(argv[2], ".bmp")!=NULL)
    {
        printf(".bmp file present\n");
        decInfo->stego_image_fname=argv[2];
    }

    else
    {
        printf(".bmp file is not present\n");
        return e_failure;
    }

    if(argv[3]!=NULL)
    {
        printf("destination file is present\n");
        char *token=strtok(argv[3],".");
        decInfo->output_file_fname=token;
    }

    else
    {
        decInfo->output_file_fname="output.txt";
    }

    return e_success;
}

// function definition for decoding
Status do_decoding(char *argv[], DecodeInfo *decInfo)
{
    printf("<------ Started Doing Decoding ------->\n");

    // function call for opening the required files for decoding in required mode
    if(open_files_dec(decInfo)==e_success)
    {
        printf("All files opened successfully\n");
    }

    else
    {
        printf("All files did not opened successfully\n");
        return e_failure;
    }

    // skipping the 54 bytes of header
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    char magic[10];
    printf("Enter the magic string:");
    scanf(" %[^\n]", magic);

    int len=strlen(magic);

    // function call for decoding the magic string
    if(decode_magic_string(len, decInfo)==e_success)
    {
       printf("Magic string decoded successfully\n");
       printf("User magic     : %s\n", magic);
       printf("Decoded magic  : %s\n", decInfo->magic_string);
    }

    else
    {
        printf("Magic string not decoded successfully\n");
        return e_failure;
    }

    // comparing the magic string whether the user has entered correct magic string or not
    if(strcmp(magic, decInfo->magic_string)!=0)
    {
        printf("Magic String Mismatch !!! \n");
        return e_failure;
    }

    // function call for decoding the stego file extension size
    if(decode_stego_file_extn_size(decInfo)==e_success)
    {
        printf("Stego file extension size decoded successfully\n");
        printf("Stego file extension size = %d bytes\n", decInfo->extn_size);
    }

    else
    {
        printf("Stego file extension size not decoded successfully\n");
        return e_failure;
    }

    // function call for decoding the secret file extension encoded in stego file
    if(decode_stego_file_extn(decInfo)==e_success)
    {
        printf("Stego file extension decoded successfully\n");
        printf("Stego file extension is %s\n", decInfo->extn);
    }

    else
    {
        printf("Stego file extension size not decoded successfully\n");
        return e_failure;
    }

    if(argv[3]!=NULL)
    {
        strcat(decInfo->output_file_fname, decInfo->extn);
        printf("Output file name is %s\n", decInfo->output_file_fname);
    }

    else
    {
        printf("Output file name is %s\n", decInfo->output_file_fname);
    }

    decInfo->fptr_output_file=fopen(decInfo->output_file_fname, "w");

    // function call for decoding the secret file size encoded in the stego file
    if(decode_stego_file_size(decInfo)==e_success)
    {
        printf("Stego file size encoded successfully\n");
        printf("Secret data size is %d\n", decInfo->secret_data_size);
    }

    else
    {
        printf("Stego file size not encoded successfully\n");
        return e_failure;
    }

    // function call decoding the secret data encoded in the stego file
    if(decode_stego_file_secret_data(decInfo)==e_success)
    {
        printf("Secret data decoded successfully\n");
    }

    else
    {
        printf("Secret file data not decoded successfully\n");
        return e_failure;
    }

    return e_success;
}

// Function definition for opening files for decoding in required modes
Status open_files_dec(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image=fopen(decInfo->stego_image_fname, "r");

    if(decInfo->fptr_stego_image==NULL)
    {
        printf(".bmp file is not present\n");
        return e_failure;
    }

    else
    {
        printf(".bmp file is present\n");
    }

    return e_success;
}

// function definition for decoding data from the image
Status decode_data_from_image(int size, DecodeInfo *decInfo)
{
   char buffer[8];

   for(int i=0;i<size;i++)
   {
     fread(buffer, 8, 1, decInfo->fptr_stego_image);
     decode_byte_from_lsb(&decInfo->magic_string[i], buffer);
   }

   decInfo->magic_string[size]='\0';

   return e_success;
}

Status decode_data_from_image2(int size, DecodeInfo *decInfo)
{
   char buffer[8];

   for(int i=0;i<size;i++)
   {
     fread(buffer, 8, 1, decInfo->fptr_stego_image);
     decode_byte_from_lsb(&decInfo->extn[i], buffer);
   }

   decInfo->extn[size]='\0';

   return e_success;
}

Status decode_data_from_image3(char *data, int size, DecodeInfo *decInfo)
{
    char buffer[8];

    for(int i=0;i<size;i++)
    {
        fread(buffer,8,1,decInfo->fptr_stego_image);
        decode_byte_from_lsb(&data[i], buffer);
        fwrite(&data[i],1,1,decInfo->fptr_output_file);
    }

    return e_success;
}

Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    *data=0;
    for(int i=0;i<=7;i++)
    {
        *data=*data | ((image_buffer[i]&1)<<(7-i));
    }

    return e_success;
}

Status decode_int_from_lsb(DecodeInfo *decInfo)
{
    char buffer[32];

    fread(buffer,32,1,decInfo->fptr_stego_image);

    decInfo->extn_size=0;
    for(int i=0;i<=31;i++)
    {
        decInfo->extn_size=decInfo->extn_size | ((buffer[i]&1)<<(31-i));
    }

    return e_success;
}

Status decode_int_from_lsb1(DecodeInfo *decInfo)
{
    char buffer[32];

    fread(buffer,32,1,decInfo->fptr_stego_image);

    decInfo->secret_data_size=0;
    for(int i=0;i<=31;i++)
    {
        decInfo->secret_data_size=decInfo->secret_data_size | ((buffer[i]&1)<<(31-i));
    }

    return e_success;
}

// function definition for decoding the magic string
Status decode_magic_string(int size, DecodeInfo *decInfo)
{
    if(decode_data_from_image(size, decInfo)==e_success)
    {
        return e_success;
    }

}

// function definition for decoding the secret file extension size encoded in the stego file
Status decode_stego_file_extn_size(DecodeInfo *decInfo)
{
    if(decode_int_from_lsb(decInfo)==e_success)
    {
        return e_success;
    }

}

// function definition for decoding the secret file extension encoded in the stego file
Status decode_stego_file_extn(DecodeInfo *decInfo)
{
    if(decode_data_from_image2(decInfo->extn_size, decInfo)==e_success)
    {
        return e_success;
    }
}

// function definition for decoding the secret file size encoded in the stego file
Status decode_stego_file_size(DecodeInfo *decInfo)
{
    if(decode_int_from_lsb1(decInfo)==e_success)
    {
        return e_success;
    }
}

// functioin definition for decoding the secret file data encoded in the stego file
Status decode_stego_file_secret_data(DecodeInfo *decInfo)
{
    char buffer[decInfo->secret_data_size];

    if(decode_data_from_image3(buffer, decInfo->secret_data_size, decInfo)==e_success)
    {
        return e_success;
    }
}
