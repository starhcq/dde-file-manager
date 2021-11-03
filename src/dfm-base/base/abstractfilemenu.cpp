#include "abstractfilemenu.h"

#include <QAction>
#include <QMenu>

DFMBASE_BEGIN_NAMESPACE

namespace FileMenuTypes{
const QString CREATE_NEW_DIR {QObject::tr("Create New Dir")};
const QString CREATE_NEW_DOC {QObject::tr("Create New Doc")};
const QString VIEW_MODE_SWITCH {QObject::tr("Switch View Mode")};
const QString VIEW_SORT_SWITCH {QObject::tr("Switch View Sort")};
const QString OPEN_AS_ADMIN {QObject::tr("Open As Admin")};
const QString OPEN_IN_TERMINAL {QObject::tr("Open In Terminal")};
const QString SELECT_ALL {QObject::tr("Select All")};
const QString PROPERTY {QObject::tr("Property")};
const QString CUT {QObject::tr("Cut")};
const QString COPY {QObject::tr("Copy")};
const QString PASTE {QObject::tr("Paste")};
const QString RENAME {QObject::tr("Rename")};
} // namespace FileMenuTypes

AbstractFileMenu::AbstractFileMenu(QObject *parent)
    : QObject (parent)
{

}

AbstractFileMenu::~AbstractFileMenu()
{

}

QMenu *AbstractFileMenu::build(AbstractFileMenu::MenuMode mode,
                               const QUrl &rootUrl,
                               const QUrl &foucsUrl,
                               const QList<QUrl> &selected)
{
    Q_UNUSED(rootUrl)
    Q_UNUSED(foucsUrl)
    Q_UNUSED(selected)

    if (AbstractFileMenu::MenuMode::Empty == mode) {
        QMenu *menu = new QMenu(nullptr);
        menu->addAction(FileMenuTypes::CREATE_NEW_DIR);
        menu->addAction(FileMenuTypes::CREATE_NEW_DOC);
        menu->addAction(FileMenuTypes::VIEW_MODE_SWITCH);
        menu->addAction(FileMenuTypes::VIEW_SORT_SWITCH);
        menu->addAction(FileMenuTypes::OPEN_AS_ADMIN);
        menu->addAction(FileMenuTypes::OPEN_IN_TERMINAL);
        menu->addAction(FileMenuTypes::SELECT_ALL);
        menu->addAction(FileMenuTypes::PROPERTY);
        return menu;
    }

    if (AbstractFileMenu::MenuMode::Normal == mode) {
        QMenu *menu = new QMenu(nullptr);
        menu->addAction(FileMenuTypes::CREATE_NEW_DIR);
        menu->addAction(FileMenuTypes::CREATE_NEW_DOC);
        menu->addAction(FileMenuTypes::VIEW_MODE_SWITCH);
        menu->addAction(FileMenuTypes::VIEW_SORT_SWITCH);
        menu->addAction(FileMenuTypes::OPEN_AS_ADMIN);
        menu->addAction(FileMenuTypes::OPEN_IN_TERMINAL);
        menu->addAction(FileMenuTypes::SELECT_ALL);
        menu->addAction(FileMenuTypes::PROPERTY);
        return menu;
    }

    return nullptr;
}

DFMBASE_END_NAMESPACE