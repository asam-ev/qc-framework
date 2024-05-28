import subprocess
import xml.etree.ElementTree as ET
from lxml import etree
import sys, os
import argparse
import shutil

BIN_FOLDER = "/app/build/bin"
SCHEMA_FOLDER = "/app/doc/schema/"


def on_windows():
    return os.name == "nt"


def check_executable_exists(executable):
    return executable in list_files_in_cwd()


def validate_xml(xml_file, schema_file):
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


def run_command(cmd_list):
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


def list_files_in_cwd():
    cwd = os.getcwd()
    files = os.listdir(cwd)
    files = [f for f in files if os.path.isfile(os.path.join(cwd, f))]
    return files


def get_os_command_from_app_name(app_name):
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


def run_commands_from_xml(xml_file):
    tree = ET.parse(xml_file)
    root = tree.getroot()

    os.chdir(BIN_FOLDER)

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
    run_command([os_command])

    # 3. Execute Report Modules
    print("#" * 50)
    print("3. Execute Report Modules")
    print("#" * 50)
    for element in root.findall(".//ReportModule"):
        app_name = element.get("application")
        os_command = get_os_command_from_app_name(app_name)
        cmd_list = [os_command, xml_file]
        run_command(cmd_list)


def main(xml_file):

    schema_file = os.path.join(SCHEMA_FOLDER, "config_format.xsd")
    if not validate_xml(xml_file, schema_file):
        print("Aborting due to invalid XML.")
        sys.exit()

    run_commands_from_xml(xml_file)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Process XML configuration file.")
    parser.add_argument(
        "-config",
        type=str,
        help="Path to the XML configuration file",
        required=True,
    )

    args = parser.parse_args()

    main(f"/data/{args.config}")
