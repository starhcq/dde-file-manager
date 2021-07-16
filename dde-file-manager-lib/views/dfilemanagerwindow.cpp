/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *               2016 ~ 2018 dragondjf
 *
 * Author:     dragondjf<dingjiangfeng@deepin.com>
 *
 * Maintainer: dragondjf<dingjiangfeng@deepin.com>
 *             zccrs<zhangjide@deepin.com>
 *             Tangtong<tangtong@deepin.com>
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

#include "dfilemanagerwindow.h"
#include "dtoolbar.h"
#include "dfileview.h"
#include "fileviewhelper.h"
#include "ddetailview.h"
#include "dfilemenu.h"
#include "extendview.h"
#include "dstatusbar.h"
#include "dfilemenumanager.h"
#include "computerview.h"
#include "dtabbar.h"
#include "windowmanager.h"
#include "dfileservices.h"
#include "dfilesystemmodel.h"
#include "dfmviewmanager.h"
#include "dfmsidebar.h"
#include "dfmaddressbar.h"
#include "dfmsettings.h"
#include "dfmapplication.h"
#include "dfmstandardpaths.h"
#include "dfmopticalmediawidget.h"

#include "app/define.h"
#include "dfmevent.h"
#include "app/filesignalmanager.h"
#include "deviceinfo/udisklistener.h"
#include "usershare/usersharemanager.h"
#include "controllers/pathmanager.h"
#include "shutil/fileutils.h"
#include "gvfs/networkmanager.h"
#include "dde-file-manager/singleapplication.h"

#include "xutil.h"
#include "utils.h"
#include "dfmadvancesearchbar.h"
#include "dtagactionwidget.h"
#include "droundbutton.h"
#include "dfmrightdetailview.h"

#include "drenamebar.h"
#include "singleton.h"
#include "dfileservices.h"
#include "controllers/appcontroller.h"
#include "view/viewinterface.h"
#include "plugins/pluginmanager.h"
#include "controllers/trashmanager.h"
#include "controllers/filecontroller.h"
#include "models/dfmrootfileinfo.h"
#include "controllers/vaultcontroller.h"
#include "dfmsplitter.h"
#include "views/dfmvaultactiveview.h"
#include "accessibility/ac-lib-file-manager.h"

#include <DPlatformWindowHandle>
#include <DTitlebar>

#include <QScreen>
#include <QStatusBar>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <QThread>
#include <QDesktopWidget>
#include <QStackedLayout>
#include <QTabBar>
#include <QPair>
#include <QtConcurrent>
#include <DAnchors>
#include <DApplicationHelper>
#include <DHorizontalLine>

DWIDGET_USE_NAMESPACE

std::unique_ptr<RecordRenameBarState>  DFileManagerWindow::renameBarState{ nullptr };
std::atomic<bool> DFileManagerWindow::flagForNewWindowFromTab{ false };

class DFileManagerWindowPrivate
{
public:
    DFileManagerWindowPrivate(DFileManagerWindow *qq)
        : q_ptr(qq) {}

    void setCurrentView(DFMBaseView *view);
    bool processKeyPressEvent(QKeyEvent *event);
    bool processTitleBarEvent(QMouseEvent *event);
    bool cdForTab(Tab *tab, const DUrl &fileUrl);
    void initAdvanceSearchBar();
    bool isAdvanceSearchBarVisible() const;
    void setAdvanceSearchBarVisible(bool visible);
    void initRenameBar();
    bool isRenameBarVisible() const;
    void setRenameBarVisible(bool visible);
    void resetRenameBar();
    void storeUrlListToRenameBar(const QList<DUrl> &list) noexcept;

    QFrame *centralWidget{ nullptr };//中央区域（所有的除顶部区域）
    DFMSideBar *sideBar{ nullptr };
    QFrame *rightView { nullptr };
    DFMRightDetailView *detailView { nullptr };
    QFrame *rightDetailViewHolder { nullptr };
    QVBoxLayout *rightViewLayout { nullptr };
    DToolBar *toolbar{ nullptr };
    TabBar *tabBar { nullptr };
    DIconButton *newTabButton { nullptr };
    QFrame *tabTopLine { nullptr };
    QFrame *tabBottomLine { nullptr };
    DFMBaseView *currentView { nullptr };
    DStatusBar *statusBar { nullptr };
    QVBoxLayout *mainLayout { nullptr };
    DFMSplitter *splitter { nullptr };
    QFrame *titleFrame { nullptr };
    QStackedLayout *viewStackLayout { nullptr };//右边文件显示的布局
    QFrame *emptyTrashHolder { nullptr };
    DHorizontalLine *emptyTrashSplitLine { nullptr };
    DRenameBar *renameBar{ nullptr };
    DFMAdvanceSearchBar *advanceSearchBar = nullptr;
    DIconButton *windowIconButton = nullptr;

    QMap<DUrl, QWidget *> views;

    bool move;
    QPoint startPoint;
    QPoint windowPoint;

    DFileManagerWindow *q_ptr{ nullptr };

    D_DECLARE_PUBLIC(DFileManagerWindow)
};

void DFileManagerWindowPrivate::setCurrentView(DFMBaseView *view)
{
    Q_Q(DFileManagerWindow);

    if (currentView && currentView->widget()) {
        currentView->widget()->removeEventFilter(q);
    }

    currentView = view;

    if (currentView && currentView->widget()) {
        currentView->widget()->installEventFilter(q);
        if (sideBar && sideBar->sidebarView()) {
            QWidget::setTabOrder(currentView->widget(), sideBar->sidebarView());
        }
    }

    if (!view) {
        return;
    }

    toolbar->setCustomActionList(view->toolBarActionList());

    if (!tabBar->currentTab()) {
        toolbar->addHistoryStack();
        tabBar->createTab(view);
    } else {
        tabBar->currentTab()->setFileView(view);
    }
}

bool DFileManagerWindowPrivate::processKeyPressEvent(QKeyEvent *event)
{
    Q_Q(DFileManagerWindow);

    switch (event->modifiers()) {
    case Qt::NoModifier: {
        switch (event->key()) {
        case Qt::Key_F5:
            if (currentView) {
                currentView->refresh();
            }
            return true;
        }
        break;
    }
    case Qt::ControlModifier: {
        switch (event->key()) {
        case Qt::Key_Tab:
            tabBar->activateNextTab();
            return true;
        case Qt::Key_Backtab:
            tabBar->activatePreviousTab();
            return true;
        case Qt::Key_F:
            appController->actionctrlF(q->windowId());
            return true;
        case Qt::Key_L:
            appController->actionctrlL(q->windowId());
            return true;
        case Qt::Key_Left:
            appController->actionBack(q->windowId());
            return true;
        case Qt::Key_Right:
            appController->actionForward(q->windowId());
            return true;
        case Qt::Key_W:
            emit fileSignalManager->requestCloseCurrentTab(q->windowId());
            return true;
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:
            toolbar->triggerActionByIndex(event->key() - Qt::Key_1);
            return true;
        }
        break;
    }
    case Qt::AltModifier:
    case Qt::AltModifier | Qt::KeypadModifier:
        if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_8) {
            tabBar->setCurrentIndex(event->key() - Qt::Key_1);
            return true;
        }

        switch (event->key()) {
        case Qt::Key_Left:
            appController->actionBack(q->windowId());
            return true;
        case Qt::Key_Right:
            appController->actionForward(q->windowId());
            return true;
        }

        break;
    case Qt::ControlModifier | Qt::ShiftModifier:
        if (event->key() == Qt::Key_Question) {
            if (!DFMGlobal::isTablet())
                appController->actionShowHotkeyHelp(q->windowId());
            return true;
        } else if (event->key() == Qt::Key_Backtab) {
            tabBar->activatePreviousTab();
            return true;
        }
        break;
    }

    return false;
}

