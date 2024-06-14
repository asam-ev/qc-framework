# Copyright 2024, ASAM e.V.
# This Source Code Form is subject to the terms of the Mozilla
# Public License, v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

import sys, os, glob
from lxml import etree
import subprocess


def update_param_value(xml_file, name, new_value, output_file):
    # Parse the XML file
    tree = etree.parse(xml_file)
    root = tree.getroot()

    # Find the Param element with the name attribute "config" and update its value attribute
    for param in root.findall(f".//Param[@name='{name}']"):
        param.set("value", new_value)

    # Write the updated XML to the output file
    tree.write(output_file, encoding="utf-8", pretty_print=True, xml_declaration=True)


def main():
    input_path = "/input_path"
    input_name = os.getenv("INPUT_NAME")

    input_file = None

    full_input = os.path.join(input_path, input_name)

    XODR_TEMPLATE_PATH = "/app/demo_pipeline/templates/xodr_template.xml"
    XOSC_TEMPLATE_PATH = "/app/demo_pipeline/templates/xosc_template.xml"

    if os.path.isdir(full_input):
        print("Folder specified as input. Searching for xosc or xodr files..")
        xosc_files = glob.glob(os.path.join(full_input, f"*.xosc"))
        xodr_files = glob.glob(os.path.join(full_input, f"*.xodr"))
        all_files = xosc_files + xodr_files
        if all_files:
            input_file = all_files[0]
        else:
            print(
                f"""No xosc or xodr file found in {full_input}.
                \nImpossible to run demo runtime.
                \n Exiting..."""
            )
            return
    elif os.path.isfile(full_input):
        print(f"File specified as input. Using {full_input}")
        input_file = full_input
    else:
        print("Error. Impossible to read specified input. Exiting...")
        return

    print("Input file: ", input_file)
    _, input_file_extension = os.path.splitext(input_file)

    if input_file_extension == ".xosc":
        print("XOSC selected")
        update_param_value(
            XOSC_TEMPLATE_PATH, "XoscFile", input_file, "/out/generated_xosc.xml"
        )
    elif input_file_extension == ".xodr":
        print("XODR selected")
        update_param_value(
            XOSC_TEMPLATE_PATH, "XodrFile", input_file, "/out/generated_xodr.xml"
        )
    else:
        print(f"Error in input file extension. Unrecognized {input_file_extension}")

    ### TEST

    config_xml = "/out/generated_xosc.xml"
    install_dir = "/app/framework/build/bin"
    schema_dir = "/app/framework/doc/schema"
    runtime_script = "/app/framework/runtime/runtime/runtime.py"

    process = subprocess.Popen(
        f"python3 {runtime_script} --config={config_xml} --install_dir={install_dir} --schema_dir={schema_dir}",
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        cwd="/out",
    )
    stdout, stderr = process.communicate()
    exit_code = process.returncode
    if exit_code == 0:
        print("Command executed successfully.")
        print("Output:")
        print(stdout.decode())
    else:
        print("Error occurred while executing the command.")
        print("Error message:")
        print(stderr.decode())


if __name__ == "__main__":
    main()
