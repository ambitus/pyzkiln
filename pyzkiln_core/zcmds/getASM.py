#!/usr/bin/env python3

import re
from zoautil_py import opercmd


#####################################################################
# z/OS Display based on the Auxiliary Storage information           #
# Issue a Display ASM, ALL and return the contents in an list of    #
# dictionaries. This Dictionary keys are                            #
# "TYPE", "%FULL", "STATUS", "DEVICE","DATASET NAME"                #
#####################################################################
def getAsm():
    asmDict = {}

    #################################################################
    # First issue the 'D ASM,ALL' operator command.                #
    #################################################################
    cmdData = opercmd.execute(command="D", parameters="ASM,ALL").to_dict()
    rc = cmdData["rc"]
    asmDict["rc"] = rc
    if rc != 0:
        print("Data not Retrieved. Return code is: " + str(rc))
        asmDict["err_response"] = cmdData["stderr_response"]
    else:
        #############################################################
        # Now start to deal with the text.                          #
        #############################################################
        asmList = []
        keys = ["TYPE", "%FULL", "STAT", "DEV", "DATASET NAME"]
        cmdText = cmdData["stdout_response"].split("\n")
        bFind = False
        for textLine in cmdText:
            if not bFind:
                #####################################################
                # Find the title line first                         #
                # 'TYPE     FULL STAT   DEV  DATASET NAME'          #
                #####################################################
                if -1 != textLine.find(
                    "TYPE     FULL STAT   DEV  DATASET NAME"
                ):
                    bFind = True
            else:
                #####################################################
                # Now we take each line, break it into a list,      #
                # put each into a dictionary if the 2nd element     #
                # in the list is percent.                           #
                #####################################################
                asmInfo = list(filter(None, textLine.strip().split(" ")))
                if (
                    len(asmInfo) == 5 and
                    re.match(r"([0-9]{1,3})%$", asmInfo[1])
                ):
                    asmList.append(dict(zip(keys, asmInfo)))

        asmDict["data"] = asmList

    return asmDict


if __name__ == "__main__":
    print(getAsm())
