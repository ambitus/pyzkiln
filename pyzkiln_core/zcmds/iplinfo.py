#!/usr/bin/env python3
"""
iplinfo
return the IPL info of the z/OS system as a dictionary.

if you run iplinfo without any arguments you will get the message
IEE254I. That will generate a dictionary with the following keys:
 IPL Time - The time of the IPL
 IPL Date - The date of the IPL
 Release - The Operating System and it's release
 License - The Operating System License
 Member Used - The Member containing Information used for the IPL
 Dataset - The dataset in which that member is found
 Device - The Device on which the Dataset lives
 Arch Level - The Architechture level
 MLT Share - Member Tape Library Share info
 IEASYM List - System used symbols list
 IEASYS List - The operator entered Symbols at IPL Time
 Original IODF - The Original IODF Device
 Current IODF - The Current IODF Device
 Original IPL Device - The original IPL device
 Current IPL Device - The current IPL device
 IPL Volume - The Volume where the IPL device lives

If you run iplinfo with a sysparm, it will return a dictionary
with the following Keys
 Parameter - The parameter queried
 State - The state of that paramter
"""
import sys
import os
import argparse
from zoautil_py import opercmd


def iplinfo(sysparm=None):
    """
    Format the ipl data from the operator command.
    The Current supported inputs are none or a sysparm to be queried
    """
    # define a dictionary in which the data will be placed
    return_dictionary = {}
    if sysparm is None:
        cmd_data = opercmd.execute("DISPLAY IPLINFO").to_dict()
    else:
        cmd_data = opercmd.execute(f"DISPLAY IPLINFO,{sysparm}").to_dict()

    if cmd_data["rc"] > 0:
        print(f"Data not Retrieved. Return code is: {cmd_data['rc']}")
        return cmd_data

    cmdresponse = cmd_data["stdout_response"]
    if "IEE254I" in cmdresponse:
        cmdlist = cmdresponse.split("\n")
        for line in cmdlist:
            if "SYSTEM IPLED" in line:
                return_dictionary["IPL Time"] = line[
                    line.find("AT") + 3 : line.find("AT") + 11
                ]
                return_dictionary["IPL Date"] = line[line.find("ON") + 3 :]

            elif "RELEASE" in line:
                return_dictionary["Release"] = line[
                    line.find("RELEASE") + 8 : line.find("LICENSE") - 1
                ].strip()
                return_dictionary["License"] = line[line.find("=") + 2 :]

            elif "USED" in line:
                return_dictionary["Member Used"] = line[
                    line.find("USED") + 5 : line.find("IN") - 1
                ].strip()
                return_dictionary["Dataset"] = line[
                    line.find("IN") + 3 : line.find("ON") - 1
                ].strip()
                return_dictionary["Device"] = line[line.find("ON") + 3 :].strip()

            elif "ARCHLVL" in line:
                return_dictionary["Arch Level"] = line[
                    line.find("ARCHLVL =") + 10 : line.find("MTL")
                ].strip()
                return_dictionary["MLT Share"] = line[
                    line.find("MTLSHARE =") + 11 :
                ].strip()

            elif "IEASYM" in line:
                return_dictionary["IEASYM List"] = line[
                    line.find("(") + 1 : line.rfind(")")
                ]

            elif "IEASYS" in line:
                return_dictionary["IEASYS List"] = line[line.find("(") :]

            elif "IODF" in line:
                return_dictionary["Original IODF"] = line[
                    line.find("ORIGINAL(") + 9 : line.find(")")
                ]
                return_dictionary["Current IODF"] = line[
                    line.find("CURRENT(") + 8 : line.find(")", (line.find(")") + 1))
                ]

            elif "IPL DEVICE" in line:
                return_dictionary["Original IPL Device"] = line[
                    line.find("ORIGINAL(") + 9 : line.find(")")
                ]
                return_dictionary["Current IPL Device"] = line[
                    line.find("CURRENT(") + 8 : line.find(")", line.find(")") + 1)
                ]
                return_dictionary["IPL Volume"] = line[
                    line.find("VOLUME(") + 7 : line.rfind(")")
                ]

    if "IEE255I" in cmdresponse:
        return_dictionary["Parameter"] = cmdresponse[
            cmdresponse.find("'") + 1 : cmdresponse.rfind("'")
        ]
        return_dictionary["State"] = cmdresponse[cmdresponse.rfind(":") + 1 :].strip()

    return return_dictionary


def parse_args(argv=None):
    """
    Handle the arguments.
    It relies on the argparse module.
    """
    program_name = os.path.basename(sys.argv[0])

    if argv is None:
        argv = sys.argv[1:]

        parser = argparse.ArgumentParser(program_name)
        parser.add_argument(
            "-s", "--sysparm", default=None, help="A sysparm you want to query."
        )
    opts = parser.parse_args(argv)
    return opts


def main():
    """
    main function:
    gather parms and call the iplinfo function
    """
    args = parse_args()
    print(iplinfo(args.sysparm))


if __name__ == "__main__":
    main()
