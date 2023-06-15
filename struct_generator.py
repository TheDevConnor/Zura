import os

print("|─ src/")
def print_folder_structure(folder_path, indent=''):
    files = os.listdir(folder_path)
    # files.sort()  # Sort files alphabetically

    for file in files:
        file_path = os.path.join(folder_path, file)
        if os.path.isdir(file_path):
            print(indent + '├── ' + file + '/')
            print_folder_structure(file_path, indent + '│   ')
        else:
            print(indent + '├── ' + file)

# Provide the root folder path here
root_folder = 'C:/Users/conno/Desktop/Azura-v2/src'

print_folder_structure(root_folder)
