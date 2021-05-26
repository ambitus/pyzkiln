#!/usr/bin/env python3
from zoautil_py import opercmd


#####################################################################
# z/OS Display Active Jobs                                          #
# Issue a Display Jobs on the console and return the contents in an #
# array of dictionaries. This Dictionary keys match the columns     #
# that would normally appear in response to a display job list      #
# operator command.                                                 #
# The first dictionary in the list contains numbers. The keys are:  #
# JOBS -                                                            #
# MS -                                                              #
# TS USERS -                                                        #
# SYSAS -                                                           #
# INITS -                                                           #
# ACTIVE/MAX -                                                      #
# VTAM -                                                            #
# OAS -                                                             #
# The rest of the dictionaries in the list contain information      #
# about a specific job. The keys are:                               #
# JobName - The name of the job                                     #
# StepName - The name of the currently executing step               #
# ProcName - The name of the proc/step of the executing code        #
# Swap Status - Whether the address space is swappable              #
# JobType - The type of job (S is started task - O is Unix)         #
#####################################################################
def displayActiveJobs():
    activejoblist = []
    #################################################################
    # First issue the DISPLAY JOBS operator command                 #
    #################################################################
    cmdData = opercmd.execute("DISPLAY J,LIST").to_dict()
    if cmdData["rc"] > 0:
        print("Data not Retrieved. Return code is: " + cmdData["rc"])
        activelist = [cmdData["rc"], cmdData["stder_response"]]
        activejoblist.append(activelist)
        return activejoblist

    #################################################################
    # Now start to deal with the text.                              #
    #################################################################
    cmdtext = cmdData["stdout_response"]
    #################################################################
    # Remove the content that no one needs. This includes the       #
    # identity that issued the command and the command itself. Also #
    # remove the title row.                                         #
    #################################################################
    for count in range(5):
        tuple = cmdtext.partition("\n")
        cmdtext = tuple[2]
    #################################################################
    # Now we take each line, break it into a list and put each into #
    # a dictionary.                                                 #
    #################################################################
    textlines = cmdtext.splitlines()
    firstline = True
    for textline in textlines:
        ##############################################################
        # The first line contains overall info                       #
        ##############################################################
        if firstline is True:
            firstline = False
            dataDict = {
                "JOBS": textline[0:8],
                "M/S": textline[10:16],
                "TS USERS": textline[18:23],
                "SYSAS": textline[29:34],
                "INITS": textline[38:43],
                "ACTIVE/MAX": textline[48:59],
                "VTAM": textline[60:65],
                "OAS": textline[66:71],
            }
            # Add the dictionary to the array
            activejoblist.append(dataDict)
        else:
            textline = textline.strip()
            dataDict = {
                "JobName": textline[0:8].strip(),
                "StepName": textline[9:16].strip(),
                "ProcName": textline[18:26].strip(),
                "Swap Status": textline[27:31].strip(),
                "JobType": textline[32:34].strip(),
            }
            activejoblist.append(dataDict)
            dataDict = {
                "JobName": textline[35:43].strip(),
                "StepName": textline[44:52].strip(),
                "ProcName": textline[53:61].strip(),
                "Swap Status": textline[62:66].strip(),
                "JobType": textline[67:],
            }
            activejoblist.append(dataDict)
    # Return the array of dictionaries that hold the information.
    return activejoblist


if __name__ == "__main__":
    print(displayActiveJobs())
