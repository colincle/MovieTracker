#pragma once

#include "Title.hpp"

#include <vector>
#include <QString>

class AppStorage
{
public:
    AppStorage();
    void setOmdbApiKey(QString key);

    QString getKey() {return omdbApiKey;}

    private:
    QString appFilePath;
    QString omdbApiKey;
    std::vector<Title> titles;

    void load();
};