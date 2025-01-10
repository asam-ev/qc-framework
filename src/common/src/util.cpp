// SPDX-License-Identifier: MPL-2.0
/*
 * Copyright 2023 CARIAD SE.
 * Copyright 2024 ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common/util.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtXml/QDomDocument>

#include "common/result_format/c_file_location.h"

#include <iostream>

void GetFileName(std::string *strFilePath, const bool bRemoveExtension)
{
    if (strFilePath->size() < 1)
        return;

    // Remove directory if present.
    // Do this before extension removal incase directory has a period character.
    const size_t last_slash_idx = strFilePath->find_last_of("\\/");
    if (std::string::npos != last_slash_idx)
    {
        strFilePath->erase(0, last_slash_idx + 1);
    }

    if (bRemoveExtension)
    {
        // Remove extension if present.
        const size_t period_idx = strFilePath->rfind('.');
        if (std::string::npos != period_idx)
        {
            strFilePath->erase(period_idx);
        }
    }
}

// Returns the file size of a document
std::ifstream::pos_type GetFileSize(const char *filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

// true wenn strings identisch
bool Equals(const std::string &in1, const std::string &in2)
{
    return !in1.compare(in2);
}

// True if strings match
bool Equals(const wchar_t *in1, const wchar_t *in2)
{
    return wcscmp(in1, in2) == 0;
}

bool IsDouble(char i_chars[])
{
    int i = 0;
    int decimalPoint = 0;
    for (; i_chars[i] != 0; i++)
    {
        if (i_chars[i] == '.')
        {
            if (decimalPoint == 1)
            {
                return false;
            }
            decimalPoint = decimalPoint + 1;
        }
        else
        {
            if (!isdigit(i_chars[i]))
            {
                return false;
            }
        }
    }
    return true;
}

bool StringEndsWith(std::string const &value, std::string const &ending)
{
    if (ending.size() > value.size())
    {
        return false;
    }
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

bool InBound(const tBoundary boundary, const float value)
{
    return ((value >= boundary.first) && (value < boundary.second));
}

tHistogram ComputeHistogram(const std::vector<float> boundaries, const std::vector<float> values)
{
    std::map<std::pair<float, float>, std::vector<float>> computedHistogramm;

    if (boundaries.size() < 2 || values.size() < 1)
    {
        return std::map<std::pair<float, float>, std::vector<float>>();
    }

    float firstBoundary = boundaries.front();

    // Iterate bounds, begin at bounds + 1 because we already have first entry.
    for (std::vector<float>::const_iterator currentUpperBound = boundaries.begin() + 1;
         currentUpperBound != boundaries.end(); currentUpperBound++)
    {
        std::pair<float, float> currentBound(firstBoundary, *currentUpperBound);
        float vCount = 0.0f;
        float vSum = 0.0f;

        for (std::vector<float>::const_iterator currentValue = values.begin(); currentValue != values.end();
             currentValue++)
        {
            if (InBound(currentBound, *currentValue))
            {
                vCount++;
                vSum += *currentValue;
            }
        }

        firstBoundary = *currentUpperBound;

        computedHistogramm[currentBound] = std::vector<float>{vCount, vCount / values.size(), vSum};
    }

    return computedHistogramm;
}

std::string PrintHistogram(const tHistogram &histogram)
{
    std::stringstream ssMapResultString;

    if (histogram.size() == 0)
    {
        return "no entries.";
    }

    ssMapResultString.setf(std::ios::fixed);
    ssMapResultString.precision(2);
    for (std::map<std::pair<float, float>, std::vector<float>>::const_iterator it = histogram.begin();
         it != histogram.end(); it++)
    {
        std::pair<float, float> key(it->first);
        ssMapResultString << "[" << ((key.first == FLT_MAX) ? "inf" : PrintDecimalValue<float>(key.first, 0)) << ","
                          << ((key.second == FLT_MAX) ? "inf" : PrintDecimalValue<float>(key.second, 0))
                          << ") = " << PrintDecimalValue<float>(it->second.at(0), 0) << "(" << it->second.at(1) * 100.0f
                          << "%); ";
    }
    return ssMapResultString.str();
}

std::string ToLower(const std::string &source)
{
    std::string result;
    result.resize(source.size());

    std::transform(source.begin(), source.end(), result.begin(), ::tolower);

    return result;
}

std::string ToUpper(const std::string &source)
{
    std::string result;
    result.resize(source.size());

    std::transform(source.begin(), source.end(), result.begin(), ::toupper);

    return result;
}

bool CheckIfFileExists(std::string filePath, const bool printLog)
{
    struct stat fileStatus;

    if (stat(filePath.c_str(), &fileStatus) == -1) // ==0 ok; ==-1 error
    {
        if (printLog)
            std::cout << "File " << filePath << " could not be parsed. Please check file path and if file exists"
                      << std::endl;

        return false;
    }
    return true;
}

bool Replace(std::string &str, const std::string &from, const std::string &to)
{
    if (from.empty())
        return false;

    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
    return true;
}

bool IsNumber(const std::string &input)
{
    long double ld{0};
    return ((std::istringstream(input) >> ld >> std::ws).eof() && !input.empty());
}

bool GetXodrFilePathFromXosc(const std::string xoscFilePath, std::string &strPathOut)
{
    QDomDocument xmlBOM;
    QString xodrFilePath = "";
    QString sceneGraphFileName = "";
    std::string basicErrorMsg = "Could not retrieve xodr path: ";

    if (xoscFilePath.empty())
    {
        std::cerr << basicErrorMsg << "Invalid XOSC file path." << std::endl;
        return false;
    }

    // Load xml file as raw data
    QFile f(xoscFilePath.c_str());
    if (!f.open(QIODevice::ReadOnly))
    {
        // Error while loading file
        std::cerr << basicErrorMsg << "Error while loading file" << std::endl;
        return false;
    }
    // Set data into the QDomDocument before processing
    xmlBOM.setContent(&f);
    f.close();

    // Extract the root markup
    QDomElement root = xmlBOM.documentElement();

    for (int i = 0; i < root.childNodes().count(); ++i)
    {
        QDomElement element = root.childNodes().item(i).toElement();
        if (!element.isNull())
        {
            if (element.tagName().toLower() == "roadnetwork")
            {
                // Get the first entry of the child
                QDomElement Entry = element.firstChild().toElement();

                // Read each entry of the child node
                while (!Entry.isNull())
                {
                    if (Entry.tagName().toLower() == "logics" ||
                        Entry.tagName().toLower() == "logicfile") // XOSC 0.9 & 1.0
                        xodrFilePath = Entry.attribute("filepath", "");
                    if (Entry.tagName().toLower() == "scenegraph" ||
                        Entry.tagName().toLower() == "scenegraphfile") // XOSC 0.9 & 1.0
                        sceneGraphFileName = Entry.attribute("filepath", "");

                    // Next Entry
                    Entry = Entry.nextSibling().toElement();
                }
                break;
            }
        }
    }

    if (xodrFilePath.isEmpty())
    {
        std::cerr << basicErrorMsg << "xodr file is not specified in xosc file." << std::endl;
        return false;
    }

    QString absoluteXodrFile = xodrFilePath;

    if (!QFileInfo(xodrFilePath).isAbsolute())
    {
        QDir xoscDir(QFileInfo(xoscFilePath.c_str()).canonicalPath());
        absoluteXodrFile = xoscDir.absoluteFilePath(xodrFilePath);
    }

    strPathOut = absoluteXodrFile.toStdString();

    return true;
}
