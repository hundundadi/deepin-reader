/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SheetBrowser_H
#define SheetBrowser_H

#include <DGraphicsView>
#include <QLabel>

#include "document/Model.h"
#include "Global.h"

class SheetOperation;
class BrowserPage;
class DocSheet;
class TipsWidget;
class BrowserAnnotation;
class SheetBrowser : public Dtk::Widget::DGraphicsView
{
    Q_OBJECT
public:
    explicit SheetBrowser(DocSheet *parent = nullptr);

    ~SheetBrowser() override;

    static QImage firstThumbnail(const QString &filePath);

    bool openFilePath(Dr::FileType fileType, const QString &);

    bool loadPages(SheetOperation &operation, const QSet<int> &bookmarks);

    void setMouseShape(const Dr::MouseShape &shape);

    void setBookMark(int index, int state);

    void deform(SheetOperation &operation);      //根据当前参数进行变换

    bool hasLoaded();

    int allPages();

    int currentPage();

    int  viewPointInIndex(QPoint viewPoint);

    void setCurrentPage(int page);

    bool getImage(int index, QImage &image, double width, double height, Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio);

    void openMagnifier();

    void closeMagnifier();

    bool magnifierOpened();

    int maxWidth();

    int maxHeight();

    void needBookmark(int index, bool state);

    QString selectedWordsText();

    void wordsChangedLater();

    void addHighlightAnnotation(QString text, QColor color);

    QList< deepin_reader::Annotation * > annotations();

    deepin_reader::Outline outline();

    void jumpToOutline(const qreal  &left, const qreal &top, unsigned int page);

    void jumpToHighLight(deepin_reader::Annotation *annotation);

    void removeAnnotation(BrowserAnnotation *annotation);

    bool removeAnnotation(deepin_reader::Annotation *annotation);

    bool updateAnnotation(BrowserAnnotation *annotation, const QString &text, QColor color = QColor(Qt::white));

    bool updateAnnotation(deepin_reader::Annotation *annotation, const QString &text, QColor color = QColor(Qt::white));

signals:
    void sigPageChanged(int page);

    void sigWheelUp();

    void sigWheelDown();

    void sigSizeChanged();

    void sigNeedPageFirst();

    void sigNeedPagePrev();

    void sigNeedPageNext();

    void sigNeedPageLast();

    void sigNeedBookMark(int index, bool state);

protected:
    void showEvent(QShowEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

    void dragEnterEvent(QDragEnterEvent *event);

    void wheelEvent(QWheelEvent *event);

    bool getImagePoint(QPoint viewPoint, double scaleFactor, QImage &image);

    int addIconAnnotation(QPointF &);

    BrowserPage *mouseClickInPage(QPointF &);

private slots:
    void onVerticalScrollBarValueChanged(int value);

    void onHorizontalScrollBarValueChanged(int value);

    void onWordsChanged();

private:
    deepin_reader::Document *m_document = nullptr;
    QLabel *m_magnifierLabel = nullptr;
    DocSheet *m_sheet = nullptr;
    TipsWidget *m_tipsWidget = nullptr;
    QTimer *m_resizeTimer = nullptr;
    QTimer *m_scrollTimer = nullptr;

    QList<BrowserPage *> m_items;
    double m_lastScaleFactor = 0;
    int m_maxWidth = 0;                 //最大一页的宽度
    int m_maxHeight = 0;                //最大一页的高度
    bool m_hasLoaded = false;           //是否已经加载过每页的信息
    int m_initPage = 1;                 //用于刚显示跳转的页数
    QPointF m_selectPressedPos;         //scene
};

#endif // SheetBrowser_H