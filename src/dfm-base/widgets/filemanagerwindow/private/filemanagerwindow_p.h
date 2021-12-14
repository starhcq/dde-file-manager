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
#ifndef FILEMANAGERWINDOW_P_H
#define FILEMANAGERWINDOW_P_H

#include "dfm-base/dfm_base_global.h"

#include <DTitlebar>
#include <DButtonBox>

#include <QObject>
#include <QUrl>
#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE
DFMBASE_BEGIN_NAMESPACE

class FileManagerWindow;
class FileManagerWindowPrivate : public QObject
{
    Q_OBJECT
    friend class FileManagerWindow;
    FileManagerWindow *const q;

public:
    explicit FileManagerWindowPrivate(const QUrl &url, FileManagerWindow *qq);

private:
    QUrl currentUrl;
    static constexpr int kMinimumWindowWidth = 760;
    static constexpr int kMinimumWindowHeight = 420;
    static constexpr int kDefaultWindowWidth = 1100;
    static constexpr int kDefaultWindowHeight = 700;
};

DFMBASE_END_NAMESPACE

#endif   // FILEMANAGERWINDOW_P_H