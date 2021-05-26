#!/usr/bin/env python3

import re
from zoautil_py import opercmd


def _getParmItem(textLine):
    itemSplitWords = [
        "THE FOLLOWING ARE THE OPTIONS FOR THE (.*?) STATEMENT:",
        "CURRENT SETTINGS FOR (.*?):",
        "THE FOLLOWING ARE THE PARMLIB OPTIONS FOR (.*?):",
        "THE FOLLOWING ARE THE (.*?) PAIRS FOR TRANSREC:",
        "THERE ARE NO (.*?) COMMANDS *",
        "THERE ARE NO (.*?) PROGRAMS *",
        "CURRENT PARMLIB SETTINGS FOR (.*?):",
    ]

    for splitWord in itemSplitWords:
        matched = re.search(splitWord, textLine)
        if matched:
            return matched.group(1)

    return None


def _splitByColon(textLine):
    # Used to split the statement with colon
    # E.g.
    # THE FOLLOWING ARE THE PARMLIB OPTIONS FOR TRANSREC
    # DAPREFIX: TUPREFIX  SPOOLCL:  B          CIPHER:  YES
    # OUTWARN:  10000     ,5000                SYSOUT:  *
    # SYSCTL:             LOGNAME: MISC
    ret = {}
    text = textLine.strip()
    while len(text) > 0:
        index = text.find(":")
        if index == -1:
            return ret

        key = text[0:index].strip()

        # Extract the value
        remainText = text[index + 1:].strip()
        # Find the next colon first
        nextColonIndex = remainText.find(":")
        if nextColonIndex == -1:
            value = remainText[0:]
            text = ""
        else:
            # find the last space and get the value for the first colon / key
            rspaceIndex = remainText[0:nextColonIndex].rfind(" ")
            # Should find the last space
            if rspaceIndex == -1:
                value = ""
            else:
                value = remainText[0:rspaceIndex].strip()

            text = remainText[rspaceIndex + 1:]

        # Add the pair
        ret[key] = value

    return ret


