#!/bin/env python3
from pyzkiln_core import display_active_jobs
import sys
##############################################################################
#                                                                            #
# findActiveJobs                                                             #
# Find a set of jobs that are actively running that contain a particular     #
# character string. Reurn those jobs in a dictionary.                        #
# Arguments: searchFor - a string containing the string I am searching for   #
#            searchKey - Key where the searched content should be checked    #
# Returns:   a list of dictionarys that match the criteria in searchFor.     #
#            each dictionary will have the following keys:                   #
#            JobName - The name of the job                                   #
#            StepName - The name of the currently executing step             #
#            ProcName - The name of the proc/step of the executing code      #
#            Swap Status - Whether the address space is swappable            #
#            JobType - The type of job (S is started task - O is Unix)       #
##############################################################################


def _primitivefind(base, content, key):
    results = []

    for index in range(1, len(base)):
        if base[index][key].find(content) > -1:
            results.append(base[index])
    return results


def findActiveJobs(searchFor, searchKey="JobName"):
    foundJobs = []

    # First we will call displayActiveJobs to get all of the active jobs     #

    allActiveJobs = displayActiveJobs()

    # if the searchKey is any key make sure you check them all               #

    if searchKey == "any":
        searchKeys = ["JobName", "StepName", "ProcName",
                      "Swap Status", "JobType"]
    else:
        searchKeys = [searchKey]

    # Loop through the list of keys to make sure you find the string         #

    for key in searchKeys:
        latestList = _primitivefind(allActiveJobs, searchFor, key)
        if len(latestList) > 0:
            foundJobs.extend(latestList)

    # Remove any duplicates                                                   #

    foundJobs = [i for n, i in enumerate(foundJobs)
                 if i not in foundJobs[n + 1:]]

    return foundJobs


if __name__ == "__main__":
    print(sys.argv[1])
    print(sys.argv[2])
    print(findActiveJobs(sys.argv[1], sys.argv[2]))
