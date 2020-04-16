/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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

#include "Central.h"

#include <QFileInfo>
#include <QMimeData>
#include <QUrl>
#include <DMessageManager>
#include <QStackedLayout>
#include <DFileDialog>

#include "business/AppInfo.h"
#include "utils/utils.h"
#include "CentralNavPage.h"
#include "CentralDocPage.h"
#include "TitleMenu.h"
#include "TitleWidget.h"
#include "MainWindow.h"

Central::Central(DWidget *parent)
    : CustomWidget(parent)
{
    setAcceptDrops(true);

    initWidget();
}

Central::~Central()
{
}

TitleMenu *Central::titleMenu()
{
    return m_menu;
}

TitleWidget *Central::titleWidget()
{
    return m_widget;
}

void Central::openFile(QString filePath)
{
    m_docPage->openFile(filePath);
}

void Central::openFilesExec()
{
    DFileDialog dialog;
    dialog.setFileMode(DFileDialog::ExistingFiles);
    dialog.setNameFilter(Utils::getSuffixList());
    dialog.setDirectory(QDir::homePath());

    if (QDialog::Accepted != dialog.exec()) {
        return;
    }

    QStringList filePathList = dialog.selectedFiles();

    if (filePathList.count() <= 0) {
        return;
    }

    QList<DocSheet *> sheets = DocSheet::g_map.values();

    foreach (QString filePath, filePathList) {
        //如果存在则活跃该窗口
        bool hasFind = false;

        foreach (DocSheet *sheet, sheets) {
            if (sheet->filePath() == filePath) {
                MainWindow *window = MainWindow::windowContainSheet(sheet);
                if (nullptr != window) {
                    window->activateSheet(sheet);
                    hasFind = true;
                    break;
                }
            }
        }

        if (!hasFind) {
            openFile(filePath);
        }
    }
}

void Central::addSheet(DocSheet *sheet)
{
    m_docPage->addSheet(sheet);
}

bool Central::hasSheet(DocSheet *sheet)
{
    return m_docPage->hasSheet(sheet);
}

void Central::showSheet(DocSheet *sheet)
{
    m_docPage->showSheet(sheet);
}

bool Central::saveAll()
{
    return m_docPage->saveAll();
}

void Central::handleShortcut(QString shortcut)
{
    m_docPage->OnAppShortCut(shortcut);
}

void Central::onSheetCountChanged(int count)
{
    auto pLayout = this->findChild<QStackedLayout *>();
    if (pLayout) {
//        pLayout->setCurrentIndex(count > 0 ? 1 : 0);
        if (count > 0) {
            pLayout->setCurrentIndex(1);
        } else if (count == 0) {
            pLayout->setCurrentIndex(0);
            if (m_widget) {
                m_widget->setControlEnabled(false);
            }
        }
    }
}

void Central::keyPressEvent(QKeyEvent *event)
{
    //  不是正常显示, 则是全屏模式或者幻灯片模式, 进行页面跳转
    QStringList pFilterList = QStringList() << KeyStr::g_pgup << KeyStr::g_pgdown
                              << KeyStr::g_down << KeyStr::g_up
                              << KeyStr::g_left << KeyStr::g_right << KeyStr::g_space;
    QString key = Utils::getKeyshortcut(event);
    if (pFilterList.contains(key)) {
        QJsonObject obj;
        obj.insert("type", "keyPress");
        obj.insert("key", key);

        QJsonDocument doc = QJsonDocument(obj);

        m_docPage->OnAppMsgData(doc.toJson(QJsonDocument::Compact));
    }

    CustomWidget::keyPressEvent(event);
}

void Central::OnSetCurrentIndex()
{
    auto pLayout = this->findChild<QStackedLayout *>();
    if (pLayout) {
        pLayout->setCurrentIndex(0);
    }
}

void Central::onFilesOpened()
{
    auto pLayout = this->findChild<QStackedLayout *>();
    if (pLayout) {
        pLayout->setCurrentIndex(1);
    }
}

void Central::onCurSheetChanged(DocSheet *sheet)
{
    if (nullptr == sheet) {
        m_layout->setCurrentIndex(0);
    } else {
        m_layout->setCurrentIndex(1);
    }
}

