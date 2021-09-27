#!/usr/bin/env python3
import sys
import os
import argparse
from zoautil_py import opercmd
'''
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
 State - The sate of that paramter
'''


def iplinfo(sysparm=None):
    '''
    iplinfo function:
    Take the input and return a dictionary based on the input. The Current
    supported inputs are none or a sysparm to be queried
    '''
    if sysparm is None:
        cmdData = opercmd.execute("DISPLAY IPLINFO").to_dict()
    else:
        cmdData = opercmd.execute(f"DISPLAY IPLINFO,{sysparm}").to_dict()

    if cmdData["rc"] > 0:
        print(f"Data not Retrieved. Return code is: {cmdData['rc']}")
        return cmdData

    cmdresponse = cmdData['stdout_response']
    if "IEE254I" in cmdresponse:
        cmdlist = cmdresponse.split("\n")
        time = date = release = license = used = indata = ondata = ""
        archlvl = mltshr = ieasym = ieasys = iodfcurr = iodforigin = ""
        iplorigin = iplcurr = iplvol = ""
        for line in cmdlist:
            if "SYSTEM IPLED" in line:
                time = line[line.find("AT")+3:line.find("AT")+11]
                date = line[line.find("ON")+3:]

            if "RELEASE" in line:
                release = line[line.find("RELEASE")+8:
                               line.find("LICENSE")-1].strip()
                license = line[line.find("=")+2:]

            if "USED" in line:
                used = line[line.find("USED")+5:line.find("IN")-1].strip()
                indata = line[line.find("IN")+3:line.find("ON")-1].strip()
                ondata = line[line.find("ON")+3:].strip()

            if "ARCHLVL" in line:
                archlvl = line[line.find("ARCHLVL =")+10:
                               line.find("MTL")].strip()
                mltshr = line[line.find("MTLSHARE =")+12:].strip()

            if "IEASYM" in line:
                ieasym = line[line.find("(")+1:line.find(")")-1]

            if "IEASYS" in line:
                ieasys = line[line.find("("):]

            if "IODF" in line:
                iodforigin = line[line.find("ORIGINAL(")+9:line.find(")")-1]
                iodfcurr = line[line.find("CURRENT(")+9:line.find(")",
                                line.find(")")+1)]

            if "IPL DEVICE" in line:
                iplorigin = line[line.find("ORIGINAL(")+10:line.find(")")-1]
                iplcurr = line[line.find("CURRENT(")+9:line.find(")",
                               line.find(")")+1)]
                iplvol = line[line.find("VOLUME(")+8:line.rfind(")")-1]

        return_dictionary = {"IPL Time": time, "IPL Date": date,
                             "Release": release, "License": license,
                             "Member Used": used, "Dataset": indata,
                             "Device": ondata, "Arch Level": archlvl,
                             "MLT Share": mltshr, "IEASYM List": ieasym,
                             "IEASYS List": ieasys,
                             "Original IODF": iodforigin,
                             "Current IODF": iodfcurr,
                             "Original IPL Device": iplorigin,
                             "Current IPL Device": iplcurr,
                             "IPL Volume": iplvol}

    if "IEE255I" in cmdresponse:
        parm = cmdresponse[cmdresponse.find("'")+1:cmdresponse.rfind("'")]
        state = cmdresponse[cmdresponse.rfind(":")+1:].strip()
        return_dictionary = {"Parameter": parm, "State": state}

    return return_dictionary


def parse_args(argv=None):
    '''
    This function is responsible for handling arguments. It relies on
    the argparse module.
    '''
    program_name = os.path.basename(sys.argv[0])

    if argv is None:
        argv = sys.argv[1:]

    try:
        parser = argparse.ArgumentParser(program_name)
        parser.add_argument("-s", "--sysparm", default=None,
                            help="A sysparm you want to query.")
        opts = parser.parse_args(argv)
        return opts

    except Exception as e:
        indent = len(program_name) * " "
        sys.stderr.write(program_name + ": " + repr(e) + "\n")
        sys.stderr.write(indent + "  for help use --help")
        sys.exit(1)


def main():
    '''
    main function:
    gather parms and call the iplinfo function
    '''
    args = parse_args()
    print(iplinfo(args.sysparm))


if __name__ == "__main__":
    main()
