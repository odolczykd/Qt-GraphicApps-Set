#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    width = ui->frame->width();
    height = ui->frame->height();
    begX = ui->frame->x();
    begY = ui->frame->y();

    maskSize = ui->maskSizeSpinBox->value();

    img = new QImage(width, height, QImage::Format_RGB32);
    imgCopy = new QImage(width, height, QImage::Format_RGB32);
    orig = new QImage(width, height, QImage::Format_RGB32);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *){
    QPainter p(this);
    p.drawImage(begX, begY, *img);
}


void MainWindow::on_exitButton_clicked(){
    qApp->quit();
}


QImage* MainWindow::RGBtoGrey(QImage* im){
    int i, j, pos;
    unsigned char *ptr, *ptrN;
    double grey;
    QImage *n;

    n = new QImage(width, height, QImage::Format_RGB32);
    ptr = im->bits();
    ptrN = n->bits();

    for(i=0; i<width; i++){
        for(j=0; j<height; j++){
            pos = 4*j*width + 4*i;
            grey = 0.2126*ptr[pos+2] + 0.7152*ptr[pos+1] + 0.0722*ptr[pos];
            ptrN[pos] = ptrN[pos+1] = ptrN[pos+2] = (int)round(grey);
        }
    }

    return n;
}

QImage* MainWindow::thresholding(QImage *im){
    int i, j, pos;
    unsigned char *ptr, *ptrN;
    QImage *n;

    n = new QImage(width, height, QImage::Format_RGB32);
    ptr = im->bits();
    ptrN = n->bits();

    for(i=0; i<width; i++){
        for(j=0; j<height; j++){
            pos = 4*j*width + 4*i;
            ptrN[pos] =     (ptr[pos] >= 128 ? 255 : 0);
            ptrN[pos+1] = (ptr[pos+1] >= 128 ? 255 : 0);
            ptrN[pos+2] = (ptr[pos+2] >= 128 ? 255 : 0);
        }
    }

    return n;
}

void MainWindow::loadImage(QString path){
    QImage *im, *tmp;
    tmp = new QImage(path);
    im = new QImage(width, height, QImage::Format_RGB32);

    *im = tmp->scaled(width, height, Qt::IgnoreAspectRatio);
    im = RGBtoGrey(im);
    im = thresholding(im);

    *img = *im;
}

void MainWindow::dilation(){
    int i, j, a, b, howMany;
    long long pos;
    QImage *cp;
    unsigned char *ptr, *ptrCp;

    cp = new QImage(width, height, QImage::Format_RGB32);
    *cp = *img;
    ptr = img->bits();
    ptrCp = cp->bits();

    for(i=0; i<width; i++){
        for(j=0; j<height; j++){

            howMany = 0;

            for(a=-maskSize/2; a<=maskSize/2; a++){
                for(b=-maskSize/2; b<=maskSize/2; b++){
                    if(a==0 && b==0) continue;
                    pos = 4*(j+a)*width + 4*(i+b);
                    if(j+a>=0 && j+a<height && i+b>=0 && i+b<width) { if(ptrCp[pos] == 0) howMany++; }
                }
            }

            if(howMany > 0){
                ptr[4*j*width + 4*i] = 0;
                ptr[4*j*width + 4*i + 1] = 0;
                ptr[4*j*width + 4*i + 2] = 0;
            }

        }
    }
}

void MainWindow::erosion(){
    int i, j, a, b, howMany;
    long long pos;
    QImage *cp;
    unsigned char *ptr, *ptrCp;

    cp = new QImage(width, height, QImage::Format_RGB32);
    *cp = *img;
    ptr = img->bits();
    ptrCp = cp->bits();

    for(i=0; i<width; i++){
        for(j=0; j<height; j++){

            howMany = 0;

            for(a=-maskSize/2; a<=maskSize/2; a++){
                for(b=-maskSize/2; b<=maskSize/2; b++){
                    if(a==0 && b==0) continue;
                    pos = 4*(j+a)*width + 4*(i+b);
                    if(j+a>0 && j+a<height-1 && i+b>0 && i+b<width-1) { if(ptrCp[pos] == 255) howMany++; }
                }
            }

            if(howMany > 0){
                ptr[4*j*width + 4*i] = 255;
                ptr[4*j*width + 4*i + 1] = 255;
                ptr[4*j*width + 4*i + 2] = 255;
            }
        }
    }
}

void MainWindow::opening(){
    erosion();
    dilation();
}

void MainWindow::closing(){
    dilation();
    erosion();
}


void MainWindow::on_loadImgButton_clicked()
{
    filePath = QFileDialog::getOpenFileName(this, tr("Wczytaj obraz..."), "..\\zad9_operacjeMorfologiczne\\img", tr("Pliki graficzne (*.png *.jpg *.bmp)"));
    if(!filePath.isEmpty()){
        loadImage(filePath);
        *orig = *img;
    }

}


void MainWindow::on_resetButton_clicked()
{
    *img = *orig;
    update();
}


void MainWindow::on_op1Button_clicked()
{
    dilation();
    update();
}


void MainWindow::on_op2Button_clicked()
{
    erosion();
    update();
}


void MainWindow::on_op3Button_clicked()
{
    opening();
    update();
}


void MainWindow::on_pushButton_clicked()
{
    closing();
    update();
}


void MainWindow::on_maskSizeSpinBox_valueChanged(int arg1)
{
    maskSize = arg1;
}

