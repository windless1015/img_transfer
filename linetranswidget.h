#ifndef LINETRANSWIDGET_H
#define LINETRANSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <vector>

struct FormulaParam{
    FormulaParam():k(0.00),b(0.00){}
    double k;  //斜率
    double b;  //y轴截距
};


namespace Ui {
class lineTransWidget;
}

class lineTransWidget : public QWidget
{
    Q_OBJECT

public:
    explicit lineTransWidget(QWidget *parent = 0);
    ~lineTransWidget();

private slots:
    void on_openImgBtn_clicked();

    void on_lineTransBtn_clicked();

private:
    Ui::lineTransWidget *ui;
    QImage ScaleImage2Label(QImage& qImage, QLabel* qLabel);
    void ReadKAndB();
    QImage convertGrayScale( QImage &qColorImg, FormulaParam* fp);
    QImage GammaTransfer(QImage& src, double gamma, double comp); //r变换
    QImage LogTransfer(QImage& src, double dc); //对数变换
    QImage toGray( QImage& src_image);
    QImage *scaleGrayImg;
    FormulaParam f1,f2,f3,f4;
};

#endif // LINETRANSWIDGET_H
