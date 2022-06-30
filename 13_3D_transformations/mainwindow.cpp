#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    width = ui->frame->width();
    height = ui->frame->height();

    begX = ui->frame->x();
    begY = ui->frame->y();

    img = new QImage(width, height, QImage::Format_RGB32);
    clearImage();

    T = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    Rx = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    Ry = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    Rz = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    S = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    Tr = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};

    d = -ui->distanceSlider->value();

    setCube();

    transformAll();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent*){
    QPainter p(this);
    p.drawImage(begX, begY, *img);
}

void MainWindow::setCube(){
    QVector3D v;
    v.setX(-100);  v.setY(-100);  v.setZ(-100);  cube.push_back(v);
    v.setX(100);  v.setY(-100);  v.setZ(-100);  cube.push_back(v);
    v.setX(100);  v.setY(100);  v.setZ(-100);  cube.push_back(v);
    v.setX(-100);  v.setY(100);  v.setZ(-100);  cube.push_back(v);
    v.setX(-100);  v.setY(-100);  v.setZ(100);  cube.push_back(v);
    v.setX(100);  v.setY(-100);  v.setZ(100);  cube.push_back(v);
    v.setX(100);  v.setY(100);  v.setZ(100);  cube.push_back(v);
    v.setX(-100);  v.setY(100);  v.setZ(100);  cube.push_back(v);
}

void MainWindow::resetTrMatrix(){
    Tr = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
}

void MainWindow::resetMatrices(){
    int pos;
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            pos = 4*i+j;
            if(pos%5 == 0)  T[pos] = S[pos] = Rx[pos] = Ry[pos] = Rz[pos] = Tr[pos] = 1;
            else            T[pos] = S[pos] = Rx[pos] = Ry[pos] = Rz[pos] = Tr[pos] = 0;
        }
    }
}

void MainWindow::transformAll(){
    int i, j;
    float tx, ty, tz;
    QVector<QPair<int,int>> v;
    QPair<int,int> p;

    clearImage();
    resetTrMatrix();

    Tr = multiply(Tr, T);
    Tr = multiply(Tr, Rx);
    Tr = multiply(Tr, Ry);
    Tr = multiply(Tr, Rz);
    Tr = multiply(Tr, S);

    for(i=0; i<cube.size(); i++){

        tx = Tr[0]*cube[i].x() + Tr[1]*cube[i].y() + Tr[2]*cube[i].z() + Tr[3];
        ty = Tr[4]*cube[i].x() + Tr[5]*cube[i].y() + Tr[6]*cube[i].z() + Tr[7];
        tz = Tr[8]*cube[i].x() + Tr[9]*cube[i].y() + Tr[10]*cube[i].z() + Tr[11];
        if(d+tz >= 0) tz = -d;

        p.first = (int)((float)(tx*d) / (float)(tz+d)) + 300;
        p.second = (int)((float)(ty*d) / (float)(tz+d)) + 300;
        v.push_back(p);
        //qDebug() << p.first << p.second;

        paintPixel(p.first, p.second);
    }

    for(i=0; i<4; i++){
        if(i!=3) drawLine(v[i].first, v[i].second, v[i+1].first, v[i+1].second);
        drawLine(v[i].first, v[i].second, v[i+4].first, v[i+4].second);
    }

    drawLine(v[0].first, v[0].second, v[3].first, v[3].second);
    drawLine(v[4].first, v[4].second, v[7].first, v[7].second);

    for(i=4; i<8; i++){
        if(i!=7) drawLine(v[i].first, v[i].second, v[i+1].first, v[i+1].second);
    }

    update();

}

QVector<float> MainWindow::multiply(QVector<float> A, QVector<float> B){
    int i, j, k;
    QVector<float> R = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    for(i=0; i<4; i++){
        for(j=0; j<4; j++){
            for(k=0; k<4; k++){
                R[4*i+j] += A[4*i+k] * B[4*k+j];
            }
        }
    }

    return R;
}

void MainWindow::setTranslation(int tx, int ty, int tz){
    T[3] = tx;
    T[7] = ty;
    T[11] = tz;
}

void MainWindow::setRotationX(int alpha){
    float sn = sin((float)(M_PI*alpha)/180.0);
    float cs = cos((float)(M_PI*alpha)/180.0);

    Rx[5] = cs;
    Rx[6] = -sn;
    Rx[9] = sn;
    Rx[10] = cs;
}

