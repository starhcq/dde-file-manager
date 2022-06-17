/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     xushitong<xushitong@uniontech.com>
 *
 * Maintainer: max-lv<lvwujun@uniontech.com>
 *             lanxuesong<lanxuesong@uniontech.com>
 *             zhangsheng<zhangsheng@uniontech.com>
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
#ifndef DFMPLUGIN_AVFSBROWSER_GLOBAL_H
#define DFMPLUGIN_AVFSBROWSER_GLOBAL_H

#define DPAVFSBROWSER_NAMESPACE dfmplugin_avfsbrowser

#define DPAVFSBROWSER_BEGIN_NAMESPACE namespace DPAVFSBROWSER_NAMESPACE {
#define DPAVFSBROWSER_END_NAMESPACE }
#define DPAVFSBROWSER_USE_NAMESPACE using namespace DPAVFSBROWSER_NAMESPACE;

DPAVFSBROWSER_BEGIN_NAMESPACE

namespace AvfsMenuActionId {
inline constexpr char kOpen[] { "act-avfs-open" };
inline constexpr char kCopy[] { "act-avfs-copy" };
inline constexpr char kProperty[] { "act-avfs-property" };
}

DPAVFSBROWSER_END_NAMESPACE

#endif   // DFMPLUGIN_AVFSBROWSER_GLOBAL_H
