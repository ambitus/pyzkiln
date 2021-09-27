#!/usr/bin/env python3
from zoautil_py import opercmd
'''
z/OS Display based on the Auxiliary Storage information
Issue a Display ASM, All and return the contents in a list of
dictionaries. Each dictionary will have the following keys:
TYPE - Type of dataset
%FULL - The percentage of the dataset that is full
STATUS - The current status of the dataset
DEVICE - The device number where the dataset exists
DATASET NAME - The name of the Dataset
PAGEDEL - The status of the PAGEDEL command
'''


def get_asm():

    # Initialize the list to be returned
    asm_list = []

    # Issue the 'D ASM,ALL' operator command.
    cmd_data = opercmd.execute(command="D", parameters="ASM,ALL").to_dict()

    # Deal with an error state
    if cmd_data["rc"] > 0:
        print(f"Data not Retrieved. Return code is: {cmd_data['rc']}")
        asm_list.append(cmd_data)
        return asm_list

    # Create a list of text strings from each line of text
    response_text = cmd_data["stdout_response"].split("\n")

    # Get the System Name
    system_name = (response_text[0].split())[0]

    # Look at each line of text
    for text_line in response_text:

        # Split the text into single strings
        text_list = text_line.split()

        # if the text line is blank
        if len(text_list) == 0:
            break

        # If this is a data line create a dictionary of that data
        if not text_list[0] in [system_name, "TYPE", "PAGEDEL"]:
            asm_list.append({"TYPE": text_list[0],
                             "%FULL": text_list[1],
                             "STATUS": text_list[2],
                             "DEVICE": text_list[3],
                             "DATASET NAME": text_list[4]})

        # If this list contains the PAGEDEL it differently
        if "PAGEDEL" in text_list:
            asm_list.append({"PAGEDEL": f"{text_list[3]} {text_list[4]}"})

    return asm_list


def main():
    '''
    Main routine. Call the get_asm routine
    '''
    print(get_asm())


if __name__ == "__main__":
    main()
