# Copyright 2024, ASAM e.V.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
from argparse import ArgumentParser
from .report_formatter import ReportFormatter
import logging
import sys


class ReportApplication:
    r"""
    Class that wraps the common behavior for a ``ReportFormatter`` in order to create
    a command line application which is compatible with the design specification of the
    Quality Checker runtime.

    Application created with this utility class have the following command line
    arguments:

     * For each formatter parameters, a custom automatic optional argument is created
       programmatically, with declared default values.
     * create a specific configuration file for the current formatter. The configuration
       file will contain all the parameters supported. Default value for the parameter
       can be changed with command line input. It is possible to define the output file
     * report directly from a result file. It is possible to use the additional arguments
       to define the path of the input file
     * report from a configuration file. In this case, the precedence for the parameters
       follows:
        
        * lowest: default value
        * configuration file
        * highest: command line arguments

    It is possible to define stdout / stderr output by using the ``__stdout__`` / 
    ``__stderr__`` strings in command line arguments.

    There are optional arguments for the current class, provided as ``kwargs``.

    Use the ``main`` method to create the actual application.

    Example usage::

        def main():
            app = ReportApplication(formatter)
            return app.main()

        if __name__ == "__main__":
            main()

    :param formatter: formatter object for application creation
    :param prog: string that represent the name to be used in help
    :param description: description to include in the program help
    :param license: license epilogue to include in the program help
    """

    class Constants:
        DEFAULT_OUTPUT_CONFIG_FILE_NAME = f"Config.xml"
        INPUT_FILE_IS_STDIN = "__stdin__"
        OUTPUT_FILE_IS_STDERR = "__stderr__"
        OUTPUT_FILE_IS_STDOUT = "__stdout__"

    def __init__(self, formatter: ReportFormatter, *args, **kwargs):
        self._formatter = formatter

        _default_license = "Quality Framework, Copyright 2024, ASAM e.V.: License MPLv2"
        _default_description = f"Formatter {self._formatter.module_name} implementation"
        _default_prog = self._formatter.module_name

        self._log = logging.getLogger(self._formatter.module_name)

        self._args = ArgumentParser(
            prog=kwargs.get("prog") or _default_prog,
            description=kwargs.get("description") or _default_description,
            epilog=kwargs.get("license") or _default_license,
        )
        self._define_arguments()

    def main(self) -> int:
        r"""
        Main function. Executes the formatter.

        :return: integer value as exit number
        """
        try:
            args, override_params = self._parse_args_and_params()

            for name, value in override_params.items():
                self._formatter.set_param(name, value)

            if args.default_conf:
                self.export_default_config(args.default_conf_filename)
                return 0

            if args.from_result:
                self.report_from_result()
                return 0

            if args.from_config:
                self.report_from_config(args.from_config, override_params)
                return 0

        except Exception as e:
            self._log.exception(
                "Error while executing the report tool module: %s - %s",
                type(e).__name__,
                e,
            )
            return -1
        return 1

    def _define_arguments(self):
        r"""
        Defines application arguments. Parameters specific for the formatter
        are created as ``"param_{name of the parameter}"``
        """
        group = self._args.add_mutually_exclusive_group(required=True)
        group.add_argument(
            "-c",
            "--from-configuration",
            dest="from_config",
            type=str,
            help="run the application with an input configuration file",
        )
        group.add_argument(
            "-r",
            "--from-result",
            dest="from_result",
            action="store_true",
            help="run the application directly on an input result file",
        )
        group.add_argument(
            "-x",
            "--export-config",
            dest="default_conf",
            action="store_true",
            help="write default (or modified) configuration",
        )
        self._args.add_argument(
            "-xo",
            "--export-config-output",
            dest="default_conf_filename",
            default=self.Constants.DEFAULT_OUTPUT_CONFIG_FILE_NAME,
            help="optional name of file for default configuration",
        )

        # Automatically assigning parameters to command line argument
        class ArgsDefaulter:
            def __init__(self, default_type):
                self.default_type = default_type
            def __call__(self, arg):
                if arg is None:
                    return None
                return self.default_type(arg)
            
        param_group = self._args.add_argument_group("Parameters")
        for name in self._formatter.get_params_list():
            value = self._formatter.get_param(name)
            value_type = type(value)

            param_group.add_argument(
                f"--param-{name}",
                default=None,
                type=ArgsDefaulter(value_type),
                dest=f"param_{name}",
                help=f"formatter parameter {name} (default value: `{value}` [{value_type.__name__}])",
            )

    def _parse_args_and_params(self):
        r"""
        Evaluate command line parameters and extract the parameters override dictionary

        :return: a tuple of namespace and dictionary with override params
        """
        args = self._args.parse_args()

        override_params = {}
        for name in self._formatter.get_params_list():
            param_name = f"param_{name}"
            if param_name in args:
                value = getattr(args, param_name)
                if value:
                    override_params[name] = getattr(args, param_name)

        return args, override_params

    def export_default_config(self, output_filename):
        r"""
        Export the default configuration for the formatter. Command line
        parameter values can be used to customize the created configuration file.

        :param output_filename: name of the output file
        """
        from qc_baselib import Configuration
        from qc_baselib.models.config import Config, ReportModuleType
        from qc_baselib.models.common import ParamType

        parameters = [
            ParamType(name=name, value=self._formatter.get_param(name))
            for name in self._formatter.get_params_list()
        ]

        config = Config(
            params=[],
            checker_bundles=[],
            reports=[
                ReportModuleType(
                    application=self._formatter.module_name,
                    params=parameters,
                )
            ],
        )
        # FIXME: Fix this approach when public interface for reports is available
        config_string = config.to_xml(
            pretty_print=True, xml_declaration=True, standalone=False, encoding="utf-8"
        ).decode('utf-8')

        if output_filename == self.Constants.OUTPUT_FILE_IS_STDOUT:
            print(config_string, file=sys.stdout, flush=True)
            return
        if output_filename == self.Constants.OUTPUT_FILE_IS_STDERR:
            print(config_string, file=sys.stderr, flush=True)
            return
        with open(output_filename, "w") as output_fp:
            output_fp.write(config_string)  # type: ignore

    def report_from_result(self):
        r"""
        Runs the formatter by loading directly a result file
        """
        input: str = self._formatter.get_param(self._formatter.default_input_file_param_name)  # type: ignore
        output: str = self._formatter.get_param(self._formatter.default_output_file_param_name)  # type: ignore

        self._formatter.dump(input, output)

    def report_from_config(self, configuration_file, override_params):
        r"""
        Runs the formatter by loading a configuration file.

        .. info:: command line parameters override configuration parameter values

        :param configuration_file: path of the configuration file to load
        :param override_param: command line override params
        """
        from qc_baselib import Configuration
        from qc_baselib.models.config import Config

        configuration = Configuration()
        if configuration_file == self.Constants.INPUT_FILE_IS_STDIN:
            inner_config = Config.from_xml(sys.stdin.read().encode("utf-8"))
            # FIXME: Fix this for update public interface of models
            configuration._configuration = inner_config
        else:
            configuration.load_from_file(configuration_file)

        for name in self._formatter.get_params_list():
            if override_params.get(name) is not None:
                continue
            param_value = configuration.get_report_module_param(
                self._formatter.module_name, name
            )
            if param_value:
                self._formatter.set_param(name, param_value)

        self.report_from_result()
