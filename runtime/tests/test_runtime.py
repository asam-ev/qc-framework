# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

import pytest
import runtime.runtime as runtime
import os
import subprocess
from lxml import etree
from typing import List


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


def test_runtime_execution():
    config_xml = os.path.join(
        "..", "..", "runtime", "tests", "test_data", "DemoCheckerBundle_config.xml"
    )

    install_dir = os.path.join("..", "build", "bin")
    os.chdir(install_dir)

    schema_dir = os.path.join("..", "..", "doc", "schema")

    runtime_script = os.path.join("..", "..", "runtime", "runtime", "runtime.py")
    process = subprocess.Popen(
        f"python3 {runtime_script} --config={config_xml} --install_dir=./ --schema_dir={schema_dir}",
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        cwd=os.getcwd(),
    )
    _, _ = process.communicate()
    # Check that result file is correctly generated
    result_file = os.path.join("Result.xqar")
    assert os.path.isfile(result_file)
    # Check that result file follows its xsd schema
    xsd_file = os.path.join(schema_dir, "xqar_report_format.xsd")
    assert is_valid_xml(result_file, xsd_file)
    # Check that at least one node called "Issue" is present in the result
    node_name = "Issue"
    assert check_node_exists(result_file, node_name)
