import pytest
import runtime.runtime as runtime
import os
import subprocess
import xml.etree.ElementTree as ET
from lxml import etree


@pytest.fixture
def pytest_setup():
    pytest.BIN_FOLDER = "/app/build/bin"
    pytest.SCHEMA_FOLDER = "/app/doc/schema/"


def get_test_cmd_on_config(config_xml):
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


def run_command(cmd):
    print(f"Executing command: {' '.join(cmd)}")
    process = subprocess.Popen(
        cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=os.getcwd()
    )
    _, _ = process.communicate()

    return process.returncode


def is_valid_xml(xml_file, schema_file):
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


def check_node_exists(xml_doc, node_name):
    # Parse the XML document
    tree = etree.parse(xml_doc)
    # Use XPath to search for the node
    nodes = tree.xpath(f'//*[local-name()="{node_name}"]')
    return len(nodes) > 0


def test_runtime_execution(pytest_setup):
    config_xml = "/app/tests/test_data/DemoCheckerBundle_config.xml"
    cmd_result = run_command(get_test_cmd_on_config(config_xml))
    assert cmd_result == 0
    result_file = os.path.join(pytest.BIN_FOLDER, "Result.xqar")
    assert os.path.isfile(result_file)
    xsd_file = os.path.join(pytest.SCHEMA_FOLDER, "xqar_report_format.xsd")
    assert is_valid_xml(result_file, xsd_file)
    node_name = "Issue"
    assert check_node_exists(result_file, node_name)
