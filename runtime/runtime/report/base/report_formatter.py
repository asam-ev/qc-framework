# Copyright 2024, ASAM e.V.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

from __future__ import annotations
from typing import Optional, Union, Dict, List
from io import IOBase, StringIO, TextIOBase
from qc_baselib import Result
import sys


TParamValue = Optional[Union[str, int, float, bool]]
TParams = Dict[str, TParamValue]
TResultArg = Union[str, IOBase, Result]
TOuptuArg = Optional[Union[str, IOBase, StringIO, TextIOBase]]
TStream = Union[IOBase, StringIO, TextIOBase]


class ReportFormatter:
    r"""
    Base interface declaration for a formatter class. It is possible to
    use this class in two way, that mainly deal in implementation details:

     * extension via inheritance (and override for existing methods). In 
       this case the implementers **must** override the function ``_dump``
       provided by the current class. Constructor must define required 
       properties for a correct extension
     * extension via dependency injection. In this case the implementers
       **must** provide a ``dump_function`` in constructor. The interface for the 
       function is the following::

           def dump_function(
               result: Reuslt, 
               get_param: Callable[[str], Optional[TParamValue]], 
               output: TStream) -> None:
               pass

       where:

       :param result: the ``qc_baselib.Result`` object to convert
       :param get_param: a callable function that given a parameter name returns the value
       :param output: IO stream object where result should be written

    All reporter formatter are expected to have input and output parameters. Those parameters
    are already defined by the base class, with the following keys:

     * ``"InputFile"``: ``"Result.xqar"``
     * ``"OutputFile"``: ``"Report.txt"``

    the value can be modified by using injection or inheritance. To inject new values, simply
    provide new values via ``kwargs``. To change via inheritance, override the appropriate property.
    A full list of avaialbale ``kwargs`` is provided below.

    Additional parameters can be defined via injection (``kwargs``) or by an override of the property.
    The custom parameters can be used also to modify the default parameters.

    Example for a custom formatter via inheritance is provided below::

        class CustomFormatter(BaseFormatter):
            def __init__(self):
                super().__init__()

            def _dump(self, result: "Result", output: TStream):
                # Parameter example
                count_spaces = self.get_param("CountSpaces")  # Note: self.!
                output.write(" " * count_spaces)
                # Result dump example
                output.write(str(result))

            @property
            def module_name(self) -> str:
                return "custom-formatter"

            @property
            def default_output_file_param_value(self) -> str:
                return "CustomReport.bin"

            @property
            def default_custom_parameters(self) -> TParams:
                return {"CountSpaces": 2}

        formatter = CustomFormatter()
        formatter.dump("__stdin__", "__stdout__")

    Example for an identical injected formatter is provided below::

        def custom_dumper(result: "Result", get_param: Callable[[str], Optional[TParamValue]], output: TStream):
            # Parameter example
            count_spaces = get_param("CountSpaces")  # Note: no self.!
            output.write(" " * count_spaces)
            # Result dump example
            output.write(str(result))

        def new_custom_formatter():
            default_custom_parameters = {"CountSpaces": 2}
            return BaseFormatter(module_name="custom-formatter",
                dump_function=custom_dumper,
                default_output_file_param_value="CustomReport.bin",
                default_custom_parameters=default_custom_parameters)

        formatter = new_custom_formatter()
        formatter.set_param("CountSpaces", 4)
        formatter.dump("__stdin__", "__stdout__")

    The two implementation have virtually identical behavior.

    .. info:: It is possible also to use a mix of the two approaches

    From a user perspective, the only important methods are the constructor and the 
    ``dump`` method. ``dump`` method supports some placeholder string filename to integrate
    support for ``stdin`` as input, ``stdout`` and ``stderr`` as output, in place of 
    existing file.

    Parameters are exposed via the methods: ``get_params_list``, ``get_params``, ``set_params``.

    :param module_name: optional ``kwargs`` to define the module name. Should be a string 
    :param dump_function: optional ``kwargs`` to define the dump function. Should be a valid
                          callable with the aforementioned signature
    :param default_input_file_param_name: optional ``kwargs`` to change parameter input file key
                                         (default is ``"InputFile"``). Should be a string.
    :param default_input_file_param_name: optional ``kwargs`` to change parameter input file value
                                         (default is ``"Result.xqar"``). Should be a string.
    :param default_output_file_param_name: optional ``kwargs`` to change parameter output file key
                                         (default is ``"OutputFile"``). Should be a string.
    :param default_output_file_param_name: optional ``kwargs`` to change parameter output file value
                                         (default is ``"Report.txt"``). Should be a string.
    :param default_custom_parameters: optional ``kwargs`` for the definition of additional parameters.
                                      (dafault is ``{}``). With the same ``kwargs`` it is possible
                                      to override the default parameters value (but not keys)
    """

    class Constants:
        DEFAULT_INPUT_FILE_PARAM_NAME = "InputFile"
        DEFAULT_OUTPUT_FILE_PARAM_NAME = "OutputFile"
        DEFAULT_INPUT_FILE_PARAM_VALUE = "Result.xqar"
        DEFAULT_OUTPUT_FILE_PARAM_VALUE = f"Report.txt"
        INPUT_FILE_IS_STDIN = "__stdin__"
        OUTPUT_FILE_IS_STDERR = "__stderr__"
        OUTPUT_FILE_IS_STDOUT = "__stdout__"

    def __init__(self, *args, **kwargs):
        r"""
        Default constructor. Can be used for injection, or used in inhierited constructors.

        Supported parameters defined in class documentation. 
        """
        self._module_name = kwargs.get("module_name") or type(self).__name__
        self._dump_function = kwargs.get("dump_function")

        for name in ["default_input_file_param_name", "default_input_file_param_value", 
                    "default_output_file_param_name", "default_output_file_param_value"]:
            setattr(self, f"_{name}", kwargs.get(name) or getattr(self.Constants, name.upper()))
        
        self._default_custom_parameters = (
            kwargs.get("default_custom_parameters") or {}
        )

        self._parameters = self.default_parameters()

    def set_param(self, name: str, value: TParamValue) -> "ReportFormatter":
        r"""
        Method to set pèarameters value for the current formatter. Only parameters
        already defined can be changed. It currently silently fails if name is 
        not available. This behavior may change in the future.

        .. warning:: There is no validation system in place for parameters!

        :param name: name of the parameter to set
        :param value: value to set for the parameter
        :return: this instance: the method is chainable
        """
        if name in self._parameters:
            self._parameters[name] = value
        return self
    
    def get_params_list(self) -> List[str]:
        r"""
        Returns a list of available paramters names

        :return: List of paramters names
        """
        return list(self._parameters.keys())

    def get_param(self, key: str) -> Optional[TParamValue]:
        r"""
        Get current value for a parameter. Silently fails if parameter name 
        is not available

        :param name: name of the parameter to query
        :return: value of the parameter or None
        """
        return self._parameters.get(key)

    def dump(
        self, input: TResultArg, output: TOuptuArg = None
    ) -> Optional[str]:
        r"""
        Dumps the content of a result file in a different format. Input arguments
        controls noth the loading and the data emission strategy.

        Regarding result loading:

         * if ``input`` is a ``str``, the result file is loaded from a file. If it 
           contains the specific placeholder ``__stdin__``, the content of the result
           file is consumed from the ``stdin`` stream.
         * if ``input`` is a ``io.IOBase``, content is consumed to be loaded inside the
           appropriate result object.
         * if ``input`` already contains a ``Result`` object, the object is used as is.

        Regarding the report output:

         * if ``output`` parameter is ``None``, the method returns a string
         * if ``ouptut`` parameter is a ``str``, the result is written in a new
           file with ``output`` as filename. If the value contains the stdout or 
           stderr placeholder names (by default ``__stdout__`` and ``__stderr__``),
           those file are used for emitting the report.
         * if ``output`` is an ``io.IOBase`` compatible object (stream), the result is
           written directly to the stream.

        :param input: the result object to dump, or the filename as string, or a stream object
        :param output: optional output argument, as a string, or as a stream object
        :return: a string if ``output`` is None, None otherwise
        """
        result = self._result_loader(input)

        # Export to output
        if output is None:
            with StringIO() as string:
                self._dump(result, string)
                return string.getvalue()

        if isinstance(output, IOBase):
            self._dump(result, output)

        if isinstance(output, str):
            if output == self.Constants.OUTPUT_FILE_IS_STDOUT:
                self._dump(result, sys.stdout)  # type: ignore
            if output == self.Constants.OUTPUT_FILE_IS_STDERR:
                self._dump(result, sys.stderr)  # type: ignore
            with open(output, "w") as filepointer:
                self._dump(result, filepointer)

    def default_parameters(self) -> TParams:
        r"""
        Declares the default parameter object. Includes custom parameters.

        :return: dictionary with default parameters values
        """
        base_params: TParams = {
            self.default_input_file_param_name: self.default_input_file_param_value,
            self.default_output_file_param_name: self.default_output_file_param_value,
        }
        base_params.update(self.default_custom_parameters)
        return base_params
    
    def _result_loader(self, input: TResultArg) -> Result:
        r"""
        Load result file or stream
        """
        import io

        if isinstance(input, Result):
            return input
        
        result = Result()

        if isinstance(input, str):
            if input == self.Constants.INPUT_FILE_IS_STDIN:
                from qc_baselib.models.result import CheckerResults
                # FIXME: this should be fixed when public interface for Result is completed
                checker_results = CheckerResults.from_xml(sys.stdin.read().encode("utf-8"))
                result._report_results = checker_results
            else:
                result.load_from_file(input)
            return result
        

        if isinstance(input, io.IOBase):
            from qc_baselib.models.result import CheckerResults
            # FIXME: this should be fixed when public interface for Result is completed
            checker_results = CheckerResults.from_xml(input.read().encode("utf-8"))
            result._report_results = checker_results
        
        return result

    def _dump(self, result: "Result", output: TStream):
        r"""
        Dump logic. Input is a result object, output is the provided stream.

        .. info:: Override this function to create a custom report formatter with custom
                  dump logic from Result objet to text or binary data.

        .. warning:: This function shall not open or close the output stream

        :param result: result object to dump
        :param output: destination for the dump as an IO object (use ``write`` method)
        """
        if self._dump_function is None:
            raise NotImplementedError("Missing dump function for the current implementation")
        self._dump_function(result, self.get_param, output)

    @property
    def module_name(self) -> str:
        r"""
        Defines the module name

        .. info:: override this property to customize via inheritance the report formatter

        :return: module name to be used in default config
        """
        return self._module_name
    
    @property
    def default_input_file_param_name(self):
        r"""
        Defines the default input file name parameter name. Defaults to ``"InputFile"``

        .. info:: override this property to customize via inheritance the report formatter

        :return: input parameter name
        """
        return self._default_input_file_param_name  # type: ignore (dinamically defined)
    
    @property
    def default_input_file_param_value(self):
        r"""
        Defines the default input file name parameter value. Defaults to ``"Result.xqar"``

        .. info:: override this property to customize via inheritance the report formatter

        :return: input parameter value
        """
        return self._default_input_file_param_value  # type: ignore (dinamically defined)
    
    @property
    def default_output_file_param_name(self):
        r"""
        Defines the default output file name parameter name. Defaults to ``"OutputFile"``

        .. info:: override this property to customize via inheritance the report formatter

        :return: output parameter name
        """
        return self._default_output_file_param_name  # type: ignore (dinamically defined)
    
    @property
    def default_output_file_param_value(self):
        r"""
        Defines the default output file name parameter value. Defaults to ``"Report.txt"``

        .. info:: override this property to customize via inheritance the report formatter

        :return: output parameter value
        """
        return self._default_output_file_param_value  # type: ignore (dinamically defined)
    
    @property
    def default_custom_parameters(self) -> TParams:
        r"""
        Defines custom parameters default values for formatter

        .. info:: override this property to customize via inheritance the report formatter

        :return: dictionary of custom parameters
        """
        return self._default_custom_parameters  # type: ignore