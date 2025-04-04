Import("env")
import os, zipfile, shutil
from pathlib import Path

# Get the version number from the build environment.
firmware_version = os.environ.get('VERSION', "")
if firmware_version == "":
    firmware_version = "0.0.1"
firmware_version = firmware_version.lstrip("v")
firmware_version = firmware_version.strip(".")

# Get the ZIP filename from the build environment.
community_project = env.GetProjectOption('custom_community_project', "")

# Get the custom folder from the build environment.
custom_source_folder = env.GetProjectOption('custom_source_folder', "")

# Get the foldername inside the zip file from the build environment.
zip_filename = env.GetProjectOption('custom_zip_filename', "")

platform = env.BoardConfig().get("platform", {})

def copy_fw_files (source, target, env):
    fw_file_name=str(target[0])

    if os.path.exists("./_build/" + custom_source_folder) == False:
        os.makedirs("./_build/" + custom_source_folder + "/Community/firmware")
        shutil.copytree(custom_source_folder + "/Community", "./_build/" + custom_source_folder + "/Community", dirs_exist_ok=True)
        # set FW version within boad.json files
        replacements = {
            "0.0.1": firmware_version
        }
        build_path_json = Path("./_build/" + custom_source_folder + "/Community/boards")
        for file_path in build_path_json.rglob("*.json"):
            replace_in_file(file_path, replacements)
        print("Creating /_build folder")
    
    if platform == "raspberrypi":
        fw_file_name=fw_file_name[0:-3] + "uf2"

    print("Copying community folder")
    shutil.copy(fw_file_name, "./_build/" + custom_source_folder + "/Community/firmware")
    original_folder_path = "./_build/" + custom_source_folder + "/Community"
    zip_file_path = './_dist/' + zip_filename + '_' + firmware_version + '.zip'
    print("Creating zip file")
    createZIP(original_folder_path, zip_file_path, community_project)

def createZIP(original_folder_path, zip_file_path, new_folder_name):
    if os.path.exists("./_dist") == False:
        os.mkdir("./_dist")
    with zipfile.ZipFile(zip_file_path, 'w') as zipf:
        for root, dirs, files in os.walk(original_folder_path):
            for file in files:
                # Create a new path in the ZIP file
                new_path = os.path.join(new_folder_name, os.path.relpath(os.path.join(root, file), original_folder_path))
                # Add the file to the ZIP file
                zipf.write(os.path.join(root, file), new_path)

def replace_in_file(file_path, replacements):
    """Replace all keys in `replacements` with their values in the given file."""
    with open(file_path, "r", encoding="utf-8") as file:
        content = file.read()

    for old, new in replacements.items():
        content = content.replace(old, new)

    with open(file_path, "w", encoding="utf-8") as file:
        file.write(content)

env.AddPostAction("$BUILD_DIR/${PROGNAME}.hex", copy_fw_files)
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", copy_fw_files)
