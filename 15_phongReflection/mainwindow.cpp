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

    txtrFront = new QImage("://img/textureFront.png");
    txtrBack = new QImage("://img/textureBack.png");
    txtrTop = new QImage("://img/textureTop.png");
    txtrBottom = new QImage("://img/textureBottom.png");
    txtrLeft = new QImage("://img/textureLeft.png");
    txtrRight = new QImage("://img/textureRight.png");

    widthT = txtrFront->width();
    heightT = txtrFront->height();

    clearImage();

    T = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    Rx = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    Ry = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    Rz = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    S = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    Tr = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};

    d = -ui->distanceSlider->value();
    observer = QVector3D(0,0,d);

    lightBulb1 = QVector3D(ui->light1XSlider->value(),ui->light1YSlider->value(),ui->light1ZSlider->value());
    lightBulb1 = QVector3D(ui->light2XSlider->value(),ui->light2YSlider->value(),ui->light2ZSlider->value());

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
    QVector3D t1, t2, N, obs;
    QPair<int,int> p;
    QVector<QPair<int,int>> points2D;
    QVector<QVector3D> newCube;
    QVector<QVector3D> sides[6];

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

        QVector3D pt;
        pt.setX(tx);
        pt.setY(ty);
        pt.setZ(tz);
        newCube.push_back(pt);
    }

    QVector<QVector3D> v;

    v.clear();
    v << newCube[5] << newCube[6] << newCube[7] << newCube[4];
    sides[0] = v;

    v.clear();
    v << newCube[1] << newCube[0] << newCube[3] << newCube[2];
    sides[1] = v;

    v.clear();
    v << newCube[1] << newCube[5] << newCube[4] << newCube[0];
    sides[2] = v;

    v.clear();
    v << newCube[2] << newCube[3] << newCube[7] << newCube[6];
    sides[3] = v;

    v.clear();
    v << newCube[0] << newCube[4] << newCube[7] << newCube[3];
    sides[4] = v;

    v.clear();
    v << newCube[1] << newCube[2] << newCube[6] << newCube[5];
    sides[5] = v;

    for(i=0; i<6; i++){
        t1.setX(sides[i][0].x()-sides[i][1].x());
        t1.setY(sides[i][0].y()-sides[i][1].y());
        t1.setZ(sides[i][0].z()-sides[i][1].z());
        t2.setX(sides[i][2].x()-sides[i][1].x());
        t2.setY(sides[i][2].y()-sides[i][1].y());
        t2.setZ(sides[i][2].z()-sides[i][1].z());

        N = crossProduct(t1,t2);

        obs.setX(observer.x()+sides[i][1].x());
        obs.setY(observer.y()+sides[i][1].y());
        obs.setZ(observer.z()+sides[i][1].z());

        if(dotProduct(N, obs) > 0){

            points2D.clear();

            for(j=0; j<=3; j++){
                p.first = (int)((float)(sides[i][j].x()*d) / (float)(sides[i][j].z()+d)) + 300;
                p.second = (int)((float)(sides[i][j].y()*d) / (float)(sides[i][j].z()+d)) + 300;
                points2D.push_back(p);
            }

            for(j=0; j<=3; j++){

                drawLine(points2D[j%4].first, points2D[j%4].second, points2D[(j+1)%4].first, points2D[(j+1)%4].second, QColor(34,34,34));

                if(i==0){
                    texture(points2D[0], points2D[1], points2D[2], QPair<int,int>(200,0), QPair<int,int>(200,200), QPair<int,int>(0,200), txtrFront);
                    texture(points2D[0], points2D[3], points2D[2], QPair<int,int>(0,0), QPair<int,int>(200,0), QPair<int,int>(200,200), txtrFront);
                }

                if(i==1){
                    texture(points2D[0], points2D[1], points2D[2], QPair<int,int>(200,0), QPair<int,int>(200,200), QPair<int,int>(0,200), txtrBack);
                    texture(points2D[0], points2D[3], points2D[2], QPair<int,int>(0,0), QPair<int,int>(200,0), QPair<int,int>(200,200), txtrBack);
                }

                if(i==2){
                    texture(points2D[0], points2D[1], points2D[2], QPair<int,int>(200,0), QPair<int,int>(200,200), QPair<int,int>(0,200), txtrTop);
                    texture(points2D[0], points2D[3], points2D[2], QPair<int,int>(0,0), QPair<int,int>(200,0), QPair<int,int>(200,200), txtrTop);
                }

                if(i==3){
                    texture(points2D[0], points2D[1], points2D[2], QPair<int,int>(200,0), QPair<int,int>(200,200), QPair<int,int>(0,200), txtrBottom);
                    texture(points2D[0], points2D[3], points2D[2], QPair<int,int>(0,0), QPair<int,int>(200,0), QPair<int,int>(200,200), txtrBottom);
                }

                if(i==4){
                    texture(points2D[0], points2D[1], points2D[2], QPair<int,int>(200,0), QPair<int,int>(200,200), QPair<int,int>(0,200), txtrLeft);
                    texture(points2D[0], points2D[3], points2D[2], QPair<int,int>(0,0), QPair<int,int>(200,0), QPair<int,int>(200,200), txtrLeft);
                }

                if(i==5){
                    texture(points2D[0], points2D[1], points2D[2], QPair<int,int>(200,0), QPair<int,int>(200,200), QPair<int,int>(0,200), txtrRight);
                    texture(points2D[0], points2D[3], points2D[2], QPair<int,int>(0,0), QPair<int,int>(200,0), QPair<int,int>(200,200), txtrRight);
                }
            }
        }
    }

    phongShading();

    update();

}

