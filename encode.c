#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
Status read_and_validate_encode_args(int argc,char *argv[],EncodeInfo *encInfo);
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status check_capacity(EncodeInfo *encInfo)
{
    printf("how are");
    int src_len=get_image_size_for_bmp(encInfo->fptr_src_image);
    printf("hii");
    /*int len_magic=strlen(magic_string);*/
    char *str=strchr(encInfo->secret_fname,'.');
    int len_sec=strlen(str);
    char ch;
    while((ch=getc(encInfo->fptr_secret))!=EOF)
    {
	ftell(encInfo->fptr_secret);
    }
    encInfo->size_secret_file=ftell(encInfo->fptr_secret);
    int final_len=(4+4+4+len_sec+encInfo->size_secret_file)*8+54;
    if(final_len<=src_len)
    {
	return e_success;
    }
}

Status do_encoding(EncodeInfo *encInfo)
{
    printf("hii");
    if((open_files(encInfo))==e_failure)
    {
	return e_failure;
    }
    char str[30];
    printf("Enter the magic string:");
    scanf("%s",str);
    if(check_capacity(encInfo)==e_failure)
    {
	return e_failure;
    }
    printf("hello");
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
	return e_failure;
    }
    if(encode_magic_string_size(strlen(str),encInfo)==e_failure)
    {
	return e_failure;
    }
    if(encode_magic_string(str,encInfo)==e_failure)
    {
	return e_failure;
    }
    if(encode_secret_file_extn_size(strlen(strstr(encInfo->secret_fname,".txt")),encInfo)==e_failure)
    {
	return e_failure;
    }
    if(encode_secret_file_extn(encInfo->secret_fname,encInfo)==e_failure)
    {
	return e_failure;
    }
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_failure)
    {
	return e_failure;
    }
    if(encode_secret_file_data(encInfo)==e_failure)
    {
	return e_failure;
    }
    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
	return e_failure;
    }
}
/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];
    /*cursor to the starting point*/
    fseek(fptr_src_image,0,SEEK_SET);
    /*read the string  54 bytes into from source file*/
    fread(buffer,54,1,fptr_src_image);
    /*write the string 54 bytes to the destination file*/
    fwrite(buffer,54,1,fptr_dest_image);
    return e_success;
}
/*store Magic string size*/
Status encode_magic_string_size(int magic_string_len,EncodeInfo *encInfo)
{
    char buffer[32];
    /*read the soure image to buffer*/
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_byte_to_lsb(magic_string_len,buffer);/*calling the function*/
    /*write the buffer to stego image file*/
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return  e_success;
}
/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char buffer[8];
    int i;
    for(i=0;magic_string[i]!=0;i++)
    {
     fread(buffer,8,1,encInfo->fptr_src_image);
     encode_data_to_image(magic_string[i],buffer);
     fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_byte_to_lsb(int data,char *image_buffer)
{
    for(int i=31;i>=0;i++)  
    {
	int a=(data&((1<<i))>>i);
	int b=image_buffer[31-i] &(~(1));
	image_buffer[31-i]=a|b;
    }
   return  e_success;
}

/* Encode function, which does the real encoding */
Status encode_data_to_image(char data,char *buffer)
{
	for(int j=7;j>=0;j--)
	{
	    int a=(data&(1<<j))>>j;
	    int b=buffer[7-j] &(~(1));
	    buffer[7-j]=a|b;
	}
    return e_success;
}

/* Encode secret file  extension size */
Status encode_secret_file_extn_size(int extension_size, EncodeInfo *encInfo)
{

    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_byte_to_lsb(extension_size,buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return  e_success;

}

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char buffer[8];
    for(int i=0;file_extn[i]!=0;i++)
    {
     fread(buffer,8,1,encInfo->fptr_src_image);
     encode_data_to_image(file_extn[i],buffer);
     fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}

/* Encode secret file size */
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_byte_to_lsb(file_size,buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    char buffer[8];
    rewind(encInfo->fptr_src_image);
    while((ch=fgetc(encInfo->fptr_secret))!=EOF)
    {
	fread(buffer,8,1,encInfo->fptr_src_image);
	encode_data_to_image(ch,buffer);
	fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    /*Run the loop upto feof!*/
    while(fread(&ch,1,1,fptr_src))
    {
       fwrite(&ch,1,1,fptr_dest);
    }
    /*return e_Success*/
    return e_success;
}
