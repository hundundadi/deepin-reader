#include "PlayControlWidget.h"
#include "subjectObserver/MsgHeader.h"
#include "subjectObserver/ModuleHeader.h"
#include "utils/PublicFunction.h"
#include "utils/utils.h"
#include "docview/docummentproxy.h"
#include <DPlatformWindowHandle>
#include <QTimer>
#include <QHBoxLayout>
#include <QtDebug>

PlayControlWidget::PlayControlWidget(DWidget *parnet)
    : DFloatingWidget(parnet)
{
    m_bcanshow = false;
    m_bautoplaying = true;
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setBlurBackgroundEnabled(true);
    setFramRadius(18);
    setFixedSize(260, 80);//DFloatingWidget有问题设置尺寸比实际显示尺寸宽高小10

    m_ptimer = new QTimer(this);
    m_ptimer->setInterval(3000);
    initWidget();
    initConnections();

    m_pNotifySubject = NotifySubject::getInstance();
    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }

    m_pMsgSubject = MsgSubject::getInstance();
    if (m_pMsgSubject) {
        m_pMsgSubject->removeObserver(this);
    }
}

PlayControlWidget::~PlayControlWidget()
{
    m_ptimer->stop();
    m_ptimer->deleteLater();
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }

    if (m_pMsgSubject) {
        m_pMsgSubject->removeObserver(this);
    }
}

int PlayControlWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_NOTIFY_KEY_MSG == msgType) {
        if (KeyStr::g_space == msgContent) {
            changePlayStatus();
        }
    }
    if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    }
    return  0;
}

void PlayControlWidget::sendMsg(const int &msgType, const QString &msgContent)
{
    m_pMsgSubject->sendMsg(msgType, msgContent);
}

void PlayControlWidget::notifyMsg(const int &msgType, const QString &msgContent)
{
    m_pMsgSubject->sendMsg(msgType, msgContent);
}

void PlayControlWidget::activeshow(int ix, int iy)
{
    if (m_ptimer->isActive())
        m_ptimer->stop();
    m_ptimer->start();
    move(ix, iy);
    raise();
    show();
}

void PlayControlWidget::killshow()
{
    m_bautoplaying = true;
    m_ptimer->stop();
    hide();
}

void PlayControlWidget::initWidget()
{
    QWidget *pwidget = new QWidget;
    this->setWidget(pwidget);
    QHBoxLayout *playout = new QHBoxLayout(pwidget);
    playout->setContentsMargins(10, 10, 10, 10);
    playout->setSpacing(10);
    m_pbtnpre = createBtn(QString("previous_normal"));
    m_pbtnplay = createBtn(QString("suspend_normal"));
    m_pbtnnext = createBtn(QString("next_normal"));
    m_pbtnexit = createBtn(QString("exit_normal"));
    playout->addWidget(m_pbtnpre);
    playout->addWidget(m_pbtnplay);
    playout->addWidget(m_pbtnnext);
    playout->addWidget(m_pbtnexit);

    setWidget(pwidget);
    // this->setLayout(playout);

}

void PlayControlWidget::initConnections()
{
    connect(m_ptimer, &QTimer::timeout, this, [this] {this->hide();});
    connect(m_pbtnpre, &DIconButton::clicked, this, &PlayControlWidget::slotPreClicked);
    connect(m_pbtnplay, &DIconButton::clicked, this, &PlayControlWidget::slotPlayClicked);
    connect(m_pbtnnext, &DIconButton::clicked, this, &PlayControlWidget::slotNextClicked);
    connect(m_pbtnexit, &DIconButton::clicked, this, &PlayControlWidget::slotExitClicked);
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
}

DIconButton *PlayControlWidget::createBtn(const QString &strname)
{
    DIconButton *btn = new  DIconButton(this);
    btn->setObjectName(strname);
    btn->setFixedSize(QSize(50, 50));
    btn->setIcon(QIcon(Utils::renderSVG(PF::getImagePath(strname, Pri::g_icons), QSize(36, 36))));
    btn->setIconSize(QSize(36, 36));
    return  btn;
}

void PlayControlWidget::pagejump(bool bpre)
{
    bool bstart = false;
    if (nullptr != DocummentProxy::instance() &&
            DocummentProxy::instance()->getAutoPlaySlideStatu()) {
        DocummentProxy::instance()->setAutoPlaySlide(false);
        bstart = true;
    }
    int nCurPage = DocummentProxy::instance()->currentPageNo();
    if (bpre)
        nCurPage--;
    else
        nCurPage++;

    int nPageSize = DocummentProxy::instance()->getPageSNum();
    if (nCurPage < 0 || nCurPage == nPageSize) {
        return;
    }

    DocummentProxy::instance()->pageJump(nCurPage);
    if (bstart && nullptr != DocummentProxy::instance()) {
        DocummentProxy::instance()->setAutoPlaySlide(true);
        bstart = false;
    }
}

void PlayControlWidget::changePlayStatus()
{
    m_bautoplaying = m_bautoplaying ? false : true;
    if (m_bautoplaying)
        m_pbtnplay->setIcon(QIcon(Utils::renderSVG(PF::getImagePath("suspend_normal", Pri::g_icons), QSize(36, 36))));
    else {
        m_pbtnplay->setIcon(QIcon(Utils::renderSVG(PF::getImagePath("play_normal", Pri::g_icons), QSize(36, 36))));
    }
    m_pbtnplay->setFixedSize(QSize(50, 50));
    m_pbtnplay->setIconSize(QSize(36, 36));
}

void PlayControlWidget::enterEvent(QEvent *event)
{
    m_ptimer->stop();
    show();
    DFloatingWidget::enterEvent(event);
}

void PlayControlWidget::leaveEvent(QEvent *event)
{
    m_ptimer->start();
    DFloatingWidget::enterEvent(event);
}

void PlayControlWidget::slotUpdateTheme()
{
    QList<DIconButton *> plist = findChildren<DIconButton *>();
    foreach (DIconButton *btn, plist) {
        if (btn == m_pbtnplay) {
            if (m_bautoplaying) {
                m_pbtnplay->setIcon(QIcon(Utils::renderSVG(PF::getImagePath("suspend_normal", Pri::g_icons), QSize(36, 36))));
            } else {
                m_pbtnplay->setIcon(QIcon(Utils::renderSVG(PF::getImagePath("play_normal", Pri::g_icons), QSize(36, 36))));
            }
        } else {
            btn->setIcon(QIcon(Utils::renderSVG(PF::getImagePath(btn->objectName(), Pri::g_icons), QSize(36, 36))));
        }
    }
}

void PlayControlWidget::slotPreClicked()
{
    // notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_pgup);
    pagejump(true);
}

void PlayControlWidget::slotPlayClicked()
{
    changePlayStatus();
    notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_space);
}

void PlayControlWidget::slotNextClicked()
{
    // notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_pgdown);
    pagejump(false);
}

void PlayControlWidget::slotExitClicked()
{
    notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_esc);
    m_ptimer->stop();
    hide();
}