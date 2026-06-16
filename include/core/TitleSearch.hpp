#pragma once

#include "SortEnums.hpp"
#include "Title.hpp"

#include <vector>
#include <QString>

std::vector<Title> scoreAndRankTitles(const std::vector<Title> &titles, const QString &query);
std::vector<Title> filterTitles(const std::vector<Title> &titles, LibraryTab tab, ViewFilter filter);
void sortTitles(std::vector<Title> &titles, SortMode mode);
