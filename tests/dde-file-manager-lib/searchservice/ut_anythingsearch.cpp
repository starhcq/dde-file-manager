/*
* Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
*
* Author:     gongheng <gongheng@uniontech.com>
*
* Maintainer: zhengyouge <zhengyouge@uniontech.com>
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

#include <QStandardPaths>
#include <QUuid>
#include <QDir>

#include <gtest/gtest.h>

#define private public
#include "searcher/anything/anythingsearcher.h"

namespace {
class TestAnythingSearcher : public testing::Test
{
public:
    static void SetUpTestCase()
    {
        DUrl targetUrl("");
        QStringList paths = QStandardPaths::standardLocations(QStandardPaths::TempLocation);
        if (!paths.isEmpty()) {
            QString dirName = QUuid::createUuid().toString(QUuid::WithoutBraces);
            filePath = paths.at(0) + QDir::separator() + dirName;
            QDir dir;
            dir.mkdir(filePath);
            targetUrl = DUrl::fromLocalFile(filePath);
        }
        QString keyWord = "123qweasdzxc";
        bool dataFlag;
        search = new AnythingSearcher(targetUrl, keyWord, dataFlag);
    }

    static void TearDownTestCase()
    {
        QDir dir(filePath);
        if (dir.exists())
            dir.rmpath(filePath);
    }

    void SetUp() override
    {
        std::cout << "start TestSearchService" << std::endl;
    }

    void TearDown() override
    {
        std::cout << "end TestSearchService" << std::endl;
    }

public:
    static AnythingSearcher *search;
    static QString filePath;
};

AnythingSearcher *TestAnythingSearcher::search = { nullptr };
QString TestAnythingSearcher::filePath = "";
}

TEST_F(TestAnythingSearcher, tst_search) {
    EXPECT_NO_FATAL_FAILURE(search->search());
}

TEST_F(TestAnythingSearcher, tst_stop) {
    EXPECT_NO_FATAL_FAILURE(search->stop());
}

TEST_F(TestAnythingSearcher, tst_hasItem) {
    EXPECT_NO_FATAL_FAILURE(search->hasItem());
}

TEST_F(TestAnythingSearcher, tst_takeAll) {
    EXPECT_NO_FATAL_FAILURE(search->takeAll());
}

TEST_F(TestAnythingSearcher, tst_isSupported) {
    DUrl url = DUrl("file://" + filePath);
    bool isPrependData;
    EXPECT_NO_FATAL_FAILURE(AnythingSearcher::isSupported(url, isPrependData));
}

TEST_F(TestAnythingSearcher, tst_tryNotify) {
    EXPECT_NO_FATAL_FAILURE(search->tryNotify());
}

TEST_F(TestAnythingSearcher, tst_hasSymLinkDirInSearchPath) {
    AnythingSearcher::SearchInfo info;
    EXPECT_NO_FATAL_FAILURE(search->hasSymLinkDirInSearchPath(filePath, info));
}