void MainWindow::phongShading(){

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

float MainWindow::dotProduct(QVector3D A, QVector3D B){
    return A.x()*B.x() + A.y()*B.y() + A.z()*B.z();
}

QVector3D MainWindow::crossProduct(QVector3D A, QVector3D B){
    QVector3D res;
    res.setX(A.y()*B.z() - A.z()*B.y());
    res.setY(A.z()*B.x() - A.x()*B.z());
    res.setZ(A.x()*B.y() - A.y()*B.x());
    return res;
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

void MainWindow::paintPixel(int x, int y, QColor c){
    unsigned char *ptr;
    ptr = img->bits();

    if(x>=0 && x<width && y>=0 && y<height){
        ptr[4*y*width + 4*x]     = c.blue();
        ptr[4*y*width + 4*x + 1] = c.green();
        ptr[4*y*width + 4*x + 2] = c.red();
    }
}

void MainWindow::clearImage(){
    unsigned char *ptr;
    ptr = img->bits();

    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            ptr[4*j*width + 4*i]     = 100;
            ptr[4*j*width + 4*i + 1] = 100;
            ptr[4*j*width + 4*i + 2] = 100;
        }
    }
}

void MainWindow::drawLine(int x0, int y0, int x1, int y1, QColor c){
    int i;
    double a = (double)(y1-y0)/(x1-x0);
    double b = (double)y0 - a*x0;

    // obsluga rysowania pionowych kresek (kat nachylenia = 90)
    if(x0 == x1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            paintPixel(x0, i, c);
        }
        return;
    }

    if(a<=-1 || a>1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            double x = (double)(i-b)/a;
            paintPixel((int)round(x), i, c);
        }
    }

    else{
        if(x0>x1) std::swap(x0, x1);
        for(i=x0; i<=x1; i++){
            double y = (double)a*i + b;
            paintPixel(i, (int)round(y), c);
        }
    }
}

void MainWindow::texture(QPair<int,int> A, QPair<int,int> B, QPair<int,int> C, QPair<int,int> At, QPair<int,int> Bt, QPair<int,int> Ct, QImage *txtr){
    double u, v, w, xT, yT;

    long W = (B.first-A.first)*(C.second-A.second) - (C.first-A.first)*(B.second-A.second);
    long Wv, Ww;

    int minX = findMinMax(1, A.first, B.first, C.first);
    int minY = findMinMax(1, A.second, B.second, C.second);

    int maxX = findMinMax(2, A.first, B.first, C.first);
    int maxY = findMinMax(2, A.second, B.second, C.second);

    for(int i=minX; i<=maxX; i++){
        for(int j=minY; j<=maxY; j++){
            Wv = (i-A.first)*(C.second-A.second) - (C.first-A.first)*(j-A.second);
            Ww = (B.first-A.first)*(j-A.second) - (i-A.first)*(B.second-A.second);

            v = (double)Wv/(double)W;
            w = (double)Ww/(double)W;
            u = 1.0-v-w;

            //jesli punkt lezy wewnatrz trojkata
            if(u>=0. && u<=1. && v>=0. && v<=1. && w>=0. && w<=1.){
                xT = u*At.first + v*Bt.first + w*Ct.first;
                yT = u*At.second + v*Bt.second + w*Ct.second;
                paintPixel(i, j, bilinearInterpolation(txtr, xT, yT));
            }
        }
    }
}

QColor MainWindow::bilinearInterpolation(QImage *im, double x, double y){
    QColor c;

    int Px = (int)x, Py = (int)y;
    double a = x-Px, b = y-Py;
    int value;

    unsigned char *ptr;
    ptr = im->bits();

    value = b*((1.-a)*ptr[widthT*4*Py + 4*Px] + a*ptr[widthT*4*Py + 4*(Px+1)])
            + (1.-b)*((1.-a)*ptr[widthT*4*(Py-1) + 4*Px] + a*ptr[widthT*4*(Py-1) + 4*(Px+1)]);
    c.setBlue(value);

    value = b*((1.-a)*ptr[widthT*4*Py + 4*Px + 1] + a*ptr[widthT*4*Py + 4*(Px+1) + 1])
            + (1.-b)*((1.-a)*ptr[widthT*4*(Py-1) + 4*Px + 1] + a*ptr[widthT*4*(Py-1) + 4*(Px+1) + 1]);
    c.setGreen(value);

    value = b*((1.-a)*ptr[widthT*4*Py + 4*Px + 2] + a*ptr[widthT*4*Py + 4*(Px+1) + 2])
            + (1.-b)*((1.-a)*ptr[widthT*4*(Py-1) + 4*Px + 2] + a*ptr[widthT*4*(Py-1) + 4*(Px+1) + 2]);
    c.setRed(value);

    return c;
}

// which==1: znajdz min, which==2: znajdz max
int MainWindow::findMinMax(int which, int A, int B, int C){
    QList<int> l;
    l << A << B << C;
    std::sort(l.begin(), l.end());

    if(which == 1) return l[0];
    if(which == 2) return l[2];

    return -1;
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
    ui->light1XSlider->setSliderPosition(-200);
    ui->light1YSlider->setSliderPosition(-200);
    ui->light1ZSlider->setSliderPosition(-200);
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
    observer.setX(0);
    observer.setY(0);
    observer.setZ(-value);
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