#####################################################################
# z/OS Display the PARMLIB settings                                 #
# Issue a Display IKJTSO,ALL and return the contents                #
# in an dictionaries                                                #
#####################################################################
def getParmlib():
    parmlibDict = {}

    #################################################################
    # First issue the 'D IKJTSO,ALL' operator command.              #
    #################################################################
    cmdData = opercmd.execute(command="D", parameters="IKJTSO,ALL").to_dict()
    rc = cmdData["rc"]
    parmlibDict["rc"] = rc
    if rc != 0:
        print("Data not Retrieved. Return code is: " + str(rc))
        parmlibDict["err_response"] = cmdData["stderr_response"]
    else:
        #############################################################
        # Now start to deal with the text.                          #
        #############################################################
        dataDict = {}
        cmdText = list(filter(None, cmdData["stdout_response"].split("\n")))

        currItem = None
        # List or Dict
        currValueListType = True
        currValueList = []
        currValueDict = {}

        mutiSpaces = " ".rjust(54)
        cmdLen = len(cmdText)
        lineNum = 0
        while lineNum < cmdLen:
            textLine = cmdText[lineNum]
            # If the next lines start with 54 spaces
            # concatenat to the current line
            while (
                lineNum < cmdLen-1 and
                cmdText[lineNum+1].startswith(mutiSpaces)
            ):
                textLine = textLine + " " + cmdText[lineNum+1].strip()
                lineNum = lineNum + 1

            parmItem = _getParmItem(textLine)
            # Get each statement in the "D IKJTSO,ALL"
            if parmItem:
                # If already got one statement before this item
                # Means that we get all the output of this current statement
                # Store it
                if currItem:
                    if currValueListType:
                        dataDict[currItem] = currValueList
                    else:
                        # The "NODE/SMF" is part of "TRANSREC"
                        # use the key "TRANSREC" to store
                        if currItem == "NODE/SMF":
                            currItem = "TRANSREC"

                        # If already storing the "TRANSREC"
                        # merge the "TRANSREC" and "NODE/SMF" values
                        # Else
                        #     Store the statement directly
                        if currItem == "TRANSREC" and dataDict.get(currItem):
                            dataDict[currItem].update(currValueDict)
                        else:
                            dataDict[currItem] = currValueDict

                # Change the current statement item to the new one in this line
                # Reset the values
                currItem = parmItem
                currValueListType = True
                currValueList = []
                currValueDict = {}

                # If statement value is in the same line with statement item
                # Like:
                #    THE FOLLOWING ARE THE OPTIONS FOR THE ALLOCATE
                #     STATEMENT: DEFAULT  (OLD)
                # Extract the value and contine to handle the value
                # Else
                #    No value to handle (i.e. this line is 'empty')
                index = textLine.find(":")
                if index != -1:
                    textLine = textLine[textLine.find(":")+1:]
                else:
                    textLine = ""

            if currItem and textLine:
                # Handle the "CONSOLE" statement
                # Split by the space and store as key/value pair
                # E.g. statement values like
                #     INITUNUM              1000
                #     INITSNUM              1000
                if currItem in ["CONSOLE"]:
                    currValueListType = False
                    keyValuePair = list(filter(
                                    None, textLine.strip().split(" ")
                                    ))
                    currValueDict[keyValuePair[0]] = keyValuePair[1]

                # Handle the "HELP", "TEST", "TRANSREC" statements
                # Split by the colon and store as key/value pair
                # E.g. statement values like
                #     LANGUAGE: ENU  DATASET(S): SYS1.HELP
                elif currItem in ["HELP", "TEST", "TRANSREC"]:
                    currValueListType = False
                    currValueDict.update(_splitByColon(textLine))

                # Handle the "NODE/SMF" statement
                # Consider it as the subset of "TRANSREC" and merge finally
                # E.g. statement value like
                #     THE FOLLOWING ARE THE NODE/SMF PAIRS FOR TRANSREC:
                #     *       /  *
                elif currItem in ["NODE/SMF"]:
                    currValueListType = False
                    if currValueDict.get("NODE/SMF"):
                        currValueDict["NODE/SMF"].append(textLine.strip())
                    else:
                        currValueDict["NODE/SMF"] = [textLine.strip()]

                # Handle the "SEND" and "LOGON" statements
                # Split by the '()' and store as key/value pair
                # E.g. statement value like
                #    PASSPHRASE(ON)
                #    OPERSEND(ON)
                elif currItem in ["SEND", "LOGON"]:
                    currValueListType = False
                    index = textLine.find("(")
                    key = textLine[0:index].strip()
                    value = textLine[index + 1:].strip().strip(")")
                    currValueDict[key] = value

                # Handle the statements for
                # "ALLOCATE", "AUTHCMD", "AUTHPGM", "AUTHTSF", and "NOTBKGND"
                # Split by the space and store as list
                # Consider this output format as the default one
                # E.g. statement value like
                #     OPER      OPERATOR  TERM      TERMINAL
                else:
                    currValueListType = True
                    for itemValue in list(
                        filter(None, textLine.strip().split(" "))
                    ):
                        currValueList.append(itemValue)

            lineNum = lineNum + 1

        # Store the last statement item in the "D IKJTSO,ALL"
        if currValueListType:
            dataDict[currItem] = currValueList
        else:
            # In case the last statement item is "TRANSREC" or "NODE/SMF"
            # The "NODE/SMF" is part of "TRANSREC"
            # use the key "TRANSREC" to store
            if currItem == "NODE/SMF":
                currItem = "TRANSREC"

            # If already storing the "TRANSREC"
            #     merge the "TRANSREC" and "NODE/SMF" values
            # Else
            #     Store the statement directly
            if currItem == "TRANSREC" and dataDict.get(currItem):
                dataDict[currItem].update(currValueDict)
            else:
                dataDict[currItem] = currValueDict

        parmlibDict["data"] = dataDict

    return parmlibDict


if __name__ == "__main__":
    print(getParmlib())
