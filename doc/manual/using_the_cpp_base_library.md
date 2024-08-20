<!---
Copyright 2024 ASAM e.V.

This Source Code Form is subject to the terms of the Mozilla
Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
-->

# Using the C++ Base Library

It is recommended to use the Python Base Library if possible as it contains the most advanced development. The C++ Base Library is complete in terms of functionality, but it is not as easy to use as the Python Base Library.

## Installation

To install the base library it is necessary to clone the qc-framework repository

```
git clone https://github.com/asam-ev/qc-framework.git
```

And create a folder in the `examples` directory. E.g. `tutorial_example`

For executing the examples below, you need a `tutorial_example/src/main.cpp` file and `tutorial_example/CMakeLists.txt` that links your main logic with the base library as follows:

```
cmake_minimum_required(VERSION 3.16 FATAL_ERROR) # CMake policy CMP0095

set(TUTORIAL_EXAMPLE TutorialExample)
project(${TUTORIAL_EXAMPLE})

set(CMAKE_INSTALL_RPATH $ORIGIN/../lib)

add_executable(${TUTORIAL_EXAMPLE}
    src/main.cpp
)

target_link_libraries(${TUTORIAL_EXAMPLE} PRIVATE qc4openx-common $<$<PLATFORM_ID:Linux>:stdc++fs>)
install(TARGETS ${TUTORIAL_EXAMPLE} DESTINATION examples/tutorial_example/bin)
qc4openx_install_qt(examples/tutorial_example/bin ${TUTORIAL_EXAMPLE}.exe)

file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/src"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/examples/tutorial_example
    )

set_target_properties(${TUTORIAL_EXAMPLE} PROPERTIES FOLDER examples/checker_bundles)
```

Then in the main `examples/CMakeLists.txt` you can add the new folder created

```
add_subdirectory(tutorial_example)
```

Then from the `qc-framework` folder you can build the new tutorial with the command:

```
cmake -G "Unix Makefiles" -B./build -S . -DCMAKE_INSTALL_PREFIX="/home/$USER/qc-build" \
    -DENABLE_FUNCTIONAL_TESTS=ON  \
    -DQt5_DIR="/usr/lib/x86_64-linux-gnu/cmake/Qt5/" \
    -DQt5XmlPatterns_DIR="/usr/lib/x86_64-linux-gnu/cmake/Qt5XmlPatterns/"

cmake --build ./build --target install --config Release -j4

cmake --install ./build
```

After these commands you will find your executable in `/home/$USER/qc-build/examples/tutorial_example/bin/TutorialExample`

## Examples

#### Creating a checker bundle, checker, issue and write results


```cpp
   #include "common/result_format/c_issue.h"

   #define CHECKER_BUNDLE_NAME "TutorialBundle"

   // Define a result container which contains our results.
   cResultContainer pResultContainer;

   // Lets go on with the checker bundle. A bundle contains 0 or more checks.
   cCheckerBundle *pExampleCheckerBundle = new cCheckerBundle(CHECKER_BUNDLE_NAME);

   // Add the checkerBundle to our results. You can do it later as well.
   pResultContainer.AddCheckerBundle(pExampleCheckerBundle);

   // Create a checker with a factory in the checker bundle
   cChecker *pTutorialChecker = pExampleCheckerBundle->CreateChecker("utorialChecker", "This is a description");
   // Lets add now an issue
   pTutorialChecker->AddIssue(new cIssue("This is an information from the demo usecase", INFO_LVL));

   // And now just write the report.
   pResultContainer.WriteResults("tutorial_result.xqar");
   pResultContainer.Clear();
```

#### Add information to checker and issues

**Create a test checker with an inertial location**

```cpp
   cChecker *pExampleInertialChecker =
      pExampleCheckerBundle->CreateChecker("exampleInertialChecker", "This is a description of inertial checker");
   std::list<cLocationsContainer *> listLoc;
   listLoc.push_back(new cLocationsContainer("inertial position", new cInertialLocation(1.0, 2.0, 3.0)));
   pExampleInertialChecker->AddIssue(new cIssue("This is an information from the demo usecase", INFO_LVL, listLoc));
```

**Create a test checker with RuleUID and metadata**

```cpp
    cChecker *pExampleRuleUIDChecker =
        pExampleCheckerBundle->CreateChecker("exampleRuleUIDChecker", "This is a description of ruleUID checker");
    pExampleRuleUIDChecker->AddRule(new cRule("test.com::qwerty.qwerty"));
    pExampleRuleUIDChecker->AddMetadata(
        new cMetadata("run date", "2024/06/06", "Date in which the checker was executed"));
    pExampleRuleUIDChecker->AddMetadata(
        new cMetadata("reference project", "project01", "Name of the project that created the checker"));

```

**Create a test checker with Issue and RuleUID**
```cpp
    cChecker *pExampleIssueRuleChecker = pExampleCheckerBundle->CreateChecker(
        "exampleIssueRuleChecker", "This is a description of checker with issue and the involved ruleUID");

    pExampleIssueRuleChecker->AddIssue(
        new cIssue("This is an information from the demo usecase", ERROR_LVL, "test.com::qwerty.qwerty"));
```

**Create a test checker with domain specific info**
```cpp
    cChecker *pExampleDomainChecker = pExampleCheckerBundle->CreateChecker(
        "exampleDomainChecker", "This is a description of example domain info checker");
    std::list<cDomainSpecificInfo *> listDomainSpecificInfo;

    std::string xmlString =
        "<DomainSpecificInfo name=\"test_domain\"><RoadLocation id=\"aa\" b=\"5.4\" c=\"0.0\"/></DomainSpecificInfo>";

    listDomainSpecificInfo.push_back(new cDomainSpecificInfo(getRootFromString(xmlString), "domain info test"));
    pExampleDomainChecker->AddIssue(
        new cIssue("This is an information from the demo usecase", INFO_LVL, listDomainSpecificInfo));
```
