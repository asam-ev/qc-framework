// SPDX-License-Identifier: MPL-2.0
/*
 * Copyright 2023 CARIAD SE.
 * Copyright 2024 ASAM e.V.
 *
 * This Source Code Form is subject to the terms of the Mozilla
 * Public License, v. 2.0. If a copy of the MPL was not distributed
 * with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "common/result_format/c_issue.h"
#include "common/result_format/c_locations_container.h"
#include "viewer/i_connector.h"
#include <cstring>
#include <stdio.h>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QList>
#include <QtCore/QSettings>
#include <QtCore/QProcess>
#include <QtXml/QDomDocument>
#include <QtXmlPatterns/QXmlQuery>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/DOMLSOutput.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>

XERCES_CPP_NAMESPACE_USE

const char *lasterrormsg = "";
QString current_input_path = "";
QString current_format = "";
QString current_format_name = "";
QString current_start_executable = "";
QString current_start_template = "";
QString current_show_location_xpath = "";
QString current_show_executable = "";
QString current_show_template = "";
QString current_stop_executable = "";
QString current_stop_template = "";

QList<QProcess *> current_processes;

// Internal Helpers

QSettings GetSettings()
{
#ifdef USE_INI_SETTINGS
    return QSettings(QSettings::IniFormat, QSettings::UserScope, "ASAM", "QCReportGUICommandViewer");
#else
    return QSettings("ASAM", "QCReportGUICommandViewer");
#endif
}

void MaybeInitializeDefaultSettings()
{
    QSettings settings = GetSettings();

    // Maybe initialize default settings
    if (!settings.contains("formats/size"))
    {
        settings.beginWriteArray("formats");
        settings.setArrayIndex(0);
        settings.setValue("format", ".xodr");
        settings.setValue("name", "OpenDRIVE");
        settings.setValue("start_executable", "opendrive_viewer");
        settings.setValue("start_template", "%1 %2");
        settings.setValue("show_executable", "opendrive_viewer");
        settings.setValue("show_template", "%1 --at %3:%4 %2");
        settings.setValue("show_location_xpath", "//FileLocation/(@row/string(),@column/string())");
        settings.setValue("stop_executable", "opendrive_viewer");
        settings.setValue("stop_template", "%1 --close %2");
        settings.setArrayIndex(1);
        settings.setValue("format", ".osi");
        settings.setValue("name", "OSI Single Trace File");
        settings.setValue("show_executable", "osiviewer");
        settings.setValue("show_location_xpath", "//MessageLocation/(@time/string(),@index/string())");
        settings.setValue("show_template", "%1 --file %2 --time %3 --index %4");
        settings.setArrayIndex(2);
        settings.setValue("format", ".mcap");
        settings.setValue("name", "OSI Multi Trace File");
        settings.setValue("show_executable", "c:\\Users\\pmai\\AppData\\Local\\Programs\\lichtblick\\Lichtblick.exe");
        settings.setValue("show_location_xpath", "//MessageLocation/(@time/string(),@channel/string())");
        settings.setValue("show_template", "%1 --time=%3 \"%2\"");
        settings.endArray();
    }
}

bool MatchXML(cLocationsContainer *location, const QString &expression, QStringList &results)
{
    // Build a Xerces DOMDocument, ask the location to populate it, serialize to QString,
    // then load into a QDomDocument.
    XMLCh *lsId = XMLString::transcode("LS");
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(lsId);
    XMLString::release(&lsId);
    if (!impl) {
        lasterrormsg = "ERROR: No DOMImplementation with LS support.";
        return false;
    }

    DOMDocument *xercesDoc = impl->createDocument();
    if (!xercesDoc) {
        lasterrormsg = "ERROR: Failed to create Xerces DOMDocument.";
        return false;
    }

    // Let the location populate the Xerces DOM. Signature assumed: void WriteXML(DOMDocument*)
    DOMElement *pSummary = location->WriteXML(xercesDoc);
    xercesDoc->appendChild(pSummary);

    // Serialize Xerces DOM to a UTF-8 string
    DOMImplementationLS *implLS = dynamic_cast<DOMImplementationLS *>(impl);
    if (!implLS) {
        lasterrormsg = "ERROR: DOMImplementationLS not available.";
        xercesDoc->release();
        return false;
    }

    DOMLSSerializer *serializer = implLS->createLSSerializer();
    if (!serializer) {
        lasterrormsg = "ERROR: Failed to create serializer.";
        xercesDoc->release();
        return false;
    }
    
    DOMLSOutput *output = implLS->createLSOutput();
    XMLCh* utf8Encoding = XMLString::transcode("UTF-8");
    output->setEncoding(utf8Encoding);
    XMLString::release(&utf8Encoding);
    MemBufFormatTarget* memTarget = new MemBufFormatTarget();
    output->setByteStream(memTarget);
    serializer->write(xercesDoc, output);

    const XMLByte* xmlBytes = memTarget->getRawBuffer();
    size_t size = memTarget->getLen();

    QString xmlString = QString::fromUtf8(reinterpret_cast<const char*>(xmlBytes), static_cast<int>(size));

    delete memTarget;
    output->release();
    serializer->release();
    xercesDoc->release();
    // Now parse the serialized XML into a QDomDocument
    QXmlQuery query(QXmlQuery::XQuery10);
    query.setFocus(xmlString);
    query.setQuery(expression);
    if (!query.evaluateTo(&results)) {
#ifdef DEBUG_OUTPUT
        std::cout << "Failed to evaluate XPath expression: " << expression.toStdString() << (query.isValid()?"":" is invalid") << std::endl;
#endif
        lasterrormsg = "ERROR: Failed to evaluate XPath expression.";
        return false;
    }
#ifdef DEBUG_OUTPUT
    std::cout << "XPath Results:" << std::endl;
    for (const QString &res : results)
    {
        std::cout << " - " << res.toStdString() << std::endl;
    }
#endif
    return results.size() > 0;
}

void MaybeCleanupProcesses()
{
    QMutableListIterator<QProcess*> i(current_processes);
    while (i.hasNext()) {
        QProcess* process = i.next();
#ifdef DEBUG_OUTPUT
        std::cout << "Checking process (PID " << process->processId() << ") state " << process->state() << std::endl;
#endif
        if (process->state() == QProcess::NotRunning)
        {
#ifdef DEBUG_OUTPUT
            std::cout << "Cleaning up process (PID " << process->processId() << ") now." << std::endl;
#endif
            i.remove();
            delete process;
        }
    }
}

void ShutdownProcesses()
{
    QMutableListIterator<QProcess*> i(current_processes);
    while (i.hasNext()) {
        QProcess* process = i.next();
#ifdef DEBUG_OUTPUT
        std::cout << "Shutting down process (PID " << process->processId() << ") state " << process->state() << std::endl;
#endif
        if (process->state() == QProcess::NotRunning)
        {
#ifdef DEBUG_OUTPUT
            std::cout << "Cleaning up process (PID " << process->processId() << ") now." << std::endl;
#endif
            i.remove();
            delete process;
        }
        else
        {
#ifdef DEBUG_OUTPUT
            std::cout << "Terminating process (PID " << process->processId() << ") now." << std::endl;
#endif
            process->terminate();
            process->waitForFinished(3000); // wait up to 3 seconds
            if (process->state() != QProcess::NotRunning)
            {
#ifdef DEBUG_OUTPUT
                std::cout << "Killing process (PID " << process->processId() << ") now." << std::endl;
#endif
                process->kill();
                process->waitForFinished(1000); // wait up to 1 second
            }
            i.remove();
            delete process;
        }
    }
}

bool MaybeStartCommand(const QString &cmd_template, const QString &executable, const QStringList &arguments = QStringList())
{
    MaybeCleanupProcesses();
    if (!cmd_template.isEmpty())
    {
        QString command = cmd_template.arg(executable, current_input_path);
        for (const QString &arg : arguments)
        {
            command = command.arg(arg);
        }
#ifdef DEBUG_OUTPUT
        std::cout << "EXECUTING: " << command.toStdString() << std::endl;
#endif

        QProcess* process = new QProcess();
        QStringList args = QProcess::splitCommand(command);
        process->start(args.takeFirst(), args);
        process->waitForStarted(5000);
        if (process->state() != QProcess::Running)
        {
#ifdef DEBUG_OUTPUT
            std::cout << "Failed to start process (PID " << process->processId() << "): " << process->error() << std::endl;
#endif
            lasterrormsg = "ERROR: Failed to start process.";
            delete process;
            return false;
        }

#ifdef DEBUG_OUTPUT
        std::cout << "Started process (PID " << process->processId() << ")." << std::endl;
#endif
        current_processes.append(process);
    }
    return true;
}


// Plugin Interface

bool CanSupportFormat(const char *inputPath)
{
    if (inputPath == nullptr || std::strcmp(inputPath, "") == 0)
    {
        lasterrormsg = "ERROR: No valid input file found.";
        return false;
    }

    // Maybe initialize default settings on startup
    MaybeInitializeDefaultSettings();

#ifdef DEBUG_OUTPUT
    std::cout << "CAN COMMAND VIEWER SUPPORT FORMAT WITH INPUT FILE: " << inputPath << "? ";
#endif
    QString inputPathStr = QString(inputPath);
    QSettings settings = GetSettings();
    int size = settings.beginReadArray("formats");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        QString format = settings.value("format").toString();
        QString name = settings.value("name").toString();
        if (inputPathStr.endsWith(format))
        {
            settings.endArray();
#ifdef DEBUG_OUTPUT
            std::cout << "YES - " << name.toStdString() << " (" << format.toStdString() << ")" << std::endl;
#endif
            return true;
        }
    }
    settings.endArray();
#ifdef DEBUG_OUTPUT
    std::cout << "NO" << std::endl;
#endif

    return false;
}

bool StartViewer()
{
#ifdef DEBUG_OUTPUT
    std::cout << "START COMMAND VIEWER" << std::endl;
#endif
    return true;
}

bool Initialize(const char *inputPath)
{
    if (inputPath == nullptr || std::strcmp(inputPath, "") == 0)
    {
        lasterrormsg = "ERROR: No valid input file found.";
        return false;
    }

#ifdef DEBUG_OUTPUT
    std::cout << "INITIALIZE COMMAND VIEWER WITH INPUT FILE: " << inputPath << std::endl;
#endif

    current_input_path = QString(inputPath);

    QSettings settings = GetSettings();
    int size = settings.beginReadArray("formats");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        QString format = settings.value("format").toString();
        if (current_input_path.endsWith(format))
        {
            current_format = format;
            current_format_name = settings.value("name").toString();
            current_start_executable = settings.value("start_executable").toString();
            current_start_template = settings.value("start_template").toString();
            current_show_location_xpath = settings.value("show_location_xpath").toString();
            current_show_executable = settings.value("show_executable").toString();
            current_show_template = settings.value("show_template").toString();
            current_stop_executable = settings.value("stop_executable").toString();
            current_stop_template = settings.value("stop_template").toString();
            settings.endArray();
#ifdef DEBUG_OUTPUT
            std::cout << "Format " << current_format_name.toStdString() << " (" << current_format.toStdString() << ") - Start Executable " << current_start_executable.toStdString() << std::endl;
#endif

            return MaybeStartCommand(current_start_template, current_start_executable);
        }
    }
    settings.endArray();

    lasterrormsg = "ERROR: No valid format found for input file.";
    return false;
}

bool AddIssue(void *issueToAdd)
{
#ifdef DEBUG_OUTPUT
    auto issue = static_cast<cIssue *>(issueToAdd);
    std::cout << "COMMAND VIEWER ADD ISSUE: " << issue->GetDescription() << std::endl;
#endif
    return true;
}

bool CanShowIssue(void *itemToShow, void *locationToShow)
{
    if (locationToShow == nullptr)
        return false;

    cLocationsContainer *location = static_cast<cLocationsContainer *>(locationToShow);
    QStringList results;
    return MatchXML(location, current_show_location_xpath, results);
}

bool ShowIssue(void *itemToShow, void *locationToShow)
{
   auto location = static_cast<cLocationsContainer *>(locationToShow);
#ifdef DEBUG_OUTPUT
    auto issue = static_cast<cIssue *>(itemToShow);
    std::cout << "COMMAND VIEWER SHOW ISSUE: " << issue->GetDescription() << std::endl;
    std::cout << "CV LOCATION: " << location->GetDescription() << std::endl;
#endif

    QStringList results;
    if (!MatchXML(location, current_show_location_xpath, results))
    {
        lasterrormsg = "ERROR: Location does not match show xpath.";
        return false;
    }

    return MaybeStartCommand(current_show_template, current_show_executable, results);
}

const char *GetName()
{
    return "Command Viewer";
}

bool CloseViewer()
{
#ifdef DEBUG_OUTPUT
    std::cout << "CLOSE COMMAND VIEWER" << std::endl;
#endif
    MaybeStartCommand(current_stop_template, current_stop_executable);
    ShutdownProcesses();

    return true;
}

const char *GetLastErrorMessage()
{
    return lasterrormsg;
}