void Central::onMenuTriggered(const QString &action)
{
    if (action == "New window") {
        Utils::runApp(QString());
    } else if (action == "New tab") {
        openFilesExec();
    } else if (action == "Save") { //  保存当前显示文件
        m_docPage->saveCurrent();
    } else if (action == "Save as") {
        m_docPage->saveAsCurrent();
    } else if (action == "Print") {
        m_docPage->OnPrintFile();
    } else if (action == "Slide show") { //  开启幻灯片
        m_docPage->OnOpenSliderShow();
    } else if (action == "Magnifer") {   //  开启放大镜
        if (m_docPage->OnOpenMagnifer()) {
            m_widget->setMagnifierState();
        }
    } else if (action == "Document info") {
        m_docPage->onShowFileAttr();
    } else if (action == "Display in file manager") {    //  文件浏览器 显示
        m_docPage->OpenCurFileFolder();
    }
}

void Central::onOpenFilesExec()
{
    openFilesExec();
}

void Central::onShowTips(const QString &text, int iconIndex)
{
    if (0 == iconIndex)
        DMessageManager::instance()->sendMessage(this, QIcon(":/icons/deepin/builtin/ok.svg"), text);
    else
        DMessageManager::instance()->sendMessage(this, QIcon(":/icons/deepin/builtin/warning.svg"), text);
}

void Central::dragEnterEvent(QDragEnterEvent *event)
{
    auto mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        event->accept();
    } else if (mimeData->hasFormat("reader/tabbar")) {
        event->accept();
    }
}

void Central::dropEvent(QDropEvent *event)
{
    auto mimeData = event->mimeData();
    if (mimeData->hasFormat("deepin_reader/tabbar")) {
        event->setDropAction(Qt::MoveAction);
        event->accept();

        QString id = mimeData->data("deepin_reader/uuid");
        DocSheet *sheet = DocSheet::getSheet(id);
        if (nullptr != sheet)
            m_docPage->onCentralMoveIn(sheet);

    } else if (mimeData->hasUrls()) {
        QStringList noOpenFileList;
        QStringList canOpenFileList;

        for (auto url : mimeData->urls()) {
            QString sFilePath = url.toLocalFile();

            QFileInfo file(sFilePath);
            if (file.isFile()) {
                QString sSuffix = file.completeSuffix();
                if (sSuffix == "pdf" || sFilePath.endsWith(QString(".pdf"))) {  //  打开第一个pdf文件
                    canOpenFileList.append(sFilePath);
                } else {
                    if (!noOpenFileList.contains(sSuffix)) {
                        noOpenFileList.append(sSuffix);
                    }
                }
            }
        }

        foreach (auto s, noOpenFileList) {
            QString msgContent = tr("The format is not supported");
            onShowTips(msgContent, 1);
        }

        if (canOpenFileList.count() > 0) {
            foreach (auto s, canOpenFileList) {
                openFile(s);
            }
        }
    }
}

void Central::initWidget()
{
    m_menu    = new TitleMenu(this);
    m_widget  = new TitleWidget(this);
    m_docPage = new CentralDocPage(this);
    m_navPage = new CentralNavPage(this);

    m_layout = new QStackedLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_navPage);
    m_layout->addWidget(m_docPage);
    setLayout(m_layout);

    connect(m_menu, SIGNAL(sigActionTriggered(QString)), this, SLOT(onMenuTriggered(QString)));
    connect(m_navPage, SIGNAL(sigNeedOpenFilesExec()), SLOT(onOpenFilesExec()));

    connect(m_docPage, SIGNAL(sigCurSheetChanged(DocSheet *)), this, SLOT(onCurSheetChanged(DocSheet *)));
    connect(m_docPage, SIGNAL(sigCurSheetChanged(DocSheet *)), m_menu, SLOT(onCurSheetChanged(DocSheet *)));
    connect(m_docPage, SIGNAL(sigCurSheetChanged(DocSheet *)), m_widget, SLOT(onCurSheetChanged(DocSheet *)));
    connect(m_docPage, SIGNAL(sigTitleShortCut(QString)), m_widget, SLOT(onTitleShortCut(QString)));
    connect(m_docPage, SIGNAL(sigNeedShowTips(const QString &, int)), this, SLOT(onShowTips(const QString &, int)));
    connect(m_docPage, SIGNAL(sigNeedClose()), this, SIGNAL(sigNeedClose()));
    connect(m_docPage, SIGNAL(sigSheetCountChanged(int)), this, SLOT(onSheetCountChanged(int)));
    connect(m_docPage, SIGNAL(sigNeedShowState(int)), this, SIGNAL(sigNeedShowState(int)));
    connect(m_docPage, SIGNAL(sigNeedOpenFilesExec()), SLOT(onOpenFilesExec()));
    connect(m_docPage, SIGNAL(sigFindOperation(const int &)), m_widget, SLOT(slotFindOperation(const int &)));
}