/*
 * Copyright (C) 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangsheng<zhangsheng@uniontech.com>
 *
 * Maintainer: max-lv<lvwujun@uniontech.com>
 *             lanxuesong<lanxuesong@uniontech.com>
 *             xushitong<xushitong@uniontech.com>
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
#ifndef WORKSPACEWIDGET_H
#define WORKSPACEWIDGET_H

#include "dfm-base/interfaces/abstractframe.h"

#include <QUrl>

class FileView;
class QVBoxLayout;
class QStackedLayout;
class WorkspaceWidget : public dfmbase::AbstractFrame
{
    Q_OBJECT
public:
    explicit WorkspaceWidget(QFrame *parent = nullptr);
    void setCurrentUrl(const QUrl &url) override;
    QUrl currentUrl() const override;

private:
    void initializeUi();
    void initConnect();
    void initTabBar();
    void initViewLayout();

private:
    QUrl workspaceUrl;
    QVBoxLayout *widgetLayout { nullptr };
    QStackedLayout *viewStackLayout { nullptr };
    FileView *fileView { nullptr };
};

#endif   // WORKSPACEWIDGET_H