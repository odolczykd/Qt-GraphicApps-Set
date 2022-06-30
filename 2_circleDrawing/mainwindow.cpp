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

    // domyslnie uruchamiany jest tryb rysowania okregow
    drawCircleMode = true;
    drawPolygonMode = false;
    drawPolEllMode = false;
    ui->sidesCounter->setEnabled(false);
    howManySides = 3;
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

    // ustawiamy wartosci  punktu x1...
    // ...wraz z obsluga wyjscia poza granice miejsca do rysowania

    x1 = (x>width) ? width : ((x<0) ? 0 : x);
    y1 = (y<0) ? 0 : ((y>height) ? height : y);

    // narysuj figure zgodnie z wybranym trybem
    if(drawCircleMode){
        drawCircle(x0, y0, x1, y1);
    }
    if(drawPolygonMode) {
        drawPolygon(x0, y0, x1, y1);
    }
    if(drawPolEllMode){
        drawEllipse(x0, y0, x1, y1);
        drawPolygon(x0, y0, x1, y1);
    }

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

void MainWindow::drawCircle(int x0, int y0, int x1, int y1){
    double R = sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
    int x,y;

    for(x=0; x<=(R*M_SQRT1_2); x++){
        y = round(sqrt(R*R - x*x));

        paintPixel(x+x0, y+y0);
        paintPixel(-x+x0, y+y0);
        paintPixel(x+x0, -y+y0);
        paintPixel(-x+x0, -y+y0);

        paintPixel(y+x0, x+y0);
        paintPixel(-y+x0, x+y0);
        paintPixel(y+x0, -x+y0);
        paintPixel(-y+x0, -x+y0);
    }
}

void MainWindow::drawEllipse(int x0, int y0, int x1, int y1){
    double x, y, t;

    //wyznaczanie wartosci polosi elipsy
    int a = abs(x1-x0);
    int b = abs(y1-y0);

    // rysowanie elipsy
    for(t=0; t<=M_PI/2; t+=EPSILON){
        x = round(a*cos(t));
        y = round(b*sin(t));
        paintPixel(x+x0, y+y0);
        paintPixel(-x+x0, y+y0);
        paintPixel(x+x0, -y+y0);
        paintPixel(-x+x0, -y+y0);
    }
}

void MainWindow::drawPolygon(int x0, int y0, int x1, int y1){
    int i;
    double t;

    int a = abs(x1-x0);
    int b = abs(y1-y0);

    // wyznaczanie bokow wielokata
    std::vector<int> plgnX, plgnY;

    // zakres jest do 2*M_PI+1, aby nie pominac zadnego punktu...
    // ...(blad przyblizenia powoduje pomijanie niektorych odcinkow)
    for(t=0; t<=2*M_PI+EPSILON; t+=2*M_PI/howManySides){
        plgnX.push_back(round(a*cos(t))+x0);
        plgnY.push_back(round(b*sin(t))+y0);
    }
    // rysowanie bokow wielokata
    for(i=1; i<(int)plgnX.size(); i++){
        drawLine(plgnX[i-1], plgnY[i-1], plgnX[i], plgnY[i]);
    }
}

void MainWindow::paintPixel(int x, int y){
    unsigned char *ptr;
    ptr = img->bits();

    // zamaluj piksel na kolor bialy (RGB: #ffffff)
    if(x>0 && x<width && y>0 && y<height){
        ptr[width*4*y + 4*x]     = 255;
        ptr[width*4*y + 4*x + 1] = 255;
        ptr[width*4*y + 4*x + 2] = 255;
    }
}

void MainWindow::on_radioButtonCircle_clicked()
{
    drawCircleMode = true;
    drawPolygonMode = false;
    drawPolEllMode = false;
    ui->sidesCounter->setEnabled(false);
}

void MainWindow::on_radioButtonPolygon_clicked()
{
    drawCircleMode = false;
    drawPolygonMode = true;
    drawPolEllMode = false;
    ui->sidesCounter->setEnabled(true);
}

void MainWindow::on_radioButtonPolEll_clicked()
{
    drawCircleMode = false;
    drawPolygonMode = false;
    drawPolEllMode = true;
    ui->sidesCounter->setEnabled(true);
}

void MainWindow::on_sidesCounter_valueChanged(int arg1)
{
    howManySides = arg1;
}
