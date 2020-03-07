/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
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
#include "MainTabBar.h"

#include <QDebug>

#include "MainTabWidgetEx.h"
#include "FileDataModel.h"

#include "business/SaveDialog.h"

#include "gof/bridge/IHelper.h"

MainTabBar::MainTabBar(DWidget *parent)
    : DTabBar(parent)
{
    m_strObserverName = "MainTabBar";
    this->setTabsClosable(true);
    this->setMovable(true);
    this->expanding();

    m_pMsgList = {MSG_TAB_ADD, MSG_MENU_NEW_TAB};

    __InitConnection();

    dApp->m_pModelService->addObserver(this);
}

MainTabBar::~MainTabBar()
{
    dApp->m_pModelService->removeObserver(this);
}

int MainTabBar::dealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_TAB_ADD == msgType) {
        AddFileTab(msgContent);
    } else if (MSG_MENU_NEW_TAB == msgType) {
        SlotTabAddRequested();
    }

    if (m_pMsgList.contains(msgType)) {
        return MSG_OK;
    }

    return MSG_NO_OK;
}

void MainTabBar::notifyMsg(const int &msgType, const QString &msgContent)
{
    dApp->m_pModelService->notifyMsg(msgType, msgContent);
}

//void MainTabBar::resizeEvent(QResizeEvent *event)
//{
//    DTabBar::resizeEvent(event);
//}

void MainTabBar::__InitConnection()
{
    connect(this, SIGNAL(tabBarClicked(int)), SLOT(SlotTabBarClicked(int)));
    connect(this, SIGNAL(tabCloseRequested(int)), SLOT(SlotTabCloseRequested(int)));
    connect(this, SIGNAL(tabAddRequested()), SLOT(SlotTabAddRequested()));
    connect(this, SIGNAL(currentChanged(int)), SLOT(SlotTabBarClicked(int)));
}

void MainTabBar::SlotTabBarClicked(int index)
{
    QString sPath = this->tabData(index).toString();
    if (sPath != "") {
        emit sigTabBarIndexChange(sPath);
    }
}


void MainTabBar::__SetTabMiniWidth()
{

}

void MainTabBar::AddFileTab(const QString &sContent)
{
    QStringList filePaths;

    QList<QString> sOpenFiles = MainTabWidgetEx::Instance()->qGetAllPath();

    QStringList canOpenFileList = sContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    foreach (auto s, canOpenFileList) {
        if (sOpenFiles.contains(s)) {
            notifyMsg(CENTRAL_SHOW_TIP, tr("The file is already open"));
        } else {
            filePaths.append(s);
        }
    }

    int nSize = filePaths.size();
    if (nSize > 0) {
        for (int iLoop = 0; iLoop < nSize; iLoop++) {
            QString s = filePaths.at(iLoop);
            if (s != "") {
                QString sName = getFileName(s);
                int iIndex = this->addTab(sName);
                this->setTabData(iIndex, s);
                this->setTabMinimumSize(iIndex, QSize(140, 36));
                emit sigAddTab(s);
            }
        }

        int nCurIndex = this->currentIndex();
        if (nCurIndex > -1) {
            SlotTabBarClicked(nCurIndex);
        }
    }
}

QString MainTabBar::getFileName(const QString &strFilePath)
{
    int nLastPos = strFilePath.lastIndexOf('/');
    nLastPos++;
    return strFilePath.mid(nLastPos);
}

//  新增
void MainTabBar::SlotTabAddRequested()
{
    notifyMsg(E_OPEN_FILE);
}

//  关闭
void MainTabBar::SlotTabCloseRequested(int index)
{
    QString sPath = this->tabData(index).toString();
    if (sPath != "") {
        emit sigCloseTab(sPath);
    }
}

void MainTabBar::SlotRemoveFileTab(const QString &sPath)
{
    if (sPath != "") {
        int nCount = this->count();
        for (int iLoop = 0; iLoop < nCount; iLoop++) {
            QString sTabData = this->tabData(iLoop).toString();
            if (sTabData == sPath) {
                this->removeTab(iLoop);
                break;
            }
        }

        nCount = this->count();
        if (nCount == 0) {
            notifyMsg(CENTRAL_INDEX_CHANGE);
        }
    }
}
