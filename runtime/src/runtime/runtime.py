import subprocess
import xml.etree.ElementTree as ET
from lxml import etree
import sys, os
import argparse
import shutil


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
        result = subprocess.run(cmd_list, check=True, capture_output=True, text=True)
        print(f"Output: {result.stdout}")
        return result
    except subprocess.CalledProcessError as e:
        print(f"Error: {e.stderr}")


def list_files_in_cwd():
    cwd = os.getcwd()
    files = os.listdir(cwd)
    files = [f for f in files if os.path.isfile(os.path.join(cwd, f))]
    return files


def run_commands_from_xml(xml_file):
    tree = ET.parse(xml_file)
    root = tree.getroot()

    BIN_FOLDER = "/app/bin"

    os.chdir(BIN_FOLDER)

    # 1. Execute all CheckerBundle
    for element in root.findall(".//CheckerBundle"):
        command = element.get("application")

        if on_windows():
            command += ".exe"

        if not check_executable_exists(command):
            print(f"Executable {command} does not exist!\nAbort...")
            sys.exit()

        if command and check_executable_exists(command):
            cmd_list = [command, xml_file]
            run_command(cmd_list)
        else:
            print("No command found in process element.")

    # 2. Execute ResultPooling
    command = "ResultPooling"
    if on_windows():
        command += ".exe"

    if not check_executable_exists(command):
        print(f"Executable {command} does not exist!\nAbort...")
        sys.exit()

    run_command([command])


def main(xml_file, xodr_file, xosc_file, schema_file):
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

    # Define mutually exclusive group
    group = parser.add_mutually_exclusive_group()
    group.add_argument("-xodr", type=str, help="Xodr file under test")
    group.add_argument(
        "-xosc",
        type=str,
        help="Xosc file under test",
    )
    args = parser.parse_args()

    schema_file = "/app/doc/schema/config_format.xsd"
    main(f"/data/{args.config}", args.xodr, args.xosc, schema_file)
