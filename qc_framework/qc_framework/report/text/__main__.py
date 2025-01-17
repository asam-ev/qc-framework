# SPDX-License-Identifier: MPL-2.0
# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.


def main():
    from ..base import ReportApplication
    from .text_formatter import TextFormatter

    formatter = TextFormatter()
    app = ReportApplication(
        formatter,
        prog="qc_report_text",
        description="Creates report in human readable text format")
    return app.main()


if __name__ == "__main__":
    main()