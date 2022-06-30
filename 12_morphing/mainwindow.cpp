#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    width = ui->frameLeft->width();
    height = ui->frameLeft->height();
    begLX = ui->frameLeft->x();
    begLY = ui->frameLeft->y();
    begRX = ui->frameRight->x();
    begRY = ui->frameRight->y();
    begWX = ui->frameResult->x();
    begWY = ui->frameResult->y();

    N = ui->framesSpinBox->value();

    imgL = new QImage(width, height, QImage::Format_RGB32);
    imgR = new QImage(width, height, QImage::Format_RGB32);
    imgW = new QImage(width, height, QImage::Format_RGB32);
    imgLcp = new QImage(width, height, QImage::Format_RGB32);
    imgRcp = new QImage(width, height, QImage::Format_RGB32);

    isTriangleEnabledL = isTriangleEnabledR = true;
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *){
    QPainter p(this);
    p.drawImage(begLX, begLY, *imgL);
    p.drawImage(begRX, begRY, *imgR);
    p.drawImage(begWX, begWY, *imgW);
}

void MainWindow::on_exitButton_clicked(){
    qApp->quit();
}

void MainWindow::mousePressEvent(QMouseEvent *event){

    int Lx = event->position().x() - begLX;
    int Ly = event->position().y() - begLY;

    int Rx = event->position().x() - begRX;
    int Ry = event->position().y() - begRY;

    QPair<int,int> p;

    // rysowanie trojkata na pierwszym (lewym) obrazie
    if(Lx>0 && Lx<width && Ly>0 && Ly<height && isTriangleEnabledL){
        p.first = Lx;
        p.second = Ly;
        vertL.push_back(p);

        paintAround(imgL, Lx, Ly);

        for(int i=0; i<vertL.size()-1; i++){
            drawLine(imgL, vertL[i].first, vertL[i].second, vertL[i+1].first, vertL[i+1].second);
            if(vertL.size() == 3) {
                drawLine(imgL, vertL[2].first, vertL[2].second, vertL[0].first, vertL[0].second);
                isTriangleEnabledL = false;
            }
        }
    }

    // rysowanie trojkata na drugim (prawym) obrazie
    if(Rx>0 && Rx<width && Ry>0 && Ry<height && isTriangleEnabledR){
        p.first = Rx;
        p.second = Ry;
        vertR.push_back(p);

        paintAround(imgR, Rx, Ry);

        for(int i=0; i<vertR.size()-1; i++){
            drawLine(imgR, vertR[i].first, vertR[i].second, vertR[i+1].first, vertR[i+1].second);
            if(vertR.size() == 3) {
                drawLine(imgR, vertR[2].first, vertR[2].second, vertR[0].first, vertR[0].second);
                isTriangleEnabledR = false;
            }
        }
    }

}

// minOrMax == 1 -> min, minOrMax == 2 -> max
// which == 1 -> wspolrzedna x, which == 2 -> wspolrzedna y
int MainWindow::getVert(QVector<QPair<float,float>> v, int minOrMax, int which){
    QList<int> l;

    if(which == 1){
        l << v[0].first << v[1].first << v[2].first;
        std::sort(l.begin(), l.end());
        if(minOrMax == 1) return l[0];
        if(minOrMax == 2) return l[2];
    }

    if(which == 2){
        l << v[0].second << v[1].second << v[2].second;
        std::sort(l.begin(), l.end());
        if(minOrMax == 1) return l[0];
        if(minOrMax == 2) return l[2];
    }
}

