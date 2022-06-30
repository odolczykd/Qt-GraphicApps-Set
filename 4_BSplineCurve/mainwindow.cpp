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

    img = new QImage(width, height, QImage::Format_RGB32);

    drawMode = true;
    moveMode = false;
    delMode  = false;
    sel = -1;
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *){
    QPainter p(this);
    p.drawImage(begX, begY, *img);
}

void MainWindow::on_clearButton_clicked(){
    clearImage();
    x.clear();
    y.clear();
    update();
}


void MainWindow::on_exitButton_clicked(){
    qApp->quit();
}

void MainWindow::clearImage(){
    unsigned char *ptr;
    ptr = img->bits();

    int i, j;
    for(i=0; i<height; i++){
        for(j=0; j<width; j++){
            ptr[width*4*i + 4*j]     = 0; // skladowa BLUE
            ptr[width*4*i + 4*j + 1] = 0; // skladowa GREEN
            ptr[width*4*i + 4*j + 2] = 0; // skladowa RED
        }
    }
}


void MainWindow::mousePressEvent(QMouseEvent *event){
    int i, j;
    int xTmp = event->position().x();
    int yTmp = event->position().y();
    xTmp -= begX;
    yTmp -= begY;

    if(xTmp<3 || xTmp>=width-3 || yTmp<3 || yTmp>=height-3) return;

    // tryb rysowania krzywych
    if(drawMode){
        paintAround(xTmp, yTmp, 255, 0, 0);

        x.push_back(xTmp);
        y.push_back(yTmp);

        if(x.size() >= 4){
            for(i=0; i<(int)x.size()-3; i++){
                drawBSpline(x, y, i);
            }
        }
    }

    // tryb przesuwania punktow
    if(moveMode){
        for(i=0; i<(int)x.size(); i++){
            if(abs(x[i]-xTmp)<=3 && abs(y[i]-yTmp)<=3){
                sel = i;
                paintAround(x[i], y[i], 0, 255, 0);
            }
        }
    }

    // tryb usuwania punktow
    if(delMode){
        for(i=0; i<(int)x.size(); i++){
            if(abs(x[i]-xTmp)<=3 && abs(y[i]-yTmp)<=3){
                sel = i;

                x.removeAt(sel);
                y.removeAt(sel);

                clearImage();

                for(j=0; j<(int)x.size(); j++){
                    paintAround(x[j], y[j], 255, 0, 0);
                }

                for(j=0; j<(int)x.size()-3; j++){
                    drawBSpline(x, y, j);
                }
            }
        }
    }

    update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    int i;
    int xTmp = event->position().x();
    int yTmp = event->position().y();
    xTmp -= begX;
    yTmp -= begY;

    if(xTmp<3 || xTmp>=width-3 || yTmp<3 || yTmp>=height-3) return;

    if(moveMode && sel!=-1){

        x[sel] = xTmp;
        y[sel] = yTmp;

        clearImage();
        for(i=0; i<(int)x.size(); i++){
            paintAround(x[i], y[i], 255, 0, 0);
        }

        for(i=0; i<(int)x.size()-3; i++){
            drawBSpline(x, y, i);
        }

        sel = -1;
    }
    update();
}

void MainWindow::drawBSpline(QVector<int> x, QVector<int> y, int k){
    double t;
    double xB, yB;

   for(t=0; t<=1; t+=EPSILON){
        xB = (-t*t*t + 3*t*t - 3*t + 1) * x[k]
           + (3*t*t*t - 6*t*t + 4) * x[k+1]
           + (-3*t*t*t + 3*t*t + 3*t + 1) * x[k+2]
           + (t*t*t) * x[k+3];
        xB /= 6;
        yB = (-t*t*t + 3*t*t - 3*t + 1) * y[k]
           + (3*t*t*t - 6*t*t + 4) * y[k+1]
           + (-3*t*t*t + 3*t*t + 3*t + 1) * y[k+2]
           + (t*t*t) * y[k+3];
        yB /= 6;
        paintPixel((int)round(xB), (int)round(yB), 255, 255, 255);
    }
}

void MainWindow::paintPixel(int x, int y, int R, int G, int B){
    unsigned char *ptr;
    ptr = img->bits();
    if(x>=0 && x<width && y>=0 && y<height){
        ptr[width*4*y + 4*x]     = B;
        ptr[width*4*y + 4*x + 1] = G;
        ptr[width*4*y + 4*x + 2] = R;
    }
}

// malowanie czerwonej otoczki wokol punktu
void MainWindow::paintAround(int x, int y, int R, int G, int B){
    int i, j;
    for(i=-3; i<=3; i++){
        for(j=-3; j<=3; j++){
            paintPixel(x+i, y+j, R, G, B);
        }
    }
}

void MainWindow::on_drawButton_clicked(){
    drawMode = true;
    moveMode = false;
    delMode  = false;
}


void MainWindow::on_moveButton_clicked(){
    drawMode = false;
    moveMode = true;
    delMode  = false;
}


void MainWindow::on_delButton_clicked(){
    drawMode = false;
    moveMode = false;
    delMode  = true;
}

