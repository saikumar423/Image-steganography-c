#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>

OperationType check_opreation_type(char *argv[]);

int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    uint img_size;
    
    if(check_operation_type(argv)==e_encode)
    {
	if(read_and_validate_encode_args(argc,argv,&encInfo)==e_success)
	{
	    printf("sai open");
	    do_encoding(&encInfo);
	}
    }
    /*else if(check_operation_type(argv)==e_decode)
    {
	if(read_and_validate_decode_args(argv,&encInfo))
	{
	    printf("Read success");
	    return e_success;
	}
    }*/
	else
	{
	    printf("please enter encode operation or decode operation \n");
	}

    // Fill with sample filenames
    encInfo.src_image_fname = "beautiful.bmp";
    encInfo.secret_fname = "secret.txt";
    encInfo.stego_image_fname = "stego_img.bmp";
}
OperationType check_operation_type(char *argv[])
{
    int i=0;
    int count=0;
    while(argv[i]!=0)
    {
	count++;
	i++;
    }
    if(count>1)
    {
	if(strcmp(argv[1],"-e")==0)
	{
	    return e_encode;
	}
	else if(strcmp(argv[1],"-d")==0)
	{
	    return e_decode;
	}
	else
	{
	    return e_unsupported;
	}
    }
	else
	{
	    printf("Invalid number of arguments");
	}
}
Status read_and_validate_encode_args(int argc,char *argv[],EncodeInfo *encInfo)
{
    if(argc==1)
    {
	printf("Enter the remaining arguments");
	return e_failure;
    }
    else if(argc==4 ||argc==5)
    {
	if(strstr(argv[2],".bmp")==NULL)
	{
	    printf("Invalid bmp name\n");
	    return e_failure;
	}
	/*if(strstr(argv[2],".bmps")==NULL)
	{
	    return e_failure;
	}*/
	if(strstr(argv[3],".")==NULL)
	{
	    printf("Invalid bmp name");
	    return e_failure;
	}
	if(argv[4]!=NULL)
	{
	    if(strstr(argv[4],".bmp"))
	    {
		encInfo->stego_image_fname="stego_img.bmp";
	    }
	}
	if(argv[4]==NULL)
	{
	    encInfo->stego_image_fname="file.bmp";
	}
	encInfo->src_image_fname=argv[2];
	encInfo->secret_fname=argv[3];
	return e_success;
    }
    else
    {
	printf("Invalid number of arguments");
	return e_failure;
    }
}


   /* // Test open_files
    if (open_files(&encInfo) == e_failure)
    {
    	printf("ERROR: %s function failed\n", "open_files" );
    	return 1;
    }
    else
    {
    	printf("SUCCESS: %s function completed\n", "open_files" );
    }

    // Test get_image_size_for_bmp
    img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
    printf("INFO: Image size = %u\n", img_size);

    return 0;
}*/