void MainWindow::texturing(QImage *im, QVector<QPair<float,float>> src, QImage *srcImg, QVector<QPair<float,float>> dst){
    double u, v, w, xT, yT;

    long W = (dst[1].first-dst[0].first)*(dst[2].second-dst[0].second) - (dst[2].first-dst[0].first)*(dst[1].second-dst[0].second);
    long Wv, Ww;

    int minX = getVert(dst, 1, 1);
    int minY = getVert(dst, 1, 2);

    int maxX = getVert(dst, 2, 1);
    int maxY = getVert(dst, 2, 2);

    for(int i=minX; i<=maxX; i++){
        for(int j=minY; j<=maxY; j++){
            Wv = (i-dst[0].first)*(dst[2].second-dst[0].second) - (dst[2].first-dst[0].first)*(j-dst[0].second);
            Ww = (dst[1].first-dst[0].first)*(j-dst[0].second) - (i-dst[0].first)*(dst[1].second-dst[0].second);

            v = (double)Wv/(double)W;
            w = (double)Ww/(double)W;
            u = 1.0-v-w;

            //jesli punkt lezy wewnatrz trojkata
            if(u>0. && u<1. && v>0. && v<1. && w>0. && w<1.){
                xT = u*src[0].first + v*src[1].first + w*src[2].first;
                yT = u*src[0].second + v*src[1].second + w*src[2].second;
                paintPixel(im, i, j, bilinearInterpolation(srcImg, xT, yT));
            }
        }
    }
}

void MainWindow::blend(int n, QImage *im, QImage* A, QImage *B){

    unsigned char *a, *b, *ii;
    a = A->bits();
    b = B->bits();
    ii = im->bits();

    long pos;

    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){

            pos = 4*j*width + 4*i;

            ii[pos] = (int)((1.0-(float)n/N) * a[pos] + (float)n/N * b[pos]);
            ii[pos+1] = (int)((1.0-(float)n/N) * a[pos+1] + (float)n/N * b[pos+1]);
            ii[pos+2] = (int)((1.0-(float)n/N) * a[pos+2] + (float)n/N * b[pos+2]);
        }
    }

    repaint();
}

void MainWindow::morphing(){

    QImage *P1, *P2;
    P1 = new QImage(width, height, QImage::Format_RGB32);
    P2 = new QImage(width, height, QImage::Format_RGB32);

    for(int n=0; n<=N; n++){

        clearImage(P1);
        clearImage(P2);

        // wspolrzedne trojkata "posredniego"
        QVector<QPair<float,float>> t;
        QPair<int,int> p;

        p.first = (1.0-(float)n/N)*vertL[0].first + (float)n/N*vertR[0].first;
        p.second = (1.0-(float)n/N)*vertL[0].second + (float)n/N*vertR[0].second;
        t.push_back(p);

        p.first = (1.0-(float)n/N)*vertL[1].first + (float)n/N*vertR[1].first;
        p.second = (1.0-(float)n/N)*vertL[1].second + (float)n/N*vertR[1].second;
        t.push_back(p);

        p.first = (1.0-(float)n/N)*vertL[2].first + (float)n/N*vertR[2].first;
        p.second = (1.0-(float)n/N)*vertL[2].second + (float)n/N*vertR[2].second;
        t.push_back(p);

        // wklej teksture trojkata vertL (przchowywana w imgLcp) do trojkata t i zapisz wynik do obrazu P1
        texturing(P1, vertL, imgLcp, t);
        texturing(P2, vertR, imgRcp, t);

        // zmieszaj trojkaty o wspolrzednych t z obrazkow P1 i P2 i zapisz wynik do imgW
        blend(n, imgW, P1, P2);
    }
}

QColor MainWindow::bilinearInterpolation(QImage *im, double x, double y){
    QColor c;

    int Px = (int)x, Py = (int)y;
    double a = x-Px, b = y-Py;
    int value;

    unsigned char *ptr;
    ptr = im->bits();

    value = b*((1.-a)*ptr[width*4*Py + 4*Px] + a*ptr[width*4*Py + 4*(Px+1)])
            + (1.-b)*((1.-a)*ptr[width*4*(Py-1) + 4*Px] + a*ptr[width*4*(Py-1) + 4*(Px+1)]);
    c.setBlue(value);

    value = b*((1.-a)*ptr[width*4*Py + 4*Px + 1] + a*ptr[width*4*Py + 4*(Px+1) + 1])
            + (1.-b)*((1.-a)*ptr[width*4*(Py-1) + 4*Px + 1] + a*ptr[width*4*(Py-1) + 4*(Px+1) + 1]);
    c.setGreen(value);

    value = b*((1.-a)*ptr[width*4*Py + 4*Px + 2] + a*ptr[width*4*Py + 4*(Px+1) + 2])
            + (1.-b)*((1.-a)*ptr[width*4*(Py-1) + 4*Px + 2] + a*ptr[width*4*(Py-1) + 4*(Px+1) + 2]);
    c.setRed(value);

    return c;
}

