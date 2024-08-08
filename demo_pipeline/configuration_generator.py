# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

import sys, os, glob
from lxml import etree

XODR_TEMPLATE_PATH = "/app/demo_pipeline/templates/xodr_template.xml"
XOSC_TEMPLATE_PATH = "/app/demo_pipeline/templates/xosc_template.xml"
OTX_TEMPLATE_PATH = "/app/demo_pipeline/templates/otx_template.xml"
GENERATED_CONFIG_PATH = "/tmp/generated_config"


def update_param_value(xml_file, name, new_value, output_file):
    # Parse the XML file
    tree = etree.parse(xml_file)
    root = tree.getroot()

    # Find the Param element with the name attribute and update its value attribute
    for param in root.findall(f".//Param[@name='{name}']"):
        param.set("value", new_value)

    # Write the updated XML to the output file
    tree.write(output_file, encoding="utf-8", pretty_print=True, xml_declaration=True)


def main():
    input_directory = "/input_directory"
    input_filename = os.getenv("INPUT_FILENAME")

    full_input_path = os.path.join(input_directory, input_filename)

    os.makedirs(GENERATED_CONFIG_PATH, exist_ok=True)

    if not os.path.isfile(full_input_path):
        print(
            "No file specified as input. Please provide xosc, xodr or otx file. Exiting..."
        )
        return

    print("Input file: ", full_input_path)
    _, input_file_extension = os.path.splitext(full_input_path)

    if input_file_extension == ".xosc":
        print("XOSC selected")
        update_param_value(
            XOSC_TEMPLATE_PATH,
            "InputFile",
            full_input_path,
            os.path.join(GENERATED_CONFIG_PATH, "config.xml"),
        )
    elif input_file_extension == ".xodr":
        print("XODR selected")
        update_param_value(
            XODR_TEMPLATE_PATH,
            "InputFile",
            full_input_path,
            os.path.join(GENERATED_CONFIG_PATH, "config.xml"),
        )
    elif input_file_extension == ".otx":
        print("OTX selected")
        update_param_value(
            OTX_TEMPLATE_PATH,
            "InputFile",
            full_input_path,
            os.path.join(GENERATED_CONFIG_PATH, "config.xml"),
        )
    else:
        print(f"Error in input file extension. Unrecognized {input_file_extension}")
        return


if __name__ == "__main__":
    main()
