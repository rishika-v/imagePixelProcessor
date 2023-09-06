//Nandita Balaji
//nbalaji2

//Rishika Vadlamudi
//rvadlam2

/*****************************************************************************
 * Midterm Project - A program to run the image processing operations
 * Note: for naming convention, we try to follow Google C++ style guide:
 *       https://google.github.io/styleguide/cppguide.html
 * It is not compulsory, but you are highly encouraged to follow a convention.
 *
 * Summary: This file implements a program for image processing operations.
 *          Different operations take different input arguments. In general,
 *            ./project <input> <output> <operation name> [operation params]
 *          The program will return 0 and write an output file if successful.
 *          Otherwise, the below error codes should be returned:
 *            1: Wrong usage (i.e. mandatory arguments are not provided)
 *            2: Input file I/O error
 *            3: Output file I/O error
 *            4: The Input file cannot be read as a PPM file
 *            5: Unsupported image processing operations
 *            6: Incorrect number of arguments for the specified operation
 *            7: Invalid arguments for the specified operation
 *            8: Other errors 
 *****************************************************************************/
#include "ppm_io.h" // PPM I/O header
#include "img_processing.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// TODO: include requried headers for your projects.  
// We recommend to put your image processing operations in 
//  img_processing.h for decleartions and
//  img_processing.c for their defintions
// Then you should include the below header:
//#include "img_processing.h" // Image processing header

int main(int argc, char **argv) {
  //if too few or too many command line args entered
  if (argc < 4 || argc > 8 ) {
    fprintf(stderr, "Error: incorrect number of arguments to run program\n");
    return 1;
  }
  
  char* inputname = argv[1];
  char* outputname = argv[2];

  //open input file, check if properly open
  FILE *inputFile = fopen(inputname, "r");
  if (!inputFile){
    fprintf(stderr, "Error: Could not open input file\n");
    return 2;
  }

  //open output file, check if properly opened
  FILE *outputFile = fopen(outputname, "w");
  if (!outputFile){
    fprintf(stderr, "Error: Could not open output file\n");
    return 3;
  }
  
  //read inputFile into inputImage
  Image* inputImage = ReadPPM(inputFile);
  if (!inputImage){
    fprintf(stderr, "Error: input file could not be read as PPM file\n");
    fclose(inputFile);
    fclose(outputFile);
    return 4;
  }

  //create outputImage; set to NULL until function is called
  Image* outputImage = NULL;
  
  //Check if the arguments after function call are integers or float numbers (decimal points)
  for (int h = 4; h < argc; h++){
    int point = 0;
    for(int i = 0; i < (int)strlen(argv[h]); i++){
      if (argv[h][i] == 46){
	point++;
      }
      if ((!isdigit(argv[h][i]) && argv[h][i] != 46) || point > 1){
	fprintf(stderr, "Error: Invalid argument for specified operation\n");
	free(inputImage->data);
	free(inputImage);
	fclose(inputFile);
	fclose(outputFile);
	return 7;
      }
    }
  }

  //check if proper number of parameters specified for each function
  if (((strcmp(argv[3], "grayscale") == 0 || strcmp(argv[3], "transpose") == 0 || strcmp(argv[3], "gradient") == 0) && argc!=4) || (strcmp( argv[3], "binarize") == 0 && argc!=5) || (strcmp( argv[3], "crop") == 0 && argc!=8) || (strcmp(argv[3], "seam") == 0 && argc!=6) ){
    fprintf(stderr, "Error: Incorrect number of arguments for specified operation\n");
    free(inputImage->data);
    free(inputImage);
    fclose(inputFile);
    fclose(outputFile);
    return 6;
  }

  //perform function operations corresponding to command line input
  if (strcmp(argv[3], "grayscale") == 0){  
    outputImage = grayscale(inputImage);
  }
  
  else if (strcmp(argv[3], "binarize") == 0){
    int t = atoi(argv[4]);
    outputImage = binarize(inputImage,t );
  }
  
  else if (strcmp(argv[3], "crop") == 0){
    int	tc = atoi(argv[4]);
    int tr = atoi(argv[5]);
    int	bc = atoi(argv[6]);
    int	br = atoi(argv[7]);
    
    //check that new dimensions are reasonable, throw error if not                                                                               
    if (tc < 0 || tc > inputImage->cols || bc < 0 || bc > inputImage->cols || tr < 0 || tr > inputImage->rows || br < 0 || br > inputImage->rows){
      printf("Error: invalid argument for specified operation\n");
      free(inputImage->data);
      free(inputImage);
      fclose(inputFile);
      fclose(outputFile);
      return 7;
    }
    
    outputImage = crop(inputImage, tc, tr, bc, br);
  }
  
  else if (strcmp(argv[3], "transpose") == 0) {	
    outputImage = transpose(inputImage);
  }
  
  else if (strcmp(argv[3], "gradient") == 0) {
    outputImage = gradient(inputImage);
  }
  
  else if (strcmp(argv[3], "seam") == 0) {
    float scaleFactor1 = atof(argv[4]);
    float scaleFactor2 = atof(argv[5]);

    //check if scale factors are greater than 1, then throw error
    if (scaleFactor1 > 1 || scaleFactor2 > 1){      
      fprintf(stderr, "Error: Invalid argument for specified operation\n");
      free(inputImage->data);
      free(inputImage);
      fclose(inputFile);
      fclose(outputFile);
      return 7;
    }
    
    outputImage = seam(inputImage, scaleFactor1, scaleFactor2);  
  }

  //if 
  else {
    fprintf(stderr, "Error: Invalid function call to unsupported image processing operation\n");
    free(inputImage->data);
    free(inputImage);
    fclose(inputFile);
    fclose(outputFile);
    return 5;
  }

  //write outputImage into PPM file
  int ret_val =  WritePPM(outputFile, outputImage);
  
  //if output file was unable to be written properly
  if(ret_val == -1){
    fprintf(stderr, "Error: Could not convert output image to output file\n");
    free(inputImage->data);
    free(inputImage);
    fclose(inputFile);
    fclose(outputFile);
    return 8;
  }

  fclose(inputFile);
  fclose(outputFile);  
  free(inputImage->data);
  free (inputImage);
  
  return 0;
}
