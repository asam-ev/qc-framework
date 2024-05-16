/**
 * Copyright 2023 CARIAD SE.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef c_osc_schema_checker_h__
#define c_osc_schema_checker_h__

#include "c_parser_error_handler.h"
#include "common/qc4openx_filesystem.h"
#include "common/result_format/c_checker.h"
#include "common/result_format/c_checker_bundle.h"
#include "common/result_format/c_file_location.h"
#include "common/result_format/c_locations_container.h"
#include "common/result_format/c_result_container.h"
#include "common/util.h"

#include <iostream>
#include <list>
#include <map>
#include <sstream>

class OSCSchemaChecker
{
  private:
    cResultContainer *pResultContainer;
    cCheckerBundle *pXSDCheckerBundle;
    std::string checkerBundleName;
    cParameterContainer inputParams;
    std::string xoscFilePath;
    std::map<std::string, std::list<fs::path>> versionToXsdFile;
    std::list<std::string> unsupportedVersions;

  public:
    OSCSchemaChecker()
    {
    }
    OSCSchemaChecker(const std::string &name, cParameterContainer params,
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
    bool ValidateXSD(const std::list<fs::path> &lSchemaFiles);
    bool ExtractXOSCVersion(unsigned *i_uRevMajor, unsigned *i_uRevMinor);
    void WriteEmptyReport();
};

#endif