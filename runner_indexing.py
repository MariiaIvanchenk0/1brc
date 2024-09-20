import os
import argparse
import subprocess
import sys

def main():
    """
    Run the file reading with the desired number of reruns.
    """
    parser = argparse.ArgumentParser()
    parser.add_argument("times", type=int, help="Number of times to read")

    if len(sys.argv) < 2:
        print("Incorrect number of arguments. Should be exactly 1 argument.")
        sys.exit(1)

    args = parser.parse_args()
    times = args.times

    if "build" not in os.listdir():
        os.system("mkdir build")
        os.chdir("build")
        os.system("cmake -DCMAKE_BUILD_TYPE=Release ..")
        os.system("make")
    else:
        os.chdir("build")

    config_file = "../config1.cfg"

    general = []

    run_command = f"./weather_par {config_file}"
    for _ in range(times):
        subprocess.run(run_command, shell=True, stdout=subprocess.PIPE)
        with open("../res.txt", "r") as file:
            file_content = file.readlines()

        current = []

        for _, line in enumerate(file_content):

            line = line.split("=")[1].split("/")
            min_value = line[0]
            max_value = line[2]
            average_value = line[1]

            current.append(float(min_value))
            current.append(float(max_value))
            current.append(float(average_value))

        general.append(current)


    if compare(general):
        print("All values passed absolute error test.")



def compare(values):

    absolute_err = 0.1

    for i in range(len(values)-1):
        result = values[i]
        result1 = values[i+1]

        for j in range(len(values)-1, 3):

            if (
                    abs(result1[j] - result[j]) > absolute_err
                    and abs(result1[j+1] - result[j+1]) > absolute_err
                    and abs(result1[j+2] - result[j+2]) > absolute_err
            ):
                print("Absolute error test is failed.")
                return False

    return True


if __name__ == "__main__":
    main()
