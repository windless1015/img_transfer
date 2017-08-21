#include "linetranswidget.h"
#include "ui_linetranswidget.h"
#include <QImage>
#include <QMessageBox>
#include <QFileDialog>
#include <QRgb>
#include <QVector>

lineTransWidget::lineTransWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::lineTransWidget)
{
    scaleGrayImg = new QImage();
    ReadKAndB();
    ui->setupUi(this);
}

lineTransWidget::~lineTransWidget()
{
    delete scaleGrayImg;
    delete ui;
}

void lineTransWidget::ReadKAndB(){
    //
    f1.k = 2;
    f1.b = -55;
    //
    f2.k = 0.5;
    f2.b = -55;
    //
    f3.k = 1;
    f3.b = 55;
    //图像的反相显示
    f4.k = -1;
    f4.b = 255;
}

QImage lineTransWidget::toGray( QImage& src_image){
    int height = src_image.height();
    int width = src_image.width();
    QImage ret(width, height, QImage::Format_Indexed8);
    ret.setColorCount(256);
    for(int i = 0; i < 256; i++)
        ret.setColor(i, qRgb(i, i, i));
    switch(src_image.format())
    {
        case QImage::Format_Indexed8:
            for(int i = 0; i < height; i ++)
            {
                const uchar *pSrc = (uchar *)src_image.constScanLine(i);
                uchar *pDest = (uchar *)ret.scanLine(i);
                memcpy(pDest, pSrc, width);
            }
            break;
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
            for(int i = 0; i < height; i ++)
            {
                const QRgb *pSrc = (QRgb *)src_image.constScanLine(i);
                uchar *pDest = (uchar *)ret.scanLine(i);
                for( int j = 0; j < width; j ++)
                {
                     pDest[j] = qGray(pSrc[j]);
                }
            }
            break;
    }
    return ret;
}

QImage lineTransWidget::convertGrayScale(QImage &qColorImg, FormulaParam* fp)
{
    QImage qGrayImg = QImage(qColorImg.width(), qColorImg.height(), QImage::Format_Indexed8);
    QVector<QRgb> table( 256 );
    for ( int i = 0 ; i < 256 ; ++ i ){
        table[i] = qRgb(i,i,i);
    }
    qGrayImg.setColorTable( table );
    for ( int i = 0 ; i < qGrayImg.width(); i++ ){
        for ( int j= 0 ; j < qGrayImg.height(); j++ ){
            QRgb pix_value = qColorImg.pixel( i, j );
            uint g_value = uint(fp->k * qGray(pix_value) + fp->b);
            if (g_value < 0)
                g_value = 0;
            else if (g_value > 255)
                g_value =255;
            qGrayImg.setPixel( i, j, g_value);
        }
    }
    return qGrayImg;
}

QImage lineTransWidget::ScaleImage2Label(QImage& qImage, QLabel* qLabel){
    QImage qScaledImage;
    QSize qImageSize = qImage.size();
    QSize qLabelSize = qLabel->size();
    double dWidthRatio = 1.0*qImageSize.width() / qLabelSize.width();
    double dHeightRatio = 1.0*qImageSize.height() / qLabelSize.height();
    if (dWidthRatio>dHeightRatio)
        qScaledImage = qImage.scaledToWidth(qLabelSize.width());
    else
        qScaledImage = qImage.scaledToHeight(qLabelSize.height());
    return qScaledImage;
}

void lineTransWidget::on_openImgBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("打开图像"),"c:/User/st/Desktop/",
                                                       tr("Image (*.jpg *.jpeg *.bmp *.png)"));
        if(fileName.isEmpty())
            return;
        else{
            QImage sourceImg;
            if(!(sourceImg.load(fileName))){
                QMessageBox::information(this, QString::fromLocal8Bit("打开图像失败"), QString::fromLocal8Bit("打开图像失败!"));
                return;
            }
            *scaleGrayImg = toGray(ScaleImage2Label(sourceImg, ui->src_label));
            ui->src_label->setPixmap(QPixmap::fromImage(*scaleGrayImg));
            ui->src_label->setAlignment(Qt::AlignCenter);
        }
}

QImage lineTransWidget::GammaTransfer(QImage& src, double gamma, double comp){
    QImage qGrayImg = QImage(src.width(), src.height(), QImage::Format_Indexed8);
    QVector<QRgb> table( 256 );
    for ( int i = 0 ; i < 256 ; ++ i ){
        table[i] = qRgb(i,i,i);
    }
    qGrayImg.setColorTable( table );
    for ( int i = 0 ; i < qGrayImg.width(); i++ ){
        for ( int j= 0 ; j < qGrayImg.height(); j++ ){
            QRgb pix_value = src.pixel( i, j );
            uint g_value = uint( pow((qGray(pix_value)+ comp)/255.0, gamma) * 255);
            if (g_value < 0)
                g_value = 0;
            else if (g_value > 255)
                g_value =255;
            qGrayImg.setPixel( i, j, g_value);
        }
    }
    return qGrayImg;
}

QImage lineTransWidget::LogTransfer(QImage& src, double dc){
    QImage qGrayImg = QImage(src.width(), src.height(), QImage::Format_Indexed8);
    QVector<QRgb> table( 256 );
    for ( int i = 0 ; i < 256 ; ++ i ){
        table[i] = qRgb(i,i,i);
    }
    qGrayImg.setColorTable( table );
    for ( int i = 0 ; i < qGrayImg.width(); i++ ){
        for ( int j= 0 ; j < qGrayImg.height(); j++ ){
            QRgb pix_value = src.pixel( i, j );
            uint g_value = uint(dc * log((double)(qGray(pix_value) + 1)));
            if (g_value < 0)
                g_value = 0;
            else if (g_value > 255)
                g_value =255;
            qGrayImg.setPixel( i, j, g_value);
        }
    }
    return qGrayImg;
}

void lineTransWidget::on_lineTransBtn_clicked()
{
    QImage dispalyImg1 = convertGrayScale(*scaleGrayImg, &f1);
    QImage dispalyImg2 = convertGrayScale(*scaleGrayImg, &f2);
    QImage dispalyImg3 = convertGrayScale(*scaleGrayImg, &f3);
    QImage dispalyImg4 = convertGrayScale(*scaleGrayImg, &f4);
    //伽马变换
    QImage dispalyImg5 = GammaTransfer(*scaleGrayImg, 1.2, 2);
    //对数变换
    QImage dispalyImg6 = LogTransfer(*scaleGrayImg, 10);
    ui->tran_label1->setPixmap(QPixmap::fromImage(dispalyImg1));
    ui->tran_label1->setAlignment(Qt::AlignCenter);
    ui->tran_label2->setPixmap(QPixmap::fromImage(dispalyImg2));
    ui->tran_label2->setAlignment(Qt::AlignCenter);
    ui->tran_label3->setPixmap(QPixmap::fromImage(dispalyImg3));
    ui->tran_label3->setAlignment(Qt::AlignCenter);
    ui->tran_label4->setPixmap(QPixmap::fromImage(dispalyImg4));
    ui->tran_label4->setAlignment(Qt::AlignCenter);

    ui->tran_label5->setPixmap(QPixmap::fromImage(dispalyImg5));
    ui->tran_label5->setAlignment(Qt::AlignCenter);
    ui->tran_label6->setPixmap(QPixmap::fromImage(dispalyImg6));
    ui->tran_label6->setAlignment(Qt::AlignCenter);
}
