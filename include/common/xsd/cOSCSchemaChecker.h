/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef cOSCSchemaChecker_h__
#define cOSCSchemaChecker_h__

#include "cParserErrorHandler.h"
#include "common/qc4openx_filesystem.h"
#include "common/result_format/cChecker.h"
#include "common/result_format/cCheckerBundle.h"
#include "common/result_format/cFileLocation.h"
#include "common/result_format/cLocationsContainer.h"
#include "common/result_format/cResultContainer.h"
#include "common/util.h"

#include <iostream>
#include <list>
#include <map>
#include <sstream>

class OSCSchemaChecker {
private:
    cResultContainer* pResultContainer;
    cCheckerBundle* pXSDCheckerBundle;
    std::string checkerBundleName;
    cParameterContainer inputParams;
    std::string xoscFilePath;
    std::map<std::string, std::list<fs::path>> versionToXsdFile;
    std::list<std::string> unsupportedVersions;

public:
    OSCSchemaChecker()
    {
    }
    OSCSchemaChecker(const std::string& name,
                     cParameterContainer params,
                     std::map<std::string, std::list<fs::path>> versionMap)
        : checkerBundleName(name), inputParams(params), versionToXsdFile(versionMap)
    {
        Init();
    }
    ~OSCSchemaChecker()
    {
    }
    void Init();
    bool RunChecks();
    bool ValidateXSD(const std::list<fs::path>& lSchemaFiles);
    bool ExtractXOSCVersion(unsigned* i_uRevMajor, unsigned* i_uRevMinor);
    void WriteEmptyReport();
};

#endif