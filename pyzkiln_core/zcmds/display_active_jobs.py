#!/usr/bin/env python3
from zoautil_py import opercmd
'''
z/OS Display Active Jobs
Issue a Display Jobs on the console and return the contents in an
array of dictionaries. This Dictionary keys match the columns
that would normally appear in response to a display job list
operator command.
The first dictionary in the list contains numbers. The keys are:
 JOBS -
 MS -
 TS USERS -
 SYSAS -
 INITS -
 ACTIVE/MAX -
 VTAM -
 OAS -
The rest of the dictionaries in the list contain information
about a specific job. The keys are:
 JobName - The name of the job
 StepName - The name of the currently executing step
 ProcName - The name of the proc/step of the executing code
 Swap Status - Whether the address space is swappable
 JobType - The type of job (S is started task - O is Unix)
'''


def display_active_jobs():
    '''
    This function runs the opercmd then formats the output
    '''
    # initialize the list that will hold the output
    activejoblist = []

    # Issue the DISPLAY JOBS operator command
    cmd_dictionary = opercmd.execute("DISPLAY J,LIST").to_dict()

    # If there is a problem return the problem as the list item
    if cmd_dictionary["rc"] > 0:
        print(f"Data not Retrieved. Return code is: {cmd_dictionary['rc']}")
        activelist = [cmd_dictionary["rc"], cmd_dictionary["stder_response"]]
        activejoblist.append(activelist)
        return activejoblist

    # Now start to deal with the text.
    cmdtext = cmd_dictionary["stdout_response"]

    # Get to the text we care about and make them separate list lines
    textlines = cmdtext[cmdtext.find("\n", cmdtext.find("JOBS")+1)+1:].splitlines()

    firstline = True
    for textline in textlines:
        textline = textline.strip()
        # The first line contains overall info make it a separate dictionary
        if firstline is True:
            firstline = False

            activejoblist.append({"JOBS": textline[0:8].strip(),
                                  "M/S": textline[10:16].strip(),
                                  "TS USERS": textline[18:23],
                                  "SYSAS": textline[29:34],
                                  "INITS": textline[38:43],
                                  "ACTIVE/MAX": textline[48:59],
                                  "VTAM": textline[60:65],
                                  "OAS": textline[66:71]})

        else:
            # Make sure the spaces are gone
            textline = textline.strip()

            # Create a dictionary from the first set of data
            activejoblist.append({"JobName": textline[0:8].strip(),
                                  "StepName": textline[9:16].strip(),
                                  "ProcName": textline[18:26].strip(),
                                  "Swap Status": textline[27:31].strip(),
                                  "JobType": textline[32:34].strip()})
            # Make Sure there is a second set of data
            if len(textline) > 34:
                # Create a dictionary from the second set of data
                activejoblist.append({"JobName": textline[35:43].strip(),
                                      "StepName": textline[44:52].strip(),
                                      "ProcName": textline[53:61].strip(),
                                      "Swap Status": textline[62:66].strip(),
                                      "JobType": textline[67:]})

    # Return the array of dictionaries that hold the information.
    return activejoblist


if __name__ == "__main__":
    print(display_active_jobs())
