#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <png++/png.hpp>

#include <iostream>
#include <vector>
#include <random>

#include "utils.h"

#define pow2(x) ((x)*(x))

using namespace std;

typedef double real;
typedef vector<real> Array;
typedef vector<Array>  Matrix;
typedef vector<Matrix> Image;

const int CHANNEL = 3;



Matrix getGaussian(int kh, int kw, real sigma)
{
    Matrix kernel(kh, Array(kw));
    real sum=0.0;

    for (int i=0 ; i<kh ; i++) {
        for (int j=0 ; j<kw ; j++) {
            kernel[i][j] = exp(-(pow2(i-kh/2)+pow2(j-kw/2))/(2*sigma*sigma));
            sum += kernel[i][j];
        }
    }

    for (int i=0 ; i<kh ; i++) {
        for (int j=0 ; j<kw ; j++) {
            kernel[i][j] /= sum;
        }
    }

    return kernel;
}

Image genNoise(int width, int height)
{
    std::random_device rd;
    std::mt19937 gen(rd());  // Any faster CPU-based random generator?
    std::uniform_real_distribution<real> rdis(0.0, 255.0);

    Image imageMatrix(CHANNEL, Matrix(height, Array(width)));
    for (int y=0; y<height; y++)
    for (int x=0; x<width ; x++) {
        auto tmp = rdis(gen);
        imageMatrix[0][y][x] = tmp;   //red
        imageMatrix[1][y][x] = tmp;   //green
        imageMatrix[2][y][x] = tmp;   //blue
    }

    return imageMatrix;
}

Image loadImage(const char *filename)
{
    png::image<png::rgb_pixel> image(filename);
    Image imageMatrix(3, Matrix(image.get_height(), Array(image.get_width())));

    real maxd = -1e10;
    real mind =  1e10;

    for (int h=0 ; h<image.get_height() ; h++) {
        for (int w=0 ; w<image.get_width() ; w++) {
            imageMatrix[0][h][w] = image[h][w].red;
            imageMatrix[1][h][w] = image[h][w].green;
            imageMatrix[2][h][w] = image[h][w].blue;
            maxd = ( image[h][w].red > maxd ) ? image[h][w].red : maxd;
            mind = ( image[h][w].red < maxd ) ? image[h][w].red : mind;
        }
    }

    std::cout << "Load image " << image.get_width() << " x " << image.get_height() << " in [" << mind << ":" << maxd << "]" << std::endl;

    return imageMatrix;
}

void saveImage(Image &image, const char *filename)
{
    assert(image.size()==CHANNEL);

    int height = image[0].size();
    int width = image[0][0].size();

    png::image<png::rgb_pixel> imageFile(width, height);

    for (int y=0 ; y<height ; y++) {
        for (int x=0 ; x<width ; x++) {
            imageFile[y][x].red   = image[0][y][x];
            imageFile[y][x].green = image[1][y][x];
            imageFile[y][x].blue  = image[2][y][x];
        }
    }
    imageFile.write(filename);
}

Image applyFilter(Image &image, Matrix &filter){
    assert(image.size()==3 && filter.size()>2);

    int height = image[0].size();
    int width = image[0][0].size();
    int kh = filter.size();
    int kw = filter[0].size();
    int newImageHeight = height; //-filterHeight+1;
    int newImageWidth = width;   //-filterWidth+1;
    int d,i,j,h,w;

    Image newImage(3, Matrix(newImageHeight, Array(newImageWidth)));

/*
    for (int i=kh/2; i<height-(kh-1-kh/2); ++i)
    for (int j=kw/2; j<width -(kw-1-kw/2); ++j)
    for (int h=0; h<kh; ++h)
    for (int w=0; w<kw; ++w)
            newImage[d][i][j] += filter[h][w]*image[d][i-kh/2+h][j-kw/2+w];
*/


    for (d=0 ; d<3 ; d++)
    for (i=kh/2 ; i<newImageHeight-(kh-1-kh/2); i++)
    for (j=kw/2 ; j<newImageWidth -(kw-1-kw/2); j++)
    for (h=0 ; h<kh ; h++)
    for (w=0 ; w<kw ; w++) {
        newImage[d][i][j] += filter[h][w]*image[d][i-kh/2+h][j-kw/2+w];
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

void blurPhoto()
{
    utils::reset_timer();
    Matrix filter = getGaussian(7, 7, 3.0);
    printf("Build kernel in %f ms\n", utils::msecs_since());

    utils::reset_timer();
    Image image = loadImage("run/sample1k.png");
    printf("Load image in %f ms\n", utils::msecs_since());

    int iter=20;
    for (int i=1;i<=iter;++i) {

      utils::reset_timer();
      Image newimage = applyFilter(image, filter);
      std::cout << "Apply filter in " << utils::msecs_since() << " ms\n";

      if (i%5==0) {
        string fn("run/s" + std::to_string(i) + ".png");
        utils::reset_timer();
        saveImage(newimage, fn.c_str());
        std::cout << "Save image in " << utils::msecs_since() << " ms\n";
      }

      image = newimage;

    }
}

void blurNoise()
{
    utils::reset_timer();
    Matrix filter = getGaussian(7, 7, 3.0);
    printf("Build kernel in %f ms\n", utils::msecs_since());

    utils::reset_timer();
    Image image = genNoise(1000,600);
    printf("Gen noisy image in %f ms\n", utils::msecs_since());
    saveImage(image, "run/n0.png");

    int iter=20;
    for (int i=1;i<=iter;++i) {

      utils::reset_timer();
      Image newimage = applyFilter(image, filter);
      std::cout << "Apply filter in " << utils::msecs_since() << " ms\n";

      if (i%5==0) {
       string fn("run/n" + std::to_string(i) + ".png");
       utils::reset_timer();
       saveImage(newimage, fn.c_str());
       std::cout << "Save image in " << utils::msecs_since() << " ms\n";
      }

      image = newimage;

    }
}


int main()
{
    blurPhoto();
    blurNoise();
    return 0;
}