# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

import subprocess
from lxml import etree
import sys, os
import argparse
from typing import List


def on_windows() -> bool:
    """Check if script is executed in Windows OS

    Returns:
        bool: True if executing the script on Windows
    """
    return os.name == "nt"


def check_executable_exists(executable: str) -> bool:
    """Check if specified executable exists in current working directory

    Args:
        executable (str): executable name to find

    Returns:
        bool: True if executable exists
    """
    return executable in list_files_in_cwd()


def is_valid_xml(xml_file: str, schema_file: str) -> bool:
    """Check if input xml file (.xml) is valid against the input schema file (.xsd)
    Args:
        xml_file (str): XML file path to test
        schema_file (str): XSD file path containing the schema for the validation
    Returns:
        bool: True if file pointed by xml_file is valid w.r.t. input schema file. False otherwise
    """
    with open(schema_file, "rb") as schema_f:
        schema_doc = etree.parse(schema_f)
        schema = etree.XMLSchema(schema_doc)

    with open(xml_file, "rb") as xml_f:
        xml_doc = etree.parse(xml_f)

    if schema.validate(xml_doc):
        print("XML is valid.")
        return True
    else:
        print("XML is invalid!")
        for error in schema.error_log:
            print(error.message)
        return False


def run_command(cmd_list: List[str]) -> None:
    """Execute command specified in cmd_list list parameter

    Args:
        cmd_list (List): A list containing all the strings that constitutes the command to execute
                         E.g. cmd_list = ["python3", "script.py", "--help"]

    """
    try:
        print(f"Executing command: {' '.join(cmd_list)}")
        process = subprocess.Popen(
            cmd_list, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=os.getcwd()
        )
        stdout, stderr = process.communicate()
        exit_code = process.returncode
        if exit_code == 0:
            print("Command executed successfully.")
            print("Output:")
            print(stdout.decode())
        else:
            print("Error occurred while executing the command.")
            print("Error message:")
            print(stderr.decode())
    except Exception as e:
        print(f"Error: {str(e)}")


def list_files_in_cwd() -> List[str]:
    """List all files in current working directory

    Returns:
        List: List with all the executables found in working directory
    """
    cwd = os.getcwd()
    files = os.listdir(cwd)
    files = [f for f in files if os.path.isfile(os.path.join(cwd, f))]
    return files


def get_os_command_from_app_name(app_name: str) -> str:
    """Get command to execute the specified app_name as input

    The function checks the operating system and build the returning command needed
    e.g. app_name: HelloWorld
    if the script is executed on Linux the result is "./HelloWorld"
    if on Windows the result is HelloWorld.exe

    Args:
        app_name (str): The executable name that needs to be decorated with required chars

    Returns:
        str: A ready to execute string representation of app_name considering the current OS requirements.
    """
    os_command = None

    if on_windows():
        app_name = app_name + ".exe"
        os_command = app_name
    else:
        os_command = "./" + app_name

    if not check_executable_exists(app_name):
        print(f"Executable {app_name} does not exist!\nAbort...")
        sys.exit()

    return os_command


def run_commands_from_xml(xml_file: str, install_folder: str) -> None:
    """Execute the qc-framework runtime steps specified in the input xml file

        The function:
        - For each checker bundle specified in configuration, execute its process
        - Execute result pooling for collect results from all checker bundle executed in step 2
        - For each report module specified in configuration, execute its process
    Args:
        xml_file (str): input configuration xml file
        install_folder (str): folder where executables specified in the input xml files are installed
    """

    root = etree.parse(xml_file)

    print("Executable found in install directory: ", list_files_in_cwd())

    # 1. Execute all CheckerBundle
    print("#" * 50)
    print("1. Execute all CheckerBundle")
    print("#" * 50)
    for element in root.findall(".//CheckerBundle"):
        app_name = element.get("application")
        os_command = get_os_command_from_app_name(app_name)
        cmd_list = [os_command, xml_file]
        run_command(cmd_list)

    # 2. Execute ResultPooling
    print("#" * 50)
    print("2. Execute ResultPooling")
    print("#" * 50)
    app_name = "ResultPooling"
    os_command = get_os_command_from_app_name(app_name)
    cmd_list = [os_command, xml_file]
    run_command(cmd_list)

    # 3. Execute Report Modules
    print("#" * 50)
    print("3. Execute Report Modules")
    print("#" * 50)
    for element in root.findall(".//ReportModule"):
        app_name = element.get("application")
        os_command = get_os_command_from_app_name(app_name)
        cmd_list = [os_command, xml_file]
        run_command(cmd_list)


def execute_runtime(xml_file: str, install_folder: str):
    """Execute all runtime operations on input xml file

    Args:
        xml_file (str): input configuration xml file
        install_folder (str):  folder where executables specified in the input xml files are installed
    """
    os.chdir(install_folder)

    schema_file = os.path.join("..", "doc", "schema", "config_format.xsd")

    if not is_valid_xml(xml_file, schema_file):
        print("Aborting due to invalid XML.")
        sys.exit()
    run_commands_from_xml(xml_file, install_folder)


def main():
    parser = argparse.ArgumentParser(description="Process XML configuration file.")
    parser.add_argument(
        "--config",
        type=str,
        help="Path to the XML configuration file",
        required=True,
    )
    parser.add_argument(
        "--install_dir",
        type=str,
        help="Path where compiled binaries are installed",
        required=True,
    )

    args = parser.parse_args()

    execute_runtime(args.config, args.install_dir)


if __name__ == "__main__":
    main()
