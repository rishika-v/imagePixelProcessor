//Nandita Balaji
//nbalaji2

//Rishika Vadlamudi
//rvadlam2

#include "ppm_io.h"
#include "img_processing.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

Image * grayscale(Image * input){
		   
  //New Pixel array pointer pointing to same data to make changes
  Pixel* temp = input->data;
  int red = 0;
  int green = 0;
  int blue = 0;
  unsigned char grayscale = 0;

  //For loop that goes through the Pixel array 
  for (int i = 0; i < (input->rows * input->cols); i++){

    //Takes the value of each colour
    red = (int)(temp->r);
    green = (int)(temp->g);
    blue = (int)(temp->b);
    
    //Converts to grayscale using the conversion given and type unsigned char
    grayscale = (unsigned char)(0.3*red + 0.59*green + 0.11*blue);

    //New grayscale value is assigned back
    temp->r = grayscale;
    temp->g = grayscale;
    temp->b = grayscale;

    temp++;
  }
  
  return input;
}

Image * binarize(Image* input, int threshold){
  input = grayscale(input);
  Pixel* temp = input->data;

  //For loop goes through temp pointer which points to same data
  for (int i = 0; i < (input->rows * input-> cols) ; i++){
    if ((int)(temp->r) < threshold){
      //Sets value to zero if below the threshold
      temp->r = 0;
      temp->g = 0;
      temp->b = 0;
    }
    
    else{
      //Sets value to 255 if above/equal to threshold
      temp->r = 255;
      temp->g = 255;
      temp->b = 255;
    }
    
    temp++;
  }
  
  return input;
}

Image * crop(Image* input, int top_c, int top_r, int bot_c, int bot_r){
  
  int new_col  = bot_c - top_c;
  int new_row  = bot_r - top_r;


  Pixel *pix = (Pixel *) (malloc(sizeof(Pixel) * new_col * new_row)); 

  
  //copy from original image into new image

  for (int row = 0; row < new_row; row++) {
    for (int col = 0; col < new_col; col++) {
      pix[row * new_col + col].r = input->data[ (row + top_r) * (input -> cols) + col + top_c ].r;
      pix[row * new_col + col].g = input->data[ (row + top_r) * (input -> cols) + col + top_c ].g;
      pix[row * new_col + col].b = input->data[ (row + top_r) * (input -> cols) + col + top_c ].b;
    }
  }

  
  //assign new pixel array and resize image
  free(input->data);
  input->data = NULL;
  input->data = pix;
  input->cols = new_col;
  input->rows = new_row;
  
  return input;
}

Image * transpose(Image* input){
  //store flipped height and width
  int new_height = input->cols;
  int new_width = input->rows;

  //dynamically allocate new pixel array
  Pixel *pix = (Pixel *) (malloc(sizeof(Pixel) * new_height * new_width));

  // Pixel pix[new_height * new_width];

  //go through each pixel and switch (x,y) to (y,x)
  for (int row = 0; row < new_height; row++) {
    for (int col = 0; col < new_width; col++) {
      pix[row * new_width + col].r = input->data[col  * new_height  + row].r;
      pix[row * new_width + col].g = input->data[col  * new_height  + row].g;
      pix[row * new_width + col].b = input->data[col  * new_height  + row].b;
    }
  }
      
  
  //assign new dimensions and values to image
  input->rows = new_height;
  input->cols = new_width;
  free(input->data);
  input->data = NULL;
  input->data = pix;

  return input;
}

