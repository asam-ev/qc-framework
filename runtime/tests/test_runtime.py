# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

import pytest
import runtime.runtime as runtime
import os
import subprocess
import xml.etree.ElementTree as ET
from lxml import etree
from typing import List


@pytest.fixture
def pytest_setup():
    pytest.BIN_FOLDER = "/app/build/bin"
    pytest.SCHEMA_FOLDER = "/app/doc/schema/"


def get_test_cmd_on_config(config_xml: str) -> List:
    """Get list of strings ready for execution given an input xml configuration string

    Args:
        config_xml (str): path of the xml that needs to be decorated with full
                          command list including the call to runtime.py script

    Returns:
        List: A ready for subprocess list of strings for executing runtime.py script on input xml file
    """
    return [
        "python3",
        "/app/runtime/runtime.py",
        "--config",
        config_xml,
        "--install_dir",
        pytest.BIN_FOLDER,
        "--schema_dir",
        pytest.SCHEMA_FOLDER,
    ]


def run_command(cmd_list: List) -> int:
    """Wrapper function for input cmd_list

    Args:
        cmd_list (List): A list containing all the strings that constitutes the command to execute
                         E.g. cmd_list = ["python3", "script.py", "--help"]

    Returns:
        int: return code of the process started with command specified in cmd_list
    """
    print(f"Executing command: {' '.join(cmd_list)}")
    process = subprocess.Popen(
        cmd_list, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=os.getcwd()
    )
    _, _ = process.communicate()

    return process.returncode


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


def check_node_exists(xml_file: str, node_name: str) -> bool:
    """Check if input node node_name is present in xml document specified in xml_file

    Args:
        xml_file (str): XML file path to inspect
        node_name (str): Name of the node to search in the xml doc

    Returns:
        bool: True if node at least 1 node called node_name is found in the xml file
    """
    # Parse the XML document
    tree = etree.parse(xml_file)
    # Use XPath to search for the node
    nodes = tree.xpath(f'//*[local-name()="{node_name}"]')
    return len(nodes) > 0


def test_runtime_execution(pytest_setup):
    config_xml = "/app/tests/test_data/DemoCheckerBundle_config.xml"
    cmd_result = run_command(get_test_cmd_on_config(config_xml))
    assert cmd_result == 0
    # Check that result file is correctly generated
    result_file = os.path.join(pytest.BIN_FOLDER, "Result.xqar")
    assert os.path.isfile(result_file)
    # Check that result file follows its xsd schema
    xsd_file = os.path.join(pytest.SCHEMA_FOLDER, "xqar_report_format.xsd")
    assert is_valid_xml(result_file, xsd_file)
    # Check that at least one node called "Issue" is present in the result
    node_name = "Issue"
    assert check_node_exists(result_file, node_name)