bool DFileManagerWindowPrivate::processTitleBarEvent(QMouseEvent *event)
{
    // tmp: 删除和恢复文件走的主线程，不能拖动窗口，后面修改回复和删除文件的方式后去除以下代码
    if (TrashManager::isWorking()) {
        if (!event)
            return false;

        Q_Q(DFileManagerWindow);

        if (event->type() == QEvent::MouseButtonPress) {
            auto mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                move = true;
                /*记录鼠标的全局坐标.*/
                startPoint = mouseEvent->globalPos();
                /*记录窗体的全局坐标.*/
                windowPoint = q->frameGeometry().topLeft();
                return true;
            }
            return false;
        }

        if (event->type() == QEvent::MouseButtonRelease) {
            auto mouseEvent = static_cast<QMouseEvent *>(event);
            /*改变移动状态.*/
            if (mouseEvent->buttons() & Qt::LeftButton) {
                move = false;
                return true;
            }
            return false;
        }

        if (event->type() == QEvent::MouseButtonDblClick) {
            move = false;
            return false;
        }

        if (event->type() == QEvent::MouseMove) {
            if (move) {
                auto mouseEvent = static_cast<QMouseEvent *>(event);
                /*移动中的鼠标位置相对于初始位置的相对位置.*/
                QPoint relativePos = mouseEvent->globalPos() - startPoint;
                /*然后移动窗体即可.*/
                if (event->buttons() == Qt::LeftButton) {
                    q->move(windowPoint + relativePos);
                }
            }
            return false;
        }

        move = false;
        return false;
    }

    return false;
}

bool DFileManagerWindowPrivate::cdForTab(Tab *tab, const DUrl &fileUrl)
{
    Q_Q(DFileManagerWindow);
    qDebug() << "  cd   to " << fileUrl;
    DFMBaseView *current_view = tab->fileView();

    // fix 6942 取消判断先后请求地址差异判断
    // fix 28857 高频进入光驱会死锁
    if (current_view && current_view->rootUrl() == fileUrl && fileUrl.scheme() == BURN_ROOT) {
        return false;
    }

    if (fileUrl.scheme() == BURN_SCHEME) {

        // 如果当前设备正在执行刻录或擦除，激活进度窗口，拒绝跳转至文件列表页面
        QString strVolTag = DFMOpticalMediaWidget::getVolTag(fileUrl);
        if (!strVolTag.isEmpty() && DFMOpticalMediaWidget::g_mapCdStatusInfo[strVolTag].bBurningOrErasing) {
            emit fileSignalManager->activeTaskDlg();
            return false;
        }
    }

    if (fileUrl.scheme() == DFMROOT_SCHEME) {
        DAbstractFileInfoPointer fi = DFileService::instance()->createFileInfo(q_ptr, fileUrl);
        if (fi->suffix() == SUFFIX_USRDIR) {
            return cdForTab(tab, fi->redirectedFileUrl());
        } else if (fi->suffix() == SUFFIX_UDISKS) {
            QString blkDevicePath = fi->extraProperties()["udisksblk"].toString();
            QScopedPointer<DBlockDevice> blk(DDiskManager::createBlockDevice(blkDevicePath));
            if (blk->mountPoints().empty()) {
                qDebug() << "mount the device{" << blkDevicePath <<" } at:" << blk->mount({});
            }
        }
    }
    if (fileUrl.scheme() == DFMVAULT_SCHEME ||
            VaultController::isVaultFile(fileUrl.fragment())) {
        if (VaultController::Unlocked != VaultController::ins()->state()
                || fileUrl.host() == "delete") {

            DUrl realUrl = fileUrl.isTaggedFile() ? DUrl::fromLocalFile(fileUrl.fragment()) : fileUrl;

            //! set scheme to get vault file info.
            realUrl.setScheme(DFMVAULT_SCHEME);

            DFMBaseView *view = DFMViewManager::instance()->createViewByUrl(realUrl);
            view->widget()->setParent(q);
            bool ret = view->setRootUrl(realUrl);
            delete view;
            view = nullptr;
            return ret;
        }
    }
    if (!current_view || !DFMViewManager::instance()->isSuited(fileUrl, current_view)) {
        DFMBaseView *view = DFMViewManager::instance()->createViewByUrl(fileUrl);
        if (view) {
            viewStackLayout->addWidget(view->widget());

            if (tab == tabBar->currentTab())
                viewStackLayout->setCurrentWidget(view->widget());

            q_ptr->handleNewView(view);
        } else {
            qWarning() << "Not support url: " << fileUrl;

            //###(zccrs):
            const DAbstractFileInfoPointer &fileInfo = DFileService::instance()->createFileInfo(q_ptr, fileUrl);
            if (fileInfo) {
                /* Call fileInfo->exists() twice. First result is false and the second one is true;
                           Maybe this is a bug of fuse when smb://10.0.10.30/people is mounted and cd to mounted folder immediately.
                        */
                qDebug() << fileInfo->exists() << fileUrl;
                qDebug() << fileInfo->exists() << fileUrl;
            }

            if (!fileInfo || !fileInfo->exists()) {
                DUrl searchUrl = current_view ? current_view->rootUrl() : DUrl::fromLocalFile(QDir::homePath());

                if (searchUrl.isComputerFile()) {
                    searchUrl = DUrl::fromLocalFile("/");
                }

                if (searchUrl.isSearchFile()) {
                    searchUrl = searchUrl.searchTargetUrl();
                }

                if (!q_ptr->isCurrentUrlSupportSearch(searchUrl)) {
                    return false;
                }

                const DUrl &newUrl = DUrl::fromSearchFile(searchUrl, fileUrl.toString());
                const DAbstractFileInfoPointer &fileInfo = DFileService::instance()->createFileInfo(q_ptr, newUrl);

                if (!fileInfo || !fileInfo->exists()) {
                    return false;
                }

                return cdForTab(tab, newUrl);
            }

            return false;
        }

        if (current_view) {
            current_view->deleteLater();
        }
        tab->setFileView(view);
        if (tab == tabBar->currentTab())
            setCurrentView(view);

        current_view = view;
    }

    bool ok = false;
    if (current_view) {
        // 为了解决 bug 34363: [4K屏]下且[缩放]，[ICON视图]下[浏览大量文件]，同时[疯狂滚动 scrollbar] 导致的崩溃问题
        auto fileView = dynamic_cast<DFileView *>(current_view);
        if (fileView && fileView->isIconViewMode()) {
            auto model = fileView->model();
            if (model) {
                auto state = model->state();
                // busy 状态说明文件太多，还没加载完
                if (state == DFileSystemModel::Busy) {
                    // 目前系统缩放区间为 [1.0, 2.75], ratio > 1.0 则为缩放, 目前 1.0 没有出现崩溃
                    qreal ratio = qApp->primaryScreen()->devicePixelRatio();
                    if (ratio > 1.0) {
                        // 如果疯狂滚动，那么Qt可能会由于绘制的原因崩溃，因此在切换前选中第一个item
                        // 这样界面就不会处于一个疯狂刷新绘制的状态，即不会调用绘制的函数，因此避免了绘制崩溃的问题
                        fileView->setCurrentIndex(model->index(0, 0));
                    }
                }
            }
        }
        ok = current_view->setRootUrl(fileUrl);

        if (ok) {
            tab->onFileRootUrlChanged(fileUrl);
            if (tab == tabBar->currentTab()) {
                emit q_ptr->currentUrlChanged();
            }
        }
    }

    return ok;
}

