// SPDX-License-Identifier: MPL-2.0
/**
 * Copyright 2023 CARIAD SE.
 * Copyright 2024 ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _UTIL_H__
#define _UTIL_H__

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sys/stat.h>  // explicit include necessary for Linux support
#include <sys/types.h> // explicit include necessary for Linux support
#include <vector>
#include <wchar.h>

#include "softwareVersion.h"

// Necessary for Linux support
#ifndef FLT_MAX
#define FLT_MAX __FLT_MAX__
#endif

typedef std::pair<float, float> tBoundary;
typedef std::map<tBoundary, std::vector<float>> tHistogram;

// Returns the file sze of a document
std::ifstream::pos_type GetFileSize(const char *filename);

// Returns the filename from a given path.
void GetFileName(std::string *strFilePath, const bool bRemoveExtension);

// Checks if a char is a double
bool IsDouble(char i_chars[]);

// True if strings match
bool Equals(const std::string &in1, const std::string &in2);

// True if strings match
bool Equals(const wchar_t *in1, const wchar_t *in2);

/*
  Computes a histogram out of given boundaries and values.
  \param boundaries Given boundaries for the input vector. Have to be at least 2 values.
            Format: { 1.0f, 2.0f, 3.0f } => [1.0f, 2.0f],  [2.0f, 3.0f]
  \param values Values as vector
*/
tHistogram ComputeHistogram(const std::vector<float> boundaries, const std::vector<float> values);

// Prints a histogram to a text
std::string PrintHistogram(const tHistogram &histogram);

// Returns true if a value is in a given boundary
bool InBound(const tBoundary boundary, const float value);

/*
  Prints a decimal value as a string
  \param value value to print
  \param precision The number of decimal points
*/
template <typename T> std::string PrintDecimalValue(const T value, const int precision = 2)
{
    std::ostringstream out;
    out.setf(std::ios::fixed);
    out.precision(precision);
    out << value;
    return out.str();
}

// Returns true if a string has a certain ending
bool StringEndsWith(const std::string &value, const std::string &ending);

/*
Simple method to concatenate strings
*/
template <typename T> std::string ToString(T val)
{
    std::stringstream out;
    out << val;
    return out.str();
}

/*
Simple method to concatenate strings
*/
template <typename T, typename... Args> std::string ToString(T val, Args... args)
{
    // recursive variadic function
    std::stringstream out;
    out << val << ToString(args...);
    return out.str();
}

// Joins a collection by some string and returns the result.
template <typename T> std::string Join(const T &v, const std::string &delim)
{
    std::ostringstream s;
    for (const auto &i : v)
    {
        if (&i != &v[0])
        {
            s << delim;
        }
        s << i;
    }
    return s.str();
}

// Converts a string to lower case
std::string ToLower(const std::string &source);

// Converts a string to upper case
std::string ToUpper(const std::string &source);

// Checks if a file at a certain path exists
bool CheckIfFileExists(std::string filePath, const bool printLog = true);

// Replaced a string in anorther string
bool Replace(std::string &str, const std::string &from, const std::string &to);

// Returns true if the string is a number
bool IsNumber(const std::string &input);

// parse an XOSC file to get the XODR path
bool GetXodrFilePathFromXosc(const std::string xoscFilePath, std::string &strPathOut);

#endif
