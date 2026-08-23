/* Name: Prashant Laxman Savantri
   Batch: INT26001B
   Title of the Project: Steganography
   Date: May 05, 2026
   Description: Implementaion of Steganography which involves encoding and decoding
                Encoding: Storing every bit of the secret data into the lsb bit of every character present in the 
                          stego file so that no difference will be found between the source image and stego image.
                    
                Decoding: Extracting the lsb bit of every character encoded from the stego file storing that into 
                          the .txt file which should be resembles the original data present in the secret file.      */


#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>

int main(int argc, char *argv[])
{
    // calling function for checking whether operation is encoding or not
    if(check_operation_type(argv)==e_encode)
    {
        printf("You choosed encoding\n");

        EncodeInfo encInfo;

        // calling function read and validate arguments passed for encoding 
        if(read_and_validate_encode_args(argv, &encInfo)==e_success)
        {
            printf("Read and validate successfully\n");

            // calling function decoding
            if(do_encoding(&encInfo)==e_success)
            {
                printf("\033[32mEncoding Successful\033[0m\n");
                return e_success;
            }

            else
            {
                printf("\033[31mEncoding Unsuccessfull\033[0m\n");
                return e_failure;
            }
        }

        else
        {
            printf("Read and validate unsuccessfully\n");
            return e_failure;
        }

        return e_success;
    }

    // calling function to check whether the operation is decoding or not
    else if(check_operation_type(argv)==e_decode)
    {
        printf("You choosed decoding\n");

        DecodeInfo decInfo;

        // calling function for reading and validating the arguments passed for decoding
        if(read_and_validate_decode_args(argv, &decInfo)==e_success)
        {
            printf("Read and validate successfully\n");

            // calling function do_decoding to do decoding
            if(do_decoding(argv, &decInfo)==e_success)
            {
                printf("\033[32mDecoding Successful\033[0m\n");
                return e_success;
            }

            else
            {
                printf("\033[31mDecoding Unsuccessfull\033[0m\n");
                return e_failure;
            }
        }
        return e_success;
    }

    else
    {
        printf("Invalid arguments\nfor encode -> ./a.out -e beautiful.bmp secret.txt\n");
        printf("for decoding -> ./a.out -d stego.bmp\n");
        return e_failure;
    }
    
}

// definition for checking operation type
OperationType check_operation_type(char *argv[])
{
    if(argv[1]==NULL)
    {
        return e_unsupported;
    }

    if(strcmp(argv[1],"-e")==0)
    {
        return e_encode;
    }

    else if(strcmp(argv[1],"-d")==0)
    {
        return e_decode;
    }

}