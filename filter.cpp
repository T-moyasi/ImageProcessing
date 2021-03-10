#define _USE_MATH_DEFINES
#include "bmp.h"
#include <cmath>
using std::exp;


void grayScale(bmpProcessor *obj){
    for(int i = 0;i<obj->getWidth();i++){
        for(int j = 0;j<obj->getHeight();j++){
            int gray = 0;
            vector<uint8_t> color = obj->getColor(i,j);
            gray = 0.3*color[0] + 0.59*color[1] + 0.11*color[2];
            obj->setColor(i,j,gray);
        }
    }
    printf("gray\n");
}

void binarization_Otsu(bmpProcessor *obj,bmpProcessor *gray){
    int threshold = 0;
    double max = 0;
    vector<int> agg(256,0);
    for(int i = 0;i < gray->getWidth(); i++){
        for(int j = 0;j < gray->getHeight(); j++){
            agg[gray->getColor(i,j)[0]]++;
        }
    }
    
    for(int i=0;i<256;i++){
        int p1=0,p2=0;
        long sum1=0,sum2=0;
        double ave1=0.0,ave2=0.0;

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
    for(int i = 0;i<obj->getWidth();i++){
        for(int j = 0;j<obj->getHeight();j++){
            vector<uint8_t> grayColor = gray->getColor(i,j);
            if(grayColor[0]<threshold){
                obj->setColor(i,j,0);
            }else{
                obj->setColor(i,j,255);
            }
        }
    }
    printf("threshold Otsu : %d\n",threshold);
}

void meanFilter(bmpProcessor *obj,bmpProcessor *base){
    vector<vector<uint8_t> > color(9);
    vector<uint8_t> rgb;
    int r,g,b;
    //printf("W%d H%d\n",base->getWidth(),base->getHeight());
    for(int row=0;row<base->getWidth();row++){
        for(int col=0;col<base->getHeight();col++){
            r=g=b=0;
            if(col==0||row==0||row==base->getWidth()-1||col==base->getHeight()-1){
                obj->setColor(row,col, base->getColor(row,col));
            }else{
                for(int k=row-1,m=0;k<=row+1;k++){
                    for(int l=col-1,n=0;l<=col+1;l++){
                        color[m*3+n]=base->getColor(k,l);
                        n++;
                    }
                    m++;
                }
                for(int k=0;k<9;k++){
                    rgb=color[k];
                    r+=rgb[0];
                    g+=rgb[1];
                    b+=rgb[2];
                }
                obj->setColor(row,col,r/9,g/9,b/9);
            }
        }
    }
    printf("mean\n");
}

void medianFilter(bmpProcessor *obj,bmpProcessor *base){
    vector<vector<uint8_t> > color(9);
    vector<uint8_t> rgb;
    int r,g,b;
    for(int row=0;row<base->getWidth();row++){
        for(int col=0;col<base->getHeight();col++){
            r=g=b=0;
            if(col==0||row==0||row==base->getWidth()-1||col==base->getHeight()-1){
                obj->setColor(row,col, base->getColor(row,col));
            }else{
                for(int k=row-1,m=0;k<=row+1;k++){
                    for(int l=col-1,n=0;l<=col+1;l++){
                        color[m*3+n]=base->getColor(k,l);
                        n++;
                    }
                    m++;
                }
                for(int k=8;k<=1;k--){
                    int max=0;
                    for(int l=1;l<k;l++){
                        if(color[max]<color[l]){
                            max=l;
                        }
                    }
                    rgb=color[max];
                    color[max]=color[k];
                    color[k]=rgb;                    
                }
                rgb=color[4];
                r=rgb[0];
                g=rgb[1];
                b=rgb[2];
                obj->setColor(row,col,r,g,b);
            }
        }
    }
    printf("median\n");
}
void gaussianFilter(bmpProcessor *obj,bmpProcessor *base,int filterSize,int sigma){
    vector<vector<uint8_t> > color(filterSize*filterSize);
    vector<float> karnel(filterSize*filterSize);
    float r,g,b;
    float gauseSum=0.0f;
    int hSize=filterSize/2;
    //カーネル作成
    for(int y=0;y<=filterSize;y++){
        for(int x=0;x<=filterSize;x++){
            karnel[y*3+x] = (float)exp(-((x * x + y * y) / (2 * sigma * sigma))) / (2 * M_PI * sigma * sigma);
            //printf("%d %lf\n",y*3+x,karnel[y*3+x]);
            gauseSum+=karnel[y*3+x];
        }
    }
    for(int i=0;i<filterSize*filterSize;i++){
        karnel[i]=karnel[i]/gauseSum;
        //printf("%lf ",karnel[i]);
    }

    //フィルタリング
    for(int row=0;row<base->getWidth();row++){
        for(int col=0;col<base->getHeight();col++){
            r=g=b=0;
            if(col<hSize||row<hSize||row>=base->getWidth()-hSize||col>=base->getHeight()-hSize){
                obj->setColor(row,col, base->getColor(row,col));
            }else{
                int m2,n2;
                for(int k=row-hSize,m=0;k<=row+hSize;k++){
                    for(int l=col-hSize,n=0;l<=col+hSize;l++){                        
                        color[m*3+n]=base->getColor(k,l);
                        r+=color[m*3+n][0]*karnel[m*3+n];
                        g+=color[m*3+n][1]*karnel[m*3+n];
                        b+=color[m*3+n][2]*karnel[m*3+n];
                        n++;
                        n2=n;
                    }
                    m++;
                    m2=m;
                }
                m2--;
                obj->setColor(row,col,r,g,b);
            }
        }
    }
    
    printf("gaussian\n");
}
void sobelFilter(bmpProcessor *obj,bmpProcessor *gray,int mode){
    vector<vector<uint8_t> > color(9);
    uint8_t out=0;
    int temp,temp2;
    if(mode==0){
        //横
        for(int row=0;row<gray->getWidth();row++){
            for(int col=0;col<gray->getHeight();col++){
                if(col==0||row==0||row==gray->getWidth()-1||col==gray->getHeight()-1){
                    obj->setColor(row,col, 0);
                }else{
                    for(int k=row-1,m=0;k<=row+1;k++){                                           
                        for(int l=col-1,n=0;l<=col+1;l++){ 
                            color[m*3+n]=gray->getColor(k,l);
                            n++;
                        }
                        m++;
                    }
                    temp=-1*color[0][0]+(-2)*color[3][0]+(-1)*color[6][0]
                         +1*color[2][0]+2*color[5][0]+1*color[8][0];
                    //temp/=4;
                    if(temp>=255)
                        temp=255;
                    if(temp<0)
                        temp=0;
                    out=temp;
                    if(out>255)
                        out=255;
                    obj->setColor(row,col,(uint8_t)out);
                }
            }
        }
    }else if(mode==1){
        //縦
        for(int row=0;row<gray->getWidth();row++){
            for(int col=0;col<gray->getHeight();col++){
                if(col==0||row==0||row==gray->getWidth()-1||col==gray->getHeight()-1){
                    obj->setColor(row,col, 0);
                }else{
                    for(int l=col-1,n=0;l<=col+1;l++){ 
                        for(int k=row-1,m=0;k<=row+1;k++){                                           
                            color[m*3+n]=gray->getColor(k,l);
                            m++;
                        }
                        n++;
                    }
                    temp=-1*color[0][0]+(-2)*color[1][0]+(-1)*color[2][0]
                         +1*color[6][0]+2*color[7][0]+1*color[8][0];
                    //temp/=4;
                    if(temp>=255)
                        temp=255;
                    if(temp<0)
                        temp=0;
                    out=temp;
                    obj->setColor(row,col,(uint8_t)out);
                }
            }
        }
    }else{
        //縦横合成
        mode = 2;
                for(int row=0;row<gray->getWidth();row++){
            for(int col=0;col<gray->getHeight();col++){
                if(col==0||row==0||row==gray->getWidth()-1||col==gray->getHeight()-1){
                    obj->setColor(row,col, 0);
                }else{
                    for(int l=col-1,n=0;l<=col+1;l++){ 
                        for(int k=row-1,m=0;k<=row+1;k++){                                           
                            color[m*3+n]=gray->getColor(k,l);
                            m++;
                        }
                        n++;
                    }
                    temp=-1*color[0][0]+(-2)*color[1][0]+(-1)*color[2][0]
                         +1*color[6][0]+2*color[7][0]+1*color[8][0];
                    temp2=-1*color[0][0]+(-2)*color[3][0]+(-1)*color[6][0]
                         +1*color[2][0]+2*color[5][0]+1*color[8][0];
                    //temp/=4;
                    if(temp>255)
                        temp=255;
                    if(temp<0)
                        temp=0;
                    if(temp2>255)
                        temp2=255;
                    if(temp2<0)
                        temp2=0;
                    out=temp+temp2;
                    obj->setColor(row,col,(uint8_t)out);
                }
            }
        }
    }
    printf("sobel mode:%d\n",mode);
}
void prewittFilter(bmpProcessor *obj,bmpProcessor *gray,int mode){
        vector<vector<uint8_t> > color(9);
    uint8_t out=0;
    int temp,temp2;
    if(mode==0){
        //横
        for(int row=0;row<gray->getWidth();row++){
            for(int col=0;col<gray->getHeight();col++){
                if(col==0||row==0||row==gray->getWidth()-1||col==gray->getHeight()-1){
                    obj->setColor(row,col, 0);
                }else{
                    for(int k=row-1,m=0;k<=row+1;k++){                                           
                        for(int l=col-1,n=0;l<=col+1;l++){ 
                            color[m*3+n]=gray->getColor(k,l);
                            n++;
                        }
                        m++;
                    }
                    temp=-1*color[0][0]+(-1)*color[3][0]+(-1)*color[6][0]
                         +1*color[2][0]+1*color[5][0]+1*color[8][0];
                    //temp/=4;
                    if(temp>=255)
                        temp=255;
                    if(temp<0)
                        temp=0;
                    out=temp;
                    obj->setColor(row,col,(uint8_t)out);
                }
            }
        }
    }else if(mode==1){
        //縦
        for(int row=0;row<gray->getWidth();row++){
            for(int col=0;col<gray->getHeight();col++){
                if(col==0||row==0||row==gray->getWidth()-1||col==gray->getHeight()-1){
                    obj->setColor(row,col, 0);
                }else{
                    for(int l=col-1,n=0;l<=col+1;l++){ 
                        for(int k=row-1,m=0;k<=row+1;k++){                                           
                            color[m*3+n]=gray->getColor(k,l);
                            m++;
                        }
                        n++;
                    }
                    temp=-1*color[0][0]+(-1)*color[1][0]+(-1)*color[2][0]
                         +1*color[6][0]+1*color[7][0]+1*color[8][0];
                    //temp/=4;
                    if(temp>=255)
                        temp=255;
                    if(temp<0)
                        temp=0;
                    out=temp;
                    obj->setColor(row,col,(uint8_t)out);
                }
            }
        }
    }else{
        //縦横合成
        mode = 2;
                for(int row=0;row<gray->getWidth();row++){
            for(int col=0;col<gray->getHeight();col++){
                if(col==0||row==0||row==gray->getWidth()-1||col==gray->getHeight()-1){
                    obj->setColor(row,col, 0);
                }else{
                    for(int l=col-1,n=0;l<=col+1;l++){ 
                        for(int k=row-1,m=0;k<=row+1;k++){                                           
                            color[m*3+n]=gray->getColor(k,l);
                            m++;
                        }
                        n++;
                    }
                    temp=-1*color[0][0]+(-1)*color[1][0]+(-1)*color[2][0]
                         +1*color[6][0]+1*color[7][0]+1*color[8][0];
                    temp2=-1*color[0][0]+(-1)*color[3][0]+(-1)*color[6][0]
                         +1*color[2][0]+1*color[5][0]+1*color[8][0];
                    //temp/=4;
                    if(temp>255)
                        temp=255;
                    if(temp<0)
                        temp=0;
                    if(temp2>255)
                        temp2=255;
                    if(temp2<0)
                        temp2=0;
                    out=temp+temp2;
                    obj->setColor(row,col,(uint8_t)out);
                }
            }
        }
    }
    printf("prewitt mode:%d\n",mode);
}
void cannyEdgeFilter(){
    //5*5のガウシアンかけてノイズ除去
    //ソーベル縦横＋角度から方向決定
    //非極大値の抑制
    //画素値の微分値が maxVal 以上であれば正しいエッジとみなし，
    //minVal 以下の値であればエッジではないとみなし除外します．
    //微分値が二つのしきい値の間であれば，
    //正しいエッジとそうでないエッジとの隣接関係を基に区別します．
    //正しいエッジと区別された画素につながっていれば正しいエッジとみなし，そうでなければエッジではない画素とみなします．
}
void Dilation(bmpProcessor *obj,bmpProcessor *binary){
    for(int row=1;row<binary->getWidth()-1;row++){
        for(int col=1;col<binary->getHeight()-1;col++){
            for(int l=col-1,n=0;l<=col+1;l++){ 
                for(int k=row-1,m=0;k<=row+1;k++){                                           
                    if(binary->getColor(k,l)[0]==255){
                        if(obj->getColor(k,l)[0]!=255)
                            obj->setColor(k,l,255);   
                    }
                }
            }
        }
    }
    //printf("dilation\n");
}
void Erosion(bmpProcessor *obj,bmpProcessor *binary){
    //printf("%d\n",binary->getWidth());
    for(int row=1;row<binary->getWidth()-1;row++){
        for(int col=1;col<binary->getHeight()-1;col++){
            for(int l=col-1;l<=col+1;l++){ 
                for(int k=row-1;k<=row+1;k++){
                    if(l==col&&k==row)
                        continue;                                           
                    if(binary->getColor(k,l)[0]==0){
                        //printf("%d",k);
                        obj->setColor(k,l,0);
                        
                        //printf("%d,%d\n",k,l);
                    }
                }
            }
        }
    }
    printf("erosion\n");
}
void Opening(bmpProcessor *obj,bmpProcessor *binary, int n){
    bmpProcessor inst=*binary;
    if(n<=0){
        return;        
    }
    for(int i=0;i<n;i++){
        Erosion(obj,&inst);
        inst=*obj;
    }
    /*
    for(int i=0;i<n;i++){
        Dilation(obj , &inst);
        inst=*obj;
    }
    */
    printf("opening %d\n",n);
}
void Closing(bmpProcessor obj,bmpProcessor binary,bmpProcessor inst,int n){
    inst=binary;
    if(n<=0){
        return;        
    }
    for(int i=0;i<n;i++){
       Dilation(&obj , &inst);
       inst=obj;
    }
    for(int i=0;i<n;i++){
        Erosion(&obj,&inst);
        inst = obj;
    }
    printf("closing %d\n",n);
}

int main(int argc,char* argv[]){
    string fileName = argv[1];
    string baseFileName = (string)argv[1];
    string grayFileName = baseFileName.insert(baseFileName.find("."),"_g");
    string binaryFilename = baseFileName.replace(baseFileName.find("_"),2,"_b");
    string otsuBinaryFilename = baseFileName.insert(baseFileName.find("_b")+2,"_otsu");
    baseFileName=fileName;
    string meanFileName = baseFileName.insert(baseFileName.find("."),"_mean");
    baseFileName=fileName;
    string medianFileName = baseFileName.insert(baseFileName.find("."),"_median");
    baseFileName=fileName;
    string gaussianFileName = baseFileName.insert(baseFileName.find("."),"_gaussian");
    baseFileName=fileName;
    string sobelHFileName = baseFileName.insert(baseFileName.find("."),"_sobel_H");
    baseFileName=fileName;
    string sobelWFileName = baseFileName.insert(baseFileName.find("."),"_sobel_W");
    baseFileName=fileName;
    string sobelFileName = baseFileName.insert(baseFileName.find("."),"_sobel");
    baseFileName=fileName;
    string prewittHFileName = baseFileName.insert(baseFileName.find("."),"_prewitt_H");
    baseFileName=fileName;
    string prewittWFileName = baseFileName.insert(baseFileName.find("."),"_prewitt_W");
    baseFileName=fileName;
    string prewittFileName = baseFileName.insert(baseFileName.find("."),"_prewitt");
    baseFileName=fileName;
    string erosionFileName = baseFileName.insert(baseFileName.find("."),"_erosion");
    baseFileName=fileName;
    string dilationFileName = baseFileName.insert(baseFileName.find("."),"_dilation");
    baseFileName=fileName;
    string openFileName = baseFileName.insert(baseFileName.find("."),"_opening");
    baseFileName=fileName;
    string closeFileName = baseFileName.insert(baseFileName.find("."),"_closing");
    baseFileName=fileName;
    string labelFileName = baseFileName.insert(baseFileName.find("."),"_labeling");
    
    bmpProcessor gray,threthold,median,mean,gaussian,src,
                 sobel_H,sobel_W,sobel,prewitt,prewitt_H,prewitt_W,
                 inst,erosion,dilation,opening,closing,labeling;

    gray.loadImage(fileName);
    threthold.loadImage(fileName);
    //median.loadImage(fileName);
    //mean.loadImage(fileName);
    //gaussian.loadImage(fileName);
    //sobel_H.loadImage(fileName);
    //sobel_W.loadImage(fileName);
    //sobel.loadImage(fileName);
    //prewitt_H.loadImage(fileName);
    //prewitt_W.loadImage(fileName);
    //prewitt.loadImage(fileName);
    src.loadImage(fileName);
    erosion.loadImage(fileName);
    dilation.loadImage(fileName);
    opening.loadImage(fileName);
    closing.loadImage(fileName);
    labeling.loadImage(fileName);
    inst.loadImage(fileName);
    /*大津法*/
    //grayScale(&gray);
    //gray.saveImage(grayFileName);
    //binarization_Otsu(&threthold,&gray);
    //threthold.saveImage(otsuBinaryFilename);
    /*
    //平均化フィルタ
    meanFilter(&mean,&src);
    mean.saveImage(meanFileName);

    //メディアンフィルタ
    medianFilter(&median,&src);
    median.saveImage(medianFileName);

    //ガウシアンフィルタ
    gaussianFilter(&gaussian,&src,3,1);
    gaussian.saveImage(gaussianFileName);
    
    //ソーベルフィルタ
    sobelFilter(&sobel_W,&gray,0);
    sobel_W.saveImage(sobelWFileName);
    sobelFilter(&sobel_H,&gray,1);
    sobel_H.saveImage(sobelHFileName);
    sobelFilter(&sobel,&gray,2);
    sobel.saveImage(sobelFileName);
    //プレヴィットフィルタ
    prewittFilter(&prewitt_W,&gray,0);
    prewitt_W.saveImage(prewittWFileName);
    prewittFilter(&prewitt_H,&gray,1);
    prewitt_H.saveImage(prewittHFileName);
    prewittFilter(&prewitt,&gray,2);
    prewitt.saveImage(prewittFileName);
    */
    //
    //erosion=threthold;
    //dilation=threthold;
    //opening=threthold;
    //closing=threthold;
    //labeling=threthold;      
    
    Erosion(&erosion,&threthold);
    //Erosion(&erosion,&erosion);
    //Erosion(&erosion,&erosion);
    //Erosion(&erosion,&erosion);
    //Erosion(&erosion,&erosion);
    //Erosion(&erosion,&erosion);
    erosion.saveImage(erosionFileName);
    //Dilation(&dilation,&threthold);
    //dilation.saveImage(dilationFileName);
    //Opening(&opening,&threthold,3);
    //opening.saveImage(openFileName);
    //Closing(closing,threthold,inst,10);
    //closing.saveImage(closeFileName);
    
    return 0;
}