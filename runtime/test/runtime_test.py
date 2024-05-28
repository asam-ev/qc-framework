import pytest
import src.runtime.runtime as runtime
import os


@pytest.fixture(scope="session", autouse=True)
def stub():
    return stub


def test_valid_xsd(stub):
    validation_result = runtime.validate_xml(
        "../../test/function/runtime/files/DemoCheckerBundle_config.xml",
        "../../doc/schema/config_format.xsd",
    )
    assert validation_result == True


def test_invalid_xsd(stub):
    validation_result = runtime.validate_xml(
        "../../test/function/runtime/files/DemoCheckerBundle_config.xml",
        os.path.join("files", "invalid_schema.xsd"),
    )
    assert validation_result == False


def test_invalid_xml(stub):
    validation_result = runtime.validate_xml(
        os.path.join("files", "invalid_config.xml"),
        "../../doc/schema/config_format.xsd",
    )
    assert validation_result == False


def test_empty_xml(stub):
    empty_config_xml = os.path.join("files", "empty_config.xml")
    validation_result = runtime.validate_xml(
        empty_config_xml,
        "../../doc/schema/config_format.xsd",
    )
    assert validation_result == True
    runtime.run_commands_from_xml(empty_config_xml)
