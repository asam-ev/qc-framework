# Copyright 2024, ASAM e.V.
#
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
from .arguments import declare_arguments
from .application import Application

import logging
_log = logging.getLogger(__name__)

def main():
    r"""
    Main entrypoint for the JSON report utility.
    """

    args = declare_arguments()

    if args.defaultconfig:
        Application.export_default_config()
        return 0
    
    if args.result:
        Application.export_result_file(args.result)
        return 0
    
    if args.configuration:
        Application.export_result_file_from_config(args.configuration)
        return 0

    _log.error("Invalid arguments (unrecognized input file %s)", args.filename)
    return 1


if __name__ == "__main__":
    main()