void DFileManagerWindowPrivate::initAdvanceSearchBar()
{
    if (advanceSearchBar) return;

    Q_Q(DFileManagerWindow);

    // blumia: we add the DFMAdvanceSearchBar widget to layout so actually we shouldn't give it a parent here,
    //         but we need to apply the currect stylesheet to it so set a parent will do this job.
    //         feel free to replace it with a better way to apply stylesheet.
    advanceSearchBar = new DFMAdvanceSearchBar(q);

    initRenameBar(); // ensure we can use renameBar.

    Q_CHECK_PTR(rightViewLayout);

    int renameWidgetIndex = rightViewLayout->indexOf(renameBar);
    int advanceSearchBarInsertTo = renameWidgetIndex == -1 ? 0 : renameWidgetIndex + 1;
    rightViewLayout->insertWidget(advanceSearchBarInsertTo, advanceSearchBar);

    QObject::connect(advanceSearchBar, &DFMAdvanceSearchBar::optionChanged, q, [ = ](const QMap<int, QVariant> &formData, bool updateView) {
        if (currentView) {
            DFileView *fv = dynamic_cast<DFileView *>(currentView);
            if (fv) {
                fv->setAdvanceSearchFilter(formData, true, updateView);
            }
        }
    });
}

bool DFileManagerWindowPrivate::isAdvanceSearchBarVisible() const
{
    return advanceSearchBar ? advanceSearchBar->isVisible() : false;
}

void DFileManagerWindowPrivate::setAdvanceSearchBarVisible(bool visible)
{
    if (!advanceSearchBar) {
        if (!visible) return;
        initAdvanceSearchBar();
    }

    advanceSearchBar->setVisible(visible);
}

void DFileManagerWindowPrivate::initRenameBar()
{
    if (renameBar) return;

    Q_Q(DFileManagerWindow);

    // see the comment in initAdvanceSearchBar()
    renameBar = new DRenameBar(q);

    rightViewLayout->insertWidget(rightViewLayout->indexOf(emptyTrashHolder) + 1, renameBar);

    QObject::connect(renameBar, &DRenameBar::clickCancelButton, q, &DFileManagerWindow::hideRenameBar);
}

bool DFileManagerWindowPrivate::isRenameBarVisible() const
{
    return renameBar ? renameBar->isVisible() : false;
}

void DFileManagerWindowPrivate::setRenameBarVisible(bool visible)
{
    if (!renameBar) {
        if (!visible) return;
        initRenameBar();
    }

    renameBar->setVisible(visible);
}

void DFileManagerWindowPrivate::resetRenameBar()
{
    if (!renameBar) return;

    renameBar->resetRenameBar();
}

void DFileManagerWindowPrivate::storeUrlListToRenameBar(const QList<DUrl> &list) noexcept
{
    if (!renameBar) initRenameBar();

    renameBar->storeUrlList(list);
}

DFileManagerWindow::DFileManagerWindow(QWidget *parent)
    : DFileManagerWindow(DUrl(), parent)
{
    AC_SET_OBJECT_NAME(this, AC_COMPUTER_MIAN_WINDOW);
    AC_SET_ACCESSIBLE_NAME(this, AC_COMPUTER_MIAN_WINDOW);
}

QPair<bool, QMutex> winId_mtx;  //异步初始化win插件
DFileManagerWindow::DFileManagerWindow(const DUrl &fileUrl, QWidget *parent)
    : DMainWindow(parent)
    , d_ptr(new DFileManagerWindowPrivate(this))
{
    AC_SET_OBJECT_NAME(this, AC_COMPUTER_MIAN_WINDOW);
    AC_SET_ACCESSIBLE_NAME(this, AC_COMPUTER_MIAN_WINDOW);
    /// init global AppController
    setWindowIcon(QIcon::fromTheme("dde-file-manager"));
    if (!winId_mtx.first) {
        //等待异步加载完成
        winId_mtx.second.lock();
        winId_mtx.first = true;
        winId_mtx.second.unlock();
    }
    winId();
    initData();
    initUI();
    initConnect();

    //202007010032【文件管理器】【5.1.2.10-1】【sp2】（.doc,ppt,xls）文件拖拽到桌面上的文件管理器图标上，被识别为文件夹打开，
    // 判断出入的url是否是一个目录，不是就取parentUrl
    DUrl newurl = fileUrl;
    //排除u盘自动挂载，并且自动打开，拖拽的文件都是FILE_SCHEME
    if (newurl.scheme() == FILE_SCHEME) {
        const DAbstractFileInfoPointer &fileInfo = DFileService::instance()->createFileInfo(nullptr, fileUrl);

        if (fileInfo && !fileInfo->isDir()) {
            newurl = fileUrl.parentUrl();
        }
    }

    if (DFMGlobal::isTablet()) {
        //平板去掉最大最小和关闭按钮
        setWindowFlags(windowFlags()
                       & ~ Qt::WindowMaximizeButtonHint
                       & ~ Qt::WindowMinimizeButtonHint
                       & ~ Qt::WindowCloseButtonHint);

        // 将不会为虚拟键盘做任何自适应操作
        qApp->ignoreVirtualKeyboard(this);
    }
    openNewTab(newurl);
}

DFileManagerWindow::~DFileManagerWindow()
{
    m_currentTab = nullptr;
}

void DFileManagerWindow::onRequestCloseTab(const int index, const bool &remainState)
{
    D_D(DFileManagerWindow);

    Tab *tab = d->tabBar->tabAt(index);

    if (!tab) {
        return;
    }

    DFMBaseView *view = tab->fileView();

    d->viewStackLayout->removeWidget(view->widget());
    view->deleteLater();

    // delete 之后某些逻辑任然被误触发, 设置标志让 view 自己能判断这种情况
    // 若 QObject 更直接的方式判断, 可以直接修改
    DFileView *dfileview = dynamic_cast<DFileView *>(view);
    if (dfileview) {
        dfileview->setDestroyFlag(true);
    }

    d->toolbar->removeNavStackAt(index);
    d->tabBar->removeTab(index, remainState);
}

