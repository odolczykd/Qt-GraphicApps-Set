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
    centerX = width/2;
    centerY = height/2;

    resetMatrices();

    img = new QImage(width, height, QImage::Format_RGB32);
    orig = new QImage("://img/apple.jpg");
    clearImage(img);
    *img = *orig;
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *){
    QPainter p(this);
    p.drawImage(begX, begY, *img);
}

void MainWindow::clearImage(QImage *im){
    unsigned char *ptr;
    ptr = im->bits();

    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            ptr[4*j*width + 4*i]     = 255;
            ptr[4*j*width + 4*i + 1] = 255;
            ptr[4*j*width + 4*i + 2] = 255;
        }
    }
}

void MainWindow::resetTrMatrix(){
    Tr = {1,0,0,0,1,0,0,0,1};
}

void MainWindow::resetMatrices(){
    T = {1,0,0,0,1,0,0,0,1};
    Tr = {1,0,0,0,1,0,0,0,1};
    S = {1,0,0,0,1,0,0,0,1};
    R = {1,0,0,0,1,0,0,0,1};
    SH = {1,0,0,0,1,0,0,0,1};
    P = {0,0,1};
}

QVector<float> MainWindow::multiply(QVector<float> A, QVector<float> B){
    int i, j, k;
    QVector<float> R = {0,0,0,0,0,0,0,0,0};

    for(i=0; i<3; i++){
        for(j=0; j<3; j++){
            for(k=0; k<3; k++){
                R[3*i+j] += A[3*i+k] * B[3*k+j];
            }
        }
    }

    return R;
}

void MainWindow::transformAll(){
    unsigned char *ptr;
    int i, j;
    long pos;
    QColor c;

    ptr = img->bits();

    clearImage(img);
    resetTrMatrix();

    Tr = multiply(Tr, SH);
    Tr = multiply(Tr, R);
    Tr = multiply(Tr, S);
    Tr = multiply(Tr, T);

    for(i=0; i<width; i++){
        for(j=0; j<height; j++){

            P[0] = i*Tr[0] + j*Tr[1] + Tr[2];
            P[1] = i*Tr[3] + j*Tr[4] + Tr[5];

            if(P[0]>=1 && P[0]<=width-1 && P[1]>=1 && P[1]<=height-1){

                pos = 4*j*width + 4*i;
                c = bilinearInterpolation(orig, P[0], P[1]);

                ptr[pos] = c.blue();
                ptr[pos+1] = c.green();
                ptr[pos+2] = c.red();
            }

        }
    }

    update();
}

void MainWindow::setTranslation(int tx, int ty){
    T[2] = (float)tx;
    T[5] = (float)ty;
}

void MainWindow::setScaling(float sx, float sy){
    S[0] = sx;
    S[4] = sy;
    S[2] = centerX*(1-sx);
    S[5] = centerY*(1-sy);
}

void MainWindow::setRotation(int alpha){
    float sn = sin((float)(M_PI*alpha)/180.0);
    float cs = cos((float)(M_PI*alpha)/180.0);

    R[0] = cs;
    R[1] = -sn;
    R[3] = sn;
    R[4] = cs;
    R[2] = (float)centerX*(1-cs) + (float)centerY*sn;
    R[5] = (float)centerY*(1-cs) - (float)centerX*sn;
}

void MainWindow::setShearing(float shx, float shy){
    SH[1] = shx;
    SH[3] = shy;
    SH[2] = -(float)centerX*shx;
    SH[5] = -(float)centerY*shy;
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

void MainWindow::on_exitButton_clicked(){
    qApp->quit();
}


void MainWindow::on_resetButton_clicked(){
    ui->translationYSlider->setSliderPosition(0);
    ui->translationXSlider->setSliderPosition(0);
    ui->rotationSlider->setSliderPosition(0);
    ui->scalingYSlider->setSliderPosition(50);
    ui->scalingXSlider->setSliderPosition(50);
    ui->shearingYSlider->setSliderPosition(50);
    ui->shearingXSlider->setSliderPosition(50);

    resetMatrices();

    *img = *orig;
    update();
}

void MainWindow::on_translationYSlider_valueChanged(int value){
    setTranslation(-ui->translationXSlider->value(), value);
    transformAll();
}

void MainWindow::on_translationXSlider_valueChanged(int value){
    setTranslation(-value, ui->translationYSlider->value());
    transformAll();
}

void MainWindow::on_rotationSlider_valueChanged(int value){
    setRotation(-value);
    transformAll();
}

void MainWindow::on_scalingYSlider_valueChanged(int value){
    setScaling(50.0/(float)ui->scalingXSlider->value(), 50.0/(float)value);
    transformAll();
}

void MainWindow::on_scalingXSlider_valueChanged(int value){
    setScaling(50.0/(float)value, 50.0/(float)ui->scalingYSlider->value());
    transformAll();
}

void MainWindow::on_shearingYSlider_valueChanged(int value){
    setShearing(1.0-((float)ui->shearingXSlider->value()/50.0), 1.0-((float)value/50.0));
    transformAll();
}

void MainWindow::on_shearingXSlider_valueChanged(int value){
    setShearing(1.0-((float)value/50.0), 1.0-((float)ui->shearingYSlider->value()/50.0));
    transformAll();
}
