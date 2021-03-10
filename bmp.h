#ifndef bmp_H 
#define bmp_H

#include <stdio.h>
#include <memory.h>
#include <string>
#include <vector>


#define FILE_HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
#define null NULL
using std::string;
using std::vector;


typedef struct FileHeader{
    uint8_t data[FILE_HEADER_SIZE];
    string fileType;
    int fileSize;
} FileHeader;

typedef struct InfoHeader{
    uint8_t data[INFO_HEADER_SIZE];
    int infoHeaderSize;
    int width,height;
    int dataSize;
} InfoHeader;

int bitToInteger(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4); 

class bmpProcessor{
private:
    FILE *bmp;
    uint8_t *img;
    FileHeader fHeader;
    InfoHeader iHeader;
    void readFileHeader();  
    void readInfoHeader();  
    void readBmpData(); 
public:
    bmpProcessor(){
        bmp=null;
        img=null;
    };
    ~bmpProcessor(){
        if(bmp!=null)
            fclose(bmp);
        delete []img;
    }
    void loadImage(string fileName);
    void saveImage(string newFileName);
    vector<uint8_t> getColor(int row,int col);//r,g,b
    void setColor(int row,int col,int r,int g,int b);
    void setColor(int row,int col,int color);
    void setColor(int row,int col,vector<uint8_t> color);
    int getHeight(){return iHeader.height;}
    int getWidth(){return iHeader.width;}
};

#endif