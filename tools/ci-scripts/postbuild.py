# Post build script
import os
import sys
import shutil
import zipfile

SOURCE_PATH = os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), '..', '..'))
# FIXME move the helper python modules somewher other than the root of the repo
sys.path.append(SOURCE_PATH)

import hifi_utils

BUILD_PATH = os.path.join(SOURCE_PATH, 'build')
INTERFACE_BUILD_PATH = os.path.join(BUILD_PATH, 'interface', 'Release')
WIPE_PATHS = []

if sys.platform == "win32":
    WIPE_PATHS = [
        'jsdoc',
        'resources/serverless'
    ]
elif sys.platform == "darwin":
    INTERFACE_BUILD_PATH = os.path.join(INTERFACE_BUILD_PATH, "Interface.app", "Contents", "Resources")
    WIPE_PATHS = [
        'jsdoc',
        'serverless'
    ]



# Customize the output filename
def computeArchiveName():
    RELEASE_TYPE = os.getenv("RELEASE_TYPE", "DEV")
    RELEASE_NUMBER = os.getenv("RELEASE_NUMBER", "")
    GIT_PR_COMMIT_SHORT = os.getenv("SHA7", "")

    if RELEASE_TYPE == "PRODUCTION":
        BUILD_VERSION = "{}-{}".format(RELEASE_NUMBER, GIT_PR_COMMIT_SHORT)
    elif RELEASE_TYPE == "PR":
        BUILD_VERSION = "PR{}-{}".format(RELEASE_NUMBER, GIT_PR_COMMIT_SHORT)
    else:
        BUILD_VERSION = "dev"

    if sys.platform == "win32":
        PLATFORM = "windows"
    elif sys.platform == "darwin":
        PLATFORM = "mac"
    else:
        PLATFORM = "other"

    ARCHIVE_NAME = "HighFidelity-Beta-Interface-{}-{}".format(BUILD_VERSION, PLATFORM)
    return ARCHIVE_NAME

def wipeClientBuildPath(relativePath):
    targetPath = os.path.join(INTERFACE_BUILD_PATH, relativePath)
    print("Checking path {}".format(targetPath))
    if os.path.exists(targetPath):
        print("Removing path {}".format(targetPath))
        shutil.rmtree(targetPath)

def fixupMacZip(filename):
    fullPath = os.path.join(BUILD_PATH, "{}.zip".format(filename))
    outFullPath = "{}.zip".format(fullPath)
    print("Fixup mac ZIP file {}".format(fullPath))
    with zipfile.ZipFile(fullPath) as inzip:
        with zipfile.ZipFile(outFullPath, 'w') as outzip:
            rootPath = inzip.infolist()[0].filename
            for entry in inzip.infolist():
                if entry.filename == rootPath:
                    continue
                newFilename = entry.filename[len(rootPath):]
                # ignore the icon
                if newFilename.startswith('Icon'):
                    continue
                # ignore the server console
                if newFilename.startswith('Console.app'):
                    continue
                # ignore the nitpick app
                if newFilename.startswith('nitpick.app'):
                    continue
                # ignore the serverless content
                if newFilename.startswith('interface.app/Contents/Resources/serverless'):
                    continue
                # if we made it here, include the file in the output
                buffer = inzip.read(entry.filename)
                entry.filename = newFilename
                outzip.writestr(entry, buffer)
            outzip.close()
    print("Replacing {} with fixed {}".format(fullPath, outFullPath))
    shutil.move(outFullPath, fullPath)

def fixupWinZip(filename):
    fullPath = os.path.join(BUILD_PATH, "{}.zip".format(filename))
    outFullPath = "{}.zip".format(fullPath)
    print("Fixup windows ZIP file {}".format(fullPath))
    with zipfile.ZipFile(fullPath) as inzip:
        with zipfile.ZipFile(outFullPath, 'w') as outzip:
            for entry in inzip.infolist():
                # ignore the server console
                if entry.filename.startswith('server-console/'):
                    continue
                # ignore the nitpick app
                if entry.filename.startswith('nitpick/'):
                    continue
                # if we made it here, include the file in the output
                buffer = inzip.read(entry.filename)
                outzip.writestr(entry, buffer)
            outzip.close()
    print("Replacing {} with fixed {}".format(fullPath, outFullPath))
    shutil.move(outFullPath, fullPath)


for wipePath in WIPE_PATHS:
    wipeClientBuildPath(wipePath)

# Need the archive name for ad-hoc zip file manipulation
archiveName = computeArchiveName()

cpackCommand = [
    'cpack', 
    '-G', 'ZIP', 
    '-D', "CPACK_PACKAGE_FILE_NAME={}".format(archiveName), 
    '-D', "CPACK_INCLUDE_TOPLEVEL_DIRECTORY=OFF"
    ]

print("Create ZIP version of installer archive")
print(cpackCommand)
hifi_utils.executeSubprocess(cpackCommand, folder=BUILD_PATH)

if sys.platform == "win32":
    fixupWinZip(archiveName)
elif sys.platform == "darwin":
    fixupMacZip(archiveName)