void DFileManagerWindow::closeCurrentTab(quint64 winId)
{
    D_D(DFileManagerWindow);

    if (winId != this->winId()) {
        return;
    }

    if (d->tabBar->count() == 1) {
        close();
        return;
    }

    emit d->tabBar->tabCloseRequested(d->tabBar->currentIndex());
}

// 关闭当前窗口的所有保险箱的标签
void DFileManagerWindow::closeAllTabOfVault(quint64 winId)
{
    D_D(DFileManagerWindow);

    // 传入的窗口ID不是当前活动窗口ID
    if (winId != this->winId()) {
        return;
    }

    // 当前只有一个标签，不用关闭
    int nCount = d->tabBar->count();
    if (nCount < 2) {
        return;
    }

    // 记录是否有除保险箱之外的标签
    bool bOtherTab = false;
    for (int i = nCount - 1; i > -1; --i) {
        Tab *tab = d->tabBar->tabAt(i);
        if (!tab) {
            return;
        }

        DUrl url = tab->currentUrl();
        if (VaultController::isVaultFile(url.toString())) {
            if (i == 0) { // 当判断到最后一个标签时，如何没有其它标签，则保留该标签
                if (!bOtherTab) {
                    return;
                }
            }
            // 删除编号对应的标签
            emit d->tabBar->tabCloseRequested(i);
        } else {
            bOtherTab = true;
        }
    }
}

void DFileManagerWindow::showNewTabButton()
{
    D_D(DFileManagerWindow);
    d->newTabButton->show();
    d->tabTopLine->show();
    d->tabBottomLine->show();
}

void DFileManagerWindow::hideNewTabButton()
{
    D_D(DFileManagerWindow);
    d->newTabButton->hide();
    d->tabTopLine->hide();
    d->tabBottomLine->hide();
}

void DFileManagerWindow::showEmptyTrashButton()
{
    Q_D(DFileManagerWindow);
    d->emptyTrashHolder->show();
    d->emptyTrashSplitLine->show();
}

void DFileManagerWindow::hideEmptyTrashButton()
{
    Q_D(DFileManagerWindow);
    d->emptyTrashHolder->hide();
    d->emptyTrashSplitLine->hide();
}

void DFileManagerWindow::onNewTabButtonClicked()
{
    DUrl url = DFMApplication::instance()->appUrlAttribute(DFMApplication::AA_UrlOfNewTab);

    if (!url.isValid()) {
        url = currentUrl();
    }

    openNewTab(url);
}

void DFileManagerWindow::requestEmptyTrashFiles()
{
    DFMGlobal::clearTrash();
}

void DFileManagerWindow::onTrashStateChanged()
{
    if (currentUrl() == DUrl::fromTrashFile("/") && !TrashManager::isEmpty()) {
        showEmptyTrashButton();
    } else {
        hideEmptyTrashButton();
    }
}

void DFileManagerWindow::onTabAddableChanged(bool addable)
{
    D_D(DFileManagerWindow);

    d->newTabButton->setEnabled(addable);
}

void DFileManagerWindow::onCurrentTabChanged(int tabIndex)
{
    D_D(DFileManagerWindow);

    Tab *tab = d->tabBar->tabAt(tabIndex);

    if (tab) {
        d->toolbar->switchHistoryStack(tabIndex);

        if (!tab->fileView()) {
            return;
        }

        switchToView(tab->fileView());
        // bug 32988 进入标签先刷新一次，解决保险箱重命名文件夹，在标签目录下出现重复文件夹
        tab->fileView()->refresh();

//        if (currentUrl().isSearchFile()) {
//            if (!d->toolbar->getSearchBar()->isVisible()) {
//                d->toolbar->searchBarActivated();
//                d->toolbar->getSearchBar()->setText(tab->fileView()->rootUrl().searchKeyword());
//            }
//        } else {
//            if (d->toolbar->getSearchBar()->isVisible()) {
//                d->toolbar->searchBarDeactivated();
//            }
//        }
    }
}

DUrl DFileManagerWindow::currentUrl() const
{
    D_DC(DFileManagerWindow);

    return d->currentView ? d->currentView->rootUrl() : DUrl();
}

DFMBaseView::ViewState DFileManagerWindow::currentViewState() const
{
    D_DC(DFileManagerWindow);

    return d->currentView ? d->currentView->viewState() : DFMBaseView::ViewIdle;
}

bool DFileManagerWindow::isCurrentUrlSupportSearch(const DUrl &currentUrl)
{
    const DAbstractFileInfoPointer &currentFileInfo = DFileService::instance()->createFileInfo(this, currentUrl);

    if (!currentFileInfo || !currentFileInfo->canIteratorDir()) {
        return false;
    }
    return true;
}

DToolBar *DFileManagerWindow::getToolBar() const
{
    D_DC(DFileManagerWindow);

    return d->toolbar;
}

DFMBaseView *DFileManagerWindow::getFileView() const
{
    D_DC(DFileManagerWindow);

    return d->currentView;
}

DFMSideBar *DFileManagerWindow::getLeftSideBar() const
{
    D_DC(DFileManagerWindow);

    return d->sideBar;
}

int DFileManagerWindow::getSplitterPosition() const
{
    D_DC(DFileManagerWindow);

    return d->splitter ? d->splitter->sizes().at(0) : DFMSideBar::maximumWidth;
}

void DFileManagerWindow::setSplitterPosition(int pos)
{
    Q_D(DFileManagerWindow);

    if (d->splitter) {
        d->splitter->setSizes({pos, d->splitter->width() - pos - d->splitter->handleWidth()});
    }
}

quint64 DFileManagerWindow::windowId()
{
    return WindowManager::getWindowId(this);
}

bool DFileManagerWindow::tabAddable() const
{
    D_DC(DFileManagerWindow);

    return d->tabBar->tabAddable();
}

bool DFileManagerWindow::cd(const DUrl &fileUrl)
{
    D_D(DFileManagerWindow);

    if (!d->tabBar->currentTab()) {
        d->toolbar->addHistoryStack();
        d->tabBar->createTab(nullptr);
    }

    if (!d->cdForTab(d->tabBar->currentTab(), fileUrl)) {
        return false;
    }

    this->hideRenameBar();

    return true;
}

bool DFileManagerWindow::cdForTab(int tabIndex, const DUrl &fileUrl)
{
    Q_D(DFileManagerWindow);

    return d->cdForTab(d->tabBar->tabAt(tabIndex), fileUrl);
}

bool DFileManagerWindow::cdForTabByView(DFMBaseView *view, const DUrl &fileUrl)
{
    Q_D(DFileManagerWindow);

    for (int i = 0; i < d->tabBar->count(); ++i) {
        Tab *tab = d->tabBar->tabAt(i);

        if (tab->fileView() == view) {
            return d->cdForTab(tab, fileUrl);
        }
    }

    return false;
}