Image * gradient(Image* input){
  input = grayscale(input);

  //Create temporary pixel array to traverse and edit the values
  Pixel *temp = (Pixel *) (malloc(sizeof(Pixel) * input->rows * input->cols));;
  int x_comp = 0;
  int y_comp = 0;
  unsigned char gradient = 0;
  
  //For loop to traverse the rows
  for (int i = 0; i < input->rows; i++){
    //For loop to traverse columns
    for (int j = 0; j < input->cols; j++){
      //Check if it is not a boundary
       if (i != 0 && i != (input->rows -1) && j != 0 && j != (input->cols-1)){
	 x_comp = (((int)input->data[(i+1)*input->cols + j].r) - ((int)input->data[(i-1)*input->cols + j].r))/2;
	 y_comp = (((int)input->data[i*input->cols + (j+1)].r) - ((int)input->data[i*input->cols + (j-1)].r))/2;
	 gradient = (unsigned char)(abs(x_comp) + abs(y_comp));
	 temp[i*input->cols +j].r = gradient;
	 temp[i*input->cols +j].g = gradient;
	 temp[i*input->cols +j].b = gradient;
       }
    }
  }

  //Repeat with boundary cases and set them to zero
  for (int i = 0; i < input->rows; i++){
    for (int j = 0; j < input->cols; j++){ 
      if (i==0 || i == (input->rows-1) || j == 0 || j == (input->cols-1)){
	temp[i*input->cols +j].r = 0;
	temp[i*input->cols +j].g = 0;
	temp[i*input->cols +j].b = 0;
      }
    }
  }
  
  //assign new pixel array
  free(input->data);
  input->data = NULL;
  input->data = temp;
  return input;
}

//seam helper function: function to create seams and returns array of points (seam) with least gradient energy
Point * make_seam(Image* input) {
  input = gradient(input);

  //2D array of points to store- each seam will take up a row in this dynamically allocated array
  Point** seam_arr = malloc (sizeof(Point*) * (input->cols));

  for (int a = 0; a < input->cols; a++){
    seam_arr[a] = malloc (sizeof(Point) * (input->rows));
  }

  //array to store the gradient values of each seam
  int * seam_gradient = malloc (sizeof(int) * (input->cols));
  
  for (int c = 0; c < input->cols; c++){
    //start at row 0
    seam_arr[c][0].x = 0;
    seam_arr[c][0].y = c;
    seam_gradient[c] = 0;

    //acts as a cursor to follow the path of the seam
    int cur = c;
    
    if(input->rows == 2){
      seam_arr[c][input->rows-1].x = input->rows-1;
      seam_arr[c][input->rows-1].y = cur;
    }
    
    //traverse the rows of the image
    for (int r = 1; r < input->rows-1; r++){
      seam_arr[c][r].x = r;

      //store the values of the gradient at each pixel to compare
      int val1 = (int)input->data[r*input->cols+cur].r;
      int val2 = 255;
      if (cur != 0){
	val2 = (int)input->data[r*input->cols+cur-1].r;
      }
      int val3 = 255;
      if(cur != input->cols-1){
	val3 = (int)input->data[r*input->cols+cur+1].r;
      }
      
      //Compare the values using if and else if to find the minimum value
      //Increment, decrement or keep cur constant depending on the minimum value and store value of cur in point for this seam
      
      if((cur != input->cols-1) && (cur!=0) && ((val1<=val2 && val1<=val3) || (cur ==1 && val1<=val3) || (cur == input->cols-2 && val1<=val2))){
	seam_arr[c][r].y = cur;
	seam_gradient[c]+= val1;

	//If at the second-last row then close the seam by connecting to the last row in the same column
	//This if statement under all possibilities
	if(r == input->rows - 2){
	  seam_arr[c][input->rows-1].x = input->rows-1;
	  seam_arr[c][input->rows-1].y = cur;
	}
      }
      
      else if((cur == input->cols-1) || ((cur!=1) && (cur!=0) && ((val2<val1 && val2<=val3) || (cur == input->cols-2 && val2<val1)))){
	cur--;
	seam_arr[c][r].y = cur;
	seam_gradient[c]+= val2;
	if(r == input->rows - 2){
	  seam_arr[c][input->rows-1].x = input->rows-1;
	  seam_arr[c][input->rows-1].y = cur;
	}
      }
      
      else{
	cur++;
	seam_arr[c][r].y = cur;
	seam_gradient[c]+=val3;
	if(r == input->rows - 2){
	  seam_arr[c][input->rows-1].x = input->rows-1;
	  seam_arr[c][input->rows-1].y = cur;
	}
      }	
    }	   
  }

  //Compare the gradient value at each element in gradient array and store
  //least value in variable and store the index at which least gradient occurs
  int least_gradient = seam_gradient[0];
  int seam_index = 0;
  
  for (int j = 0; j < input->cols; j++){
    if (seam_gradient[j] < least_gradient){
      least_gradient = seam_gradient[j];
      seam_index = j;
    }
  }

  //Allocate array of points for the seam that should be removed, based on its gradient energy
  Point * seam_to_remove = malloc(sizeof(Point) * input->rows);

  //copy elements of the seam from the seam array to newly allocated array of points
  for(int x = 0; x < input->rows; x++){
    seam_to_remove[x] = seam_arr[seam_index][x];
  }

  for (int a = 0; a < input->cols; a++){
    free(seam_arr[a]);
  }
  free(seam_arr);
  free(seam_gradient);

  //return seam to be removed
  return seam_to_remove;
}						       

