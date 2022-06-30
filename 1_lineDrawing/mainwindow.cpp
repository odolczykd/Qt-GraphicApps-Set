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

    // tworzenie obrazow (w tym obrazu pomocniczego do efektu "dynamicznego" rysowania linii)
    img = new QImage(width, height, QImage::Format_RGB32);
    imgTmp = new QImage(width, height, QImage::Format_RGB32);
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
    update();
}


void MainWindow::on_exitButton_clicked(){
    qApp->quit();
}

void MainWindow::clearImage(){
    unsigned char *ptr;
    ptr = img->bits();

    int i, j;
    // ustaw wszystkie piksele na czarno (RGB: #000000)
    for(i=0; i<height; i++){
        for(j=0; j<width; j++){
            ptr[width*4*i + 4*j]     = 0; // skladowa BLUE
            ptr[width*4*i + 4*j + 1] = 0; // skladowa GREEN
            ptr[width*4*i + 4*j + 2] = 0; // skladowa RED
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event){
    int x = event->position().x();
    int y = event->position().y();
    x -= begX;
    y -= begY;

    // ustawiamy wartosci punktu poczatkowego odcinka (tj. punktu x0)
    x0 = x;
    y0 = y;

    *imgTmp = *img;

    update();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    *img = *imgTmp;

    if(x0<0 || x0>width || y0<0 || y0>height) return;

    int x = event->position().x();
    int y = event->position().y();
    x -= begX;
    y -= begY;

    // ustawiamy wartosci punktu koncowego odcinka (tj. punktu x1)...
    // ...wraz z obsluga wyjscia poza granice miejsca do rysowania
    x1 = (x>width) ? width : ((x<0) ? 0 : x);
    y1 = (y<0) ? 0 : ((y>height) ? height : y);

    drawLine(x0, y0, x1, y1);

    update();
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

    // jesli kat nachylenia jest z przedzialu [0,45]
    if(a>=0 && a<=1){
        if(x0>x1) std::swap(x0, x1);
        for(i=x0; i<=x1; i++){
            double y = (double)a*i + b;
            paintPixel(i, (int)round(y));
        }
    }
    // jesli kat nachylenia jest z przedzialu [45,90)
    else if(a>1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            double x = (double)(i-b)/a;
            paintPixel((int)round(x), i);
        }
    }
    // jesli kat nachylenia jest z przedzialu (90, 135]
    else if(a<=-1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            double x = (double)(i-b)/a;
            paintPixel((int)round(x), i);
        }
    }
    // jesli kat nachylenia jest z przedzialu (135, 180)
    else{
        if(x0>x1) std::swap(x0, x1);
        for(i=x0; i<=x1; i++){
            double y = (double)a*i + b;
            paintPixel(i, (int)round(y));
        }
    }
}

void MainWindow::paintPixel(int x, int y){
    unsigned char *ptr;
    ptr = img->bits();

    // zamaluj piksel na kolor bialy (RGB: #ffffff)
    ptr[width*4*y + 4*x]     = 255;
    ptr[width*4*y + 4*x + 1] = 255;
    ptr[width*4*y + 4*x + 2] = 255;
}
