# Copyright 2024, ASAM e.V.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
from typing import TYPE_CHECKING
from .constants import Constants

if TYPE_CHECKING:
    from argparse import Namespace


def declare_arguments() -> "Namespace":
    r"""
    Declare and parses arguments for the report tool
    
    :return: the Namespace object of the parsed argument
    """
    from argparse import ArgumentParser

    args = ArgumentParser(
        prog=Constants.MODULE_NAME,
        description="Convert Quality Framework result file in a JSON report file",
        epilog="Quality Framework, Copyright 2024, ASAM e.V.: License MPLv2",
    )

    group = args.add_mutually_exclusive_group(required=True)
    group.add_argument("-c", "--configuration", help="run the application with an input configuration file")
    group.add_argument("-r", "--result", help="run the application with an input result file")
    group.add_argument(
        "--defaultconfig",
        action="store_true",
        help="write default configuration. Output filename is"
        f" {Constants.DEFAULT_OUTPUT_CONFIG_FILENAME}",
    )

    return args.parse_args()
