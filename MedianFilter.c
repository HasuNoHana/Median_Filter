/*
 ============================================================================
 Name        : MedianFilter.c
 Author      : Zuzanna Santorowska
 ============================================================================
 */

#include<stdio.h>
#include<stdlib.h>

    typedef struct FileHeader {
      short bfType;
      int bfSize;
      short bfReserved1;
      short bfReserved2;
      short bfOffBits;
    }FileHeader;

    typedef struct PictureHeader {
      int biSize;
      int biWidth;
      int biHeight;
      short biPlanes;
      short biBitCount;
      int biCompression;
      int biSizeImage;
      int biXPelsPerMeter;
      int biYPelsPerMeter;
      int biClrUsed;
      int biClrImportant;
     }PictureHeader;

     typedef struct KoloryRGB {
      char R;
      char G;
      char B;
     }KoloryRGB;

     typedef struct Pixel {
    	 int red;
    	 int green;
    	 int blue;
     }Pixel;

Pixel* GetDataFromFile(int* column, int* row, FileHeader *fileHeader, PictureHeader *pictureHeader) {
	FileHeader File;
	PictureHeader Picture;
	KoloryRGB Rgb;
	char dane;

    FILE *f = fopen("Bitmap/salt_noise.bmp","rb");

     fread(&File.bfType,sizeof(File.bfType),1,f);

     fread(&File.bfSize,sizeof(File.bfSize),1,f);

     fread(&File.bfReserved1,sizeof(File.bfReserved1),1,f);

     fread(&File.bfReserved2,sizeof(File.bfReserved2),1,f);

     fread(&File.bfOffBits,sizeof(File.bfOffBits),1,f);

     fseek(f,14,SEEK_SET);

     fread(&Picture.biSize,sizeof(Picture.biSize),1,f);

     fread(&Picture.biWidth,sizeof(Picture.biWidth),1,f);

     fread(&Picture.biHeight,sizeof(Picture.biHeight),1,f);

     fread(&Picture.biPlanes,sizeof(Picture.biPlanes),1,f);

     fread(&Picture.biBitCount,sizeof(Picture.biBitCount),1,f);

     fread(&Picture.biCompression,sizeof(Picture.biCompression),1,f);

     fread(&Picture.biSizeImage,sizeof(Picture.biSizeImage),1,f);

     fread(&Picture.biXPelsPerMeter,sizeof(Picture.biXPelsPerMeter),1,f);

     fread(&Picture.biYPelsPerMeter,sizeof(Picture.biYPelsPerMeter),1,f);

     fread(&Picture.biClrUsed,sizeof(Picture.biClrUsed),1,f);

     fread(&Picture.biClrImportant,sizeof(Picture.biClrImportant),1,f);

     (*fileHeader) = File;
     (*pictureHeader) = Picture;

     fseek(f,File.bfOffBits,SEEK_SET);

     Pixel* arr = malloc( Picture.biWidth * Picture.biHeight * sizeof(Pixel) );//array for main data
     int *arrR = malloc( Picture.biWidth * Picture.biHeight * sizeof(int) );//healping arrays for colours
     int *arrG = malloc( Picture.biWidth * Picture.biHeight * sizeof(int) );
     int *arrB = malloc( Picture.biWidth * Picture.biHeight * sizeof(int) );
    
     //reading data of picture
     int bmpImg;
    int j = 0;

     for (int i = File.bfOffBits; i < File.bfSize;i++) {
        bmpImg=0;   
        fseek(f,i,SEEK_SET);
        fread(&bmpImg, 1, 1, f);
          if( j % 3 == 0)   //separating colours for suitable array 
            arrR[j/3] = bmpImg;
        else if ( j % 3 == 1)
            arrG[j/3] = bmpImg;
        else if ( j % 3 == 2)
            arrB[j/3] = bmpImg;
        j++;
     }

    int k = 0;
     for(int i = 0; i < Picture.biWidth; i++)   //extracting data from colour arrays
	  {
          for(int j = 0; j < Picture.biHeight; j++)
          {
              arr[ k ].red = arrR[k];
              arr[ k ].green = arrG[k];
              arr[ k].blue = arrB[k];
  
              k++;
          }
	  }

      (*column) = Picture.biWidth;
      (*row) = Picture.biHeight;

     fclose(f);

     return arr;
}

