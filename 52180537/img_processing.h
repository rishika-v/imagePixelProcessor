//Nandita Balaji
//nbalaji2

//Rishika Vadlamudi
//rvadlam2

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

Image * grayscale(Image* input);

Image * binarize(Image* input, int threshold);

Image * crop(Image* input, int top_c, int top_r, int bot_c, int bot_r);

Image * transpose(Image* input);

Image * gradient(Image* input);

Point * make_seam(Image* input);

Image * remove_seam(Image* input, Point * seam_to_rm); 

Image * seam(Image* input, float sf1, float sf2);

#endif
