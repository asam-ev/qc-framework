import pytest
import runtime.runtime as runtime
import os


@pytest.fixture(scope="session", autouse=True)
def stub():
    return stub


def test_runtime_execution(stub):
    main_result = runtime.main(
        "/app/framework/test/function/runtime/files/DemoCheckerBundle_config.xml"
    )
    assert main_result == True
