import pytest
import runtime.runtime as runtime
import os


@pytest.fixture
def pytest_setup():
    pytest.BIN_FOLDER = "/app/build/bin"
    pytest.SCHEMA_FOLDER = "/app/doc/schema/"


def test_runtime_execution(pytest_setup):
    main_result = runtime.main(
        "/app/framework/test/function/runtime/files/DemoCheckerBundle_config.xml",
        pytest.BIN_FOLDER,
        pytest.SCHEMA_FOLDER,
    )
    assert main_result == True
