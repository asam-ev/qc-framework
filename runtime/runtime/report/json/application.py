# Copyright 2024, ASAM e.V.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
from .constants import Constants
from .json_formatter import JsonFormatter


class Application:
    r"""
    Simple empty class container for the main begavior required for the
    json report application. The class currently exposes three different 
    behavior:
    
     * export the default configuration
     * export a json report starting from a result file
     * export a json report starting from a result file, but reads the location
       of the file starting from a configuration file.
    """
    
    @classmethod
    def export_default_config(
        cls, output_filename: str = Constants.DEFAULT_OUTPUT_CONFIG_FILENAME
    ):
        r"""
        Exports the default configuration for the current json report 
        implementation. The implementation supports the following parameters:
        
         * ``InputFile``: path of the result file from which build the report
         * ``ReportFile``: path of the output file to create
         * ``Indent``: Indentation level for json file. Set to 0 for minified
         
        :param output_filename: override for name of the configuration file that
                                will be created
        """
        from qc_baselib import Configuration
        from qc_baselib.models.config import Config, ReportModuleType
        from qc_baselib.models.common import ParamType

        config = Config(
            params=[],
            checker_bundles=[],
            reports=[
                ReportModuleType(
                    application=Constants.MODULE_NAME,
                    params=[
                        ParamType(
                            name=Constants.CONFIG_INPUT_FILE_PARAM,
                            value=Constants.DEFAULT_INPUT_RESULT_FILENAME,
                        ),
                        ParamType(
                            name=Constants.CONFIG_OUTPUT_FILE_PARAM,
                            value=Constants.DEFAULT_OUTPUT_REPORT_FILENAME,
                        ),
                        ParamType(
                            name=Constants.CONFIG_INDENT_PARAM, value=Constants.DEFAULT_INDENT_LEVEL
                        ),
                    ],
                )
            ],
        )
        configuration = Configuration()
        # FIXME: Fix this approach when public interface for reports is available
        configuration._configuration = config
        configuration.write_to_file(output_filename)

    @classmethod
    def export_result_file(
        cls,
        result_file_name: str = Constants.DEFAULT_INPUT_RESULT_FILENAME,
        report_file_name: str = Constants.DEFAULT_OUTPUT_REPORT_FILENAME,
        indent: int = Constants.DEFAULT_INDENT_LEVEL,
    ):
        r"""
        Exports the result file to a Json report file format.
        
        :param result_file_name: name of the result file to load
        :param report_file_name: name of the output file in JSON format
        :param indent: indentation level for the JSON file
        """
        from qc_baselib import Result

        result = Result()
        result.load_from_file(result_file_name)
        js = JsonFormatter(indent)
        js.dump(result, report_file_name)

    @classmethod
    def export_result_file_from_config(
        cls, 
        config_file_name: str = Constants.DEFAULT_INPUT_CONFIG_FILENAME
    ):
        r"""
        Exports the reslts file in a JSON report. Parameters are
        defined using a configuration file.
        
        :param config_file_name: input configuration file name
        """
        from qc_baselib import Configuration

        configuration = Configuration()
        configuration.load_from_file(config_file_name)

        result_file_name = str(
            configuration.get_report_module_param(
                Constants.MODULE_NAME, Constants.CONFIG_INPUT_FILE_PARAM
            )
            or Constants.DEFAULT_INPUT_RESULT_FILENAME
        )
        report_file_name = str(
            configuration.get_report_module_param(
                Constants.MODULE_NAME, Constants.CONFIG_OUTPUT_FILE_PARAM
            )
            or Constants.DEFAULT_OUTPUT_REPORT_FILENAME
        )
        indent = int(
            configuration.get_report_module_param(
                Constants.MODULE_NAME, Constants.CONFIG_INDENT_PARAM
            )
            or Constants.DEFAULT_INDENT_LEVEL
        )

        cls.export_result_file(result_file_name, report_file_name, indent)
