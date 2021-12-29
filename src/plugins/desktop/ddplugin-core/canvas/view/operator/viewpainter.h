/*
 * Copyright (C) 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangyu<zhangyub@uniontech.com>
 *
 * Maintainer: zhangyu<zhangyub@uniontech.com>
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
#ifndef VIEWPAINTER_H
#define VIEWPAINTER_H

#include "view/canvasview_p.h"
#include "delegate/canvasitemdelegate.h"

#include <QPainter>

DSB_D_BEGIN_NAMESPACE

class ViewPainter : public QPainter
{
public:
    explicit ViewPainter(CanvasViewPrivate *dd);

public:
    inline CanvasView *view() const {
        return d->q;
    }

    inline CanvasModel *model() const {
        return d->q->model();
    }

    inline CanvasSelectionModel *selectionModel() const {
        return d->q->selectionModel();
    }

    inline CanvasItemDelegate *itemDelegate() const {
        return d->q->itemDelegate();
    }
public:
    void paintFiles(QStyleOptionViewItem option, QPaintEvent *event);
    void drawGirdInfos();
    void drawFile(QStyleOptionViewItem option, const QModelIndex &index, const QPoint &gridPos);
    void drawSelectRect();

    void drawDodge();
    void drawDragMove(QStyleOptionViewItem &option);
protected:
    CanvasViewPrivate *d = nullptr;
};

DSB_D_END_NAMESPACE

#endif // VIEWPAINTER_H