void MainWindow::paintPixel(QImage *im, int x, int y, QColor c){
    unsigned char *ptr;
    ptr = im->bits();

    ptr[width*4*y + 4*x]     = c.blue();
    ptr[width*4*y + 4*x + 1] = c.green();
    ptr[width*4*y + 4*x + 2] = c.red();
}

void MainWindow::paintAround(QImage *im, int x, int y){
    int i, j;
    for(i=-3; i<=3; i++){
        for(j=-3; j<=3; j++){
            paintPixel(im, x+i, y+j, Qt::red);
        }
    }
    update();
}

void MainWindow::clearImage(QImage *im){
    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            paintPixel(im, i, j, Qt::black);
        }
    }
}

void MainWindow::drawLine(QImage *im, int x0, int y0, int x1, int y1){
    int i;
    double a = (double)(y1-y0)/(x1-x0);
    double b = (double)y0 - a*x0;

    // obsluga rysowania pionowych kresek (kat nachylenia = 90)
    if(x0 == x1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            paintPixel(im, x0, i, Qt::red);
        }
        return;
    }

    // jesli kat nachylenia jest z przedzialu [0,45]
    if(a>=0 && a<=1){
        if(x0>x1) std::swap(x0, x1);
        for(i=x0; i<=x1; i++){
            double y = (double)a*i + b;
            paintPixel(im, i, (int)round(y), Qt::red);
        }
    }

    // jesli kat nachylenia jest z przedzialu [45,90)
    else if(a>1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            double x = (double)(i-b)/a;
            paintPixel(im, (int)round(x), i, Qt::red);
        }
    }

    // jesli kat nachylenia jest z przedzialu (90, 135]
    else if(a<=-1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            double x = (double)(i-b)/a;
            paintPixel(im, (int)round(x), i, Qt::red);
        }
    }

    // jesli kat nachylenia jest z przedzialu (135, 180)
    else{
        if(x0>x1) std::swap(x0, x1);
        for(i=x0; i<=x1; i++){
            double y = (double)a*i + b;
            paintPixel(im, i, (int)round(y), Qt::red);
        }
    }
}

void MainWindow::on_imgLeftButton_clicked()
{
    filePath = QFileDialog::getOpenFileName(this, tr("Wczytaj obraz..."), "..\\zad12_morphing\\img", tr("Pliki graficzne (*.png *.jpg *.bmp)"));
    if(!filePath.isEmpty()){
        imgL = new QImage(filePath);
        *imgLcp = *imgL;
    }
}


void MainWindow::on_imgRightButton_clicked()
{
    filePath = QFileDialog::getOpenFileName(this, tr("Wczytaj obraz..."), "..\\zad12_morphing\\img", tr("Pliki graficzne (*.png *.jpg *.bmp)"));
    if(!filePath.isEmpty()){
        imgR = new QImage(filePath);
        *imgRcp = *imgR;
    }
}


void MainWindow::on_resetLButton_clicked()
{
    *imgL = *imgLcp;
    imgW = new QImage(width, height, QImage::Format_RGB32);
    update();
    vertL.clear();
    isTriangleEnabledL = true;
}


void MainWindow::on_resetRButton_clicked()
{
    *imgR = *imgRcp;
    imgW = new QImage(width, height, QImage::Format_RGB32);
    update();
    vertR.clear();
    isTriangleEnabledR = true;
}


void MainWindow::on_resetButton_clicked()
{
    *imgL = *imgLcp;
    *imgR = *imgRcp;
    imgW = new QImage(width, height, QImage::Format_RGB32);
    vertL.clear();
    vertR.clear();
    isTriangleEnabledL = isTriangleEnabledR = true;
    ui->framesSpinBox->setValue(30);
    N = ui->framesSpinBox->value();

    update();
}


void MainWindow::on_startButton_clicked()
{
    if(!isTriangleEnabledL && !isTriangleEnabledR){
        morphing();
    }
}


void MainWindow::on_framesSpinBox_valueChanged(int arg1)
{
    N = arg1;
}

