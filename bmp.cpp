//windowsBitmapを扱うクラス
#include "bmp.h"

int bitToInteger(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4){
     return b1 + b2 * 256 + b3 * 256 * 256 + b4 * 256 * 256 * 256;
} 
void bmpProcessor::readFileHeader(){
    uint8_t data[FILE_HEADER_SIZE];  
    size_t size = fread(data, sizeof(uint8_t), FILE_HEADER_SIZE, bmp);  
    memcpy(fHeader.data, data, sizeof(data));  
    fHeader.fileType = "";  
    fHeader.fileType += data[0];  
    fHeader.fileType += data[1];  
    fHeader.fileSize = bitToInteger(data[2],data[3],data[4],data[5]);
}  
void bmpProcessor::readInfoHeader(){
    uint8_t data[INFO_HEADER_SIZE];  
    size_t size = fread(data, sizeof(uint8_t), INFO_HEADER_SIZE, bmp);  
    memcpy(iHeader.data, data, sizeof(data));
    iHeader.infoHeaderSize = bitToInteger(data[0],data[1],data[2],data[3]);
    iHeader.width          = bitToInteger(data[4],data[5],data[6],data[7]);
    iHeader.height         = bitToInteger(data[8],data[9],data[10],data[11]);
    iHeader.dataSize       = bitToInteger(data[20],data[21],data[22],data[23]);
}
void bmpProcessor::readBmpData(){
     if (img != NULL)  
        delete []img;  
    int sz = iHeader.dataSize;  
    img = new uint8_t [sz]; 
    size_t size = fread(img, sizeof(uint8_t), sz, bmp);  
    if (size != sz){
        printf("%d\n",img);
        printf("readBmpDate():Illegal size\n");
        return;  
    }
} 

void bmpProcessor::loadImage(string fileName){
    if(bmp!=null)
        fclose(bmp);
    bmp =fopen(fileName.c_str(),"rb");
    if(bmp==null)
        printf("loadImage():%s couldn't be opend\n",fileName.c_str());
    readFileHeader();
    readInfoHeader();
    readBmpData();
}

void bmpProcessor::saveImage(string fileName){
    FILE *out = fopen(fileName.c_str(), "wb");  
    if (out == NULL)  
        printf("saveImage():%s couldn't be opend\n",fileName.c_str());  
    fwrite(fHeader.data, sizeof(uint8_t), FILE_HEADER_SIZE, out);
    fwrite(iHeader.data, sizeof(uint8_t), INFO_HEADER_SIZE, out);  
    fwrite(img, sizeof(uint8_t), iHeader.dataSize, out);
    //printf("%d\n",iHeader.dataSize);
    fclose(out);  
}

vector<uint8_t> bmpProcessor::getColor(int row,int col){
    if (row < 0 || row >= iHeader.width)  
        printf("GetColor():row:%d %d\n",row,iHeader.width);
    if (col < 0 || col >= iHeader.height )  
        printf("GetColor():col:%d\n",col);  
    int width = 3 * iHeader.width;  
    while (width % 4)
        ++width;

    int bPos = col * width + 3 * row;  
    int gPos = bPos + 1;  
    int rPos = bPos + 2;   
    vector<uint8_t> color(3,0);  
    color[0] = img[rPos];
    color[1] = img[gPos];  
    color[2] = img[bPos];  
  
    return color;  
}
void bmpProcessor::setColor(int row,int col,int r,int g,int b){
    if (row < 0 || row >= iHeader.width)  
        printf("setColor(): row\n");  
    if (col < 0 || col >= iHeader.height )  
        printf("setColor(): col\n");  
  
    int width = 3 * iHeader.width;  
    while (width % 4)  
        ++width;
    
    int bPos = col * width + 3 * row;  
    int gPos = bPos + 1;  
    int rPos = bPos + 2;  
    img[rPos] = r;  
    img[gPos] = g;  
    img[bPos] = b; 
}
void bmpProcessor::setColor(int row,int col,int color){
    if (row < 0 || row >= iHeader.width)  
        printf("setColor2(): row\n");  
    if (col < 0 || col >= iHeader.height )  
        printf("setColor2(): col\n");  
  
    int width = 3 * iHeader.width;  
    
    while (width % 4) 
        width++;
    //printf(" %d ",width);
    
    
    int bPos = col * width + 3 * row;  
    int gPos = bPos +1;  
    int rPos = bPos +2;  
    img[rPos] = color;  
    img[gPos] = color;  
    img[bPos] = color; 
}
void bmpProcessor::setColor(int row,int col,vector<uint8_t>color){
    if (row < 0 || row >= iHeader.width)  
        printf("setColor(): row\n");  
    if (col < 0 || col >= iHeader.height )  
        printf("setColor(): col\n");  
  
    int width = 3 * iHeader.width;  
    while (width % 4)  
        ++width;
    
    int bPos = col * width + 3 * row;  
    int gPos = bPos + 1;  
    int rPos = bPos + 2;  
    img[rPos] = color[0];  
    img[gPos] = color[1];  
    img[bPos] = color[2]; 
}