//seam helper function: function to remove the seam from the image
Image * remove_seam(Image* input, Point * seam_to_rm){

  for (int row = 0; row < input->rows; row++){
    int col = 0;
    //Move to the place where seam to be removed is located in image
    while(seam_to_rm->x != row || seam_to_rm->y != col){
      input->data[row*input->cols+col] = input->data[row*input->cols+col];
      col++;
    }
    //Skip the element in the seam by assigning each pixel to the pixel beside it
    while (col < input->cols-1){
      input->data[row*input->cols+col] = input->data[row*input->cols+col+1];
      col++;
      }

    //Move to next point in the seam array
    seam_to_rm++;
  }

  //return image after removing the seam
  return input;
}

//Main seam function
Image * seam(Image* input, float sf1, float sf2){

  //Dynamically allocate new image 
  Image * img = malloc(sizeof(Image));
  Pixel * pix = malloc(sizeof(Pixel) * input->cols * input->rows);
  img->cols = input->cols;
  img->rows = input->rows;
  img->data = pix;

  //Copy the elements of the input image into new image
  for ( int f = 0; f < input->rows; f++){
    for (int u = 0; u < input->cols; u++){
      img->data[f*input->cols+u] = input->data[f*input->cols+u];
    }
  }

  //Calculate new size of image after applying scale factors
  int new_col = input->cols * sf1;
  int new_row = input->rows * sf2;

  if (new_col < 2)
    new_col = 2;
  if (new_row < 2)
    new_row = 2;

  //Compute number of seams to be removed
  int seams = input->cols - new_col;

  //iterate for number of seams to remove 1 seam each time
  for (int i = 0; i < seams; i++){
    //Call helper functions make_seam and remove_seam to find seam
    //with lowest gradient energy and remove it
    Point * seam = make_seam(img);
    input = remove_seam(input, seam);

    //Copy over the elements from input to img to pass img through seam_making again
    //in the next iteration. Copy over so that last row can be deleted without losing
    //the newly seam carved image
    
    for (int g = 0; g < (input->cols-1); g++){
      img->data[g] = input->data[g];
    }
    
    for ( int f = 1; f < input->rows; f++){
      for (int u = 0; u < (input->cols-1); u++){
	img->data[f*(input->cols-1)+u] = input->data[f*(input->cols)+u];
	input->data[f*(input->cols-1)+u] = input->data[f*(input->cols)+u];
      }
    }
    //Decrease size of img and input by decrementing columns
    input->cols--;
    img->cols--;
    
    free(seam);
  }

  //transpose both img and input to work on rows
  input = transpose(input);
  img = transpose(img);

  seams = input->cols - new_row;
  for (int i = 0; i < seams; i++){
    Point * seam = make_seam(img);
    input = remove_seam(input, seam);
    
    for (int g = 0; g < (input->cols-1); g++){
      img->data[g] = input->data[g];
    }
    
    for ( int f = 1; f < input->rows; f++){
      for (int u = 0; u < (input->cols-1); u++){
	img->data[f*(input->cols-1)+u] = input->data[f*(input->cols)+u];
	input->data[f*(input->cols-1)+u] = input->data[f*(input->cols)+u];
      }
    }
    
    input->cols--;
    img->cols--;
    
    free(seam);
  }

  //transpose input back
  input = transpose(input);

  //free dynamically allocated image img and its data
  free(img->data);
  free(img);

  //return seam carved image
  return input;
}

			    