void MainWindow::setRotationY(int alpha){
    float sn = sin((float)(M_PI*alpha)/180.0);
    float cs = cos((float)(M_PI*alpha)/180.0);

    Ry[0] = cs;
    Ry[2] = sn;
    Ry[8] = -sn;
    Ry[10] = cs;
}

void MainWindow::setRotationZ(int alpha){
    float sn = sin((float)(M_PI*alpha)/180.0);
    float cs = cos((float)(M_PI*alpha)/180.0);

    Rz[0] = cs;
    Rz[1] = -sn;
    Rz[4] = sn;
    Rz[5] = cs;
}

void MainWindow::setScaling(float sx, float sy, float sz){
    S[0] = sx;
    S[5] = sy;
    S[10] = sz;
}

void MainWindow::paintPixel(int x, int y){
    unsigned char *ptr;
    ptr = img->bits();

    if(x>=0 && x<width && y>=0 && y<height){
        ptr[4*y*width + 4*x]     = 0;
        ptr[4*y*width + 4*x + 1] = 0;
        ptr[4*y*width + 4*x + 2] = 0;
    }
}

void MainWindow::clearImage(){
    unsigned char *ptr;
    ptr = img->bits();

    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            ptr[4*j*width + 4*i]     = 255;
            ptr[4*j*width + 4*i + 1] = 255;
            ptr[4*j*width + 4*i + 2] = 255;
        }
    }
}

void MainWindow::drawLine(int x0, int y0, int x1, int y1){
    int i;
    double a = (double)(y1-y0)/(x1-x0);
    double b = (double)y0 - a*x0;

    // obsluga rysowania pionowych kresek (kat nachylenia = 90)
    if(x0 == x1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            paintPixel(x0, i);
        }
        return;
    }

    if(a<=-1 || a>1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            double x = (double)(i-b)/a;
            paintPixel((int)round(x), i);
        }
    }

    else{
        if(x0>x1) std::swap(x0, x1);
        for(i=x0; i<=x1; i++){
            double y = (double)a*i + b;
            paintPixel(i, (int)round(y));
        }
    }
}

void MainWindow::on_resetButton_clicked()
{
    ui->distanceSlider->setSliderPosition(300);
    ui->translationXSlider->setSliderPosition(0);
    ui->translationYSlider->setSliderPosition(0);
    ui->translationZSlider->setSliderPosition(0);
    ui->rotationXSlider->setSliderPosition(0);
    ui->rotationYSlider->setSliderPosition(0);
    ui->rotationZSlider->setSliderPosition(0);
    ui->scalingXSlider->setSliderPosition(50);
    ui->scalingYSlider->setSliderPosition(50);
    ui->scalingZSlider->setSliderPosition(50);
    d = -ui->distanceSlider->value();
    resetMatrices();
    transformAll();
}


void MainWindow::on_exitButton_clicked()
{
    qApp->quit();
}


void MainWindow::on_distanceSlider_valueChanged(int value)
{
    d = -value;
    transformAll();
}


void MainWindow::on_translationXSlider_valueChanged(int value)
{
    setTranslation(value, -ui->translationYSlider->value(), -ui->translationZSlider->value());
    transformAll();
}


void MainWindow::on_translationYSlider_valueChanged(int value)
{
    setTranslation(ui->translationXSlider->value(), -value, -ui->translationZSlider->value());
    transformAll();
}


void MainWindow::on_translationZSlider_valueChanged(int value)
{
    setTranslation(ui->translationXSlider->value(), -ui->translationYSlider->value(), -value);
    transformAll();
}


void MainWindow::on_rotationXSlider_valueChanged(int value)
{
    setRotationX(value);
    transformAll();
}


void MainWindow::on_rotationYSlider_valueChanged(int value)
{
    setRotationY(value);
    transformAll();
}


void MainWindow::on_rotationZSlider_valueChanged(int value)
{
    setRotationZ(value);
    transformAll();
}


void MainWindow::on_scalingXSlider_valueChanged(int value)
{
    setScaling((float)value/50.0, (float)ui->scalingYSlider->value()/50.0, (float)ui->scalingZSlider->value()/50.0);
    transformAll();
}


void MainWindow::on_scalingYSlider_valueChanged(int value)
{
    setScaling((float)ui->scalingXSlider->value()/50.0, (float)value/50.0, (float)ui->scalingZSlider->value()/50.0);
    transformAll();
}


void MainWindow::on_scalingZSlider_valueChanged(int value)
{
    setScaling((float)ui->scalingXSlider->value()/50.0, (float)ui->scalingYSlider->value()/50.0, (float)value/50.0);
    transformAll();
}

