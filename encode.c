#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"

// function definition for reading and validating the arguments passed for encoding
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(argv[2]!=NULL && strstr(argv[2],".bmp")!=NULL)
    {
        printf(".bmp file present\n");
        encInfo->src_image_fname=argv[2];
    }

    else
    {
        printf(".bmp file not present\n");
        return e_failure;
    }

    if(argv[3]!=NULL && strstr(argv[3],".txt")!=NULL)
    {
        printf(".txt file present\n");
        encInfo->secret_fname=argv[3];
    }

    else
    {
        printf(".txt file not present\n");
        return e_failure;
    }

    if(argv[4]!=NULL && strstr(argv[4],".bmp")!=NULL)
    {
        printf("stego.bmp file present\n");
        encInfo->stego_image_fname=argv[4];
    }

    else
    {
        encInfo->stego_image_fname="stego.bmp";
    }
    
    return e_success;
}

// function definition for performing encoding
Status do_encoding(EncodeInfo *encInfo)
{
    printf("<----- Started doing encoding ------->\n");
    
    // calling function for opening the required files in required mode
    if(open_files(encInfo)==e_success)
    {
        printf("All files opened successfully\n");
    }

    else
    {
        printf("All files did not opened successfully\n");
        return e_failure;
    }

    printf("Enter magic string:");
    scanf(" %[^\n]", encInfo->magic);

    // calling the function for checking the the capacity of stego file to encode the secret data
    if(check_capacity(encInfo)==e_success)
    {
        printf("Capacity checked successfully\n");
    }

    else
    {
        printf("Capacity of source file is less\n");
        return e_failure;
    }

    // function call for copying the header of the source to the destination file
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
    {
        printf("Header copied successfully\n");
    }

    else
    {
        printf("Header did not copied successfully\n");
        return e_failure;
    }

    // function call for encoding the magic string
    if(encode_magic_string(encInfo->magic, encInfo)==e_success)
    {
        printf("Magic string encoded successfully\n");
    }

    else
    {
        printf("Magic string not encoded successfully\n");
        return e_failure;
    }

    // function call for encodeing secret file extension size
    if(encode_secret_file_extn_size(strlen(strchr(encInfo->secret_fname, '.')), encInfo)==e_success)
    {
        printf("Secret file extension size encoded successfully\n");
    }

    else
    {
        printf("Secret file extension size not encoded successfully\n");
        return e_failure;
    }

    //  function call for encoding secter file extension
    if(encode_secret_file_extn(strchr(encInfo->secret_fname, '.'), encInfo)==e_success)
    {
        printf("Secret file extension encoded successfully\n");
    }

    else
    {
        printf("Secret file extension not encoded successfully\n");
        return e_failure;
    }

    // function call for encoding the secret file size
    if(encode_secret_file_size(encInfo->size_secret_file, encInfo)==e_success)
    {
        printf("Secret file size encoded successfully\n");
    }

    else
    {
        printf("Secret file size not encoded successfully\n");
        return e_failure;
    }

    // function call for encoding the secret file data
    if(encode_secret_file_data(encInfo)==e_success)
    {
        printf("Secret file data encoded successfully\n");
    }

    else
    {
        printf("Secret file size not encoded successfully\n");
        return e_failure;
    }

    // function call for copying the remaining data of source file to stego file
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
    {
        printf("Remaining image data copied successfully\n");
    }

    else
    {
        printf("Remaining image data not copied successfully\n");
        return e_failure;
    }

    return e_success;
}

// function definition for opening the required files in required mode
Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image=fopen(encInfo->src_image_fname,"r");

    if(encInfo->src_image_fname==NULL)
    {
        printf("Source file is  not present\n");
        return e_failure;
    }

    else
    {
        printf("Source file is present\n");
    }

    encInfo->fptr_secret=fopen(encInfo->secret_fname,"r");
    
    if(encInfo->secret_fname==NULL)
    {
        printf("Secret file is not present\n");
        return e_failure;
    }

    else
    {
        printf("Secret file is present\n");
    }

    if(encInfo->fptr_stego_image=fopen(encInfo->stego_image_fname,"w"))
    {
        printf("Stego file opened\n");
    }

    return e_success;
}

// function definition for checking the capacity of soure file to encode the secret data
Status check_capacity(EncodeInfo *encInfo)
{
    
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
    // printf("Image capacity = %u\n", encInfo->image_capacity);

    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
    // printf("Secret file size= %lu\n",encInfo->size_secret_file);

    if(encInfo->image_capacity>(strlen(encInfo->magic)*8+32+32+32+encInfo->size_secret_file*8))
    {
        return e_success;
    }

    else
    {
        return e_failure;
    }

    return e_success;

}

// function defintion to get the size of source image file
uint get_image_size_for_bmp(FILE *fptr_image)
{
    int wid, len;

    fseek(fptr_image,18,SEEK_SET);

    fread(&wid,4,1,fptr_image);

    fread(&len,4,1,fptr_image);

    return wid*len*3;
}

// function definition to get the size of secret file
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    long size=ftell(fptr);
    rewind(fptr);

   return size;
}

// function definition for copying the header of the source file to stego file
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image);

    char buffer[54];

    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,fptr_dest_image);

    return e_success;
}

// function definition for encoding the magic string
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    if(encode_data_to_image(magic_string, strlen(magic_string), encInfo)==e_success)
    {
        return e_success;
    }
}

// function definition for encoding the secret data to the stego image
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
    char buffer[8];

    for(int i=0;i<size;i++)
    {
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(data[i], buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }

    return e_success;
}

// function definition for encoding the characters
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=7;i>=0;i--)
    {
      image_buffer[7-i]=(data & (1<<i))>> i | (image_buffer[7-i] & 0xFE);
    }

    return e_success;
}   

// function definition for encoding the integer values
Status encode_int_to_lsb(int data, EncodeInfo *encInfo)
{
    char buffer[32];

    fread(buffer,32,1,encInfo->fptr_src_image);

    for(int i=31;i>=0;i--)
    {
        buffer[31-i]=((data & (1<<i))>>i) | (buffer[31-i]&0xFE);
    }

    fwrite(buffer,32,1,encInfo->fptr_stego_image);

    return e_success;
}

// function definition for encoding the secret file extension size
Status encode_secret_file_extn_size(int data, EncodeInfo *encInfo)
{
    if(encode_int_to_lsb(data, encInfo)==e_success)
    {
        return e_success;
    }

}

// function definition for encoding the secret file extension 
Status encode_secret_file_extn(char *secret_file_ext, EncodeInfo *encInfo)
{
    if(encode_data_to_image(secret_file_ext, strlen(secret_file_ext), encInfo)==e_success)
    {
        return e_success;
    }

}

// function definition for encoding the secret file size
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    if(encode_int_to_lsb(file_size, encInfo)==e_success)
    {
        return e_success;
    }
}

// function definition for encoding the secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char buffer[encInfo->size_secret_file];

    fread(buffer, encInfo->size_secret_file,1,encInfo->fptr_secret);

    return encode_data_to_image(buffer, encInfo->size_secret_file, encInfo);

}

// function definition for encoding the remaining data
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;

    while(fread(&ch,1,1,fptr_src))
    {
        fwrite(&ch,1,1,fptr_dest);
    }

    return e_success;
}