bool DFileManagerWindow::openNewTab(DUrl fileUrl)
{
    D_D(DFileManagerWindow);

    if (!d->tabBar->tabAddable()) {
        return false;
    }

    if (fileUrl.isEmpty()) {
        fileUrl = DFMGlobal::isTablet() ? DUrl::fromLocalFile(QDir::homePath())
                                        : DUrl::fromUserInput(DFMStandardPaths::location(DFMStandardPaths::ComputerRootPath));
    }

    d->toolbar->addHistoryStack();
    d->setCurrentView(nullptr);
    d->tabBar->createTab(nullptr);

    return cd(fileUrl);
}

void DFileManagerWindow::switchToView(DFMBaseView *view)
{
    D_D(DFileManagerWindow);

    if (d->currentView == view) {
        return;
    }

    const DUrl &old_url = currentUrl();

    DFMBaseView::ViewState old_view_state = currentViewState();

    d->setCurrentView(view);
    d->viewStackLayout->setCurrentWidget(view->widget());

    if (old_view_state != view->viewState())
        emit currentViewStateChanged();

    if (view && view->rootUrl() == old_url) {
        return;
    }

    emit currentUrlChanged();
}

void DFileManagerWindow::moveCenter(const QPoint &cp)
{
    QRect qr = frameGeometry();

    qr.moveCenter(cp);
    move(qr.topLeft());
}

void DFileManagerWindow::moveTopRight()
{
    QRect pRect;
    pRect = qApp->desktop()->availableGeometry();
    int x = pRect.width() - width();
    move(QPoint(x, 0));
}

void DFileManagerWindow::moveTopRightByRect(QRect rect)
{
    int x = rect.x() + rect.width() - width();
    move(QPoint(x, 0));
}

void DFileManagerWindow::closeEvent(QCloseEvent *event)
{
    emit aboutToClose();

    DMainWindow::closeEvent(event);
}

void DFileManagerWindow::hideEvent(QHideEvent *event)
{
    QVariantMap state;
    state["sidebar"] = getSplitterPosition();
    DFMApplication::appObtuselySetting()->setValue("WindowManager", "SplitterState", state);

    return DMainWindow::hideEvent(event);
}

void DFileManagerWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    D_DC(DFileManagerWindow);

    if (event->y() <= d->titleFrame->height()) {
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    } else {
        DMainWindow::mouseDoubleClickEvent(event);
    }
}

void DFileManagerWindow::moveEvent(QMoveEvent *event)
{
    DMainWindow::moveEvent(event);

    emit positionChanged(event->pos());
}

void DFileManagerWindow::keyPressEvent(QKeyEvent *event)
{
    Q_D(DFileManagerWindow);

    if (!d->processKeyPressEvent(event)) {
        return DMainWindow::keyPressEvent(event);
    }
}

bool DFileManagerWindow::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(DFileManagerWindow);

    if (watched == titlebar()) {
        return d->processTitleBarEvent(static_cast<QMouseEvent *>(event));
    }

    if (!getFileView() || watched != getFileView()->widget()) {
        return false;
    }

    if (event->type() != QEvent::KeyPress) {
        return false;
    }

    return d->processKeyPressEvent(static_cast<QKeyEvent *>(event));
}

void DFileManagerWindow::resizeEvent(QResizeEvent *event)
{
    DMainWindow::resizeEvent(event);
}

bool DFileManagerWindow::fmEvent(const QSharedPointer<DFMEvent> &event, QVariant *resultData)
{
    Q_UNUSED(resultData)
    Q_D(DFileManagerWindow);

    switch (event->type()) {
    case DFMEvent::Back:
        d->toolbar->back();
        return true;
    case DFMEvent::Forward:
        d->toolbar->forward();
        return true;
    case DFMEvent::OpenNewTab: {
        if (event->windowId() != this->internalWinId()) {
            return false;
        }

        openNewTab(event.staticCast<DFMUrlBaseEvent>()->url());

        return true;
    }
    default:
        break;
    }

    return false;
}

QObject *DFileManagerWindow::object() const
{
    return const_cast<DFileManagerWindow *>(this);
}

void DFileManagerWindow::handleNewView(DFMBaseView *view)
{
    Q_UNUSED(view)
}

void DFileManagerWindow::initData()
{

}

void DFileManagerWindow::initUI()
{
    D_DC(DFileManagerWindow);

    if (DFMGlobal::isTablet()) {
        const QList<QScreen *> screens = qApp->screens();
        if (screens.length() > 0) {
            const QSize &screenSize = screens.first()->virtualSize();

            setMinimumSize(screenSize.width(), screenSize.height());
            resize(screenSize.width(), screenSize.height());
            moveCenterByRect(screens.first()->virtualGeometry());
        }
    } else {
        setMinimumSize(680, 420);
        resize(DEFAULT_WINDOWS_WIDTH, DEFAULT_WINDOWS_HEIGHT);
    }

    initTitleBar();
    initCentralWidget();
    setCentralWidget(d->centralWidget);
}

