#!/bin/env python3
###############################################################################
#                                                                             #
# listJobs                                                                    #
# List a set of jobs based on a query string. Return the jobs in a list of    #
# dictionaries.                                                               #
# Arguments: query - a string containing the userid (optional)                #
# Returns:   jobList - a list of jobs                                         #
#            each dictionary contains:                                        #
#            owner - The owner of the job                                     #
#            jobName - The name of the job                                    #
#            jobID - The ID of the job                                        #
#            status - The status of the job                                   #
#                     AC=active                                               #
#                     CC=completed normally                                   #
#                     ABEND=ended abnormally                                  #
#                     ?=error or unknown                                      #
#            completionCode The completion code of the job could be:          #
#                     <number> - maximum condition code of all steps          #
#                     <alpha-numberic> - abend code of the step that died     #
#                      ? - job is still running                               #
###############################################################################
import sys
import subprocess


def listJobs(userId=" "):
    jobsList = []
    if userId != " " and userId.count("/") == 0:
        userId = "/" + userId + "/*"
    result = subprocess.run(["jls", userId], capture_output=True, text=True)
    messages = result.stderr.split()
    if messages[0] != "ISF767I":
        print("listJobs failed: " + result.stderr)
        exit(1)
    jobs = result.stdout
    tempList = jobs.split("\n")
    for job in tempList:
        if len(job) > 0:
            jobData = job.split()
            jobsList.append(
                {
                    "owner": jobData[0],
                    "jobName": jobData[1],
                    "jobID": jobData[2],
                    "status": jobData[3],
                    "completionCode": jobData[4],
                }
            )
    return jobsList


if __name__ == "__main__":
    if len(sys.argv) > 1:
        print(listJobs(sys.argv[1]))
    else:
        print(listJobs())
