# ASAM Quality Checker Framework

This framework will allow users to verify the conformity of files and implementations against ASAM standards, fostering greater adoption and understanding of the standards as well as significantly improving interoperability. The framework shall be standard agnostic, allowing the execution of a wide variety of both ASAM and user-defined checks for different standards.  

The framework shall also enable the use of additional user-defined checks, whether proprietary or open, to meet the requirements of the wide spectrum of users of ASAM standards. A detailed configuration management and result reporting shall also be included.  

## High level requirements 

- Environment Flexibility: The framework will be designed to function seamlessly both in local machine environments and server-based settings, allowing users flexibility in deployment. 

- Traceability: One of the primary goals is to provide clear traceability of issues to source code. This will enable users to swiftly identify and rectify the discrepancies in the data or implementation. 

- Extensibility: Recognizing the diverse technological ecosystems, the framework will permit defining checks in any programming language. This ensures the widest possible participation and utilization. 

- Checker Bundles: To cater to the complex and layered nature of ASAM standards, the framework will allow the bundling of checks into executable "checker bundles." This modular approach ensures comprehensive coverage and easier management. 

- Parameterization: The checks can be parameterized, enabling users to adjust the verification process's stringency or focus according to their specific needs. 

- Reporting Capabilities: The framework will provide detailed reports in diverse formats. A dedicated GUI will allow users to interact with the results, offering features like filtering by checker bundle and directly viewing discrepancies in the input file source code. 
