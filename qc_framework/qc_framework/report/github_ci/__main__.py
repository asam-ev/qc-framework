# SPDX-License-Identifier: MPL-2.0
# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.


def main():
    from ..base import ReportApplication
    from .github_ci_formatter import GithubCIFormatter

    class GithubCIApplication(ReportApplication):
        def __init__(self, formatter: GithubCIFormatter, *args, **kwargs):
            self.return_issues = False
            super().__init__(
                formatter,
                prog="qc_report_github_ci",
                description="Creates report to be used in GithubCI pipelines. Exit with error for issues",
                *args,
                **kwargs
            )

        def main(self):
            exit_value = super().main()
            if self.return_issues:
                return 2 if self._formatter.has_issue else 0  # type: ignore
            return exit_value

        def report_from_result(self):
            self.return_issues = True
            return super().report_from_result()

    formatter = GithubCIFormatter()
    app = GithubCIApplication(formatter)
    return app.main()


if __name__ == "__main__":
    main()