void Sort(int* arr)
{
    int n = 9;
    for(int i = 0; i < 9; i++)
    {
        for(int j = 0; j < n-1; j++)
        {
            if(arr[j] > arr[j+1])
            {
                int h = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = h;
            }
        }
        n--;
    }
}

Pixel Median (Pixel *med)
{
    int Red[9];
    int Green[9];
    int Blue[9];
    for(int i = 0; i < 9; i++)
    {
        Red[i] = med[i].red;    //devide med into colour arrays
        Green[i] = med[i].green;
        Blue[i] = med[i].blue;
    }
    Sort(Red);
    Sort(Green);
    Sort(Blue);
    Pixel p;
    p.blue = Blue[4];   //because there is median
    p.green = Green[4];
    p.red = Red[4];
    return p;
}

Pixel Median3x3 (Pixel *a, int row, int i)
{
    Pixel* med = malloc( 9 * sizeof(Pixel) );
   int w = 0;
    for(int n = -row; n <= row; n+=row)
    {
        for(int m = -1; m <= 1; m++)
        {
            med[ w ] = a[ i + n + m ];  //geting information from original array
             w++;
        }
    }
    return Median(med);
}

 Pixel* MedianFilter (Pixel *arr, int column, int row)
{
    Pixel* arrOut = malloc( column * row * sizeof(Pixel) ); //creating a structure that is the answer
    for(int i = 0; i< (column * row); i++)
    {
        arrOut[i] = arr[i];
    }
    for(int i = 1; i < column-1; i++)
    {
        for(int j = 1; j < row-1; j++)
        {
            arrOut[ i * row + j ] = Median3x3(arr, column, i * row + j);    //finding median for current
        }
    }
    return arrOut;
}

void SaveBitmap(Pixel *arrOut, int column, int row, FileHeader* fileHeader, PictureHeader* pictureHeader)
{

    FileHeader File = (*fileHeader);
	PictureHeader Picture = (*pictureHeader);

    FILE *f = fopen("Bitmap/median_filter.bmp","wb");

    fwrite(&File.bfType,sizeof(File.bfType),1,f);

    fwrite(&File.bfSize,sizeof(File.bfSize),1,f);

    fwrite(&File.bfReserved1,sizeof(File.bfReserved1),1,f);

    fwrite(&File.bfReserved2,sizeof(File.bfReserved2),1,f);

    fwrite(&File.bfOffBits,sizeof(File.bfOffBits),1,f);

    fseek(f,14,SEEK_SET);
    fwrite(&Picture.biSize,sizeof(Picture.biSize),1,f);

    fwrite(&Picture.biWidth,sizeof(Picture.biWidth),1,f);

    fwrite(&Picture.biHeight,sizeof(Picture.biHeight),1,f);

    fwrite(&Picture.biPlanes,sizeof(Picture.biPlanes),1,f);
    
    fwrite(&Picture.biBitCount,sizeof(Picture.biBitCount),1,f);

    fwrite(&Picture.biCompression,sizeof(Picture.biCompression),1,f);
     
    fwrite(&Picture.biSizeImage,sizeof(Picture.biSizeImage),1,f);
     
    fwrite(&Picture.biXPelsPerMeter,sizeof(Picture.biXPelsPerMeter),1,f);
     
    fwrite(&Picture.biYPelsPerMeter,sizeof(Picture.biYPelsPerMeter),1,f);
     
    fwrite(&Picture.biClrUsed,sizeof(Picture.biClrUsed),1,f);
     
    fwrite(&Picture.biClrImportant,sizeof(Picture.biClrImportant),1,f);

    int bmpImg;
    int j = 0;

     for (int i = File.bfOffBits; i < File.bfSize;i++) {
        bmpImg=0;
        fwrite(&arrOut[j].red, 1, 1, f);
        fwrite(&arrOut[j].green, 1, 1, f);
        fwrite(&arrOut[j].blue, 1, 1, f);
        j++;
     }

    fclose(f);
}

int main(void) {
    int column, row;
    FileHeader fileHeader;
	PictureHeader pictureHeader;
    Pixel* arr = GetDataFromFile(&column, &row, &fileHeader, &pictureHeader);
    Pixel* arrOut = MedianFilter(arr, column, row);
    SaveBitmap(arrOut, column, row, &fileHeader, &pictureHeader);
    
    return 0;
}


