#include "bmp.h"

void grayScale(bmpProcessor *obj){
    for(int i = 0;i<obj->getHeight();i++){
        for(int j = 0;j<obj->getWidth();j++){
            int gray = 0;
            vector<uint8_t> color = obj->getColor(i,j);
            gray = 0.3*color[0] + 0.59*color[1] + 0.11*color[2];
            obj->setColor(i,j,gray);
        }
    }
    printf("gray\n");
}

void binarize(bmpProcessor *obj,bmpProcessor *gray){
    int threshold=0;
    double sum=0;
    for(int i = 0;i<obj->getHeight();i++){
        for(int j = 0;j<obj->getWidth();j++){
            vector<uint8_t> grayColor = gray->getColor(i,j);
            sum += grayColor[0];
        }
    }
    
    threshold=sum/(obj->getHeight()*obj->getWidth());
    for(int i = 0;i<obj->getHeight();i++){
        for(int j = 0;j<obj->getWidth();j++){
            vector<uint8_t> grayColor = gray->getColor(i,j);
            if(grayColor[0]<threshold){
                obj->setColor(i,j,0);
            }else{
                obj->setColor(i,j,255);
            }
        }
    }
    printf("binary : %d\n",threshold);
}
void binarize_otsu(bmpProcessor *obj,bmpProcessor *gray){
    int threshold = 0;
    double max = 0;//「"クラス１の画素数"×"クラス２の画素数"×("クラス１の平均"-"クラス２の平均")^2」の最大値
    vector<int> agg(256,0);//輝度値の集計
    for(int i = 0;i < gray->getHeight(); i++){
        for(int j = 0;j < gray->getWidth(); j++){
            agg[gray->getColor(i,j)[0]]++;
        }
    }
    
    for(int i=0;i<256;i++){
        int p1=0,p2=0;//pixel of class1,pixel of class2
        long sum1=0,sum2=0;//SUM(class1),SUM(class2)
        double ave1=0.0,ave2=0.0;//average(class1),average(class2)

        for(int j=0;j<=i;j++){
            p1 += agg[j];
            sum1 += j*agg[j];
        }
        for(int j=i+1;j<256;j++){
            p2 += agg[j];
            sum2 += j*agg[j];
        }
        if(p1!=0)
            ave1 = (double)sum1/p1;
        if(p2!=0)
            ave2 = (double)sum2/p2;

        double inst = ((double)p1*p2*(ave1-ave2)*(ave1-ave2));
        if(inst > max){
            max = inst;
            threshold = i;
        }
    }
    for(int i = 0;i<obj->getHeight();i++){
        for(int j = 0;j<obj->getWidth();j++){
            vector<uint8_t> grayColor = gray->getColor(i,j);
            if(grayColor[0]<threshold){
                obj->setColor(i,j,0);
            }else{
                obj->setColor(i,j,255);
            }
        }
    }
    printf("binary Otsu : %d\n",threshold);
}

int main(int argc,char* argv[]){
    string fileName = argv[1];
    string baseFileName = (string)argv[1];
    string grayFileName = baseFileName.insert(baseFileName.find("."),"_g");
    string binaryFilename = baseFileName.replace(baseFileName.find("_"),2,"_b");
    string otsuBinaryFilename = baseFileName.insert(baseFileName.find("_b")+2,"_otsu");
    bmpProcessor gray,threthold,src;

    gray.loadImage(fileName);
    threthold.loadImage(fileName);
    src.loadImage(fileName);
    grayScale(&gray);
    gray.saveImage(grayFileName);
    binarize(&threthold,&gray);
    threthold.saveImage(binaryFilename);
    binarize_otsu(&threthold,&gray);
    threthold.saveImage(otsuBinaryFilename);
    return 0;
}