void DFileManagerWindow::initTitleFrame()
{
    D_D(DFileManagerWindow);

    initToolBar();
    d->titleFrame = new QFrame;
    d->titleFrame->setObjectName("TitleBar");
    AC_SET_OBJECT_NAME(d->titleFrame, AC_COMPUTER_CUSTOM_TITLE_BAR);
    AC_SET_ACCESSIBLE_NAME(d->titleFrame, AC_COMPUTER_CUSTOM_TITLE_BAR);
    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->setMargin(0);
    titleLayout->setSpacing(0);

    if (DFMGlobal::isTablet()) {
        d->windowIconButton = new DIconButton(this);
        d->windowIconButton->setIconSize(QSize(32, 32));
        d->windowIconButton->setWindowFlags(Qt::WindowTransparentForInput);
        d->windowIconButton->setFocusPolicy(Qt::NoFocus);
        d->windowIconButton->setFlat(true);
        d->windowIconButton->setIcon(QIcon::fromTheme("dde-file-manager", QIcon::fromTheme("system-file-manager")));
        titleLayout->addWidget(d->windowIconButton);
    } else {
        titlebar()->setIcon(QIcon::fromTheme("dde-file-manager", QIcon::fromTheme("system-file-manager")));
    }

    titleLayout->addWidget(d->toolbar);
    titleLayout->setSpacing(0);
    titleLayout->setContentsMargins(0, 7, 0, 7);
    d->titleFrame->setLayout(titleLayout);
    d->titleFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void DFileManagerWindow::initTitleBar()
{
    D_D(DFileManagerWindow);

    initTitleFrame();

    //此处关心dfm对menu的注册，原生自带menu的隐藏需要dtk支持
    if (!DFMGlobal::isTablet()) {
        DFileMenu *menu = fileMenuManger->createToolBarSettingsMenu();
        menu->setProperty("DFileManagerWindow", (quintptr)this);
        menu->setProperty("ToolBarSettingsMenu", true);
        menu->setEventData(DUrl(), DUrlList() << DUrl(), winId(), this);
        titlebar()->setMenu(menu);
    }

    titlebar()->setContentsMargins(0, 0, 0, 0);
    titlebar()->setCustomWidget(d->titleFrame, false);
    titlebar()->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    // fix: titlebar的move事件在自定义的耗时异步事件时会失效，因此自行处理titlebar的事件
    titlebar()->installEventFilter(this);
}

void DFileManagerWindow::initSplitter()
{
    D_D(DFileManagerWindow);

    initRightView();

    d->splitter = new DFMSplitter(Qt::Horizontal, this);
    if (!DFMGlobal::isTablet()) {
        initLeftSideBar();
        d->splitter->addWidget(d->sideBar);
    }
    d->splitter->addWidget(d->rightView);
    d->splitter->setChildrenCollapsible(false);
}

void DFileManagerWindow::initLeftSideBar()
{
    D_D(DFileManagerWindow);

    d->sideBar = new DFMSideBar(this);
    d->sideBar->setContentsMargins(0, 0, 0, 0);

    d->sideBar->setObjectName("DFMSideBar");
    d->sideBar->setMaximumWidth(DFMSideBar::maximumWidth);
    d->sideBar->setMinimumWidth(DFMSideBar::minimumWidth);

    // connections
    connect(this, &DFileManagerWindow::currentUrlChanged, this, [this, d]() {
        d->sideBar->setCurrentUrl(currentUrl());
    });
}

void DFileManagerWindow::initRightView()
{
    D_D(DFileManagerWindow);

    initTabBar();
    initViewLayout();
    d->rightView = new QFrame;
    AC_SET_OBJECT_NAME(d->rightView, AC_DM_RIGHT_VIEW);
    AC_SET_ACCESSIBLE_NAME(d->rightView, AC_DM_RIGHT_VIEW);

    QSizePolicy sp = d->rightView->sizePolicy();

    //NOTE(zccrs): 保证窗口宽度改变时只会调整right view的宽度，侧边栏保持不变
    //             QSplitter是使用QLayout的策略对widgets进行布局，所以此处
    //             设置size policy可以生效
    sp.setHorizontalStretch(1);
    d->rightView->setSizePolicy(sp);

    this->initRenameBarState();

    d->emptyTrashHolder = new QFrame(this);
    d->emptyTrashHolder->setFrameShape(QFrame::NoFrame);
    AC_SET_OBJECT_NAME(d->emptyTrashHolder, AC_DM_RIGHT_VIEW_TRASH_HOLDER);
    AC_SET_ACCESSIBLE_NAME(d->emptyTrashHolder, AC_DM_RIGHT_VIEW_TRASH_HOLDER);

    QHBoxLayout *emptyTrashLayout = new QHBoxLayout(d->emptyTrashHolder);
    QLabel *trashLabel = new QLabel(this);
    AC_SET_OBJECT_NAME(trashLabel, AC_DM_RIGHT_VIEW_TRASH_LABEL);
    AC_SET_ACCESSIBLE_NAME(trashLabel, AC_DM_RIGHT_VIEW_TRASH_LABEL);
    trashLabel->setText(tr("Trash"));
    QFont f = trashLabel->font();
    f.setPixelSize(17);
    f.setBold(true);
    trashLabel->setFont(f);
    QPushButton *emptyTrashButton = new QPushButton{ this };
    emptyTrashButton->setContentsMargins(0, 0, 0, 0);
    emptyTrashButton->setObjectName("EmptyTrashButton");
    AC_SET_ACCESSIBLE_NAME(emptyTrashButton, AC_DM_RIGHT_VIEW_EMPTY_TRASH_BUTTON);
    emptyTrashButton->setText(tr("Empty"));
    emptyTrashButton->setToolTip(QObject::tr("Empty Trash"));
    emptyTrashButton->setFixedSize({86, 36});
    DPalette pal = DApplicationHelper::instance()->palette(this);
    QPalette buttonPalette = emptyTrashButton->palette();
    buttonPalette.setColor(QPalette::ButtonText, pal.color(DPalette::Active, DPalette::TextWarning));
    emptyTrashButton->setPalette(buttonPalette);
    QObject::connect(emptyTrashButton, &QPushButton::clicked,
                     this, &DFileManagerWindow::requestEmptyTrashFiles, Qt::QueuedConnection);
    QPalette pa = emptyTrashButton->palette();
    pa.setColor(QPalette::ColorRole::Text, QColor("#FF5736"));
    emptyTrashButton->setPalette(pa);
    emptyTrashLayout->addWidget(trashLabel, 0, Qt::AlignLeft);
    emptyTrashLayout->addWidget(emptyTrashButton, 0, Qt::AlignRight);

    d->emptyTrashSplitLine = new DHorizontalLine(this);
    AC_SET_OBJECT_NAME(d->emptyTrashSplitLine, AC_DM_RIGHT_VIEW_TRASH_SPLIT_LINE);
    AC_SET_ACCESSIBLE_NAME(d->emptyTrashSplitLine, AC_DM_RIGHT_VIEW_TRASH_SPLIT_LINE);

    QHBoxLayout *tabBarLayout = new QHBoxLayout;
    tabBarLayout->setMargin(0);
    tabBarLayout->setSpacing(0);
    tabBarLayout->addWidget(d->tabBar);
    tabBarLayout->addWidget(d->newTabButton);

    d->tabTopLine->setFixedHeight(1);
    d->tabBottomLine->setFixedHeight(1);

    d->rightViewLayout = new QVBoxLayout;
    d->rightViewLayout->addWidget(d->tabTopLine);
    d->rightViewLayout->addLayout(tabBarLayout);
    d->rightViewLayout->addWidget(d->tabBottomLine);
    d->rightViewLayout->addWidget(d->emptyTrashHolder);
    d->rightViewLayout->addWidget(d->emptyTrashSplitLine);
    d->rightViewLayout->addLayout(d->viewStackLayout, 1);
    d->rightViewLayout->setSpacing(0);
    d->rightViewLayout->setContentsMargins(0, 0, 0, 0);
    d->rightView->setLayout(d->rightViewLayout);
    d->emptyTrashHolder->hide();
    d->emptyTrashSplitLine->hide();
}

void DFileManagerWindow::initToolBar()
{
    D_D(DFileManagerWindow);

    d->toolbar = new DToolBar(this);
    d->toolbar->setObjectName("ToolBar");

    AC_SET_ACCESSIBLE_NAME(d->toolbar, AC_DM_TOOLBAR);
}

void DFileManagerWindow::initTabBar()
{
    D_D(DFileManagerWindow);

    d->tabBar = new TabBar(this);
    d->tabBar->setFixedHeight(36);

    d->newTabButton = new DIconButton(this);
    d->newTabButton->setObjectName("NewTabButton");
    AC_SET_ACCESSIBLE_NAME(d->newTabButton, AC_VIEW_TAB_BAR_NEW_BUTTON);
    d->newTabButton->setFixedSize(36, 36);
    d->newTabButton->setIconSize({24, 24});
    d->newTabButton->setIcon(QIcon::fromTheme("dfm_tab_new"));
    d->newTabButton->setFlat(true);
    d->newTabButton->hide();

    d->tabTopLine = new DHorizontalLine(this);
    AC_SET_OBJECT_NAME(d->tabTopLine, AC_VIEW_TAB_BAR_TOP_LINE);
    AC_SET_ACCESSIBLE_NAME(d->tabTopLine, AC_VIEW_TAB_BAR_TOP_LINE);

    d->tabBottomLine = new DHorizontalLine(this);
    AC_SET_OBJECT_NAME(d->tabBottomLine, AC_VIEW_TAB_BAR_BOTTOM_LINE);
    AC_SET_ACCESSIBLE_NAME(d->tabBottomLine, AC_VIEW_TAB_BAR_BOTTOM_LINE);

    d->tabTopLine->hide();
    d->tabBottomLine->hide();
}

void DFileManagerWindow::initViewLayout()
{
    D_D(DFileManagerWindow);

    d->viewStackLayout = new QStackedLayout;
    d->viewStackLayout->setSpacing(0);
    d->viewStackLayout->setContentsMargins(0, 0, 0, 0);
}

void DFileManagerWindow::initCentralWidget()
{
    D_D(DFileManagerWindow);
    initSplitter();

    d->centralWidget = new QFrame(this);
    d->centralWidget->setObjectName("CentralWidget");
    AC_SET_ACCESSIBLE_NAME(d->centralWidget, AC_COMPUTER_CENTRAL_WIDGET);
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QWidget *midWidget = new QWidget;
    QHBoxLayout *midLayout = new QHBoxLayout;
    AC_SET_OBJECT_NAME(midWidget, AC_VIEW_MID_WIDGET);
    AC_SET_ACCESSIBLE_NAME(midWidget, AC_VIEW_MID_WIDGET);

    midWidget->setLayout(midLayout);
    //! lixiang start 设置后显示空间会大些
    midLayout->setContentsMargins(0, 0, 0, 0);
    //! lixiang end
    midLayout->addWidget(d->splitter);

    d->rightDetailViewHolder = new QFrame;
    d->rightDetailViewHolder->setObjectName("rightviewHolder");
    AC_SET_ACCESSIBLE_NAME(d->rightDetailViewHolder, AC_DM_RIGHT_VIEW_HOLDER);
    d->rightDetailViewHolder->setAutoFillBackground(true);
    d->rightDetailViewHolder->setBackgroundRole(QPalette::ColorRole::Base);
    d->rightDetailViewHolder->setFixedWidth(320);
    QHBoxLayout *rvLayout = new QHBoxLayout(d->rightDetailViewHolder);
    rvLayout->setMargin(0);

    d->detailView = new DFMRightDetailView(currentUrl());
    QFrame *rightDetailVLine = new QFrame;
    AC_SET_OBJECT_NAME(rightDetailVLine, AC_DM_RIGHT_VIEW_DETAIL_VLINE);
    AC_SET_ACCESSIBLE_NAME(rightDetailVLine, AC_DM_RIGHT_VIEW_DETAIL_VLINE);
    rightDetailVLine->setFrameShape(QFrame::VLine);
    rvLayout->addWidget(rightDetailVLine);
    rvLayout->addWidget(d->detailView, 1);
    midLayout->addWidget(d->rightDetailViewHolder, 1);
    d->rightDetailViewHolder->setVisible(false); //不显示先

    mainLayout->addWidget(midWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    AC_SET_OBJECT_NAME(d->centralWidget, AC_COMPUTER_CENTRAL_WIDGET);
    AC_SET_ACCESSIBLE_NAME(d->centralWidget, AC_COMPUTER_CENTRAL_WIDGET);
    d->centralWidget->setLayout(mainLayout);
}

void DFileManagerWindow::initConnect()
{
    D_D(DFileManagerWindow);

    if (titlebar()) {
        QObject::connect(titlebar(), SIGNAL(minimumClicked()), parentWidget(), SLOT(showMinimized()));
        QObject::connect(titlebar(), SIGNAL(maximumClicked()), parentWidget(), SLOT(showMaximized()));
        QObject::connect(titlebar(), SIGNAL(restoreClicked()), parentWidget(), SLOT(showNormal()));
        QObject::connect(titlebar(), SIGNAL(closeClicked()), parentWidget(), SLOT(close()));
    }

    QObject::connect(fileSignalManager, &FileSignalManager::requestCloseCurrentTab, this, &DFileManagerWindow::closeCurrentTab);

    // 请求关闭窗口所有保险箱的标签
    QObject::connect(fileSignalManager, &FileSignalManager::requestCloseAllTabOfVault,
                     this, &DFileManagerWindow::closeAllTabOfVault);

    QObject::connect(d->tabBar, &TabBar::tabMoved, d->toolbar, &DToolBar::moveNavStacks);
    QObject::connect(d->tabBar, &TabBar::currentChanged, this, &DFileManagerWindow::onCurrentTabChanged);
    QObject::connect(d->tabBar, &TabBar::tabCloseRequested, this, &DFileManagerWindow::onRequestCloseTab);
    QObject::connect(d->tabBar, &TabBar::tabAddableChanged, this, &DFileManagerWindow::onTabAddableChanged);

    QObject::connect(d->tabBar, &TabBar::tabBarShown, this, &DFileManagerWindow::showNewTabButton);
    QObject::connect(d->tabBar, &TabBar::tabBarHidden, this, &DFileManagerWindow::hideNewTabButton);
    QObject::connect(d->newTabButton, &QPushButton::clicked, this, &DFileManagerWindow::onNewTabButtonClicked);

    QObject::connect(fileSignalManager, &FileSignalManager::trashStateChanged, this, &DFileManagerWindow::onTrashStateChanged);
    QObject::connect(fileSignalManager, &FileSignalManager::currentUrlChanged, this, &DFileManagerWindow::onTrashStateChanged);
    QObject::connect(d->tabBar, &TabBar::currentChanged, this, &DFileManagerWindow::onTrashStateChanged);

    QObject::connect(this, &DFileManagerWindow::currentUrlChanged, this, [this, d] {
        DUrl url = currentUrl();

        const DAbstractFileInfoPointer &info = DFileService::instance()->createFileInfo(this, url);

        if (VaultController::isVaultFile(url.toString()))
        {
            // 如果是快捷方式，则赋值为快捷方式的源文件路径，便于正常显示快捷方式的路径
            if (info->isSymLink()) {
                url = info->symLinkTarget();
                url = VaultController::localUrlToVault(url);
            }
        }
        d->tabBar->onCurrentUrlChanged(DFMUrlBaseEvent(this, url));
        emit fileSignalManager->currentUrlChanged(DFMUrlBaseEvent(this, url));

        if (info)
        {
            setWindowTitle(info->fileDisplayName());
        } else if (url.isComputerFile())
        {
            setWindowTitle(systemPathManager->getSystemPathDisplayName("Computer"));
        }
    });

    QObject::connect(fileSignalManager, &FileSignalManager::requestMultiFilesRename, this, &DFileManagerWindow::onShowRenameBar);
    QObject::connect(d->tabBar, &TabBar::currentChanged, this, &DFileManagerWindow::onTabBarCurrentIndexChange);
    QObject::connect(d->toolbar, &DToolBar::detailButtonClicked, this, [d]() {
        if (d->rightDetailViewHolder) {
            d->rightDetailViewHolder->setVisible(!d->rightDetailViewHolder->isVisible());
            qDebug() << "File information window on the right";
        }
    });

    QObject::connect(this, &DFileManagerWindow::selectUrlChanged, this, [d](/*const QList<DUrl> &urlList*/) {
        DFileView *fv = dynamic_cast<DFileView *>(d->currentView);
        if (d->detailView && fv) {
            d->detailView->setUrl(fv->selectedUrls().value(0, fv->rootUrl()));
            if (fv->selectedIndexCount() == 0)
                d->detailView->setTagWidgetVisible(false);
        }
    });

    //! redirect when tab root url changed.
    QObject::connect(fileSignalManager, &FileSignalManager::requestRedirectTabUrl, this, &DFileManagerWindow::onRequestRedirectUrl);
    QObject::connect(fileSignalManager, &FileSignalManager::requestCloseTab, this, &DFileManagerWindow::onRequestCloseTabByUrl);

    if (DFMGlobal::isTablet() && d->windowIconButton) {
        QObject::connect(d->windowIconButton, &DIconButton::clicked, this, [this](){
            this->cd(DUrl::fromComputerFile("/"));
        });
    }
}

void DFileManagerWindow::moveCenterByRect(QRect rect)
{
    QRect qr = frameGeometry();
    qr.moveCenter(rect.center());
    move(qr.topLeft());
}


void DFileManagerWindow::onShowRenameBar(const DFMUrlListBaseEvent &event) noexcept
{
    DFileManagerWindowPrivate *const d { d_func() };

    if (event.windowId() == this->windowId()) {
        d->storeUrlListToRenameBar(event.urlList()); //### get the urls of selection.

        m_currentTab = d->tabBar->currentTab();
        d->setRenameBarVisible(true);
    }
}

void DFileManagerWindow::onTabBarCurrentIndexChange(const int &index)noexcept
{
    DFileManagerWindowPrivate *const d{ d_func() };

    if (m_currentTab != d->tabBar->tabAt(index)) {

        if (d->isRenameBarVisible() == true) {
            this->onReuqestCacheRenameBarState();//###: invoke this function before setVisible.

            hideRenameBar();
        }
    }
}

void DFileManagerWindow::hideRenameBar() noexcept //###: Hide renamebar and then clear history.
{
    DFileManagerWindowPrivate *const d{ d_func() };

    d->setRenameBarVisible(false);
    d->resetRenameBar();
}


void DFileManagerWindow::onReuqestCacheRenameBarState()const
{
    const DFileManagerWindowPrivate *const d{ d_func() };
    DFileManagerWindow::renameBarState = d->renameBar->getCurrentState();//###: record current state, when a new window is created from a already has tab.
}

void DFileManagerWindow::onRequestRedirectUrl(const DUrl &tabRootUrl, const DUrl &newUrl)
{
    D_D(DFileManagerWindow);

    int curIndex = d->tabBar->currentIndex();
    int tabCount = d->tabBar->count();
    if (tabCount > 1) {
        for (int i = 0; i < tabCount; i++) {
            Tab *tab = d->tabBar->tabAt(i);
            if (tabRootUrl == tab->fileView()->rootUrl()) {
                onRequestCloseTab(i, false);
                openNewTab(newUrl);
                d->tabBar->setCurrentIndex(curIndex);
            }
        }
    }
}

void DFileManagerWindow::onRequestCloseTabByUrl(const DUrl &rootUrl)
{
    D_D(DFileManagerWindow);
    if (rootUrl.toString() == TRASH_ROOT) {
        return;
    }
    TabBar *tabBar = d->tabBar;
    if (tabBar->count() <= 1) {
        return;
    }
    int originIndex = tabBar->currentIndex();
    for (int i = tabBar->count()-1; i >= 0 && tabBar->count() > 1; i--) {
        Tab *tab = tabBar->tabAt(i);
        if (tab->fileView()) {
            DUrl tabUrl = tab->fileView()->rootUrl();
            if (FileUtils::isAncestorUrl(rootUrl, tabUrl)) {
                onRequestCloseTab(i, false);
            }
        }
    }
    int curIndex = tabBar->currentIndex();
    if (curIndex != originIndex) {
        if (originIndex < tabBar->count()) {
            tabBar->setCurrentIndex(originIndex);
        }
    }
}

void DFileManagerWindow::showEvent(QShowEvent *event)
{
    DMainWindow::showEvent(event);

    const QVariantMap &state = DFMApplication::appObtuselySetting()->value("WindowManager", "SplitterState").toMap();
    int splitterPos = state.value("sidebar", DFMSideBar::maximumWidth).toInt();
    setSplitterPosition(splitterPos);
}

void DFileManagerWindow::initRenameBarState()
{
    DFileManagerWindowPrivate *const d{ d_func() };

    bool expected{ true };
    ///###: CAS, when we draged a tab to leave TabBar for creating a new window.
    if (DFileManagerWindow::flagForNewWindowFromTab.compare_exchange_strong(expected, false, std::memory_order_seq_cst)) {

        if (static_cast<bool>(DFileManagerWindow::renameBarState) == true) { //###: when we drag a tab to create a new window, but the RenameBar is showing in last window.
            //多标签情况下，renamebar可能存在隐藏的情况，在新窗口中打开目录会出现renamebar指针失效的问题
            //这种情况下需要跳过loadState
            if (!d->renameBar || !d->renameBar->isVisible())
                return;
            d->renameBar->loadState(DFileManagerWindow::renameBarState);

        } else { //###: when we drag a tab to create a new window, but the RenameBar is hiding.
            d->setRenameBarVisible(false);
        }

    } else { //###: when open a new window from right click menu.
        d->setRenameBarVisible(false);
    }
}


void DFileManagerWindow::requestToSelectUrls()
{
    DFileManagerWindowPrivate *const d{ d_func() };

    //多标签情况下，renamebar可能存在隐藏的情况，在新窗口中打开目录会出现renamebar指针失效的问题
    //这种情况下需要跳过loadState
    if (!d->renameBar || !d->renameBar->isVisible())
        return;

    if (static_cast<bool>(DFileManagerWindow::renameBarState) == true) {
        d->renameBar->loadState(DFileManagerWindow::renameBarState);

        QList<DUrl> selectedUrls{ DFileManagerWindow::renameBarState->getSelectedUrl() };
        quint64 winId{ this->windowId() };
        DFMUrlListBaseEvent event{ nullptr,  selectedUrls};
        event.setWindowId(winId);

        QTimer::singleShot(100, [ = ] { emit fileSignalManager->requestSelectFile(event); });

        DFileManagerWindow::renameBarState.reset(nullptr);
    }
}

bool DFileManagerWindow::isAdvanceSearchBarVisible()
{
    Q_D(DFileManagerWindow);

    return d->isAdvanceSearchBarVisible();
}

void DFileManagerWindow::toggleAdvanceSearchBar(bool visible, bool resetForm)
{
    Q_D(DFileManagerWindow);

    if (!d->currentView) return;

    if (d->isAdvanceSearchBarVisible() != visible) {
        d->setAdvanceSearchBarVisible(visible);
    }

    if (d->advanceSearchBar && resetForm) {
        d->advanceSearchBar->resetForm(!visible);
    }
}

void DFileManagerWindow::showFilterButton()
{
    Q_D(DFileManagerWindow);

    d->toolbar->showFilterButton();
}
