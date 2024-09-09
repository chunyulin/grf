#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include <iostream>
#include <vector>
#include <string>
#include <png++/png.hpp>
#include "utils.h"

#include <random>

using namespace std;
using std::string;

typedef double real;
typedef vector<real> Array;
typedef vector<Array>  Matrix;
typedef vector<Matrix> Image;

Matrix getGaussian(int height, int width, double sigma)
{
    Matrix kernel(height, Array(width));
    double sum=0.0;
    int i,j;

    for (i=0 ; i<height ; i++)
    for (j=0 ; j<width ; j++) {
            kernel[i][j] = exp(-(i*i+j*j)/(2*sigma*sigma))/(2*M_PI*sigma*sigma);
            sum += kernel[i][j];
    }

    for (i=0 ; i<height ; i++)
    for (j=0 ; j<width ; j++)
        kernel[i][j] /= sum;

    return kernel;
}

Image genNoise(int width, int height)
{
    std::random_device rd;
    std::mt19937 gen(rd());  // Any faster CPU-based random generator?
    std::uniform_real_distribution<real> rdis(0.0, 255.0);

    Image imageMatrix(3, Matrix(height, Array(width)));
    for (int y=0 ; y<height ; y++)
    for (int x=0 ; x<width ; x++) {
            imageMatrix[0][y][x] = rdis(gen);   //red
            imageMatrix[1][y][x] = rdis(gen);   //green
            imageMatrix[2][y][x] = rdis(gen);   //blue
    }
    return imageMatrix;
}

Image loadImage(const char *filename)
{
    png::image<png::rgb_pixel> image(filename);
    Image imageMatrix(3, Matrix(image.get_height(), Array(image.get_width())));

    for (int h=0 ; h<image.get_height() ; h++)
    for (int w=0 ; w<image.get_width() ; w++) {
            imageMatrix[0][h][w] = image[h][w].red;
            imageMatrix[1][h][w] = image[h][w].green;
            imageMatrix[2][h][w] = image[h][w].blue;
    }
    return imageMatrix;
}

void saveImage(Image &image, const char *filename)
{
    assert(image.size()==3);

    int height = image[0].size();
    int width = image[0][0].size();

    png::image<png::rgb_pixel> imageFile(width, height);

    for (int y=0 ; y<height ; y++)
    for (int x=0 ; x<width ; x++) {
            imageFile[y][x].red   = image[0][y][x];
            imageFile[y][x].green = image[1][y][x];
            imageFile[y][x].blue  = image[2][y][x];
    }
    imageFile.write(filename);
    std::cout << "Save image " << width << " x " << height << " to " << filename << " \n";
}

Image applyFilter(Image &image, Matrix &filter){
    assert(image.size()==3 && filter.size()!=0);

    int height = image[0].size();
    int width = image[0][0].size();
    int filterHeight = filter.size();
    int filterWidth = filter[0].size();
    int newImageHeight = height-filterHeight+1;
    int newImageWidth = width-filterWidth+1;

    Image newImage(3, Matrix(newImageHeight, Array(newImageWidth)));

    for (int d=0; d<3 ; d++)
    for (int i=0; i<newImageHeight ; i++)
    for (int j=0; j<newImageWidth ; j++)
    for (int h=i; h<i+filterHeight ; h++)
    for (int w=j; w<j+filterWidth ; w++) {
            newImage[d][i][j] += filter[h-i][w-j]*image[d][h][w];
    }

    return newImage;
}

Image applyFilter(Image &image, Matrix &filter, int times)
{
    Image newImage = image;
    for(int i=0 ; i<times ; i++) {
        newImage = applyFilter(newImage, filter);
    }
    return newImage;
}

int main()
{


    utils::reset_timer();
    Matrix filter = getGaussian(3, 3, 2.0);
    printf("Build kernel in %f ms\n", utils::msecs_since());

    int w=2000, h=1000;
    utils::reset_timer();
    Image image = genNoise(w,h);
    printf("Gen noise %d x %d in %f ms\n", w, h, utils::msecs_since());

    utils::reset_timer();
    saveImage(image, "f0.png");
    printf("Save image in %f ms\n", utils::msecs_since());

    int iter=4;
    for (int i=0;i<iter;++i) {

      utils::reset_timer();
      Image newimage = applyFilter(image, filter);
      std::cout << "Apply filter in " << utils::msecs_since() << " ms\n";

      string fn("f" + std::to_string(i+1) + ".png");
      utils::reset_timer();
      saveImage(newimage, fn.c_str());
      std::cout << "Save image in " << utils::msecs_since() << " ms\n";

      image = newimage;

    }
}
