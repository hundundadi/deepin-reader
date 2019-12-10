#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <DLabel>
#include <DWidget>

DWIDGET_USE_NAMESPACE

enum ROTATE_DIRECTION { ROTATE_LEFT = 0, ROTATE_RIGHT };

class ImageLabel : public DLabel
{
    Q_OBJECT
    Q_DISABLE_COPY(ImageLabel)

public:
    explicit ImageLabel(DWidget *parent = nullptr);

public:
    void setSelect(const bool &select);
    inline void setRadius(const int radius) { m_nRadius = radius; }
    inline void setBookMarkStatus(bool bshow) { m_bshowbookmark = bshow; }

    void setBackgroundPix(QPixmap &pixmap);
    //    {
    //        m_background = pixmap;
    //        m_bSetBp = true;
    //        m_thumbPix = pixmap;
    //    }

    void rotateImage(int angle);

protected:
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    bool m_bSelect = false;
    int m_nRadius = 8;             // 圆角
    bool m_bshowbookmark = false;  //显示书签

    QPixmap m_background;           // 缩略图
    QPixmap m_thumbPix;             // 显示缩略图
    bool m_bSetBp = false;          // 是否设置缩略图
    int m_nHighLightLineWidth = 0;  // 高亮边框宽度

    int m_nRotate = 0;  // 缩略图旋转度数
    //    bool m_bRotate = false;
};

#endif  // IMAGELABEL_H
