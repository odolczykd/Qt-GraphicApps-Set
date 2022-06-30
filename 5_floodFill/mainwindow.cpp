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
    drawLineMode = true;
    drawPolygonMode = false;
    drawCircleMode = false;
    drawEllipseMode = false;
    floodFillMode = false;
    ui->sidesCounter->setEnabled(false);
    howManySides = 3;

    white.setRed(255);
    white.setGreen(255);
    white.setBlue(255);

    fillColor.setRed(255);
    fillColor.setGreen(0);
    fillColor.setBlue(0);
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

    if(event->button() == Qt::RightButton) floodFill(x0, y0, getPixelColor(x0,y0), fillColor);

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
    if(drawLineMode){
        drawLine(x0, y0, x1, y1);
    }

    if(drawCircleMode){
        drawCircle(x0, y0, x1, y1);
    }

    if(drawPolygonMode) {
        drawPolygon(x0, y0, x1, y1);
    }

    if(drawEllipseMode){
        drawEllipse(x0, y0, x1, y1);
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
            paintPixel(x0, i, white);
        }
        return;
    }

    // jesli kat nachylenia jest z przedzialu [0,45]
    if(a>=0 && a<=1){
        if(x0>x1) std::swap(x0, x1);
        for(i=x0; i<=x1; i++){
            double y = (double)a*i + b;
            paintPixel(i, (int)round(y), white);
        }
    }
    // jesli kat nachylenia jest z przedzialu [45,90)
    else if(a>1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            double x = (double)(i-b)/a;
            paintPixel((int)round(x), i, white);
        }
    }
    // jesli kat nachylenia jest z przedzialu (90, 135]
    else if(a<=-1){
        if(y0>y1) std::swap(y0, y1);
        for(i=y0; i<=y1; i++){
            double x = (double)(i-b)/a;
            paintPixel((int)round(x), i, white);
        }
    }
    // jesli kat nachylenia jest z przedzialu (135, 180)
    else{
        if(x0>x1) std::swap(x0, x1);
        for(i=x0; i<=x1; i++){
            double y = (double)a*i + b;
            paintPixel(i, (int)round(y), white);
        }
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

void MainWindow::drawCircle(int x0, int y0, int x1, int y1){
    double R = sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
    int x,y;

    for(x=0; x<=(R*M_SQRT1_2); x++){
        y = round(sqrt(R*R - x*x));

        paintPixel(x+x0, y+y0, white);
        paintPixel(-x+x0, y+y0, white);
        paintPixel(x+x0, -y+y0, white);
        paintPixel(-x+x0, -y+y0, white);

        paintPixel(y+x0, x+y0, white);
        paintPixel(-y+x0, x+y0, white);
        paintPixel(y+x0, -x+y0, white);
        paintPixel(-y+x0, -x+y0, white);
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
        paintPixel(x+x0, y+y0, white);
        paintPixel(-x+x0, y+y0, white);
        paintPixel(x+x0, -y+y0, white);
        paintPixel(-x+x0, -y+y0, white);
    }
}

void MainWindow::paintPixel(int x, int y, QColor color){
    unsigned char *ptr;
    ptr = img->bits();

    // zamaluj piksel na kolor bialy (RGB: #ffffff)
    if(x>0 && x<width && y>0 && y<height){
        ptr[width*4*y + 4*x]     = color.blue();
        ptr[width*4*y + 4*x + 1] = color.green();
        ptr[width*4*y + 4*x + 2] = color.red();
    }
}

void MainWindow::floodFill(int x, int y, QColor curr, QColor neww){
    QStack<QPair<int,int>> st;

    QPair<int,int> pixel, tmp;
    pixel.first = x;
    pixel.second = y;
    st.push(pixel);

    while(!st.empty()){
        tmp.first = st.top().first;
        tmp.second = st.top().second;
        st.pop();

        if(getPixelColor(tmp.first, tmp.second) == curr){
            paintPixel(tmp.first, tmp.second, neww);

            QPair<int,int> neigh;

            neigh.first = tmp.first-1;
            neigh.second = tmp.second;
            st.push(neigh);

            neigh.first = tmp.first+1;
            neigh.second = tmp.second;
            st.push(neigh);

            neigh.first = tmp.first;
            neigh.second = tmp.second+1;
            st.push(neigh);

            neigh.first = tmp.first;
            neigh.second = tmp.second-1;
            st.push(neigh);
        }
    }

    return;
}

QColor MainWindow::getPixelColor(int x, int y){
    unsigned char *ptr;
    ptr = img->bits();
    QColor c;

    if(x>0 && x<width && y>0 && y<height){
        c.setBlue(ptr[width*4*y + 4*x]);
        c.setGreen(ptr[width*4*y + 4*x + 1]);
        c.setRed(ptr[width*4*y + 4*x + 2]);
    }
    return c;
}

void MainWindow::on_radioButtonLine_clicked()
{
    floodFillMode = false;
    drawLineMode = true;
    drawPolygonMode = false;
    drawCircleMode = false;
    drawEllipseMode = false;
    ui->sidesCounter->setEnabled(false);
}

void MainWindow::on_radioButtonPolygon_clicked()
{
    floodFillMode = false;
    drawLineMode = false;
    drawPolygonMode = true;
    drawCircleMode = false;
    drawEllipseMode = false;
    ui->sidesCounter->setEnabled(true);
}

void MainWindow::on_radioButtonCircle_clicked()
{
    floodFillMode = false;
    drawLineMode = false;
    drawPolygonMode = false;
    drawCircleMode = true;
    drawEllipseMode = false;
    ui->sidesCounter->setEnabled(false);
}

void MainWindow::on_radioButtonEllipse_clicked()
{
    floodFillMode = false;
    drawLineMode = false;
    drawPolygonMode = false;
    drawCircleMode = false;
    drawEllipseMode = true;
    ui->sidesCounter->setEnabled(false);
}

void MainWindow::on_sidesCounter_valueChanged(int arg1)
{
    howManySides = arg1;
}

void MainWindow::on_changeColorButton_clicked()
{
    QColor c = QColorDialog::getColor(Qt::white, this);
    if(c.isValid()) fillColor = c;
}

